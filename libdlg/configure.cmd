set BUILD_TYPE=Release
set CMAKE=cmake -G "MinGW Makefiles"
set BUILD_DIR=build

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR% || goto :EOF

%CMAKE% -D CMAKE_BUILD_TYPE=%BUILD_TYPE% ..
