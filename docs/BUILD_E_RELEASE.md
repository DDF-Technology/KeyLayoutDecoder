# Build, verifica e release

## Build locale

Prerequisito: Visual Studio 2022 con **Desktop development with C++**.

```bat
build.bat
```

Il flag `/MT` incorpora il runtime C. L'eseguibile non richiede librerie di terze
parti: usa soltanto API di sistema Windows (`user32` e `shell32`).

## Verifica manuale

```bat
KeyLayoutDecoder.exe --quiet examples\sample_input.txt
```

Aprire `examples\sample_input_converted.txt`: deve contenere `Ciao!` e una nuova riga.

## GitHub Actions

Il workflow `.github/workflows/build.yml` compila su `windows-latest`, esegue il test
sintetico e pubblica `KeyLayoutDecoder.exe` come artefatto della run.

## Preparazione di una release

1. aggiornare `CHANGELOG.md` e `VERSION`;
2. eseguire build e test su Windows x64;
3. verificare che repository e pacchetto non contengano log reali;
4. creare il tag corrispondente, per esempio `v1.0.0`;
5. allegare alla release il pacchetto portable e il relativo checksum SHA-256;
6. descrivere requisiti, utilizzo, limiti noti, licenza MIT e assenza di firma digitale.

Prima del push verificare sempre:

```bat
git status
git ls-files
```

Non devono comparire log reali, file `*_converted.txt`, eseguibili, oggetti del
compilatore o credenziali.

## Controllo integrità

```powershell
Get-FileHash .\KeyLayoutDecoder.exe -Algorithm SHA256
```

Pubblicare l'hash nelle note della release per consentire la verifica del binario.
