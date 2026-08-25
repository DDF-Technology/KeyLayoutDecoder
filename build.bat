@echo off
setlocal EnableDelayedExpansion
where cl >nul 2>nul
if errorlevel 1 (
  for %%E in (Community Professional Enterprise BuildTools) do if exist "%ProgramFiles%\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" set "VSROOT=%ProgramFiles%\Microsoft Visual Studio\2022\%%E"
  if not defined VSROOT goto :no_compiler
  call "!VSROOT!\VC\Auxiliary\Build\vcvars64.bat" >nul
)
cl /nologo /W4 /O2 /MT key_layout_decoder.c /Fe:KeyLayoutDecoder.exe /link /SUBSYSTEM:WINDOWS user32.lib shell32.lib
if errorlevel 1 exit /b 1
echo Creato KeyLayoutDecoder.exe
exit /b 0

:no_compiler
echo ERRORE: compilatore MSVC non trovato.
echo Installa "Desktop development with C++" di Visual Studio.
exit /b 1
