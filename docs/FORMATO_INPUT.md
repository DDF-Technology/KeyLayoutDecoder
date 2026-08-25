# Formato di input

## Intestazione

La prima riga deve contenere l'identificatore esadecimale del layout Windows:

```text
layout=00000410
```

`00000410` identifica il layout italiano. Il valore è passato a
`LoadKeyboardLayoutW` e deve quindi corrispondere a un layout installato nel sistema
che esegue la conversione.

## Eventi

Ogni riga successiva descrive un evento:

```text
[2026-08-25 18:47:21] vk=0x43 scan=0x02E down
[2026-08-25 18:47:22] vk=0x43 scan=0x02E up
```

| Campo | Significato |
|---|---|
| timestamp | Informativo; non influenza la conversione |
| `vk` | Virtual-key Windows in esadecimale |
| `scan` | Scan code fisico in esadecimale |
| `down` | Pressione o ripetizione del tasto |
| `up` | Rilascio del tasto |

Il testo viene prodotto dagli eventi `down`, nell'ordine in cui compaiono. Gli eventi
`up` aggiornano lo stato dei modificatori. Ripetuti eventi `down` su un normale tasto
producono la ripetizione del carattere; ripetizioni dei soli modificatori non generano
testo.

## Codifica del file

L'input contiene soltanto metadati ASCII ed è leggibile come ANSI o UTF-8. L'output è
sempre UTF-8 con BOM e usa terminatori di riga Windows `CRLF`.

## Regole di validazione

- `layout=` è obbligatorio e deve avere otto cifre esadecimali;
- righe evento non riconosciute vengono ignorate;
- valori VK fuori dall'intervallo della tabella Windows non aggiornano lo stato;
- il file di output usa il nome `<nome_input>_converted.txt`.
