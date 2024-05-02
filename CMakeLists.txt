cmake_minimum_required(VERSION 2.8.4)

project(NTPWEdit C RC)

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

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(EXE ntpwedit64)
    set(RES resource64.rc)
else()
    set(EXE ntpwedit)
    set(RES resource.rc)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

add_executable(${EXE}
    dlgabout.c
    dlghiber.c
    dlgpass.c
    main.c
    message.c
    version.c
    ${RES}
    )
target_link_libraries(${EXE} dlglib ntpw)
set_target_properties(${EXE} PROPERTIES LINK_FLAGS "${CMAKE_CREATE_WIN32_EXE} ${XLINK}")
