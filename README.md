# Key Layout Decoder

Utility Windows minimale, scritta in C, che ricostruisce il testo prodotto da una
sequenza di eventi tastiera usando il layout dichiarato nel file di input.

Il programma è pensato per l'uso tramite drag & drop: si trascina un log `.txt` su
`KeyLayoutDecoder.exe` e si ottiene un nuovo file UTF-8 accanto all'originale.

> I log di tastiera possono contenere password, messaggi o altri dati personali:
> non pubblicare mai log reali e lavorare sempre su copie conservate in locale.

## Funzionalità

- conversione basata sullo scan code fisico e sul layout Windows specificato;
- supporto per Shift, Ctrl, Alt, AltGr e Caps Lock;
- gestione di dead key, caratteri accentati e simboli specifici del layout;
- gestione di Invio, Tab e Backspace;
- output UTF-8 con BOM, leggibile direttamente con Blocco note;
- elaborazione di uno o più file trascinati contemporaneamente;
- nessuna dipendenza esterna, rete, telemetria o installazione.

## Requisiti

- Windows 10 o Windows 11, x64;
- il layout indicato nel log deve essere installato in Windows;
- Visual Studio 2022 con il workload **Desktop development with C++** solo per
  compilare il sorgente.

## Utilizzo

1. Compilare il sorgente oppure scaricare `KeyLayoutDecoder.exe` dalla release GitHub.
2. Trascinare il file di log sull'EXE.
3. Confermare il messaggio finale.
4. Aprire il file `<nome>_converted.txt` creato accanto all'input.

L'originale non viene modificato. Se il file di destinazione esiste già, viene
sovrascritto.

È disponibile anche una modalità silenziosa per script e test:

```bat
KeyLayoutDecoder.exe --quiet file.txt
```

Si possono passare più file:

```bat
KeyLayoutDecoder.exe --quiet primo.txt secondo.txt
```

## Esempio

Il repository contiene esclusivamente un campione sintetico:

- [`examples/sample_input.txt`](examples/sample_input.txt): eventi che rappresentano
  la digitazione di `Ciao!` con layout statunitense, disponibile anche sui runner CI;
- [`examples/sample_expected_output.txt`](examples/sample_expected_output.txt):
  risultato atteso.

Esecuzione:

```bat
KeyLayoutDecoder.exe --quiet examples\sample_input.txt
```

Aprire `examples\sample_input_converted.txt`: deve contenere `Ciao!` e una nuova riga.
L'output generato contiene anche il BOM UTF-8; per questo il test automatico confronta
il contenuto testuale e non i byte del campione atteso.

## Compilazione

Da Prompt dei comandi o PowerShell:

```bat
build.bat
```

Lo script individua Visual Studio 2022 e genera un eseguibile x64 con runtime C
collegato staticamente. Il comando equivalente da un Developer Command Prompt è:

```bat
cl /nologo /W4 /O2 /MT key_layout_decoder.c /Fe:KeyLayoutDecoder.exe /link /SUBSYSTEM:WINDOWS user32.lib shell32.lib
```

## Formato e funzionamento

- [Specifica del formato di input](docs/FORMATO_INPUT.md)
- [Funzionamento interno e scelte progettuali](docs/FUNZIONAMENTO.md)
- [Build, verifica e release](docs/BUILD_E_RELEASE.md)
- [Note della release 1.0.0](docs/RELEASE_NOTES_1.0.0.md)
- [Avvisi relativi a strumenti e API di sistema](THIRD_PARTY_NOTICES.md)
- [Informativa di sicurezza](SECURITY.md)
- [Cronologia delle versioni](CHANGELOG.md)

## Limiti noti

- Lo stato iniziale di Caps Lock, Num Lock e Scroll Lock non è presente nel formato:
  il programma presume che siano disattivati all'inizio del log.
- Un layout non installato sul computer non può essere caricato.
- Il formato non distingue il flag esteso `E0/E1`; per Invio e tastierino numerico il
  programma conserva il virtual-key registrato come compensazione.
- Shortcut con Ctrl vengono ignorate; Ctrl+Alt viene mantenuto perché può rappresentare
  AltGr.
- Le righe malformate vengono ignorate, mentre una prima riga `layout=` non valida
  interrompe la conversione.

## Privacy e licenza

Il software opera interamente in locale. Non registra la tastiera e non acquisisce
nuovi eventi: converte soltanto file già esistenti.

Copyright © 2026 Fabio De Deo - [www.ddf.technology](https://www.ddf.technology).
Distribuito con licenza [MIT](LICENSE.md): uso, studio, modifica e ridistribuzione
sono consentiti conservando l'avviso di copyright e la licenza.
