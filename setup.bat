@ECHO OFF
SETLOCAL EnableDelayedExpansion
SET EXIT_STATUS=0
SET ROOT=%cd%
IF EXIST cache_files\ashkal.txt (
  SET CACHE_COMMAND=powershell -Command "& { " ^
    "$setupTimestamp = (Get-Item '%~dp0setup.bat').LastWriteTime; " ^
    "$ashkalTimestamp = (Get-Item 'cache_files\\ashkal.txt').LastWriteTime; " ^
    "if($setupTimestamp -lt $ashkalTimestamp) {" ^
    "  Write-Output '0';" ^
    "} else {" ^
    "  Write-Output '1';" ^
    "}" ^
  "}"
  FOR /F "delims=" %%A IN ('CALL !CACHE_COMMAND!') DO SET IS_CACHED=%%A
  IF "!IS_CACHED!"=="0" (
    EXIT /B 0
  )
)
SET VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
FOR /F "usebackq delims=" %%i IN (` ^
    !VSWHERE! -prerelease -latest -property installationPath`) DO (
  IF EXIST "%%i\Common7\Tools\vsdevcmd.bat" (
    CALL "%%i\Common7\Tools\vsdevcmd.bat" -arch=amd64
  )
)
CALL :DownloadAndExtract "doctest-2.4.11" ^
  "https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.zip"
CALL :DownloadAndExtract "freetype-2.9.1" ^
  "https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz"
IF %BUILD_NEEDED%==1 (
  PUSHD freetype-2.9.1
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\freetype-2.9.1\build ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
IF NOT EXIST glew-2.1.0 (
  wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip/download -O glew-2.1.0.zip
  IF !ERRORLEVEL! LEQ 0 (
    tar -xf glew-2.1.0.zip
    PUSHD glew-2.1.0
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\glew-2.1.0\build ./cmake
    cmake --build . --target INSTALL --config Debug
    cmake --build . --target INSTALL --config Release
    POPD
  ) ELSE (
    SET EXIT_STATUS=1
  )
  DEL /F /Q glew-2.1.0.zip
)
IF NOT EXIST OpenCL-SDK (
  git clone https://github.com/KhronosGroup/OpenCL-SDK.git
  PUSHD OpenCL-SDK
  git submodule init
  git submodule update
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\OpenCL-SDK\build ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
CALL :DownloadAndExtract "SDL2-2.0.16" ^
  "https://www.libsdl.org/release/SDL2-2.0.16.zip"
IF %BUILD_NEEDED%==1 (
  PUSHD SDL2-2.0.16
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\SDL2-2.0.16 ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  COPY ..\SDL2Config.cmake .
  COPY CMakeFiles\Export\cmake\* .
  POPD
  POPD
)
CALL :DownloadAndExtract "SDL2_ttf-2.0.15" ^
  "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.zip"
IF %BUILD_NEEDED%==1 (
  PUSHD SDL2_ttf-2.0.15
  powershell -Command ^
    "(Get-Content CMakeLists.txt) | ForEach-Object {if ($_ -match '^add_library') {'include_directories(${FREETYPE_INCLUDE_DIRS})'}; $_} | Set-Content CMakeLists.txt"
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\SDL2_ttf-2.0.15\build -DSDL2_DIR=!ROOT!\SDL2-2.0.16\cmake -DFREETYPE_LIBRARY=!ROOT!\freetype-2.9.1\build\Release\ -DFREETYPE_INCLUDE_DIRS=!ROOT!\freetype-2.9.1\include\ ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
CALL :DownloadAndExtract "SDL2_image-2.8.8" ^
  "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-2.8.8.zip"
IF %BUILD_NEEDED%==1 (
  PUSHD SDL2_image-2.8.8
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\SDL2_image-2.8.8 -DBUILD_SHARED_LIBS=OFF -DSDL2_DIR=!ROOT!\SDL2-2.0.16\cmake -DSDL2IMAGE_VENDORED=OFF ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
CALL :DownloadAndExtract "zlib-1.3.1" ^
  "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip"
IF %BUILD_NEEDED%==1 (
  PUSHD zlib-1.3.1\contrib\vstudio\vc17
  powershell -Command "(Get-Content zlibstat.vcxproj) -replace " ^
    "'ZLIB_WINAPI;', '' -replace " ^
    "'<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>', " ^
    "'<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>' -replace " ^
    "'<RuntimeLibrary>MultiThreaded</RuntimeLibrary>', " ^
    "'<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>' | " ^
    "Set-Content zlibstat.vcxproj"
  msbuild zlibstat.vcxproj /p:UseEnv=True /p:PlatformToolset=v143 ^
    /p:Platform=x64 /p:Configuration=Debug
  msbuild zlibstat.vcxproj /p:UseEnv=True /p:PlatformToolset=v143 ^
    /p:Platform=x64 /p:Configuration=ReleaseWithoutAsm
  POPD
)
CALL :DownloadAndExtract "assimp-6.0.2" ^
  "https://github.com/assimp/assimp/archive/refs/tags/v6.0.2.zip"
IF %BUILD_NEEDED%==1 (
  PUSHD assimp-6.0.2
  cmake -DBUILD_SHARED_LIBS=OFF -DASSIMP_INSTALL=OFF
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
)
IF NOT EXIST cache_files (
  MD cache_files
)
ECHO timestamp > cache_files\ashkal.txt
ENDLOCAL
EXIT /B !EXIT_STATUS!

:DownloadAndExtract
SET FOLDER=%~1
SET URL=%~2
SET BUILD_NEEDED=0
FOR /F "tokens=* delims=/" %%A IN ("%URL%") DO (
  SET ARCHIVE=%%~nxA
)
SET EXTENSION=%ARCHIVE:~-4%
IF EXIST !FOLDER! (
  EXIT /B 0
)
powershell -Command "$ProgressPreference = 'SilentlyContinue'; "^
  "Invoke-WebRequest -Uri '%URL%' -OutFile '%ARCHIVE%'"
IF ERRORLEVEL 1 (
  ECHO Error: Failed to download !ARCHIVE!.
  SET EXIT_STATUS=1
  EXIT /B
)
SET EXTRACT_PATH=_extract_tmp
RD /S /Q "!EXTRACT_PATH!" >NUL 2>NUL
MD "!EXTRACT_PATH!"
IF /I "!EXTENSION!"==".zip" (
  powershell -Command "$ProgressPreference = 'SilentlyContinue'; "^
    "Expand-Archive -Path '%ARCHIVE%' -DestinationPath '%EXTRACT_PATH%'"
) ELSE IF /I "!EXTENSION!"==".tgz" (
  powershell -Command "$ProgressPreference = 'SilentlyContinue'; "^
    "tar -xf '%ARCHIVE%' -C '%EXTRACT_PATH%'"
) ELSE IF /I "%ARCHIVE:~-7%"==".tar.gz" (
  powershell -Command "$ProgressPreference = 'SilentlyContinue'; "^
    "tar -xf '%ARCHIVE%' -C '%EXTRACT_PATH%'"
) ELSE (
  ECHO Error: Unknown archive format for %ARCHIVE%.
  SET EXIT_STATUS=1
  EXIT /B 1
)
SET DETECTED_FOLDER=
FOR %%F IN ("!EXTRACT_PATH!\*") DO (
  IF "!DETECTED_FOLDER!"=="" (
    SET DETECTED_FOLDER=%%F
  ) ELSE (
    SET DETECTED_FOLDER=MULTIPLE
  )
)
FOR /D %%F IN ("!EXTRACT_PATH!\*") DO (
  IF "!DETECTED_FOLDER!"=="" (
    SET DETECTED_FOLDER=%%F
  ) ELSE (
    SET DETECTED_FOLDER=MULTIPLE
  )
)
IF "!DETECTED_FOLDER!"=="MULTIPLE" (
  REN "!EXTRACT_PATH!" "!FOLDER!"
) ELSE IF NOT "!DETECTED_FOLDER!"=="!EXTRACT_PATH!\!FOLDER!" (
  MOVE /Y "!DETECTED_FOLDER!" "!FOLDER!" >NUL
) ELSE (
  MOVE /Y "!EXTRACT_PATH!\!FOLDER!" "!ROOT!" >NUL
)
RD /S /Q "!EXTRACT_PATH!"
IF ERRORLEVEL 1 (
  ECHO Error: Failed to extract !ARCHIVE!.
  SET EXIT_STATUS=1
  EXIT /B 0
)
SET BUILD_NEEDED=1
DEL /F /Q !ARCHIVE!
EXIT /B 0
