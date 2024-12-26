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
#include "icons.h"

/* -------------------------------------------------------------------------- */
static App *createApp(void) {
    App *app = heap_new0(App);

    gui_respack(icons_respack);
    gui_language("");

    app->menu = createKaatibMenubar(app);
    app->window = createKaatibWindow(app);
    osapp_menubar(app->menu, app->window);
    window_origin(app->window, v2df(100.f, 100.f));
    window_show(app->window);
    return app;
}

/* -------------------------------------------------------------------------- */
static void destroyApp(App **app) {
    window_destroy(&(*app)->window);
    menu_destroy(&(*app)->menu);
    heap_delete(app, App);
}

/* -------------------------------------------------------------------------- */
#include <osapp/osmain.h>
const char_t options[] = "";
osmain(createApp, destroyApp, options, App)

/* -------------------------------------------------------------------------- */
