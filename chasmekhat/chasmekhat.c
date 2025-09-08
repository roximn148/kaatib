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

/*----------------------------------------------------------------------------*/
typedef struct _app_t App;

struct _app_t {
    Window *window;
    Panel *panel;
    Menu *menu;
    TableView *table;
    Edit *edxGlyphId;
    View *view;
    Label *lblCellsInfo;
    uint32_t colIdx;
    uint32_t rowIdx;
    uint32_t margin;
    uint32_t mouseCellX;
    uint32_t mouseCellY;
    uint32_t selectedCellX;
    uint32_t selectedCellY;
    Layout *lyMain;
    Layout *lyMiddle;
    Layout *lyControls;
    Layout *lyInfo;
    char_t temptxt[256];
    Font *fullfont;
    color_t drawcolor;
    color_t backcolor;
};

static const uint32_t NUM_COLS = 32;
static const uint32_t NUM_ROWS = 1024;
static const real32_t CELL_SIZE = 50;
static const char_t *CELLS_INFO = "Draw cells: [%d, %d] x [%d, %d]";

/** ----------------------------------------------------------------------------
 * @brief Set the content size of the view based on the number of columns and rows.
 *
 * This function calculates the width and height of the view based on the specified
 * number of columns and rows, and sets the content size of the view accordingly.
 *
 * @param view The view to set the content size for.
 * @param margin The margin around the cells.
 * 
 * Global constants:
 * - NUM_COLS: The number of columns in the grid.
 * - NUM_ROWS: The number of rows in the grid.
 * - CELL_SIZE: The size of each cell in the grid.
 * -------------------------------------------------------------------------- */
static void setViewContentSize(View *view, uint32_t margin) {
    real32_t width = NUM_COLS * CELL_SIZE + (NUM_COLS + 1) * margin;
    real32_t height = NUM_ROWS * CELL_SIZE + (NUM_ROWS + 4) * margin;
    view_content_size(
        view,
        s2df((real32_t)width, (real32_t)height),
        s2df(CELL_SIZE + margin, CELL_SIZE + margin)  /* Scroll step */
    );
}

/** ----------------------------------------------------------------------------
 * Scrolls the view to the specified cell.
 *
 * @param view The view containing the cells.
 * @param col The column index of the target cell.
 * @param row The row index of the target cell.
 * @param margin The margin around the cells.
 * -------------------------------------------------------------------------- */
static void scrollToCell(View *view, uint32_t col, uint32_t row, uint32_t margin) {
    real32_t xpos = col * CELL_SIZE + (col + 1) * margin;
    real32_t ypos = row * CELL_SIZE + (row + 1) * margin;
    xpos -= 5; // Adjust for small offset
    ypos -= 5; // Adjust for small offset
    view_scroll_x(view, xpos);
    view_scroll_y(view, ypos);
}

/*----------------------------------------------------------------------------*/
static void drawClippedView(App *app, DCtx *ctx,
    const real32_t x, const real32_t y,
    const real32_t width, const real32_t height) {
    uint32_t sti, edi;
    uint32_t stj, edj;
    real32_t cellsize = CELL_SIZE + (real32_t)app->margin;
    real32_t hcell = CELL_SIZE / 2;
    real32_t posx = 0;
    real32_t posy = 0;
    uint32_t i, j;

    /* Calculate the visible cols */
    sti = (uint32_t)bmath_floorf(x / cellsize);
    edi = sti + (uint32_t)bmath_ceilf(width / cellsize) + 1;
    if (edi > NUM_COLS)
        edi = NUM_COLS;

    /* Calculate the visible rows */
    stj = (uint32_t)bmath_floorf(y / cellsize);
    edj = stj + (uint32_t)bmath_ceilf(height / cellsize) + 1;
    if (edj > NUM_ROWS)
        edj = NUM_ROWS;

    posy = (real32_t)app->margin + stj * cellsize;

    {
        char_t text[256];
        bstd_sprintf(text, sizeof(text), CELLS_INFO, sti, stj, edi, edj);
        label_text(app->lblCellsInfo, text);
    }

    draw_fill_color(ctx, color_gray(220));
    draw_rect(ctx, ekFILL, x, y, width, height);
    draw_fill_color(ctx, color_gray(180));
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

            if (app->selectedCellX == i && app->selectedCellY == j) {
                draw_line_width(ctx, 6);
                draw_line_color(ctx, kCOLOR_RED);

                special_cell = TRUE;
            } else if (app->mouseCellX == i && app->mouseCellY == j) {
                draw_line_width(ctx, 3);
                draw_line_color(ctx, kCOLOR_BLUE);
                special_cell = TRUE;
            }

            draw_rect(ctx, ekSKFILL, posx, posy, CELL_SIZE, CELL_SIZE);
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
static void onDrawView(App *app, Event *e) {
    const EvDraw *p = event_params(e, EvDraw);
    drawClippedView(app, p->ctx, p->x, p->y, p->width, p->height);
}

/*----------------------------------------------------------------------------*/
static void onMouseAction(App *app,
                         const real32_t x, const real32_t y,
                         const uint32_t action) {
    real32_t cellsize = CELL_SIZE + (real32_t)app->margin;
    uint32_t mx = (uint32_t)bmath_floorf(x / cellsize);
    uint32_t my = (uint32_t)bmath_floorf(y / cellsize);
    real32_t xmin = mx * cellsize + (real32_t)app->margin;
    real32_t xmax = xmin + CELL_SIZE;
    real32_t ymin = my * cellsize + (real32_t)app->margin;
    real32_t ymax = ymin + CELL_SIZE;

    if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
        if (action == 0) {
            app->mouseCellX = mx;
            app->mouseCellY = my;
        } else {
            app->selectedCellX = mx;
            app->selectedCellY = my;
        }
    } else {
        app->mouseCellX = UINT32_MAX;
        app->mouseCellY = UINT32_MAX;
    }

    view_update(app->view);
}

/*----------------------------------------------------------------------------*/
static void onMouseMove(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    onMouseAction(app, p->x, p->y, 0);
}

/*----------------------------------------------------------------------------*/
static void onMouseUp(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    onMouseAction(app, p->x, p->y, 0);
}

/*----------------------------------------------------------------------------*/
static void onMouseDown(App *app, Event *e) {
    const EvMouse *p = event_params(e, EvMouse);
    onMouseAction(app, p->x, p->y, 1);
}

/*----------------------------------------------------------------------------*/
static void onKeyDown(App *app, Event *e) {
    const EvKey *p = event_params(e, EvKey);
    View *view = event_sender(e, View);
    real32_t margin = (real32_t)app->margin;
    real32_t cellsize = CELL_SIZE + margin;
    V2Df scroll;
    S2Df size;

    view_viewport(view, &scroll, &size);

    if (p->key == ekKEY_DOWN && app->selectedCellY < NUM_ROWS - 1) {
        real32_t ymin = (app->selectedCellY + 1) * cellsize + margin;
        ymin += CELL_SIZE;

        if (scroll.y + size.height <= ymin) {
            view_scroll_y(view, ymin - size.height + margin);
            app->mouseCellX = UINT32_MAX;
            app->mouseCellY = UINT32_MAX;
        }

        app->selectedCellY += 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_UP && app->selectedCellY > 0) {
        real32_t ymin = (app->selectedCellY - 1) * cellsize + (real32_t)app->margin;

        if (scroll.y >= ymin) {
            view_scroll_y(view, ymin - margin);
            app->mouseCellX = UINT32_MAX;
            app->mouseCellY = UINT32_MAX;
        }

        app->selectedCellY -= 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_RIGHT && app->selectedCellX < NUM_COLS - 1) {
        real32_t xmin = (app->selectedCellX + 1) * cellsize + margin;
        xmin += CELL_SIZE;

        if (scroll.x + size.width <= xmin) {
            view_scroll_x(view, xmin - size.width + margin);
            app->mouseCellX = UINT32_MAX;
            app->mouseCellY = UINT32_MAX;
        }

        app->selectedCellX += 1;
        view_update(app->view);
    }

    if (p->key == ekKEY_LEFT && app->selectedCellX > 0) {
        real32_t xmin = (app->selectedCellX - 1) * cellsize + (real32_t)app->margin;

        if (scroll.x >= xmin) {
            view_scroll_x(view, xmin - margin);
            app->mouseCellX = UINT32_MAX;
            app->mouseCellY = UINT32_MAX;
        }

        app->selectedCellX -= 1;
        view_update(app->view);
    }

}

/*----------------------------------------------------------------------------*/
static Layout *createControlLayout(App *app) {
    Layout *layout = layout_create(5, 1);
    
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
    layout_hsize(layout, 1, 100);

    /* Horizontal margins between controls */
    layout_hmargin(layout, 0, 5);
    layout_hmargin(layout, 1, 5);
    layout_hmargin(layout, 2, 5);
    layout_hmargin(layout, 3, 5);

    /* All the horizontal expansion will be done in the last column */
    layout_hexpand(layout, 4);

    app->edxGlyphId = ebxGlyphId;
    app->lyControls = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createInfoLayout(App *app) {
    Layout *layout = layout_create(2, 1);
    Label *lblInfo = label_create();
    char_t text[256];

    label_align(lblInfo, ekRIGHT);

    layout_label(layout, lblInfo, 1, 0);

    /* All the horizontal expansion will be done in empty column-cell(2) */
    layout_hexpand(layout, 0);

    /* Keep the labels for further updates */
    app->lblCellsInfo = lblInfo;

    /* Text for labels dimensioning */
    bstd_sprintf(text, sizeof(text), CELLS_INFO, 1000, 1000, 1000, 1000);
    label_size_text(app->lblCellsInfo, text);
    app->lyInfo = layout;
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
    Layout *lyTable = createTableLayout(app);
    View *view = view_scroll();
    view_size(view, s2df(450, 200));
    view_OnDraw(view, listener(app, onDrawView, App));
    view_OnMove(view, listener(app, onMouseMove, App));
    view_OnUp(view, listener(app, onMouseUp, App));
    view_OnDown(view, listener(app, onMouseDown, App));
    view_OnKeyDown(view, listener(app, onKeyDown, App));

    layout_view(layout, view, 0, 0);
    layout_layout(layout, lyTable, 1, 0);

    /* Add the view to tabstop list */
    layout_tabstop(layout, 0, 0, TRUE);

    /* A small horizontal margin between view cell and list (left) table (right) layouts */
    layout_hmargin(layout, 0, 3);

    /* All the horizontal expansion will be done in the middle cell (view)
       list_layout (left) and table_layout (right) will preserve the 'natural' width */
    layout_hexpand(layout, 0);

    app->view = view;
    app->lyMiddle = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createMultiLayout(App *app) {
    Layout *layout = layout_create(1, 3);

    Layout *lyControl = createControlLayout(app);
    Layout *lyMiddle = createMiddleLayout(app);
    Layout *lyInfo = createInfoLayout(app);
    layout_layout(layout, lyControl, 0, 0);
    layout_layout(layout, lyMiddle, 0, 1);
    layout_layout(layout, lyInfo, 0, 2);

    /* All the vertical expansion will be done in the middle layout
       control_layout (top) and info_layout (bottom) will preserve the 'natural' height */
    layout_vexpand(layout, 1);

    /* A vertical margins between middle and (controls, info) */
    layout_vmargin(layout, 0, 5);
    layout_vmargin(layout, 1, 5);

    /* A border margin for all layout edges */
    layout_margin(layout, 5);

    return layout;
}

/*----------------------------------------------------------------------------*/
static Panel *createCentralPanel(App *app) {
    Panel *panel = panel_create();
    Layout *layout = createMultiLayout(app);
    panel_layout(panel, layout);
    app->lyMain = layout;
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
static Menu *createMenubar(void) {
    Menu *menu = menu_create();
    MenuItem *item;
    
    item = menuitem_create();
    menuitem_text(item, "File");
    menu_add_item(menu, item);

    item = menuitem_create();
    menuitem_text(item, "Help");
    menu_add_item(menu, item);

    return menu;
}

/*----------------------------------------------------------------------------*/
static App *createApp(void) {
    App *app = heap_new0(App);

    app->colIdx = 0;
    app->rowIdx = 0;
    app->margin = 5;
    app->mouseCellX = UINT32_MAX;
    app->mouseCellY = UINT32_MAX;
    app->selectedCellX = app->colIdx;
    app->selectedCellY = app->rowIdx;
    app->drawcolor = gui_alt_color(color_rgb(80, 80, 240), color_rgb(240, 240, 80));
    app->backcolor = gui_alt_color(color_rgb(200, 240, 200), color_rgb(80, 128, 80));
    app->fullfont = font_system(40, 0);
    log_printf("Using font %s\n", font_family(app->fullfont));
    
    app->window = window_create(ekWINDOW_STDRES);
    window_title(app->window, "Chasm-e-Khat");
    window_origin(app->window, v2df(500, 200));
    window_OnMoved(app->window, listener(app, onMovedEvent, App));
    window_OnClose(app->window, listener(app, onCloseWEvent, App));
    
    Panel *panel = NULL;
    panel = createCentralPanel(app);
    window_panel(app->window, panel);
    app->panel = panel;
    setViewContentSize(app->view, app->margin);  /* Set the view size */
    
    app->menu = createMenubar();
    osapp_menubar(app->menu, app->window);

    window_show(app->window);
    scrollToCell(app->view, app->colIdx, app->rowIdx, app->margin); /* Scroll to the given cell */

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
