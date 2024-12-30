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
typedef struct _utx_file UtxFile;
struct _utx_file {
    /* String* filePath; */
    String* fileFolder;
    String* fileName;
    String* contents;
    bool_t isModified;
};

#define FILE_BUFFER_SIZE 1048576

/*----------------------------------------------------------------------------*/
typedef enum result_t Result;
enum result_t {
    ROkay = 0,
    RInvalidUtxPointer,
    RInvalidContents,
    RInvalidFilePath,
    RFileError,
};

/*----------------------------------------------------------------------------*/
#endif /* __UTX_HXX__ */
/*----------------------------------------------------------------------------*/
