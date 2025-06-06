@ECHO OFF
SETLOCAL EnableDelayedExpansion
SET DIRECTORY=%~dp0
SET ROOT=%cd%
:begin_args
SET ARG=%~1
IF "!IS_DEPENDENCY!" == "1" (
  SET DEPENDENCIES=!ARG!
  SET IS_DEPENDENCY=
  SHIFT
  GOTO begin_args
) ELSE IF NOT "!ARG!" == "" (
  IF "!ARG:~0,3!" == "-DD" (
    SET IS_DEPENDENCY=1
  ) ELSE (
    SET CONFIG=!ARG!
  )
  SHIFT
  GOTO begin_args
)
IF "!CONFIG!" == "clean" (
  git clean -ffxd -e *Dependencies*
  IF EXIST Dependencies\cache_files\ashkal.txt (
    DEL Dependencies\cache_files\ashkal.txt
  )
) ELSE IF "!CONFIG!" == "reset" (
  git clean -ffxd
  IF EXIST Dependencies\cache_files\ashkal.txt (
    DEL Dependencies\cache_files\ashkal.txt
  )
) ELSE (
  IF "!CONFIG!" == "" (
    IF EXIST CMakeFiles\config.txt (
      FOR /F %%i IN ('TYPE CMakeFiles\config.txt') DO (
        SET CONFIG=%%i
      )
    ) ELSE (
      SET CONFIG=Release
    )
  )
  IF NOT "!DEPENDENCIES!" == "" (
    CALL "!DIRECTORY!configure.bat" -DD="!DEPENDENCIES!"
  ) ELSE (
    CALL "!DIRECTORY!configure.bat"
  )
  cmake --build "!ROOT!" --target INSTALL --config "!CONFIG!"
  ECHO !CONFIG! > CMakeFiles\config.txt
)
CALL:build Applications\Scratch %*
ENDLOCAL
EXIT /B %ERRORLEVEL%

:build
IF NOT EXIST "%~1" (
  MD "%~1"
)
PUSHD "%~1"
CALL "%~dp0%~1\build.bat" -DD="%ROOT%\Dependencies" %~2 %~3 %~4 %~5 %~6 %~7
POPD
EXIT /B 0
