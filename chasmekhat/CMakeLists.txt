# ******************************************************************************
# Copyright (c) 2025. All rights reserved.
#
# This work is licensed under the Creative Commons Attribution 4.0
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
#
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************

nap_desktop_app(chasmekhat "" NRC_EMBEDDED)

nap_target_c_standard(chasmekhat 11)
nap_target_cxx_standard(chasmekhat 14)

set(SOURCES
    chasmekhat.c
)

set(HEADERS
    ""
)

SET(VCPKG_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
SET(VCPKG_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

TARGET_SOURCES(chasmekhat PRIVATE ${SOURCES} ${HEADERS})
INCLUDE_DIRECTORIES(
    ${VCPKG_INCLUDE_DIR}
)
TARGET_LINK_DIRECTORIES(chasmekhat PRIVATE ${VCPKG_LIB_DIR})

SET_TARGET_PROPERTIES(chasmekhat PROPERTIES OUTPUT_NAME "chasm-e-khat")

TARGET_LINK_LIBRARIES(chasmekhat
    ${NAPPGUI_LIBRARIES}
    Ws2_32
    freetype libpng16 brotlicommon brotlidec brotlienc zlib bz2
)
