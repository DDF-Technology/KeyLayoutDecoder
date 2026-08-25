# Sicurezza e privacy

## Dati trattati

I file di input possono ricostruire testo digitato e quindi contenere password,
token, messaggi privati o dati personali. Trattare ogni log reale come confidenziale.

## Regole per questo repository

- il repository deve rimanere **privato**;
- non aggiungere log reali, nemmeno alla cronologia Git;
- usare soltanto i campioni sintetici contenuti in `examples/`;
- non allegare output reali a issue, release o workflow;
- controllare `git status` e `git ls-files` prima di ogni push;
- se un dato sensibile viene committato, considerarlo esposto anche dopo la semplice
  cancellazione e riscrivere la cronologia prima di proseguire.

## Comportamento dell'applicazione

Key Layout Decoder:

- non acquisisce eventi dalla tastiera;
- non installa hook o servizi;
- non usa la rete e non invia telemetria;
- legge soltanto i percorsi forniti dall'utente;
- crea l'output accanto all'input e può sovrascrivere un output omonimo esistente.

## Segnalazioni

Per problemi di sicurezza usare un canale privato del proprietario del repository e
non aprire issue contenenti file di log o dati sensibili.
