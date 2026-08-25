# Funzionamento interno

## Flusso di conversione

1. Il percorso trascinato viene ricevuto da Windows tramite la riga di comando Unicode.
2. Il programma legge `layout=XXXXXXXX` e carica il layout con `LoadKeyboardLayoutW`.
3. Per ogni evento viene aggiornato un array di 256 byte compatibile con lo stato
   tastiera richiesto dalle API Win32.
4. Lo scan code viene convertito nel virtual-key proprio del layout tramite
   `MapVirtualKeyExW`.
5. `ToUnicodeEx` combina virtual-key, scan code, layout e modificatori per produrre
   caratteri Unicode.
6. Il testo viene mantenuto in memoria per consentire a Backspace di rimuovere il
   carattere precedente.
7. Il risultato viene convertito in UTF-8 e scritto con BOM.

## Perché viene privilegiato lo scan code

Il virtual-key può dipendere dal layout attivo quando l'evento è stato acquisito. Lo
scan code rappresenta invece la posizione fisica del tasto. Rimappandolo con il layout
dichiarato, simboli e lettere vengono interpretati secondo la tastiera originale.

## Modificatori e dead key

Il programma conserva separatamente tasti sinistri e destri e aggiorna anche gli stati
generici `VK_SHIFT`, `VK_CONTROL` e `VK_MENU`. Questo consente a `ToUnicodeEx` di
ricostruire maiuscole, AltGr, accenti e combinazioni con dead key.

Le shortcut Ctrl senza Alt non vengono convertite in caratteri di controllo. La coppia
Ctrl+Alt non viene filtrata perché Windows può usarla per rappresentare AltGr.

## Correzioni nel testo

- `Backspace` elimina l'ultimo carattere Unicode o l'ultima coppia `CRLF`;
- `Enter` aggiunge `CRLF`;
- `Tab` aggiunge un carattere di tabulazione;
- tasti funzione, navigazione e altri tasti senza rappresentazione testuale non
  producono output.

## Sicurezza operativa

Il programma non installa hook, non monitora la tastiera, non apre connessioni di rete
e non esegue il contenuto dell'input. Il parser tratta ogni riga esclusivamente come
dato strutturato.
