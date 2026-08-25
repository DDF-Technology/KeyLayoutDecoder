# Build, verifica e pubblicazione privata

## Build locale

Prerequisito: Visual Studio 2022 con **Desktop development with C++**.

```bat
build.bat
```

Il flag `/MT` incorpora il runtime C. Restano soltanto dipendenze di sistema Windows.

## Verifica manuale

```bat
KeyLayoutDecoder.exe --quiet examples\sample_input.txt
```

Aprire `examples\sample_input_converted.txt`: deve contenere `Ciao!` e una nuova riga.

## Repository GitHub privato

Impostazioni raccomandate:

- nome: `KeyLayoutDecoder`;
- visibilità: **Private**;
- branch predefinito: `main`;
- Issues e Discussions disabilitati se il progetto rimane personale;
- Actions abilitate per produrre gli artefatti Windows;
- nessuna licenza open source.

Creazione tramite GitHub CLI, da eseguire soltanto quando si desidera pubblicare:

```bat
git init
git branch -M main
git add .
git commit -m "Initial private release"
gh repo create DDF-Technology/KeyLayoutDecoder --private --source=. --remote=origin --push
```

Prima del push verificare sempre:

```bat
git status
git ls-files
```

Non devono comparire log reali, file `*_converted.txt`, eseguibili, oggetti del
compilatore o credenziali.

## GitHub Actions

Il workflow `.github/workflows/build.yml` compila su `windows-latest`, esegue il test
sintetico e pubblica `KeyLayoutDecoder.exe` come artefatto privato del workflow.

Per una versione numerata:

1. aggiornare `CHANGELOG.md` e `VERSION`;
2. eseguire il test locale;
3. creare un tag, per esempio `v1.0.0`;
4. effettuare il push del tag;
5. scaricare l'artefatto dalla run GitHub Actions e allegarlo a una release privata.

## Controllo integrità

```powershell
Get-FileHash .\KeyLayoutDecoder.exe -Algorithm SHA256
```

Conservare l'hash nelle note della release per consentire la verifica del binario.
