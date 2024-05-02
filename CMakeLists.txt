cmake_minimum_required(VERSION 2.8.4)

project(NTPWEdit C RC)

if(MINGW)
    find_library(OPENSSL_LIBRARIES "crypto")
else()
    find_package(OpenSSL REQUIRED)
endif(MINGW)
message(STATUS "OpenSSL LIB: ${OPENSSL_LIBRARIES}")

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCC)

if(MSVC)
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(XLINK "/MANIFEST:NO")
endif(MSVC)

add_subdirectory(libdlg)
add_subdirectory(libntpw)

include_directories(res libdlg libntpw)

if(X64)
    set(RES resource64.rc)
else()
    set(RES resource.rc)
endif(X64)

add_executable(ntpwedit
    dlgabout.c
    dlgpass.c
    main.c
    message.c
    version.c
    ${RES}
    )
target_link_libraries(ntpwedit dlglib ntpw ${OPENSSL_LIBRARIES})
set_target_properties(ntpwedit PROPERTIES LINK_FLAGS "${CMAKE_CREATE_WIN32_EXE} ${XLINK}")
