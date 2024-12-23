# ******************************************************************************
# Copyright (c) 2024. All rights reserved.
# 
# This work is licensed under the Creative Commons Attribution 4.0 
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
# 
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Kaatib)

FIND_PACKAGE(nappgui REQUIRED)
INCLUDE("${NAPPGUI_ROOT_PATH}/prj/NAppProject.cmake")
INCLUDE("${NAPPGUI_ROOT_PATH}/prj/NAppCompilers.cmake")
NAP_CONFIG_COMPILER()

NAP_PROJECT_LIBRARY(utx utx)
NAP_PROJECT_DESKTOP_APP(Kaatib kaatib)

INCLUDE(CTest)
ADD_SUBDIRECTORY(unity)
ADD_SUBDIRECTORY(test)
