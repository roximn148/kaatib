# ******************************************************************************
# Copyright (c) 2024. All rights reserved.
# 
# This work is licensed under the Creative Commons Attribution 4.0 
# International License. To view a copy of this license,
# visit # http://creativecommons.org/licenses/by/4.0/.
# 
# Author: roximn <roximn148@gmail.com>
# ******************************************************************************
ADD_EXECUTABLE(testUtx test_utx.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/utx)
TARGET_LINK_LIBRARIES(testUtx unity utx ${NAPPGUI_LIBRARIES} Ws2_32)

ADD_TEST(testUtx testUtx)
