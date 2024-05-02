set BUILD_TYPE=Release
set CMAKE=cmake.exe -G "NMake Makefiles"
set BUILD_DIR=build

pushd openssl-*
if errorlevel 1 goto nossl
  if not exist out32\libeay32.lib (
    perl Configure VC-WIN32
    call ms\do_ms.bat
    nmake -f ms\nt.mak
  )
  set CMAKE_LIBRARY_PATH=%CD%\out32
  set CMAKE_INCLUDE_PATH=%CD%\inc32
  popd
:nossl

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR% || goto :EOF

%CMAKE% -D CMAKE_BUILD_TYPE=%BUILD_TYPE% -U X64 ..
