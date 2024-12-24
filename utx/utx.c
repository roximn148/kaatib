/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include "utx.h"

/*----------------------------------------------------------------------------*/
void utx_start(void) {}

/*----------------------------------------------------------------------------*/
void utx_finish(void) {}

/*----------------------------------------------------------------------------*/
int utxCreate(const char_t *filePath) {
    unref(filePath);
    return 1;
}

/*----------------------------------------------------------------------------*/
