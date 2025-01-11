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
static const real32_t CLeadingMargin = 100.f;
static const real32_t CTrailingMargin = 50.f;
static const real32_t CTopMargin = 50.f;
static const real32_t CBottomMargin = 20.f;
static const real32_t CLineSpacing = 50.f;

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
    const color_t CMarginRed = color_hsbf(0.f, 0.33f, 0.96f);
    const color_t CGrey = color_gray(224);

    const EvDraw *p = event_params(e, EvDraw);
    DCtx *ctx = p->ctx;
    real32_t w = p->width;
    real32_t h = p->height;

    T2Df t2d;
    t2d_movef(&t2d, kT2D_IDENTf, w-1, 0);
    t2d_scalef(&t2d, &t2d, -1.f, +1.f);
    draw_matrixf(ctx, &t2d);

    draw_clear(ctx, CPaperYellow);
    draw_antialias(ctx, FALSE);

    draw_line_width(ctx, 1);
    draw_line_color(ctx, CGrey);
    real32_t lineY = CTopMargin;
    while (lineY < h - CBottomMargin - 1) {
        draw_matrixf(ctx, &t2d);
        draw_line(ctx, 0.f, lineY, w-1, lineY);
        lineY += CLineSpacing;
    }

    draw_matrixf(ctx, kT2D_IDENTf);
    draw_text_align(p->ctx, ekRIGHT, ekBOTTOM);
    lineY = CTopMargin;
    uint32_t lineNum = 1;
    while (lineY < h - CBottomMargin - 1) {
        String *s = str_printf("%d", lineNum);
        real32_t tw, th;
        draw_text_extents(p->ctx,tc(s), 0, &tw, &th);
        draw_text_color(p->ctx, kCOLOR_BLACK);
        draw_text(p->ctx, tc(s), w - (CLeadingMargin / 2.f) + (tw / 2.f), lineY);
        str_destroy(&s);

        lineY += CLineSpacing;
        lineNum += 1;
    }

    draw_matrixf(ctx, &t2d);
    draw_line_width(ctx, 2);
    draw_line_color(ctx, CMarginRed);
    draw_line(ctx, CLeadingMargin, 0.f, CLeadingMargin, h-1);
    draw_line(ctx, w-CTrailingMargin, 0.f, w-CTrailingMargin, h-1);
}

/* -------------------------------------------------------------------------- */
static void onDrawOverlay(App *app, Event *e) {
    unref(app);
    const EvDraw *p = event_params(e, EvDraw);
    String *s = str_printf("w: %0.2f, h: %0.2f", p->width, p->height);
    real32_t w, h;
    draw_text_extents(p->ctx,tc(s), 0, &w, &h);
    draw_fill_color(p->ctx, kCOLOR_BLACK);
    draw_text_color(p->ctx, kCOLOR_WHITE);
    draw_rect(p->ctx, ekFILL, 5, 5, w + 4, h);
    draw_text(p->ctx, tc(s), 5 + 2, 5);
    str_destroy(&s);
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
    S2Df sz = {CLeadingMargin + CTrailingMargin, CTopMargin + CBottomMargin};
    view_size(utv, sz);
    view_OnDraw(utv, listener(app, onDrawView, App));
    view_OnOverlay(utv, listener(app, onDrawOverlay, App));
    app->ui.utv = utv;
    
    /* Layout *****************************************************************/
    Layout *layout = layout_create(1, 2);
    layout_margin(layout, 2);
    panel_layout(panel, layout);
    
    uint32_t row = 0;
    layout_textview(layout, text, 0, row);
    /* layout_hsize(layout, 0, 1);
    layout_vsize(layout, row, 1); */
    layout_show_row(layout, row, FALSE);

    row += 1;
    layout_view(layout, utv, 0, row);
    layout_hsize(layout, 0, 800);
    layout_vsize(layout, row, 200); 
    layout_vexpand(layout, row); 

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
