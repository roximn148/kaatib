# ******************************************************************************
# Copyright (c) 2025. All rights reserved.
#
# This work is licensed under the Creative Commons Attribution 4.0
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
#
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
NAP_DESKTOP_APP(chasmekhat "" NRC_EMBEDDED)

NAP_TARGET_C_STANDARD(chasmekhat 11)
NAP_TARGET_CXX_STANDARD(chasmekhat 14)

SET(SOURCES
    chasmekhat.c
    ftx.c
)

SET(HEADERS
    ftx.h
)

SET(VCPKG_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
SET(VCPKG_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

TARGET_SOURCES(chasmekhat PRIVATE ${SOURCES} ${HEADERS})
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/utx
    ${VCPKG_INCLUDE_DIR}
)
TARGET_LINK_DIRECTORIES(chasmekhat PRIVATE ${VCPKG_LIB_DIR})

SET_TARGET_PROPERTIES(chasmekhat PROPERTIES OUTPUT_NAME "chasm-e-khat")

TARGET_LINK_LIBRARIES(chasmekhat
    utx
    ${NAPPGUI_LIBRARIES}
    Ws2_32
    freetype libpng16 brotlicommon brotlidec brotlienc zlib bz2
)
