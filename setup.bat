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
  wget https://github.com/onqtam/doctest/archive/2.4.6.zip
  IF !ERRORLEVEL! LEQ 0 (
    tar -xf 2.4.6.zip
  ) ELSE (
    SET EXIT_STATUS=1
  )
  DEL /F /Q 2.4.6.zip
)
IF NOT EXIST freetype-2.9.1 (
  wget https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz
  tar -xf freetype-2.9.1.tar.gz
  PUSHD freetype-2.9.1
  MKDIR build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\freetype-2.9.1\build ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
  DEL /F /Q freetype-2.9.1.tar.gz
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
  mkdir build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\OpenCL-SDK\build ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
)
IF NOT EXIST SDL2-2.0.16 (
  wget https://www.libsdl.org/release/SDL2-2.0.16.zip
  tar -xf SDL2-2.0.16.zip
  PUSHD SDL2-2.0.16
  mkdir build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\SDL2-2.0.16 ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  CP ..\SDL2Config.cmake .
  CP CMakeFiles\Export\cmake\* .
  POPD
  POPD
  DEL SDL2-2.0.16.zip
)
IF NOT EXIST SDL2_ttf-2.0.15 (
  wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.zip
  tar -xf SDL2_ttf-2.0.15.zip
  PUSHD SDL2_ttf-2.0.15
  sed -i "/^add_library/i include_directories(${FREETYPE_INCLUDE_DIRS})" CMakeLists.txt
  mkdir build
  PUSHD build
  cmake -DCMAKE_INSTALL_PREFIX=!ROOT!\SDL2_ttf-2.0.15\build -DSDL2_DIR=!ROOT!\SDL2-2.0.16\build -DFREETYPE_LIBRARY=!ROOT!\freetype-2.9.1\build\Release\ -DFREETYPE_INCLUDE_DIRS=!ROOT!\freetype-2.9.1\include\ ..
  cmake --build . --target INSTALL --config Debug
  cmake --build . --target INSTALL --config Release
  POPD
  POPD
  DEL SDL2_ttf-2.0.15.zip
)
IF "%NUMBER_OF_PROCESSORS%" == "" (
  SET BJAM_PROCESSORS=
) ELSE (
  SET BJAM_PROCESSORS="-j%NUMBER_OF_PROCESSORS%"
)
IF NOT EXIST boost_1_77_0 (
  wget https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.zip -O boost_1_77_0.zip
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
