/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
/** ----------------------------------------------------------------------------
 * @file main.c
 * @author roximn
 * @date 27 Dec 2024
 * @brief Kaatib application entry point.
 *
 * The kaatib application main entry point. Constructor and destructor 
 * function definitions.
 * -------------------------------------------------------------------------- */
#include "kaatib.h"
#include "menus.h"
#include "icons.h"

/** ----------------------------------------------------------------------------
 * Kaatib application constructor
 *
 * @return @c App* newly constructed application.
 * -------------------------------------------------------------------------- */
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

/** ----------------------------------------------------------------------------
 * Kaatib application destructor
 *
 * @param[in,out] app Address of App* to destroy and nullify.
 * 
 * @pre app != NULL
 * -------------------------------------------------------------------------- */
static void destroyApp(App **app) {
    cassert(app != NULL);

    utxDestroy(&(*app)->utx);
    window_destroy(&(*app)->ui.window);
    menu_destroy(&(*app)->ui.menu);
    heap_delete(app, App);
}

/* -------------------------------------------------------------------------- */
#include <osapp/osmain.h>
const char_t options[] = "";
osmain(createApp, destroyApp, options, App);  /**< NAppGui main */

/* -------------------------------------------------------------------------- */
