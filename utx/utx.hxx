/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#ifndef __UTX_HXX__
#define __UTX_HXX__

#include "utx.def"
#include <core/core.hxx>

/*----------------------------------------------------------------------------*/
struct _utx_file {
    String* filePath;
    String* contents;
    bool_t isModified;
};

/*----------------------------------------------------------------------------*/
#endif /* __UTX_HXX__ */
/*----------------------------------------------------------------------------*/
