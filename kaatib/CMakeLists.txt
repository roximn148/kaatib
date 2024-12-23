# ******************************************************************************
# Copyright (c) 2024. All rights reserved.
# 
# This work is licensed under the Creative Commons Attribution 4.0 
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
# 
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
NAP_DESKTOP_APP(Kaatib "" NRC_EMBEDDED)

target_sources(Kaatib PRIVATE main.c kaatib.c menus.c)

NAP_TARGET_C_STANDARD(Kaatib 11)
NAP_TARGET_CXX_STANDARD(Kaatib 14)
