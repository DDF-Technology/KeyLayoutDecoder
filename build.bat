@echo off
setlocal EnableExtensions
where cl >nul 2>nul
if not errorlevel 1 goto :build

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :no_compiler
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
if not defined VSROOT goto :no_compiler
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 goto :no_compiler

:build
cl /nologo /W4 /O2 /MT key_layout_decoder.c /Fe:KeyLayoutDecoder.exe /link /SUBSYSTEM:WINDOWS user32.lib shell32.lib
if errorlevel 1 exit /b 1
echo Creato KeyLayoutDecoder.exe
exit /b 0

:no_compiler
echo ERRORE: compilatore MSVC non trovato.
echo Installa "Desktop development with C++" di Visual Studio.
exit /b 1
