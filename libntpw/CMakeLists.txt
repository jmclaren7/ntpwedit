PROJECT(libntpw C)

ADD_DEFINITIONS(
    -DLTC_NO_ASM
    -DLTC_NO_PROTOTYPES
    -D_CRT_SECURE_NO_WARNINGS
    -D_CRT_NONSTDC_NO_WARNINGS
    )

INCLUDE_DIRECTORIES(libtomcrypt/src/headers)

ADD_LIBRARY(ntpw
    libsam.c
    log.c
    ntpw.c
    ntreg.c
    libtomcrypt/src/ciphers/des.c
    libtomcrypt/src/hashes/md4.c
    libtomcrypt/src/misc/crypt/crypt_argchk.c
    )

IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long -Wno-unused-parameter")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
