set BUILD_TYPE=Release
set CMAKE=cmake.exe -G "MinGW Makefiles"
set BUILD_DIR=build

pushd openssl-*
if errorlevel 1 goto nossl
  if exist out\libcrypto.a goto ssl
    call ms\mingw32.bat || goto :EOF
:ssl
  set CMAKE_LIBRARY_PATH=%CD%\out
  set CMAKE_INCLUDE_PATH=%CD%\outinc
  popd
:nossl

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR% || goto :EOF

%CMAKE% -D CMAKE_BUILD_TYPE=%BUILD_TYPE% -U X64 ..
