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

/* -------------------------------------------------------------------------- */
static void onWindowClose(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("Closing main window");
    osapp_finish();
}

/* -------------------------------------------------------------------------- */
static Panel *createCentralPanel(App *app) {
    TextView *text = textview_create();
    textview_family(text, "Calibri");
    textview_fsize(text, 24);
    textview_halign(text, ekRIGHT);
    textview_lspacing(text, 1.2);
    textview_editable(text, !app->isReadOnly);
    app->ui.textview = text;
    
    Panel *panel = panel_create();
    Layout *layout = layout_create(1, 1);
    layout_textview(layout, text, 0, 0);
    layout_hsize(layout, 0, 800);
    layout_vsize(layout, 0, 450);
    layout_margin(layout, 2);
    panel_layout(panel, layout);

    return panel;
}



/* -------------------------------------------------------------------------- */
void createKaatibWindow(App *app) {
    Panel *panel = createCentralPanel(app);
    Window *window = window_create(ekWINDOW_STDRES);
    window_panel(window, panel);
    window_title(window, "Kaatib");
    window_OnClose(window, listener(app, onWindowClose, App));

    app->ui.window = window;
}

/* -------------------------------------------------------------------------- */
