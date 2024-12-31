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

TARGET_SOURCES(kaatib PRIVATE main.c kaatib.c menus.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
SET_TARGET_PROPERTIES(kaatib PROPERTIES OUTPUT_NAME "kaatib")
TARGET_LINK_LIBRARIES(kaatib utx)

NAP_TARGET_C_STANDARD(kaatib 11)
NAP_TARGET_CXX_STANDARD(kaatib 14)
