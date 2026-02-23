@echo off
setlocal
set "DIR=%~dp0"
cd /d "%DIR%"

set "VCVARS="
if defined ProgramFiles(x86) (
    for /d %%V in ("%ProgramFiles(x86)%\Microsoft Visual Studio\2022\*") do if exist "%%V\VC\Auxiliary\Build\vcvars32.bat" set "VCVARS=%%V\VC\Auxiliary\Build\vcvars32.bat"
    if not defined VCVARS for /d %%V in ("%ProgramFiles(x86)%\Microsoft Visual Studio\2019\*") do if exist "%%V\VC\Auxiliary\Build\vcvars32.bat" set "VCVARS=%%V\VC\Auxiliary\Build\vcvars32.bat"
)
if not defined VCVARS (
    for /d %%V in ("%ProgramFiles%\Microsoft Visual Studio\2022\*") do if exist "%%V\VC\Auxiliary\Build\vcvars32.bat" set "VCVARS=%%V\VC\Auxiliary\Build\vcvars32.bat"
    if not defined VCVARS for /d %%V in ("%ProgramFiles%\Microsoft Visual Studio\2019\*") do if exist "%%V\VC\Auxiliary\Build\vcvars32.bat" set "VCVARS=%%V\VC\Auxiliary\Build\vcvars32.bat"
)

if not defined VCVARS (
    echo Visual Studio not found. Run from Developer Command Prompt for VS x86
    echo   cl /EHsc /Fe:InstructionCounting.exe "Instruction Counting.cpp" hwbrk.cpp
    exit /b 1
)

call "%VCVARS%" >nul 2>&1

cl /nologo /EHsc /Fe:InstructionCounting.exe "Instruction Counting.cpp" hwbrk.cpp
if errorlevel 1 exit /b 1

echo.
echo Built: %DIR%InstructionCounting.exe
exit /b 0
