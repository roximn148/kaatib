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
 * @file ftx.c
 * @author roximn
 * @date 27 Mar 2025
 * @brief Chasm-e-Khat font engine implementation file.
 * -------------------------------------------------------------------------- */
#include "ftx.h"

#include <draw2d/pixbuf.h>
#include <osbs/log.h>

 /** ---------------------------------------------------------------------------
 * @brief Initialize the font engine.
 *
 * This function initializes the underlying FreeType library and sets up to use
 * as font engine.
 *
 * @param app Pointer to the FontEngine structure.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int initFontEngine(FontEngine *fe) {
    if (fe == NULL) {
        return -1;
    }
    FT_Error error = FT_Init_FreeType(&fe->ftLibrary);
    if (error) {
        return -1; /* Error initializing FreeType ftLibrary */
    }
    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Close the font engine and release resources.
 *
 * This function closes the underlying FreeType library and releases associated
 * resources.
 *
 * @param app Pointer to the FontEngine structure.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int closeFontEngine(FontEngine *fe) {
    if (fe == NULL || fe->ftLibrary == NULL) {
        return 0;
    }
    if (FT_Done_FreeType(fe->ftLibrary)) {
        return -1; /* Error closing FreeType ftLibrary */
    }
    fe->ftLibrary = NULL;
    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Load a font face from the specified file and set its size.
 *
 * This function loads a font face from the given font path and sets the desired
 * font size. Needs fontEngine to be properly initialized before loading a font
 * face.
 *
 * @param app Pointer to the FontEngine structure.
 * @param fontPath Path to the font file.
 * @param fontSize Size of the font in points (1 point = 1/72 inch).
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int loadFontFace(FontEngine *fe, const char *fontPath, unsigned int fontSize, unsigned int dpi) {
    if (fe == NULL || fe->ftLibrary == NULL) {
        return -1;
    }
    FT_Error error;

    error = FT_New_Face(fe->ftLibrary, fontPath, 0, &fe->face);
    if (error != 0) {
        log_printf("Error[%d]: Unable to load font face from %s", error, fontPath);
        return -1; /* Error loading font face */
    }
    error = FT_Set_Char_Size(fe->face, fontSize * 64, 0, dpi, 0);
    if (error != 0) {
        log_printf("Error[%d]: Unable to set font face %s to size %d", error, fe->face->family_name, fontSize);
        return -1; /* Error setting font size */
    }
    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Free the FontEngine font face and release resources.
 *
 * This function frees the associated FreeType face and releases all related
 * resources.
 *
 * @param fe Pointer to the FontEngine structure.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int closeFontFace(FontEngine *fe) {
    if (fe == NULL || fe->face == NULL) {
        return 0;
    }
    FT_Done_Face(fe->face);
    fe->face = NULL;
    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Render a glyph from the loaded font face.
 *
 * This function renders a specific glyph from the loaded font face using
 * FreeType.
 *
 * @param fe Pointer to the FontEngine structure.
 * @param glyphId The index of the glyph in the font to be rendered.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int renderGlyph(FontEngine *fe, unsigned int glyphId) {
    if (fe == NULL || fe->ftLibrary == NULL || fe->face == NULL) {
        return -1;
    }
    FT_Error error = FT_Load_Glyph(fe->face, glyphId, FT_LOAD_DEFAULT);
    if (error) {
        log_printf("Error loading glyph %d, Face: %s", glyphId, fe->face->family_name);
        return -1; /* Error loading glyph */
    }
    error = FT_Render_Glyph(fe->face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        log_printf("Error rendering glyph %d, Face: %s", glyphId, fe->face->family_name);
        return -1; /* Error rendering glyph */;
    }

    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Converts an FT_Bitmap GRAY8 to RGBA32 Image.
 *
 * This function takes an FT_Bitmap structure and converts it into a Image
 * containing the glyph data in RGBA32 format. The function assumes that the
 * input FT_Bitmap is in GRAY8 format. The color of the glyph can be
 * specified. The alpha channel of the resulting Image is populated from
 * the bitmap's pixel values, while the RGB channels of all the pixels in
 * the resulting Image are set to the specified glyph color. The caller
 * is responsible for  destroying the returned Image.
 *
 * @param gBmp A pointer to the FT_Bitmap representing the glyph bitmap.
 * @param glyphColor The color of the glyph in RGBA format.
 * @return A new Image containing the glyph data, or NULL if the input is invalid.
 *         The caller is responsible for destroying the returned Image.
 * -------------------------------------------------------------------------- */
Image* ftBmp2ImageRGBA(const FT_Bitmap *ftBitmap, const color_t glyphColor) {
    if (ftBitmap == NULL || ftBitmap->buffer == NULL) {
        return NULL;
    }

    /* Create a new RGBA32 pixbuf for the destination image */
    Pixbuf *destBuffer = pixbuf_create(ftBitmap->width, ftBitmap->rows, ekRGBA32);
    uint8_t *rgbaMap = pixbuf_data(destBuffer);

    uint8_t r, g, b;
    color_get_rgb(glyphColor, &r, &g, &b);

    /* Iterate over each row */
    byte_t *line = (uint8_t *)ftBitmap->buffer;
    for (uint32_t row = 0; row < ftBitmap->rows; row++) {
        /* Iterate over each pixel in the row */
        for (uint32_t j = 0; j < ftBitmap->width; j++) {
            rgbaMap[0] = r;
            rgbaMap[1] = g;
            rgbaMap[2] = b;
            /* Copy source pixel value to destination alpha channel.
            Assumes source GRAY8 format. */
            rgbaMap[3] = line[j];

            rgbaMap += 4; /* Next pixel in RGBA map */
        }
        line += ftBitmap->pitch; /* Next row in glyph bitmap buffer skipping any padding */
    }

    Image *img = image_from_pixbuf(destBuffer, NULL);
    pixbuf_destroy(&destBuffer);
    return img;
}

/*----------------------------------------------------------------------------*/
