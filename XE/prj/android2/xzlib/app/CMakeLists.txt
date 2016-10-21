cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -O -DANDROID_NDK")

add_library( xzlib STATIC
../../../../Third_Party_lib/zip_support/ioapi.cpp
../../../../Third_Party_lib/zip_support/unzip.cpp
../../../../Third_Party_lib/zlib/adler32.c
../../../../Third_Party_lib/zlib/compress.c
../../../../Third_Party_lib/zlib/crc32.c
../../../../Third_Party_lib/zlib/deflate.c
../../../../Third_Party_lib/zlib/gzclose.c
../../../../Third_Party_lib/zlib/gzlib.c
../../../../Third_Party_lib/zlib/gzread.c
../../../../Third_Party_lib/zlib/gzwrite.c
../../../../Third_Party_lib/zlib/infback.c
../../../../Third_Party_lib/zlib/inffast.c
../../../../Third_Party_lib/zlib/inflate.c
../../../../Third_Party_lib/zlib/inftrees.c
../../../../Third_Party_lib/zlib/trees.c
../../../../Third_Party_lib/zlib/uncompr.c
../../../../Third_Party_lib/zlib/zutil.c
            )

include_directories(
    ../../../../Third_Party_lib/zip_support
    )

set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../distribution)

set_target_properties( xzlib
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY
                      "${distribution_DIR}/xzlib/lib/${ANDROID_ABI}")


