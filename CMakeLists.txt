# ******************************************************************************
# Copyright (c) 2024. All rights reserved.
#
# This work is licensed under the Creative Commons Attribution 4.0
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
#
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

PROJECT(
    kaatib
    VERSION 0.1.0
    DESCRIPTION "Unicode Urdu text editor"
    LANGUAGES C CXX
)

# Update the submodules
include(cmake/UpdateSubmodules.cmake)

# NAppGUI
FIND_PACKAGE(nappgui REQUIRED)
INCLUDE("${NAPPGUI_ROOT_PATH}/prj/NAppProject.cmake")
INCLUDE("${NAPPGUI_ROOT_PATH}/prj/NAppCompilers.cmake")
INCLUDE_DIRECTORIES("${NAPPGUI_ROOT_PATH}/inc")

NAP_CONFIG_COMPILER()

# Kaatib project
NAP_PROJECT_LIBRARY(utx utx)
NAP_PROJECT_DESKTOP_APP(kaatib kaatib)

# Testing
include(CTest)
if(BUILD_TESTING)
    ADD_SUBDIRECTORY(Unity)
    ADD_SUBDIRECTORY(tests)
endif()
