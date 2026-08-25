# Key Layout Decoder 1.0.0

Prima release pubblica con licenza MIT della utility Windows x64 per ricostruire
testo UTF-8 da log di eventi tastiera nel formato documentato.

## Funzioni incluse

- interpretazione basata su scan code fisico e layout Windows dichiarato;
- gestione di Shift, Ctrl, Alt, AltGr, Caps Lock e dead key;
- supporto per Invio, Tab e Backspace;
- elaborazione drag & drop di uno o più file;
- modalità silenziosa `--quiet` per script e test;
- output UTF-8 con BOM accanto al file originale;
- funzionamento completamente offline, senza telemetria o acquisizione della tastiera.

## Pacchetto Windows

- file: `KeyLayoutDecoder-1.0.0-windows-x64.zip`;
- dimensione: `102371` byte;
- SHA-256: `d963e4c39d9cce592333521e5f44e56f4bbfaa4e896cf83cff7bdaa29beaf190`.

Il pacchetto contiene eseguibile portable, documentazione, licenza MIT, informativa
di sicurezza e campione sintetico per la verifica.

## File di test allegato

La release offre anche `sample_input.txt` come download separato. Trascinarlo su
`KeyLayoutDecoder.exe` oppure eseguire:

```bat
KeyLayoutDecoder.exe --quiet sample_input.txt
```

Il programma deve creare `sample_input_converted.txt` contenente `Ciao!` seguito da
una nuova riga. Il campione è interamente sintetico e non contiene dati reali.

## Requisiti e avvertenze

- Windows 10 o Windows 11 x64;
- il layout indicato nel log deve essere installato nel sistema;
- la build non è firmata digitalmente e Windows può mostrare un avviso di reputazione;
- i log e il testo ricostruito possono contenere dati sensibili: elaborare soltanto
  file autorizzati e non pubblicare log reali.

Il software è fornito senza garanzia secondo i termini della licenza MIT.
