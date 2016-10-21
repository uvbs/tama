cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -O -DANDROID_NDK -DFT2_BUILD_LIBRARY=1")

add_library( freetype2 STATIC
	../../../../Third_Party_lib/ftgles/freetype2_android/src/autofit/autofit.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/basepic.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftapi.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftbase.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftbbox.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftbitmap.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftdbgmem.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftdebug.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftglyph.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftinit.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftpic.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftstroke.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftsynth.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/base/ftsystem.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/cff/cff.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/pshinter/pshinter.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/psnames/psnames.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/raster/raster.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/sfnt/sfnt.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/smooth/smooth.c
	../../../../Third_Party_lib/ftgles/freetype2_android/src/truetype/truetype.c
            )

include_directories(
    ../../../../Third_Party_lib/ftgles/freetype2_android/include
    )

set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../distribution)

set_target_properties( freetype2
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY
                      "${distribution_DIR}/freetype2/lib/${ANDROID_ABI}")

# copy out lib header file...
#add_custom_command(TARGET freetype2 POST_BUILD
#                   COMMAND "${CMAKE_COMMAND}" -E
#                   copy "${CMAKE_CURRENT_SOURCE_DIR}/src/gmath.h"
#                   "${distribution_DIR}/gmath/include/gmath.h"
#                   **** the following 2 lines are for potential future debug purpose ****
#                   COMMAND "${CMAKE_COMMAND}" -E
#                   remove_directory "${CMAKE_CURRENT_BINARY_DIR}"
#                   COMMENT "Copying gmath to output directory")

