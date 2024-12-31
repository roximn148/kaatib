/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include "utx.hxx"

/*----------------------------------------------------------------------------*/
__EXTERN_C
/*----------------------------------------------------------------------------*/

_utx_api void utx_start(void);
_utx_api void utx_finish(void);

_utx_api UtxFile* utxCreateNew(void);
_utx_api UtxFile* utxCreateFromString(const String* contents);
_utx_api UtxFile* utxCreateFromFile(const char_t *filePath);
_utx_api void utxDestroy(UtxFile** utx);
_utx_api void utxDump(const UtxFile* utx);

_utx_api Result utxSetContents(UtxFile* utx, const String* contents);
_utx_api uint32_t utxLength(const UtxFile* utx);

_utx_api Result utxReadContentsFromFile(UtxFile* utx, const char_t *filePath);
_utx_api Result utxRead(UtxFile* utx, const char_t *filePath);
_utx_api Result utxWriteContentsToFile(UtxFile* utx, const char_t *filePath);
_utx_api Result utxWrite(UtxFile* utx, const char_t *filePath);

/*----------------------------------------------------------------------------*/
__END_C
/*----------------------------------------------------------------------------*/
