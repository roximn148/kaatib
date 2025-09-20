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

#include <sewer/bmath.h>
#include <osbs/log.h>
#include <draw2d/pixbuf.h>

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
 * @param dpi Resolution of font glyph rendering.
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
    FT_Error error = FT_Done_Face(fe->face);
    if (error != 0) {
        log_printf("Error[%d]: Unable to close font face.", error);
        return -1; /* Error closing font */
    }
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
 * @param mode The glyph rendering mode, eg FT_RENDER_MODE_NORMAL or FT_RENDER_MODE_MONO.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int renderGlyph(FontEngine *fe, unsigned int glyphId, FT_Render_Mode mode) {
    if (fe == NULL || fe->ftLibrary == NULL || fe->face == NULL) {
        return -1;
    }
    FT_Error error = FT_Load_Glyph(fe->face, glyphId, FT_LOAD_DEFAULT);
    if (error) {
        log_printf("Error loading glyph %d, Face: %s", glyphId, fe->face->family_name);
        return -1; /* Error loading glyph */
    }
    error = FT_Render_Glyph(fe->face->glyph, mode);
    if (error) {
        log_printf("Error rendering glyph %d, Face: %s", glyphId, fe->face->family_name);
        return -1; /* Error rendering glyph */;
    }

    return 0; /* Success */
}

/** ----------------------------------------------------------------------------
 * @brief Gamma2.2 correction alpha map.
 *
 * Created with python script,
 *
 * @code{.py}
 * gamma22 = [round(pow(float(i) / 255.0, 1.0/2.2) * 255.0) for i in range(256)]
 * l = []
 * for i in range(0, 256, 16):
 *   l.append(", ".join([f"{gamma22[j]:3}" for j in range(i, i + 16)]))
 * print(",\n".join(l))
 * @endcode
 * -------------------------------------------------------------------------- */
uint8_t GammaCorrectedAlpha[] = {
   0,  21,  28,  34,  39,  43,  46,  50,  53,  56,  59,  61,  64,  66,  68,  70,
  72,  74,  76,  78,  80,  82,  84,  85,  87,  89,  90,  92,  93,  95,  96,  98,
  99, 101, 102, 103, 105, 106, 107, 109, 110, 111, 112, 114, 115, 116, 117, 118,
 119, 120, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 149, 150,
 151, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 160, 161, 162, 163,
 164, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175,
 175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186,
 186, 187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195, 195, 196,
 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203, 203, 204, 205, 205, 206,
 206, 207, 207, 208, 209, 209, 210, 210, 211, 212, 212, 213, 213, 214, 214, 215,
 215, 216, 217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 223, 223, 224,
 224, 225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232,
 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 240,
 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248,
 248, 249, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255
};

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
 * @param ftBitmap A pointer to FT_Bitmap struct containing the rendered glyph bitmap.
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
        if(ftBitmap->pixel_mode == FT_PIXEL_MODE_MONO) {
            uint32_t k = 0;
            for(uint32_t i = 0; i < ftBitmap->width; i++, rgbaMap += 4) {
                rgbaMap[0] = r;
                rgbaMap[1] = g;
                rgbaMap[2] = b;
                /* Copy source pixel value to destination alpha channel. */
                uint8_t shift = 7 - (i % 8);
                rgbaMap[3] = ((line[k] >> shift) & 0x01) ? 255 : 0;
                k += (shift != 0) ? 0 : 1;
            }
        } else {
            for (uint32_t i = 0; i < ftBitmap->width; i++, rgbaMap += 4) {
                rgbaMap[0] = r;
                rgbaMap[1] = g;
                rgbaMap[2] = b;
                /* Copy source pixel value to destination alpha channel.
                Assumes source GRAY8 format. */
                rgbaMap[3] = GammaCorrectedAlpha[line[i]];
                // rgbaMap[3] = line[i];
            }
        }
        line += ftBitmap->pitch; /* Next row in glyph bitmap buffer skipping any padding */
    }

    Image *img = image_from_pixbuf(destBuffer, NULL);
    pixbuf_destroy(&destBuffer);
    return img;
}

/*----------------------------------------------------------------------------*/
