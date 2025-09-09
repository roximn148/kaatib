/*******************************************************************************
* Copyright (c) 2025. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
/** ----------------------------------------------------------------------------
 * @file ftx.h
 * @author roximn
 * @date 27 Mar 2025
 * @brief Chasm-e-Khat font engine header file.
 * -------------------------------------------------------------------------- */
#include <ft2build.h>
#include FT_FREETYPE_H

#include <draw2d/image.h>
#include <draw2d/color.h>

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern "C" {
#endif
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
typedef struct {
    FT_Library ftLibrary;
    FT_Face face;
} FontEngine;

/*----------------------------------------------------------------------------*/
int initFontEngine(FontEngine *fe);
int closeFontEngine(FontEngine *fe);

int loadFontFace(FontEngine *fe, const char *fontPath, unsigned int fontSize);
int closeFontFace(FontEngine *fe);

int renderGlyph(FontEngine *fe, unsigned int glyphId);

Image* ftBmp2ImageRGBA(const FT_Bitmap *ftBitmap, const color_t glyphColor);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*----------------------------------------------------------------------------*/
