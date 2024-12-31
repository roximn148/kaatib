/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include <nappgui.h>
#include <utx.h>

/* -------------------------------------------------------------------------- */
typedef struct _app_t App;
struct _app_t {
    Window *window;
    Menu *menu;
    TextView *textview;
    UtxFile *utx;
};


/* -------------------------------------------------------------------------- */
Window *createKaatibWindow(App*);
Menu *createKaatibMenubar(App*);

MenuItem *createFileMenu(App*);
MenuItem *createEditMenu(App*);
MenuItem *createViewMenu(App*);
MenuItem *createHelpMenu(App*);

/* -------------------------------------------------------------------------- */
