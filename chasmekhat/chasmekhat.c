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
 * @file chasmekhat.c
 * @author roximn
 * @date 20 Mar 2025
 * @brief Chasm-e-Khat is a font glyph viewer application.
 * -------------------------------------------------------------------------- */
#include <nappgui.h>

typedef struct _app_t App;

struct _app_t {
    Window *window;
    Panel *panel;
    Menu *menu;
    TableView *table;
    Edit *edit1;
    View *view;
    Label *cells_label;
    uint32_t col_id;
    uint32_t row_id;
    uint32_t margin;
    uint32_t mouse_cell_x;
    uint32_t mouse_cell_y;
    uint32_t sel_cell_x;
    uint32_t sel_cell_y;
    Layout *main_layout;
    Layout *middle_layout;
    Layout *control_layout;
    Layout *info_layout;
    char_t temptxt[256];
    Font *fullfont;
    color_t drawcolor;
    color_t backcolor;
};

static const uint32_t i_NUM_COLS = 32;
static const uint32_t i_NUM_ROWS = 1024;
static const real32_t i_CELL_SIZE = 50;
static const char_t *i_CELLS_INFO = "Draw cells: [%d, %d] x [%d, %d]";

/*----------------------------------------------------------------------------*/
static void setViewContentSize(App *app) {
    real32_t width = i_NUM_COLS * i_CELL_SIZE + (i_NUM_COLS + 1) * app->margin;
    real32_t height = i_NUM_ROWS * i_CELL_SIZE + (i_NUM_ROWS + 1) * app->margin;
    view_content_size(app->view, s2df((real32_t)width, (real32_t)height), s2df(10, 10));
}

/*----------------------------------------------------------------------------*/
static void scrollToCell(App *app) {
    real32_t xpos = app->col_id * i_CELL_SIZE + (app->col_id + 1) * app->margin;
    real32_t ypos = app->row_id * i_CELL_SIZE + (app->row_id + 1) * app->margin;
    xpos -= 5;
    ypos -= 5;
    view_scroll_x(app->view, xpos);
    view_scroll_y(app->view, ypos);
}

/*----------------------------------------------------------------------------*/
static void i_draw_clipped(App *app, DCtx *ctx,
    const real32_t x, const real32_t y,
    const real32_t width, const real32_t height) {
    uint32_t sti, edi;
    uint32_t stj, edj;
    real32_t cellsize = i_CELL_SIZE + (real32_t)app->margin;
    real32_t hcell = i_CELL_SIZE / 2;
    real32_t posx = 0;
    real32_t posy = 0;
    uint32_t i, j;

    /* Calculate the visible cols */
    sti = (uint32_t)bmath_floorf(x / cellsize);
    edi = sti + (uint32_t)bmath_ceilf(width / cellsize) + 1;
    if (edi > i_NUM_COLS)
        edi = i_NUM_COLS;

    /* Calculate the visible rows */
    stj = (uint32_t)bmath_floorf(y / cellsize);
    edj = stj + (uint32_t)bmath_ceilf(height / cellsize) + 1;
    if (edj > i_NUM_ROWS)
        edj = i_NUM_ROWS;

    posy = (real32_t)app->margin + stj * cellsize;

    {
        char_t text[256];
        bstd_sprintf(text, sizeof(text), i_CELLS_INFO, sti, stj, edi, edj);
        label_text(app->cells_label, text);
    }

    draw_fill_color(ctx, color_gray(240));
    draw_rect(ctx, ekFILL, x, y, width, height);
    draw_fill_color(ctx, color_gray(200));
    draw_line_color(ctx, kCOLOR_BLUE);
    draw_line_width(ctx, 1);
    draw_text_align(ctx, ekCENTER, ekCENTER);
    draw_text_halign(ctx, ekCENTER);

    for (j = stj; j < edj; ++j)     {
        posx = (real32_t)app->margin + sti * cellsize;
        for (i = sti; i < edi; ++i) {
            char_t text[128];
            bool_t special_cell = FALSE;

            bstd_sprintf(text, sizeof(text), "%d\n%d", i, j);

            if (app->sel_cell_x == i && app->sel_cell_y == j) {
                draw_line_width(ctx, 6);
                draw_line_color(ctx, kCOLOR_RED);

                special_cell = TRUE;
            } else if (app->mouse_cell_x == i && app->mouse_cell_y == j) {
                draw_line_width(ctx, 3);
                draw_line_color(ctx, kCOLOR_BLUE);
                special_cell = TRUE;
            }

            draw_rect(ctx, ekSKFILL, posx, posy, i_CELL_SIZE, i_CELL_SIZE);
            draw_text(ctx, text, posx + hcell, posy + hcell);

            if (special_cell == TRUE) {
                draw_line_width(ctx, 1);
                draw_line_color(ctx, kCOLOR_BLUE);
            }

            posx += cellsize;
        }

        posy += cellsize;
    }
}

/*----------------------------------------------------------------------------*/
static void i_OnDraw(App *app, Event *e) {
    const EvDraw *p = event_params(e, EvDraw);
    i_draw_clipped(app, p->ctx, p->x, p->y, p->width, p->height);
}

/*----------------------------------------------------------------------------*/
static void i_mouse_cell(App *app,
                         const real32_t x, const real32_t y,
                         const uint32_t action) {
    real32_t cellsize = i_CELL_SIZE + (real32_t)app->margin;
    uint32_t mx = (uint32_t)bmath_floorf(x / cellsize);
    uint32_t my = (uint32_t)bmath_floorf(y / cellsize);
    real32_t xmin = mx * cellsize + (real32_t)app->margin;
    real32_t xmax = xmin + i_CELL_SIZE;
    real32_t ymin = my * cellsize + (real32_t)app->margin;
    real32_t ymax = ymin + i_CELL_SIZE;

    if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
        if (action == 0) {
            app->mouse_cell_x = mx;
            app->mouse_cell_y = my;
        } else {
            app->sel_cell_x = mx;
            app->sel_cell_y = my;
        }
    } else {
        app->mouse_cell_x = UINT32_MAX;
        app->mouse_cell_y = UINT32_MAX;
    }

    view_update(app->view);
}

/*----------------------------------------------------------------------------*/
static void i_OnMove(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    i_mouse_cell(app, p->x, p->y, 0);
}

/*----------------------------------------------------------------------------*/
static void i_OnUp(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    i_mouse_cell(app, p->x, p->y, 0);
}

/*----------------------------------------------------------------------------*/
static void i_OnDown(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    i_mouse_cell(app, p->x, p->y, 1);
}

/*----------------------------------------------------------------------------*/
static void i_OnKeyDown(App *app, Event *e) {
    const EvKey *p = event_params(e, EvKey);
    View *view = event_sender(e, View);
    real32_t margin = (real32_t)app->margin;
    real32_t cellsize = i_CELL_SIZE + margin;
    V2Df scroll;
    S2Df size;

    view_viewport(view, &scroll, &size);

    if (p->key == ekKEY_DOWN && app->sel_cell_y < i_NUM_ROWS - 1) {
        real32_t ymin = (app->sel_cell_y + 1) * cellsize + margin;
        ymin += i_CELL_SIZE;

        if (scroll.y + size.height <= ymin) {
            view_scroll_y(view, ymin - size.height + margin);
            app->mouse_cell_x = UINT32_MAX;
            app->mouse_cell_y = UINT32_MAX;
        }

        app->sel_cell_y += 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_UP && app->sel_cell_y > 0) {
        real32_t ymin = (app->sel_cell_y - 1) * cellsize + (real32_t)app->margin;

        if (scroll.y >= ymin) {
            view_scroll_y(view, ymin - margin);
            app->mouse_cell_x = UINT32_MAX;
            app->mouse_cell_y = UINT32_MAX;
        }

        app->sel_cell_y -= 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_RIGHT && app->sel_cell_x < i_NUM_COLS - 1) {
        real32_t xmin = (app->sel_cell_x + 1) * cellsize + margin;
        xmin += i_CELL_SIZE;

        if (scroll.x + size.width <= xmin) {
            view_scroll_x(view, xmin - size.width + margin);
            app->mouse_cell_x = UINT32_MAX;
            app->mouse_cell_y = UINT32_MAX;
        }

        app->sel_cell_x += 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_LEFT && app->sel_cell_x > 0) {
        real32_t xmin = (app->sel_cell_x - 1) * cellsize + (real32_t)app->margin;

        if (scroll.x >= xmin) {
            view_scroll_x(view, xmin - margin);
            app->mouse_cell_x = UINT32_MAX;
            app->mouse_cell_y = UINT32_MAX;
        }

        app->sel_cell_x -= 1;
        view_update(app->view);
    }

}

/*----------------------------------------------------------------------------*/
static uint32_t i_listbox_sel(ListBox *list) {
    uint32_t i, n = listbox_count(list);
    for (i = 0; i < n; ++i) {
        if (listbox_selected(list, i) == TRUE)
            return i;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

static void i_OnColored(App *app, Event *e) {
    const EvButton *p = event_params(e, EvButton);
    if (p->state == ekGUI_ON) {
        layout_bgcolor(app->main_layout, color_rgb(128, 0, 0));
        layout_bgcolor(app->middle_layout, color_rgb(0, 128, 0));
        layout_bgcolor(app->control_layout, color_rgb(0, 0, 128));
        layout_bgcolor(app->info_layout, color_rgb(128, 128, 0));
        osapp_menubar(app->menu, app->window);
    } else {
        layout_bgcolor(app->main_layout, kCOLOR_DEFAULT);
        layout_bgcolor(app->middle_layout, kCOLOR_DEFAULT);
        layout_bgcolor(app->control_layout, kCOLOR_DEFAULT);
        layout_bgcolor(app->info_layout, kCOLOR_DEFAULT);
        osapp_menubar(NULL, app->window);
    }

    panel_update(app->panel);
}

/*----------------------------------------------------------------------------*/
static Layout *createControlLayout(App *app) {
    Layout *layout = layout_create(10, 1);
    
    Label *lblGoto = label_create();
    label_text(lblGoto, "Goto Glyph:");
    layout_label(layout, lblGoto, 0, 0);

    Edit *ebxGlyphId = edit_create();
    edit_align(ebxGlyphId, ekRIGHT);
    layout_edit(layout, ebxGlyphId, 1, 0);
    
    Label *lblGlyphSize = label_create();
    label_text(lblGlyphSize, "Size:");
    layout_label(layout, lblGlyphSize, 2, 0);

    Slider *slider = slider_create();
    layout_slider(layout, slider, 3, 0);

    /* Force the width of editbox columns */
    layout_hsize(layout, 1, 50);

    /* Horizontal margins between controls */
    layout_hmargin(layout, 0, 5);
    layout_hmargin(layout, 1, 5);
    layout_hmargin(layout, 2, 5);
    layout_hmargin(layout, 3, 5);

    app->edit1 = ebxGlyphId;
    app->control_layout = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createInfoLayout(App *app) {
    Layout *layout = layout_create(4, 1);
    Label *label2 = label_create();
    char_t text[256];

    label_align(label2, ekRIGHT);

    layout_label(layout, label2, 3, 0);

    /* All the horizontal expansion will be done in empty column-cell(2) */
    layout_hexpand(layout, 2);

    /* Keep the labels for futher updates */
    app->cells_label = label2;

    /* Text for labels dimensioning */
    bstd_sprintf(text, sizeof(text), i_CELLS_INFO, 1000, 1000, 1000, 1000);
    label_size_text(app->cells_label, text);
    app->info_layout = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static void getTableData(App *app, Event *e) {
    uint32_t etype = event_type(e);

    switch (etype) {
        case ekGUI_EVENT_TBL_NROWS: {
            uint32_t *n = event_result(e, uint32_t);
            *n = 20;
            break;
        }

        case ekGUI_EVENT_TBL_CELL: {
            const EvTbPos *pos = event_params(e, EvTbPos);
            EvTbCell *cell = event_result(e, EvTbCell);
            bstd_sprintf(app->temptxt, sizeof(app->temptxt), "Name %d", pos->row);
            cell->text = app->temptxt;
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/
static Layout *createTableLayout(App *app) {
    Layout *layout = layout_create(1, 1);
    TableView *table = tableview_create();
    tableview_new_column_text(table);
    tableview_size(table, s2df(150, 200));
    tableview_column_width(table, 0, 120);
    tableview_OnData(table, listener(app, getTableData, App));
    tableview_update(table);
    layout_tableview(layout, table, 0, 0);
    app->table = table;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createMiddleLayout(App *app) {
    Layout *layout = layout_create(2, 1);
    Layout *layout3 = createTableLayout(app);
    View *view = view_scroll();
    view_size(view, s2df(450, 200));
    view_OnDraw(view, listener(app, i_OnDraw, App));
    view_OnMove(view, listener(app, i_OnMove, App));
    view_OnUp(view, listener(app, i_OnUp, App));
    view_OnDown(view, listener(app, i_OnDown, App));
    view_OnKeyDown(view, listener(app, i_OnKeyDown, App));

    layout_view(layout, view, 0, 0);
    layout_layout(layout, layout3, 1, 0);

    /* Add the view to tabstop list */
    layout_tabstop(layout, 0, 0, TRUE);

    /* A small horizontal margin between view cell and list (left) table (right) layouts */
    layout_hmargin(layout, 0, 3);

    /* All the horizontal expansion will be done in the middle cell (view)
       list_layout (left) and table_layout (right) will preserve the 'natural' width */
    layout_hexpand(layout, 0);
    app->view = view;
    app->middle_layout = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createMultiLayout(App *app) {
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = createControlLayout(app);
    Layout *layout3 = createMiddleLayout(app);
    Layout *layout4 = createInfoLayout(app);
    layout_layout(layout1, layout2, 0, 0);
    layout_layout(layout1, layout3, 0, 1);
    layout_layout(layout1, layout4, 0, 2);

    /* All the vertical expansion will be done in the middle layout
       control_layout (top) and info_layout (bottom) will preserve the 'natural' height */
    layout_vexpand(layout1, 1);

    /* A vertical margins between middle and (controls, info) */
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);

    /* A border margin for all layout edges */
    layout_margin(layout1, 5);

    return layout1;
}

/*----------------------------------------------------------------------------*/
static Panel *createCentralPanel(App *app) {
    Panel *panel = panel_create();
    Layout *layout1 = createMultiLayout(app);
    panel_layout(panel, layout1);
    app->main_layout = layout1;
    return panel;
}

/*----------------------------------------------------------------------------*/
static void onMovedEvent(App *app, Event *e) {
    const EvPos *p = event_params(e, EvPos);
    bstd_printf("Window moved: (%d, %d)\n", (uint32_t)p->x, (uint32_t)p->y);
    unref(app);
}

/*----------------------------------------------------------------------------*/
static void onCloseWEvent(App *app, Event *e) {
    osapp_finish();
    unref(app);
    unref(e);
}

/*----------------------------------------------------------------------------*/
/* This code is added for testing big menubars in resizable windows */
static Menu *createMenubar(void) {
    Menu *menu = menu_create();
    uint32_t i, n = 5;
    for (i = 0; i < n; ++i) {
        char_t text[32];
        MenuItem *item = menuitem_create();
        bstd_sprintf(text, sizeof(text), "ItemName%d", i);
        menuitem_text(item, text);
        menu_add_item(menu, item);
    }

    return menu;
}

/*----------------------------------------------------------------------------*/
static App *createApp(void) {
    App *app = heap_new0(App);
    Panel *panel = NULL;
    app->col_id = 10;
    app->row_id = 10;
    app->margin = 10;
    app->mouse_cell_x = UINT32_MAX;
    app->mouse_cell_y = UINT32_MAX;
    app->sel_cell_x = app->col_id;
    app->sel_cell_y = app->row_id;
    panel = createCentralPanel(app);
    app->window = window_create(ekWINDOW_STDRES);
    app->panel = panel;
    app->menu = createMenubar();
    
    app->fullfont = font_system(40, 0);
    log_printf("Using font %s\n", font_family(app->fullfont));
    app->drawcolor = gui_alt_color(color_rgb(80, 80, 240), color_rgb(240, 240, 80));
    app->backcolor = gui_alt_color(color_rgb(200, 240, 200), color_rgb(80, 128, 80));
    setViewContentSize(app);
    window_panel(app->window, panel);
    window_title(app->window, "Chasm-e-Khat");
    window_origin(app->window, v2df(500, 200));
    window_OnMoved(app->window, listener(app, onMovedEvent, App));
    window_OnClose(app->window, listener(app, onCloseWEvent, App));

    osapp_menubar(app->menu, app->window);
    window_show(app->window);
    scrollToCell(app);
    return app;
}

/*----------------------------------------------------------------------------*/
static void destroyApp(App **app) {
    menu_destroy(&(*app)->menu);
    window_destroy(&(*app)->window);
    font_destroy(&(*app)->fullfont);
    heap_delete(app, App);
}

/*----------------------------------------------------------------------------*/
static void updateApp(App *app, const real64_t prtime, const real64_t ctime) {
    unref(prtime);
    unref(ctime);
    unref(app);
}

/*----------------------------------------------------------------------------*/
#include <osapp/osmain.h>
osmain_sync(0.1, createApp, destroyApp, updateApp, "", App)

/*----------------------------------------------------------------------------*/
