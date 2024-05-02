set BUILD_TYPE=Release
set CMAKE=cmake.exe -G "MinGW Makefiles"
set BUILD_DIR=build

pushd openssl-*
if errorlevel 1 goto nossl
  if exist out\libcrypto.a goto ssl
    perl Configure mingw no-asm || goto :EOF
    perl util\mkfiles.pl >MINFO || goto :EOF
    perl util\mk1mf.pl no-asm Mingw32 >ms\mingw64.mak || goto :EOF
    perl -i.bak -p -e "s/-march=i486//;s/(err_all|e_4758cca|e_chil|e_sureware|t1_lib|\\ca|s_server|apps)\.o\s+(\$\(LIB_CFLAGS\)|-DMONOLITH)/\1.o -D__WINCRYPT_H__ \2/;" ms/mingw64.mak || goto :EOF
    perl -i.bak -p -e "s/static type _hide_##name/type _hide_##name/" e_os2.h || goto :EOF
    perl -i.bak -p -e "s{^(\s*typedef int pid_t;)}{/*\1*/}" engines/e_aep.c || goto :EOF
    mingw32-make -f ms/mingw64.mak || goto :EOF
:ssl
  set CMAKE_LIBRARY_PATH=%CD%\out
  set CMAKE_INCLUDE_PATH=%CD%\outinc
  popd
:nossl

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR% || goto :EOF

%CMAKE% -D CMAKE_BUILD_TYPE=%BUILD_TYPE% -D X64=1 ..
