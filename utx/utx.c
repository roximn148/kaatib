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


/** ----------------------------------------------------------------------------
 * @brief Creates a new UtxFile instance from scratch.
 *
 * This function creates a new UtxFile instance with default settings. A default
 * fileName is generated based on the counter value. fileFolder is set to NULL.
 *
 * @param None
 *
 * @return A pointer to the newly created UtxFile instance
 * -------------------------------------------------------------------------- */
UtxFile* utxCreateNew(void) {
    UtxFile *utx = heap_new0(UtxFile);

    utx->fileName = str_printf("Untitle%d.txt", counter);
    counter += 1;
    utx->contents = str_c("");
    utx->fileFolder = NULL;
    utx->isModified = TRUE;

    return utx;
}

/** ----------------------------------------------------------------------------
 * @brief Creates a new UtxFile instance from a given contents string.
 *
 * This function creates a new UtxFile instance with the specified contents.
 * Default fileName is generated with NULL fileFolder.
 *
 * @param[in] contents The contents of the new UtxFile
 *
 * @return A pointer to the newly created UtxFile instance
 * -------------------------------------------------------------------------- */
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

/** ----------------------------------------------------------------------------
 * @brief Creates a new UtxFile instance from a file path.
 *
 * This function creates a new UtxFile instance by reading its contents from
 * the specified file path. The fileName and fileFolder is extracted from the
 * given filePath.
 *
 * @param[in] filePath The file path to read the UtxFile contents from
 *
 * @return A pointer to the newly created UtxFile instance
 * -------------------------------------------------------------------------- */
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

/** ----------------------------------------------------------------------------
 * @brief Destroys a UtxFile instance and frees its resources.
 *
 * This function is called when closing or deleting a UtxFile instance and
 * should be used as a cleanup method.
 *
 * @param[in, out] utx A pointer to the UtxFile instance to destroy. The pointer
 * is set to NULL on return.
 * -------------------------------------------------------------------------- */
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

/** ----------------------------------------------------------------------------
 * @brief Dumps the contents of a UtxFile instance to the log.
 *
 * This function prints out various information about the UtxFile instance,
 * such as its fileName, fileFolder, contents length and modification flag.
 *
 * @param utx A pointer to the UtxFile instance to dump
 * -------------------------------------------------------------------------- */
void utxDump(const UtxFile* utx) {
    if (utx == NULL) {
        log_printf("utxDump: NULL pointer provided.");
        return;
    }

    log_printf("utxDump: fileName: '%s'", tc(utx->fileName));
    log_printf("utxDump: fileFolder: '%s'",
                utx->fileFolder != NULL ? tc(utx->fileName) : "NULL");

    log_printf("utxDump: contents: %d bytes, %d chars",
                str_len(utx->contents),
                str_nchars(utx->contents));
    log_printf("utxDump: isModified: %s", utx->isModified ? "TRUE" : "FALSE");

    return;
}

/** ----------------------------------------------------------------------------
 * @brief Processes the raw contents of a UtxFile instance.
 *
 * This function is called by the utxSetContents function and should be used to
 * handle any  processing of the UtxFile contents. It breaks down the given
 * string into paragraphs based on a list of paragraph separators.
 *
 * @param[in] utx A pointer to the UtxFile instance
 * @param[in] contents The contents string of the UtxFile instance
 *
 * @return An error code indicating success or failure
 * -------------------------------------------------------------------------- */
static Result processContents(UtxFile* utx, const String* contents) {
    unref(utx);
    unref(contents);
    return ROkay;
}

/** ----------------------------------------------------------------------------
 * @brief Sets the contents of a UtxFile instance.
 *
 * This function updates the file name and contents length of the UtxFile instance.
 *
 * @param utx A pointer to the UtxFile instance
 * @param contents The new contents string for the UtxFile instance
 *
 * @return An error code indicating success or failure
 * -------------------------------------------------------------------------- */
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
    utx->isModified = TRUE;
    return ROkay;
}

/** ----------------------------------------------------------------------------
 * @brief Returns the length of a UtxFile instance's contents.
 *
 * This function returns the number of bytes in the contents of the
 * UtxFile instance.
 *
 * @param[in] utx A pointer to the UtxFile instance
 *
 * @return The length of the UtxFile instance's contents
 * -------------------------------------------------------------------------- */
uint32_t utxLength(const UtxFile* utx) {
    if (utx == NULL) {
        return 0;
    }
    return str_len(utx->contents);
}

/** ----------------------------------------------------------------------------
 * @brief Reads the contents of a file into a UtxFile instance.
 *
 * This function reads the contents of a file specified by @p filePath and
 * stores it in the UtxFile instance pointed to by @p utx. The contents are
 * read as a null-terminated string, which is stored in the `contents` member
 * of the UtxFile instance. If an error occurs during reading, the function
 * returns an error code and logs an error message to the system log.
 *
 * @param[in] utx The UTX buffer where the file contents will be stored.
 * @param[in] filePath The path to the file whose contents should be read.
 *
 * @return ROkay if the operation was successful, or an error code otherwise.
 * -------------------------------------------------------------------------- */
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
/** ----------------------------------------------------------------------------
 * @brief Reads the contents of a file into a UtxFile instance.
 *
 * This function reads the contents of a file specified by @p filePath and
 * stores it in the UtxFile instance pointed to by @p utx. The contents are
 * read as a null-terminated string, which is stored in the `contents` member
 * of the UtxFile instance. If an error occurs during reading, the function
 * returns an error code and logs an error message to the system log.
 *
 * @param[in] utx The UTX buffer where the file contents will be stored.
 * @param[in] filePath The path to the file whose contents should be read.
 *
 * @return ROkay if the operation was successful, or an error code otherwise.
 * -------------------------------------------------------------------------- */
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
            log_printf("utxRead: Working directory path string exceeds string "
                       "buffer size of %d", PATH_SIZE);
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

/** ----------------------------------------------------------------------------
 * @brief Writes the contents of a UtxFile instance to a file.
 *
 * This function writes the contents of the given UtxFile instance to the
 * specified file. If an error occurs during writing, the function returns
 * an error code and logs an error message.
 *
 * @param[in] utx A pointer to the UtxFile instance containing the contents to
 * write.
 * @param[in] filePath The path to the file where the contents should be written.
 *
 * @return ROkay if the operation was successful, or an error code otherwise.
 * -------------------------------------------------------------------------- */
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

/** ----------------------------------------------------------------------------
 * @brief Writes the contents of a UtxFile instance to a file.
 *
 * This function writes the contents of the given UtxFile instance to the
 * specified file. If an error occurs during writing, the function returns
 * an error code and logs an error message. If no file folder is specified,
 * the working directory is used instead. The isModified flag is reset
 * to FALSE after the contents are written.
 *
 * @param[in] utx A pointer to the UtxFile instance containing the contents
 * to write.
 * @param[in] fileFolder The path to the directory where the file should be
 * written. If NULL, the working directory is used instead.
 *
 * @return ROkay if the operation was successful, or an error code otherwise.
 * -------------------------------------------------------------------------- */
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
            log_printf("utxWrite: Working directory path string exceeds string "
                       "buffer size of %d", PATH_SIZE);
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
