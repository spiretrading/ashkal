@ECHO OFF
SETLOCAL EnableDelayedExpansion
SET EXIT_STATUS=0
SET ROOT=%cd%
IF EXIST cache_files\ashkal.txt (
  FOR /F %%i IN (
      'ls -l --time-style=full-iso "%~dp0\setup.bat" ^| awk "{print $6 $7}"') DO (
    FOR /F %%j IN (
        'ls -l --time-style=full-iso cache_files\ashkal.txt ^| awk "{print $6 $7}"') DO (
      IF "%%i" LSS "%%j" (
        EXIT /B 0
      )
    )
  )
)
SET VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
FOR /f "usebackq delims=" %%i IN (`!VSWHERE! -prerelease -latest -property installationPath`) DO (
  IF EXIST "%%i\Common7\Tools\vsdevcmd.bat" (
    CALL "%%i\Common7\Tools\vsdevcmd.bat"
  )
)
IF NOT EXIST doctest-2.4.6 (
  wget https://github.com/onqtam/doctest/archive/2.4.6.zip --no-check-certificate
  IF !ERRORLEVEL! LEQ 0 (
    tar -xf 2.4.6.zip
  ) ELSE (
    SET EXIT_STATUS=1
  )
  DEL /F /Q 2.4.6.zip
)
IF NOT EXIST cache_files (
  MD cache_files
)
ECHO timestamp > cache_files\ashkal.txt
ENDLOCAL
EXIT /B !EXIT_STATUS!
