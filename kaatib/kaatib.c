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
    unref(app);
    const color_t CPaperYellow = color_hsbf(0.167f, 0.05f, 1.0f);
    const color_t CGrey = color_gray(224);
    static const real32_t CLeadingMargin = 100.f;
    static const real32_t CTrailingMargin = 50.f;
    static const real32_t CTopMargin = 50.f;
    static const real32_t CBottomMargin = 20.f;
    static const real32_t CLineSpacing = 50.f;

    const EvDraw *p = event_params(e, EvDraw);
    DCtx *ctx = p->ctx;
    real32_t w = p->width;
    real32_t h = p->height;

    T2Df t2d;
    t2d_movef(&t2d, kT2D_IDENTf, w-1, 0);
    t2d_scalef(&t2d, &t2d, -1.f, +1.f);
    draw_matrixf(ctx, &t2d);

    draw_clear(ctx, CPaperYellow);

    draw_line_width(ctx, 2);
    draw_line_color(ctx, CGrey);
    draw_line(ctx, CLeadingMargin, 0.f, CLeadingMargin, h-1);
    draw_line(ctx, w-CTrailingMargin, 0.f, w-CTrailingMargin, h-1);

    draw_line_width(ctx, 1);
    real32_t lineY = CTopMargin;
    while (lineY < h - CBottomMargin - 1) {
        draw_line(ctx, 0.f, lineY, w-1, lineY);
        lineY += CLineSpacing;
    }
}

/* -------------------------------------------------------------------------- */
static Panel *createCentralPanel(App *app) {
    Panel *panel = panel_create();

    /* Widgets ****************************************************************/
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
    
    /* Layout *****************************************************************/
    Layout *layout = layout_create(1, 2);
    layout_margin(layout, 2);
    layout_hsize(layout, 0, 800);
    panel_layout(panel, layout);
    
    uint32_t row = 0;
    layout_textview(layout, text, 0, row);
    layout_vsize(layout, row, 200); 

    row += 1;
    layout_view(layout, utv, 0, row);
    layout_vsize(layout, row, 200); 

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
