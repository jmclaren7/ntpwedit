PROJECT(libntpw C)

if(MINGW)
    find_path(OPENSSL_INCLUDE_DIR "openssl/des.h")
else(MINGW)
    find_package(OpenSSL REQUIRED)
endif(MINGW)
message(STATUS "OpenSSL INC: ${OPENSSL_INCLUDE_DIR}")

include_directories(${OPENSSL_INCLUDE_DIR})

ADD_LIBRARY(ntpw
    log.c
    ntpw.c
    ntreg.c
    )
