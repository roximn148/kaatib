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
 * @file kaatib.c
 * @author roximn
 * @date 28 Dec 2024
 * @brief Kaatib application GUI window.
 * -------------------------------------------------------------------------- */
#include "kaatib.h"

/* -------------------------------------------------------------------------- */
static void onWindowClose(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("Closing main window");
    osapp_finish();
}

/* -------------------------------------------------------------------------- */
static void onDrawView(App *app, Event *e) {
    const EvDraw *p = event_params(e, EvDraw);
    draw_clear(p->ctx, kCOLOR_RED);
    draw_line_width(p->ctx, 10.f);
    draw_line_color(p->ctx, kCOLOR_GREEN);
    draw_rect(p->ctx, ekSTROKE, 0, 0, p->width, p->height);
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

    View *utv = view_create();
    view_OnDraw(utv, listener(app, onDrawView, App));
    app->ui.utv = utv;
    
    Panel *panel = panel_create();
    Layout *layout = layout_create(1, 2);
    
    layout_textview(layout, text, 0, 0);
    layout_view(layout, utv, 0, 1);

    layout_hsize(layout, 0, 800);
    layout_vsize(layout, 0, 200); 
    layout_vsize(layout, 1, 200); 
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
