cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -O -DANDROID_NDK")

add_library( lua STATIC

../../../../Third_Party_lib/lua514/src/lapi.c
../../../../Third_Party_lib/lua514/src/lauxlib.c
../../../../Third_Party_lib/lua514/src/lbaselib.c
../../../../Third_Party_lib/lua514/src/lcode.c
../../../../Third_Party_lib/lua514/src/ldblib.c
../../../../Third_Party_lib/lua514/src/ldebug.c
../../../../Third_Party_lib/lua514/src/ldo.c
../../../../Third_Party_lib/lua514/src/ldump.c
../../../../Third_Party_lib/lua514/src/lfunc.c
../../../../Third_Party_lib/lua514/src/lgc.c
../../../../Third_Party_lib/lua514/src/linit.c
../../../../Third_Party_lib/lua514/src/liolib.c
../../../../Third_Party_lib/lua514/src/llex.c
../../../../Third_Party_lib/lua514/src/lmathlib.c
../../../../Third_Party_lib/lua514/src/lmem.c
../../../../Third_Party_lib/lua514/src/loadlib.c
../../../../Third_Party_lib/lua514/src/lobject.c
../../../../Third_Party_lib/lua514/src/lopcodes.c
../../../../Third_Party_lib/lua514/src/loslib.c
../../../../Third_Party_lib/lua514/src/lparser.c
../../../../Third_Party_lib/lua514/src/lstate.c
../../../../Third_Party_lib/lua514/src/lstring.c
../../../../Third_Party_lib/lua514/src/lstrlib.c
../../../../Third_Party_lib/lua514/src/ltable.c
../../../../Third_Party_lib/lua514/src/ltablib.c
../../../../Third_Party_lib/lua514/src/ltm.c
../../../../Third_Party_lib/lua514/src/lundump.c
../../../../Third_Party_lib/lua514/src/lvm.c
../../../../Third_Party_lib/lua514/src/lzio.c
../../../../Third_Party_lib/lua514/src/print.c
)

include_directories(
    ../../../../Third_Party_lib/lua514/src
    )

set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../distribution)

set_target_properties( lua
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY
                      "${distribution_DIR}/lua/lib/${ANDROID_ABI}")


