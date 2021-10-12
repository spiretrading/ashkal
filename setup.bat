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
IF NOT EXIST glew-2.1.0 (
  wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip/download -O glew-2.1.0.zip --no-check-certificate
  IF !ERRORLEVEL! LEQ 0 (
    unzip glew-2.1.0.zip
    PUSHD glew-2.1.0
    cd build
    cmake ./cmake
    cmake --build . --target glew_s --config Debug
    cmake --build . --target glew_s --config Release
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
  mkdir build
  PUSHD build
  cmake ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
IF NOT EXIST SDL2-2.0.16 (
  wget https://www.libsdl.org/release/SDL2-2.0.16.zip
  unzip SDL2-2.0.16
  PUSHD SDL2-2.0.16
  mkdir build
  PUSHD build
  cmake ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
  DEL SDL2-2.0.16.zip
)
IF "%NUMBER_OF_PROCESSORS%" == "" (
  SET BJAM_PROCESSORS=
) ELSE (
  SET BJAM_PROCESSORS="-j%NUMBER_OF_PROCESSORS%"
)
IF NOT EXIST boost_1_77_0 (
  wget https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.zip -O boost_1_77_0.zip --no-check-certificate
  IF !ERRORLEVEL! LEQ 0 (
    tar -xf boost_1_77_0.zip
    PUSHD boost_1_77_0
    PUSHD tools\build
    CALL bootstrap.bat vc142
    POPD
    tools\build\b2 !BJAM_PROCESSORS! --without-context --prefix="!ROOT!\boost_1_77_0" --build-type=complete address-model=64 toolset=msvc-14.2 link=static,shared runtime-link=shared install
    tools\build\b2 !BJAM_PROCESSORS! --with-context --prefix="!ROOT!\boost_1_77_0" --build-type=complete address-model=64 toolset=msvc-14.2 link=static runtime-link=shared install
    POPD
  ) ELSE (
    SET EXIT_STATUS=1
  )
  DEL /F /Q boost_1_77_0.zip
)
IF NOT EXIST cache_files (
  MD cache_files
)
ECHO timestamp > cache_files\ashkal.txt
ENDLOCAL
EXIT /B !EXIT_STATUS!
