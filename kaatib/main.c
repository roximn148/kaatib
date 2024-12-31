/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include "kaatib.h"
#include "menus.h"
#include "icons.h"

/* -------------------------------------------------------------------------- */
static App *createApp(void) {
    heap_verbose(TRUE);

    App *app = heap_new0(App);

    gui_respack(icons_respack);
    gui_language("");

    app->utx = utxCreateNew();
    app->isReadOnly = FALSE;

    createKaatibMenubar(app);
    createKaatibWindow(app);
    osapp_menubar(app->ui.menu, app->ui.window);
    window_origin(app->ui.window, v2df(100.f, 100.f));
    window_show(app->ui.window);

    return app;
}

/* -------------------------------------------------------------------------- */
static void destroyApp(App **app) {
    utxDestroy(&(*app)->utx);
    window_destroy(&(*app)->ui.window);
    menu_destroy(&(*app)->ui.menu);
    heap_delete(app, App);
}

/* -------------------------------------------------------------------------- */
#include <osapp/osmain.h>
const char_t options[] = "";
osmain(createApp, destroyApp, options, App)

/* -------------------------------------------------------------------------- */
