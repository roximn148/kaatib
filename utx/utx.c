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
#include <core/strings.h>
#include <core/heap.h>
#include <core/hfile.h>
#include <osbs/bfile.h>
#include <osbs/log.h>

/*----------------------------------------------------------------------------*/
static uint64_t counter = 1;

/*----------------------------------------------------------------------------*/
void utx_start(void) {}

/*----------------------------------------------------------------------------*/
void utx_finish(void) {}

/*----------------------------------------------------------------------------*/
UtxFile* utxCreateNew(void) {
    UtxFile *utx = heap_new0(UtxFile);

    utx->fileName = str_printf("Untitle%d.txt", counter);
    counter += 1;
    utx->contents = str_c("");
    utx->fileFolder = NULL;
    utx->isModified = FALSE;

    return utx;
}

/*----------------------------------------------------------------------------*/
UtxFile* utxCreateFromString(const String *contents) {
    if (contents == NULL) {
        return NULL;
    }

    UtxFile *utx = utxCreateNew();
    Result result = utxSetContents(utx, contents);
    if (result != ROkay) {
        log_printf("utxCreate: Failed to set contents [%d]", result);
        utxDestroy(&utx);
    }
    return utx;
}

/*----------------------------------------------------------------------------*/
UtxFile* utxCreateFromFile(const char_t *filePath) {
    if (filePath == NULL) {
        return NULL;
    }

    UtxFile *utx = utxCreateNew();
    Result result = utxReadContentsFromFile(utx, filePath);
    if (result != ROkay) {
        log_printf("utxCreate: Failed to read file [%s]", filePath);
        utxDestroy(&utx);
    }

    String *folder, *fileName;
    str_split_pathname(filePath, &folder, &fileName);
    str_cat(&folder, "/");
    str_upd(&utx->fileName, tc(fileName));
    str_upd(&utx->fileFolder, tc(folder));
    str_destroy(&folder);
    str_destroy(&fileName);

    utx->isModified = FALSE;

    log_printf("utxCreateFromFile: Created from '%s'", filePath);
    return utx;
}

/*----------------------------------------------------------------------------*/
void utxDestroy(UtxFile** utx) {
    if (utx == NULL) {
        return;
    }

    UtxFile *u = *utx;

    str_destroy(&u->fileName);
    str_destroy(&u->contents);
    if (u->fileFolder != NULL) {
        str_destroy(&u->fileFolder);
    }
    heap_delete(utx, UtxFile);
}

/*----------------------------------------------------------------------------*/
void utxDump(const UtxFile* utx) {
    if (utx == NULL) {
        log_printf("utxDump: null pointer provided.");
        return;
    }

    log_printf("utxDump: fileName: '%s'", tc(utx->fileName));
    log_printf("utxDump: fileFolder: '%s'", utx->fileFolder != NULL ? tc(utx->fileName) : "NULL");

    log_printf("utxDump: contents: %d bytes, %d chars", str_len(utx->contents), str_nchars(utx->contents));
    log_printf("utxDump: isModified: %s", utx->isModified ? "TRUE" : "FALSE");

    return;
}

/*----------------------------------------------------------------------------*/
Result utxSetContents(UtxFile* utx, const String* contents) {
    if (utx == NULL) {
        return RInvalidUtxPointer;
    }
    if (contents == NULL) {
        return RInvalidContents;
    }
    if (contents == utx->contents) {
        return RInvalidContents;
    }
    
    str_upd(&utx->contents, tc(contents));
    if (utx->fileFolder != NULL) {
        utx->isModified = TRUE;
    } else {
        utx->isModified = FALSE;
    }
    if (utx->fileFolder != NULL) {
        utx->isModified = TRUE;
    } else {
        utx->isModified = FALSE;
    }
    return ROkay;
}

/*----------------------------------------------------------------------------*/
uint32_t utxLength(const UtxFile* utx) {
    if (utx == NULL) {
        return 0;
    }
    return str_len(utx->contents);
}

/*----------------------------------------------------------------------------*/
Result utxReadContentsFromFile(UtxFile* utx, const char_t *filePath) {
    if (utx == NULL) {
        return RInvalidUtxPointer;
    }

    ferror_t error;
    String *contents = hfile_string(filePath, &error);
    if (error != ekFOK) {
        log_printf(
            "utxRead: Failed to read contents of '%s' with error %d",
            filePath,
            error
        );
        return RFileError;
    }

    str_upd(&(utx->contents), tc(contents));
    str_destroy(&contents);
    
    log_printf("utxRead: Successfully read contents of '%s'", filePath);
    return ROkay;
}

/*----------------------------------------------------------------------------*/
Result utxRead(UtxFile* utx, const char_t *filePath) {
    if (utx == NULL) {
        return RInvalidUtxPointer;
    }
    if (str_empty(utx->fileName)) {
        return RInvalidFilePath;
    }

    String *sFileFolder = NULL;
    if (filePath != NULL) {
        sFileFolder = str_c(filePath);
    } else if (utx->fileFolder != NULL) {
        sFileFolder = str_copy(utx->fileFolder);
    } else {
        const uint32_t PATH_SIZE = 512;
        sFileFolder = str_reserve(PATH_SIZE);

        uint32_t sz = bfile_dir_work(tcc(sFileFolder), PATH_SIZE);
        if (sz > PATH_SIZE) {
            str_destroy(&sFileFolder);
            log_printf("utxRead: Working directory path string exceeds string buffer size of %d", PATH_SIZE);
            return RInvalidFilePath;
        }
        str_cat_c(tcc(sFileFolder), PATH_SIZE, "/");
        log_printf("utxRead: using working directory '%s'", tc(sFileFolder));
    }

    String *sFilePath = str_cpath("%s%s", tc(sFileFolder), tc(utx->fileName));
    Result result = utxReadContentsFromFile(utx, tc(sFilePath));
    if (result == ROkay) {
        utx->isModified = FALSE;
        str_upd(&(utx->fileFolder), tc(sFileFolder));
    } else {
        str_destroy(&sFilePath);
    }
    str_destroy(&sFileFolder);
    
    return result;
}

/*----------------------------------------------------------------------------*/
Result utxWriteContentsToFile(UtxFile* utx, const char_t *filePath) {
    if (utx == NULL) {
        return RInvalidUtxPointer;
    }

    ferror_t error;
    hfile_from_string(filePath, utx->contents, &error);
    if (error != ekFOK) {
        log_printf(
            "utxRead: Failed to write to '%s' with error %d",
            filePath,
            error
        );
        return RFileError;
    }

    log_printf(
        "utxRead: Successfully wrote to '%s'",
        filePath
    );
    return ROkay;
}

/*----------------------------------------------------------------------------*/
Result utxWrite(UtxFile* utx, const char_t *fileFolder) {
    if (utx == NULL) {
        return 0;
    }
    if (str_empty(utx->fileName)) {
        return RInvalidFilePath;
    }

    String *sFileFolder = NULL;
    if (fileFolder != NULL) {
        sFileFolder = str_c(fileFolder);
    } else if (utx->fileFolder != NULL) {
        sFileFolder = str_copy(utx->fileFolder);
    } else {
        const uint32_t PATH_SIZE = 512;
        sFileFolder = str_reserve(PATH_SIZE);

        uint32_t sz = bfile_dir_work(tcc(sFileFolder), PATH_SIZE);
        if (sz > PATH_SIZE) {
            str_destroy(&sFileFolder);
            log_printf("utxWrite: Working directory path string exceeds string buffer size of %d", PATH_SIZE);
            return RInvalidFilePath;
        }
        str_cat_c(tcc(sFileFolder), PATH_SIZE, "/");
        log_printf("utxWrite: using working directory '%s'", tc(sFileFolder));
    }

    String *sFilePath = str_cpath("%s%s", tc(sFileFolder), tc(utx->fileName));
    Result result = utxWriteContentsToFile(utx, tc(sFilePath));
    if (result == ROkay) {
        utx->isModified = FALSE;
        str_upd(&(utx->fileFolder), tc(sFileFolder));
    } else {
        str_destroy(&sFilePath);
    }

    str_destroy(&sFileFolder);
    
    return result;
}

/*----------------------------------------------------------------------------*/
