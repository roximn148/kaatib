# ******************************************************************************
# Copyright (c) 2024. All rights reserved.
#
# This work is licensed under the Creative Commons Attribution 4.0
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
#
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
NAP_DESKTOP_APP(kaatib "" NRC_EMBEDDED)

NAP_TARGET_C_STANDARD(kaatib 11)
NAP_TARGET_CXX_STANDARD(kaatib 14)

set(SOURCES
    main.c
    kaatib.c
    menus.c
)

set(HEADERS
    kaatib.h
    menus.h
)

SET(RAQM_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
SET(RAQM_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

TARGET_SOURCES(kaatib PRIVATE ${SOURCES} ${HEADERS})
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/utx
    ${RAQM_INCLUDE_DIR}
)
TARGET_LINK_DIRECTORIES(kaatib PRIVATE ${RAQM_LIB_DIR})
SET_TARGET_PROPERTIES(kaatib PROPERTIES OUTPUT_NAME "kaatib")
TARGET_LINK_LIBRARIES(kaatib
    utx
    ${NAPPGUI_LIBRARIES}
    Ws2_32
    raqm
    fribidi
    harfbuzz harfbuzz-subset
    freetype libpng16 brotlicommon brotlidec brotlienc zlib bz2
)
