# ******************************************************************************
# Copyright (c) 2025. All rights reserved.
#
# This work is licensed under the Creative Commons Attribution 4.0
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
#
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************

# TEST Utx
ADD_EXECUTABLE(testUtx test_utx.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
TARGET_LINK_LIBRARIES(testUtx unity utx ${NAPPGUI_LIBRARIES} Ws2_32)

# TEST wcString
ADD_EXECUTABLE(testWcString test_wcString.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
TARGET_LINK_LIBRARIES(testWcString unity utx ${NAPPGUI_LIBRARIES} Ws2_32)

# TEST UtxUtils
ADD_EXECUTABLE(testUtxUtils test_utxUtils.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
TARGET_LINK_LIBRARIES(testUtxUtils unity utx ${NAPPGUI_LIBRARIES} Ws2_32)

# TEST GlyphCache
ADD_EXECUTABLE(testGlyphCache test_glyphCache.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
TARGET_LINK_LIBRARIES(testGlyphCache unity utx ${NAPPGUI_LIBRARIES} Ws2_32 gdiplus shlwapi)

FIND_PACKAGE(Freetype)
FIND_PACKAGE(harfbuzz)

# TEST Kaata
ADD_EXECUTABLE(testKaata test_kaata.c)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(harfbuzz)

SET(RAQM_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
SET(RAQM_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

INCLUDE_DIRECTORIES(${RAQM_INCLUDE_DIR})
TARGET_LINK_DIRECTORIES(testKaata PRIVATE ${RAQM_LIB_DIR})
TARGET_LINK_LIBRARIES(testKaata
    PRIVATE unity
    PRIVATE ${NAPPGUI_LIBRARIES} Ws2_32
    PRIVATE raqm
    PRIVATE fribidi
    PRIVATE harfbuzz::harfbuzz
    PRIVATE Freetype::Freetype
)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/NotoNaskhArabic-VariableFont_wght.ttf"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(testUtx testUtx)
ADD_TEST(testKaata testKaata)
ADD_TEST(testWcString testWcString)
ADD_TEST(testUtxUtils testUtxUtils)
ADD_TEST(testGlyphCache testGlyphCache)

# ******************************************************************************
