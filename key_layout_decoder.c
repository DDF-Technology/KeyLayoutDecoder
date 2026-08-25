#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

typedef struct {
    wchar_t *data;
    size_t length;
    size_t capacity;
} TextBuffer;

static int buffer_reserve(TextBuffer *buffer, size_t extra) {
    size_t required = buffer->length + extra + 1;
    size_t capacity;
    wchar_t *new_data;

    if (required <= buffer->capacity) return 1;
    capacity = buffer->capacity ? buffer->capacity : 256;
    while (capacity < required) capacity *= 2;
    new_data = (wchar_t *)realloc(buffer->data, capacity * sizeof(wchar_t));
    if (!new_data) return 0;
    buffer->data = new_data;
    buffer->capacity = capacity;
    return 1;
}

static int buffer_append(TextBuffer *buffer, const wchar_t *text, size_t count) {
    if (!buffer_reserve(buffer, count)) return 0;
    wmemcpy(buffer->data + buffer->length, text, count);
    buffer->length += count;
    buffer->data[buffer->length] = L'\0';
    return 1;
}

static void buffer_backspace(TextBuffer *buffer) {
    if (!buffer->length) return;
    if (buffer->length >= 2 && buffer->data[buffer->length - 2] == L'\r' &&
        buffer->data[buffer->length - 1] == L'\n') {
        buffer->length -= 2;
    } else {
        --buffer->length;
        if (buffer->length && buffer->data[buffer->length] >= 0xDC00 &&
            buffer->data[buffer->length] <= 0xDFFF &&
            buffer->data[buffer->length - 1] >= 0xD800 &&
            buffer->data[buffer->length - 1] <= 0xDBFF) {
            --buffer->length;
        }
    }
    buffer->data[buffer->length] = L'\0';
}

static int is_modifier(UINT vk) {
    return vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
           vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
           vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
           vk == VK_LWIN || vk == VK_RWIN;
}

static void refresh_generic_modifiers(BYTE state[256]) {
    state[VK_SHIFT] = (BYTE)((state[VK_LSHIFT] | state[VK_RSHIFT]) & 0x80);
    state[VK_CONTROL] = (BYTE)((state[VK_LCONTROL] | state[VK_RCONTROL]) & 0x80);
    state[VK_MENU] = (BYTE)((state[VK_LMENU] | state[VK_RMENU]) & 0x80);
}

static void set_key_state(BYTE state[256], UINT vk, int down, int was_down) {
    if (vk >= 256) return;
    if (down) {
        state[vk] |= 0x80;
        if (!was_down && (vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL))
            state[vk] ^= 0x01;
    } else {
        state[vk] &= 0x7F;
    }
    refresh_generic_modifiers(state);
}

static UINT layout_vk_from_scan(UINT recorded_vk, UINT scan, HKL layout) {
    UINT mapped = MapVirtualKeyExW(scan, MAPVK_VSC_TO_VK_EX, layout);
    if (!mapped) return recorded_vk;
    /* Preserve keys whose extended-key marker is absent from this log format. */
    if (recorded_vk == VK_RETURN || recorded_vk == VK_DIVIDE ||
        (recorded_vk >= VK_NUMPAD0 && recorded_vk <= VK_NUMPAD9))
        return recorded_vk;
    return mapped & 0xFF;
}

static wchar_t *make_output_path(const wchar_t *input) {
    const wchar_t *slash1 = wcsrchr(input, L'\\');
    const wchar_t *slash2 = wcsrchr(input, L'/');
    const wchar_t *slash = slash1 > slash2 ? slash1 : slash2;
    const wchar_t *dot = wcsrchr(input, L'.');
    size_t stem_length;
    size_t total;
    wchar_t *output;

    if (!dot || (slash && dot < slash)) dot = input + wcslen(input);
    stem_length = (size_t)(dot - input);
    total = stem_length + wcslen(L"_converted.txt") + 1;
    output = (wchar_t *)malloc(total * sizeof(wchar_t));
    if (!output) return NULL;
    wmemcpy(output, input, stem_length);
    wcscpy_s(output + stem_length, total - stem_length, L"_converted.txt");
    return output;
}

static int write_utf8(const wchar_t *path, const TextBuffer *text, wchar_t *error, size_t error_count) {
    FILE *file = NULL;
    char *utf8 = NULL;
    int bytes;
    const unsigned char bom[] = {0xEF, 0xBB, 0xBF};

    if (_wfopen_s(&file, path, L"wb") != 0 || !file) {
        swprintf_s(error, error_count, L"Impossibile creare:\n%s", path);
        return 0;
    }
    bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text->data,
                                (int)text->length, NULL, 0, NULL, NULL);
    if (bytes < 0 || (text->length && !bytes)) {
        fclose(file);
        swprintf_s(error, error_count, L"Errore durante la conversione in UTF-8.");
        return 0;
    }
    if (bytes) {
        utf8 = (char *)malloc((size_t)bytes);
        if (!utf8 || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text->data,
                                        (int)text->length, utf8, bytes, NULL, NULL) != bytes) {
            free(utf8);
            fclose(file);
            swprintf_s(error, error_count, L"Memoria insufficiente durante la conversione.");
            return 0;
        }
    }
    if (fwrite(bom, 1, sizeof(bom), file) != sizeof(bom) ||
        (bytes && fwrite(utf8, 1, (size_t)bytes, file) != (size_t)bytes)) {
        free(utf8);
        fclose(file);
        swprintf_s(error, error_count, L"Errore durante la scrittura di:\n%s", path);
        return 0;
    }
    free(utf8);
    fclose(file);
    return 1;
}

static int convert_file(const wchar_t *input_path, wchar_t **output_path,
                        wchar_t *error, size_t error_count) {
    FILE *input = NULL;
    char line[1024];
    char layout_name_a[9] = {0};
    wchar_t layout_name[KL_NAMELENGTH] = {0};
    HKL layout = NULL;
    BYTE state[256] = {0};
    TextBuffer text = {0};
    int ok = 0;

    *output_path = NULL;
    if (_wfopen_s(&input, input_path, L"rb") != 0 || !input) {
        swprintf_s(error, error_count, L"Impossibile aprire:\n%s", input_path);
        goto cleanup;
    }
    if (!fgets(line, sizeof(line), input) || sscanf_s(line, "layout=%8[0-9A-Fa-f]", layout_name_a, (unsigned)_countof(layout_name_a)) != 1) {
        swprintf_s(error, error_count, L"Prima riga non valida: atteso layout=XXXXXXXX.");
        goto cleanup;
    }
    if (MultiByteToWideChar(CP_ACP, 0, layout_name_a, -1, layout_name, KL_NAMELENGTH) == 0) {
        swprintf_s(error, error_count, L"Identificatore del layout non valido.");
        goto cleanup;
    }
    layout = LoadKeyboardLayoutW(layout_name, KLF_NOTELLSHELL);
    if (!layout) {
        swprintf_s(error, error_count,
                   L"Il layout %s non e' installato in Windows.\nInstallarlo e riprovare.", layout_name);
        goto cleanup;
    }
    if (!buffer_reserve(&text, 0)) {
        swprintf_s(error, error_count, L"Memoria insufficiente.");
        goto cleanup;
    }
    text.data[0] = L'\0';

    while (fgets(line, sizeof(line), input)) {
        unsigned int recorded_vk, scan;
        char action[5] = {0};
        UINT vk;
        int down, was_down;
        wchar_t translated[16];
        int count;

        if (sscanf_s(line, "%*[^]] ] vk=0x%x scan=0x%x %4s",
                     &recorded_vk, &scan, action, (unsigned)_countof(action)) != 3)
            continue;
        down = strcmp(action, "down") == 0;
        if (!down && strcmp(action, "up") != 0) continue;
        vk = layout_vk_from_scan(recorded_vk, scan, layout);
        was_down = recorded_vk < 256 && (state[recorded_vk] & 0x80) != 0;
        set_key_state(state, recorded_vk, down, was_down);
        if (vk != recorded_vk) set_key_state(state, vk, down, was_down);
        if (!down || is_modifier(recorded_vk) || is_modifier(vk)) continue;

        if (vk == VK_BACK) {
            buffer_backspace(&text);
            continue;
        }
        if (vk == VK_RETURN) {
            if (!buffer_append(&text, L"\r\n", 2)) goto memory_error;
            continue;
        }
        if (vk == VK_TAB) {
            if (!buffer_append(&text, L"\t", 1)) goto memory_error;
            continue;
        }
        /* Ignore Ctrl shortcuts, but retain Ctrl+Alt (AltGr). */
        if ((state[VK_CONTROL] & 0x80) && !(state[VK_MENU] & 0x80)) continue;
        count = ToUnicodeEx(vk, scan, state, translated, (int)_countof(translated), 0, layout);
        if (count > 0 && !buffer_append(&text, translated, (size_t)count)) goto memory_error;
        continue;

memory_error:
        swprintf_s(error, error_count, L"Memoria insufficiente.");
        goto cleanup;
    }

    *output_path = make_output_path(input_path);
    if (!*output_path) goto memory_error_after_loop;
    if (!write_utf8(*output_path, &text, error, error_count)) goto cleanup;
    ok = 1;
    goto cleanup;

memory_error_after_loop:
    swprintf_s(error, error_count, L"Memoria insufficiente.");

cleanup:
    if (input) fclose(input);
    free(text.data);
    if (!ok) {
        free(*output_path);
        *output_path = NULL;
    }
    return ok;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command_line, int show) {
    int argc = 0;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int converted = 0;
    int i, first_file = 1, quiet = 0;
    wchar_t message[4096] = L"";
    size_t used = 0;

    (void)instance; (void)previous; (void)command_line; (void)show;
    if (argv && argc > 1 && (wcscmp(argv[1], L"--quiet") == 0 || wcscmp(argv[1], L"/quiet") == 0)) {
        quiet = 1;
        first_file = 2;
    }
    if (!argv || argc <= first_file) {
        MessageBoxW(NULL,
                    L"Trascina uno o piu' file .txt su questo programma.\n\n"
                    L"Per ogni input verra' creato <nome>_converted.txt nella stessa cartella.",
                    L"Key Layout Decoder", MB_OK | MB_ICONINFORMATION);
        if (argv) LocalFree(argv);
        return 1;
    }

    for (i = first_file; i < argc; ++i) {
        wchar_t *output = NULL;
        wchar_t error[1024];
        if (convert_file(argv[i], &output, error, _countof(error))) {
            int written = swprintf_s(message + used, _countof(message) - used,
                                     L"Creato:\n%s\n\n", output);
            if (written > 0) used += (size_t)written;
            free(output);
            ++converted;
        } else {
            if (!quiet)
                MessageBoxW(NULL, error, L"Key Layout Decoder - Errore", MB_OK | MB_ICONERROR);
        }
    }
    if (converted && !quiet)
        MessageBoxW(NULL, message, L"Key Layout Decoder", MB_OK | MB_ICONINFORMATION);
    LocalFree(argv);
    return converted == argc - first_file ? 0 : 2;
}
