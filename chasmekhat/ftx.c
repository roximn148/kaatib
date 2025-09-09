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
int loadFontFace(FontEngine *fe, const char *fontPath, unsigned int fontSize) {
    if (fe == NULL || fe->ftLibrary == NULL) {
        return -1;
    }
    if (FT_New_Face(fe->ftLibrary, fontPath, 0, &fe->face)) {
        return -1; /* Error loading font face */
    }
    if (FT_Set_Char_Size(fe->face, fontSize * 64, 0, 300, 0)) {
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
 * @param glyphId The indexof the glyph in the font to be rendered.
 * @return int Returns 0 on success, -1 on failure.
 * -------------------------------------------------------------------------- */
int renderGlyph(FontEngine *fe, unsigned int glyphId) {
    if (fe == NULL || fe->ftLibrary == NULL || fe->face == NULL) {
        return -1;
    }
    FT_Error error = FT_Load_Glyph(fe->face, glyphId, FT_LOAD_DEFAULT);
    if (error) {
        return -1; /* Error loading glyph */
    }
    error = FT_Render_Glyph(fe->face->glyph, FT_RENDER_MODE_NORMAL);
    if (error)
      return -1; /* Error rendering glyph */;

    return 0; /* Success */
}

/*----------------------------------------------------------------------------*/
