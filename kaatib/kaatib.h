/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#ifndef __KAATIB_H__
#define __KAATIB_H__
/*----------------------------------------------------------------------------*/
#include <nappgui.h>
#include <utx.h>

/* -------------------------------------------------------------------------- */
typedef struct _app_t App;
struct _app_t {
    bool_t isReadOnly;
    UtxFile *utx;
    struct _ui_t {
        Window *window;
        Menu *menu;
        MenuItem *miNew;
        MenuItem *miOpen;
        MenuItem *miSave;
        MenuItem *miRevert;
        MenuItem *miRecent;

        MenuItem *miUndo;
        MenuItem *miRedo;
        MenuItem *miCopy;
        MenuItem *miCut;
        MenuItem *miPaste;
        MenuItem *miSelectAll;
        MenuItem *miReadOnly;

        MenuItem *miWhitespace;        
        MenuItem *miKeyboard;

        MenuItem *miAbout;

        TextView *textview;
    } ui;
};

/* -------------------------------------------------------------------------- */
void createKaatibWindow(App*);

/*----------------------------------------------------------------------------*/
# endif /* __KAATIB_H__ */
/*----------------------------------------------------------------------------*/
