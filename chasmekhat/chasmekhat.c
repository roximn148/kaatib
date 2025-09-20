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
#include <cache.h>

#include "ftx.h"
#include "icons.h"

/*----------------------------------------------------------------------------*/
typedef struct _view_grid_t ViewGrid;
struct _view_grid_t {
    uint16_t glyphIdx;
    uint32_t columnCount;
    uint32_t rowCount;
    uint32_t totalCells;
    real32_t cellSize;
    real32_t cellMargin;

    uint32_t hoverCellX;
    uint32_t hoverCellY;
    uint32_t selectedCellX;
    uint32_t selectedCellY;

    color_t boxColor;
};

typedef struct _app_t App;
struct _app_t {
    FontEngine fontEngine;
    RenderClosure closure;
    UtxCache *glyphCache;

    ViewGrid grid;

    char_t nameColumn[256];

    struct _ui_t {
        Window *window;
        Panel *panel;
        Menu *menu;
        TableView *table;
        Edit *edxGlyphId;
        View *view;
        Label *lblCellsInfo;
        Layout *lyMain;
        Layout *lyMiddle;
        Layout *lyControls;
        Layout *lyInfo;
        MenuItem *miOpen;
        MenuItem *miRecent;
        MenuItem *miExit;
        MenuItem *miAbout;
    } ui;
};

static const char_t GLYPH_COLOR[] = "#0000C0";
static const uint32_t GLYPH_SIZE = 16;
static const uint32_t GLYPH_DPI = 256;

/** ----------------------------------------------------------------------------
 * @brief Scrolls the view to the specified cell.
 *
 * @param view The view containing the cells.
 * @param col The column index of the target cell.
 * @param row The row index of the target cell.
 * @param margin The margin around the cells.
 * -------------------------------------------------------------------------- */
static void scrollToCell(View *view, ViewGrid *grid, uint16_t n) {
    uint32_t row = n / grid->columnCount;
    uint32_t col = n % grid->columnCount;

    grid->selectedCellX = col;
    grid->selectedCellY = row;

    real32_t ypos = grid->cellMargin +
                    row * (grid->cellSize + grid->cellMargin);
    view_scroll_y(view, ypos);
    view_update(view);
}

/*----------------------------------------------------------------------------*/
Image* fGlyphRenderer(uint16_t glyphId, void *context) {
    FontEngine *fontEngine = (FontEngine*)context;

    int err = renderGlyph(fontEngine, glyphId, FT_RENDER_MODE_NORMAL);
    if (err != 0) {
        return NULL;
    }

    const FT_Face face = fontEngine->face;
    const FT_Bitmap *glyphBitmap = &face->glyph->bitmap;
    if (glyphBitmap->width == 0 || glyphBitmap->rows == 0) {
        return NULL;
    }

    const color_t glyphColor = color_html(GLYPH_COLOR);
    Image *img = ftBmp2ImageRGBA(glyphBitmap, glyphColor);
    return img;
}

/*----------------------------------------------------------------------------*/
static void drawGlyphImage(DCtx *ctx,
                           real32_t px, real32_t py,
                           real32_t cellSize,
                           Image *img) {
    if (img == NULL) {
        return;
    }

    uint32_t w = image_width(img);
    uint32_t h = image_height(img);
    uint32_t cs = (uint32_t)cellSize;
    real32_t halfCell = cellSize / 2.0f;

    if (w > cs || h > cs) {
        uint32_t x0 = 0, y0 = 0;
        if (w > cs) {
            x0 = (w - cs) / 2;
            w = cs;
        }
        if (h > cs) {
            y0 = (h - cs) / 2;
            h = cs;
        }

        Image *resized = image_trim(img, x0, y0, w, h);
        draw_image(
            ctx, resized,
            px + halfCell - (w / 2),
            py + halfCell - (h / 2)
        );
        image_destroy(&resized);
    } else {
        draw_image(
            ctx, img,
            px + halfCell - (w / 2),
            py + halfCell - (h / 2)
        );
    }
}

/*----------------------------------------------------------------------------*/
static void drawClippedView(DCtx *ctx,
    const real32_t x, const real32_t y,
    const real32_t width, const real32_t height,
    const ViewGrid *grid,
    UtxCache *glyphCache) {

    const real32_t boxSize = grid->cellSize + grid->cellMargin;

    uint32_t sti, edi;
    uint32_t stj, edj;
    real32_t posX = 0;
    real32_t posY = 0;
    uint32_t i, j;

    /* Calculate the visible cols */
    sti = (uint32_t)bmath_floorf(x / boxSize);
    edi = sti + (uint32_t)bmath_ceilf(width / boxSize) + 1;
    if (edi > grid->columnCount) {
        edi = grid->columnCount;
    }

    /* Calculate the visible rows */
    stj = (uint32_t)bmath_floorf(y / boxSize);
    edj = stj + (uint32_t)bmath_ceilf(height / boxSize) + 1;
    if (edj > grid->rowCount) {
        edj = grid->rowCount;
    }

    posY = grid->cellMargin + stj * boxSize;

    draw_fill_color(ctx, grid->boxColor);
    draw_line_color(ctx, kCOLOR_BLUE);
    draw_line_width(ctx, 1);
    draw_text_align(ctx, ekLEFT, ekTOP);
    draw_text_halign(ctx, ekLEFT);

    char_t text[128];
    for (j = stj; j < edj; ++j) {
        posX = grid->cellMargin + sti * boxSize;
        for (i = sti; i < edi; ++i) {
            bool_t isHoverCell = FALSE;

            uint16_t n = (uint16_t)(j * grid->columnCount + i);
            if (n >= grid->totalCells) {
                goto loopEnd;
            }
            bstd_sprintf(text, sizeof(text), "%04X", n);

            if (grid->selectedCellX == i && grid->selectedCellY == j) {
                draw_line_width(ctx, 6);
                draw_line_color(ctx, kCOLOR_RED);

                isHoverCell = TRUE;
            } else if (grid->hoverCellX == i && grid->hoverCellY == j) {
                draw_line_width(ctx, 3);
                draw_line_color(ctx, kCOLOR_BLUE);
                isHoverCell = TRUE;
            }

            draw_rect(ctx, ekSKFILL, posX, posY, grid->cellSize, grid->cellSize);
            draw_text(ctx, text, posX, posY);

            if (isHoverCell == TRUE) {
                draw_line_width(ctx, 1);
                draw_line_color(ctx, kCOLOR_BLUE);
            }

            drawGlyphImage(
                ctx, posX, posY,
                grid->cellSize,
                cacheGet(glyphCache, n)
            );

            posX += boxSize;
        }

        posY += boxSize;
    }
loopEnd:;
}

/*----------------------------------------------------------------------------*/
static void onDrawView(App *app, Event *e) {
    const EvDraw *ed = event_params(e, EvDraw);
    if (app->glyphCache != NULL) {
        drawClippedView(
            ed->ctx, ed->x, ed->y, ed->width, ed->height,
            &app->grid, app->glyphCache
        );
    }
}

/*----------------------------------------------------------------------------*/
static void onMouseAction(View *view, ViewGrid *grid,
                         const real32_t x, const real32_t y,
                         const uint32_t action) {

    real32_t boxSize = grid->cellSize + grid->cellMargin;
    uint32_t mx = (uint32_t)bmath_floorf(x / boxSize);
    uint32_t my = (uint32_t)bmath_floorf(y / boxSize);
    real32_t xmin = mx * boxSize + grid->cellMargin;
    real32_t xmax = xmin + grid->cellSize;
    real32_t ymin = my * boxSize + grid->cellMargin;
    real32_t ymax = ymin + grid->cellSize;

    if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
        if (action == 0) {
            grid->hoverCellX = mx;
            grid->hoverCellY = my;
        } else {
            grid->selectedCellX = mx;
            grid->selectedCellY = my;
        }
    } else {
        grid->hoverCellX = UINT32_MAX;
        grid->hoverCellY = UINT32_MAX;
    }

    view_update(view);
}

/*----------------------------------------------------------------------------*/
static void onMouseMove(App *app, Event *e) {
    const EvMouse *em = event_params(e, EvMouse);
    onMouseAction(app->ui.view, &app->grid, em->x, em->y, 0);
}

/*----------------------------------------------------------------------------*/
static void onMouseUp(App *app, Event *e) {
    const EvMouse *em = event_params(e, EvMouse);
    onMouseAction(app->ui.view, &app->grid, em->x, em->y, 0);
}

/*----------------------------------------------------------------------------*/
static void onMouseDown(App *app, Event *e) {
    const EvMouse *em = event_params(e, EvMouse);
    onMouseAction(app->ui.view, &app->grid, em->x, em->y, 1);
}

/*----------------------------------------------------------------------------*/
static void onKeyDown(App * app, Event *e) {
    const EvKey *ek = event_params(e, EvKey);
    View *view = app->ui.view;
    ViewGrid *grid = &app->grid;
    const real32_t margin = grid->cellMargin;
    const real32_t boxSize = grid->cellSize + margin;
    V2Df scroll;
    S2Df size;

    view_viewport(view, &scroll, &size);

    if (ek->key == ekKEY_DOWN && grid->selectedCellY < grid->rowCount - 1) {
        real32_t ymin = (grid->selectedCellY + 1) * boxSize + margin;
        ymin += grid->cellSize;

        if (scroll.y + size.height <= ymin) {
            view_scroll_y(view, ymin - size.height + margin);
            grid->hoverCellX = UINT32_MAX;
            grid->hoverCellY = UINT32_MAX;
        }

        grid->selectedCellY += 1;
        view_update(view);
    }

    if (ek->key == ekKEY_UP && grid->selectedCellY > 0) {
        real32_t ymin = (grid->selectedCellY - 1) * boxSize + margin;

        if (scroll.y >= ymin) {
            view_scroll_y(view, ymin - margin);
            grid->hoverCellX = UINT32_MAX;
            grid->hoverCellY = UINT32_MAX;
        }

        grid->selectedCellY -= 1;
        view_update(view);
    }

    if (ek->key == ekKEY_RIGHT && grid->selectedCellX < grid->columnCount - 1) {
        real32_t xmin = (grid->selectedCellX + 1) * boxSize + margin;
        xmin += grid->cellSize;

        if (scroll.x + size.width <= xmin) {
            view_scroll_x(view, xmin - size.width + margin);
            grid->hoverCellX = UINT32_MAX;
            grid->hoverCellY = UINT32_MAX;
        }

        grid->selectedCellX += 1;
        view_update(view);
    }

    if (ek->key == ekKEY_LEFT && grid->selectedCellX > 0) {
        real32_t xmin = (grid->selectedCellX - 1) * boxSize + margin;

        if (scroll.x >= xmin) {
            view_scroll_x(view, xmin - margin);
            grid->hoverCellX = UINT32_MAX;
            grid->hoverCellY = UINT32_MAX;
        }

        grid->selectedCellX -= 1;
        view_update(view);
    }
}

/** ----------------------------------------------------------------------------
 * @brief Calculate the grid dimension based on the given width.
 * -------------------------------------------------------------------------- */
static void recalculateGrid(ViewGrid *grid, real32_t width) {
    real32_t columnCount = (width - grid->cellMargin) / (grid->cellSize + grid->cellMargin);
    columnCount = bmath_floorf(columnCount);
    real32_t rowCount = grid->totalCells / columnCount;
    rowCount = bmath_ceilf(rowCount);

    grid->columnCount = (uint32_t)columnCount;
    grid->rowCount = (uint32_t)rowCount;
}

/** ----------------------------------------------------------------------------
 * @brief Set the content size of the view based on the grid dimensions.
 * -------------------------------------------------------------------------- */
static void updateViewContentSize(View *view, ViewGrid *grid) {
    real32_t boxSize = grid->cellSize + grid->cellMargin;
    real32_t width = grid->cellMargin + grid->columnCount * boxSize;
    real32_t height = grid->cellMargin + grid->rowCount * boxSize;
    view_content_size(
        view,
        s2df((real32_t)width, (real32_t)height),
        s2df(boxSize, boxSize)  /* Scroll step */
    );
}

/*----------------------------------------------------------------------------*/
static void onSizeChanged(App *app, Event *e) {
    const EvSize *esz = event_params(e, EvSize);
    ViewGrid *grid = &app->grid;

    /* Subtract vertical scrollbar width to get actual viewport width */
    real32_t vscrollbarWidth;
    view_scroll_size(app->ui.view, &vscrollbarWidth, NULL);
    recalculateGrid(grid, esz->width - vscrollbarWidth);

    char_t labelText[256];
    bstd_sprintf(labelText, sizeof(labelText), "Grid: %d x %d", grid->columnCount, grid->rowCount);
    label_text(app->ui.lblCellsInfo, labelText);

    updateViewContentSize(app->ui.view, grid);
}


/*----------------------------------------------------------------------------*/
static void onGlyphIndexChanged(App *app, Event *e) {
    ViewGrid *grid = evbind_object(e, ViewGrid);
    Layout *layout = event_sender(e, Layout);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, ViewGrid, uint16_t, glyphIdx) == TRUE) {
        if (grid->totalCells > 0 && grid->glyphIdx < grid->totalCells) {
            layout_dbind_update(layout, ViewGrid, uint16_t, glyphIdx);
            scrollToCell(app->ui.view, grid, grid->glyphIdx);
        } else {
            bool_t *res = event_result(e, bool_t);
            *res = FALSE;
        }
    }
    return;
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
    cell_dbind(layout_cell(layout, 1, 0), ViewGrid, uint16_t, glyphIdx);
    layout_dbind(layout, listener(app, onGlyphIndexChanged, App), ViewGrid);
    layout_dbind_obj(layout, &app->grid, ViewGrid);

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

    app->ui.edxGlyphId = ebxGlyphId;
    app->ui.lyControls = layout;
    return layout;
}

/*----------------------------------------------------------------------------*/
static Layout *createInfoLayout(App *app) {
    Layout *layout = layout_create(2, 1);
    Label *lblInfo = label_create();

    label_align(lblInfo, ekRIGHT);

    layout_label(layout, lblInfo, 1, 0);

    /* All the horizontal expansion will be done in empty column-cell(2) */
    layout_hexpand(layout, 0);

    /* Keep the labels for further updates */
    app->ui.lblCellsInfo = lblInfo;

    /* Text for labels dimensioning */
    char_t text[256];
    bstd_sprintf(text, sizeof(text), "Grid: [%d, %d]", 1000, 1000);
    label_size_text(app->ui.lblCellsInfo, text);
    app->ui.lyInfo = layout;
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
            bstd_sprintf(app->nameColumn, sizeof(app->nameColumn), "Name %d", pos->row);
            cell->text = app->nameColumn;
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/
static Layout *createTableLayout(App *app) {
    Layout *layout = layout_create(1, 1);
    TableView *table = tableview_create();
    tableview_add_column_text(table);
    tableview_size(table, s2df(150, 200));
    tableview_column_width(table, 0, 120);
    tableview_OnData(table, listener(app, getTableData, App));
    tableview_update(table);
    layout_tableview(layout, table, 0, 0);
    app->ui.table = table;
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
    view_OnSize(view, listener(app, onSizeChanged, App));

    layout_view(layout, view, 0, 0);
    layout_layout(layout, lyTable, 1, 0);

    /* Add the view to tabstop list */
    layout_tabstop(layout, 0, 0, TRUE);

    /* A small horizontal margin between view cell and list (left) table (right) layouts */
    layout_hmargin(layout, 0, 3);

    /* All the horizontal expansion will be done in the middle cell (view)
       list_layout (left) and table_layout (right) will preserve the 'natural' width */
    layout_hexpand(layout, 0);

    app->ui.view = view;
    app->ui.lyMiddle = layout;
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
    app->ui.lyMain = layout;
    return panel;
}

/*----------------------------------------------------------------------------*/
static void onCloseWEvent(App *app, Event *e) {
    osapp_finish();
    unref(app);
    unref(e);
}

/*----------------------------------------------------------------------------*/
static void initViewGrid(ViewGrid *grid) {
    grid->cellSize = 95;
    grid->cellMargin = 5;

    grid->rowCount = 0;
    grid->columnCount = 0;

    grid->glyphIdx = 0;
    grid->hoverCellX = UINT32_MAX;
    grid->hoverCellY = UINT32_MAX;
    grid->selectedCellX = 0;
    grid->selectedCellY = 0;

    grid->boxColor = gui_alt_color(color_rgb(200, 240, 200), color_rgb(80, 128, 80));
}

/* -------------------------------------------------------------------------- */
static void loadFont(App *app, const char_t *fontFilePath) {
    cassert_no_null(fontFilePath);
    cassert_no_null(app);
    cassert(app->fontEngine.face == NULL);

    int err = loadFontFace(&app->fontEngine, fontFilePath, GLYPH_SIZE, GLYPH_DPI);
    if (err != 0) {
        return;
    }

    log_printf("Font face '%s' successfully loaded.", fontFilePath);
    log_printf("Font render size set to %d with DPI %d. Total glyph count is %d.",
                GLYPH_SIZE, GLYPH_DPI, app->fontEngine.face->num_glyphs);
    app->grid.totalCells = app->fontEngine.face->num_glyphs;

    cassert(app->glyphCache == NULL);
    app->glyphCache = cacheCreate(200, &app->closure);
    if (app->glyphCache != NULL) {
        log_printf("Application glyph cache initialized to %d capacity.", app->glyphCache->capacity);
    } else {
        log_printf("Application glyph cache failed to initialize");
    }

}

/* -------------------------------------------------------------------------- */
static void closeFont(App *app) {
    /* Clear loaded font object */
    int err = closeFontFace(&app->fontEngine);
    cassert(err == 0);

    /* Remove any cache */
    if (app->glyphCache != NULL) {
        UtxCache *cache = app->glyphCache;
        log_printf("Cache Statistics :-------------------------------------------");
        log_printf("Requests: %d, Hits: %d, Misses: %d",
                    cache->requests,
                    cache->hits,
                    cache->requests-cache->hits);
        log_printf("Load Factor: %.3f, Evictions: %d",
                    cacheLoadFactor(cache),
                    cache->evictions);
        log_printf("Hit Rate: %.2f%%, Average Hit Time %.3f ms",
                    cacheHitRate(cache)*100.,
                    cache->avgHitTime*1000.);
        log_printf("Miss Rate: %.2f%%, Average Miss Penalty: %.3f ms",
                    cacheMissRate(cache)*100.,
                    cache->avgMissPenalty*1000.);
        log_printf("Average Access Time: %.3f ms", cacheAverageAccessTime(cache)*1000.);
        log_printf("=============================================================");

        cacheDestroy(&app->glyphCache);
    }
}

/* -------------------------------------------------------------------------- */
static void onFileOpen(App *app, Event *e) {
    unref(e);

    String *homeDir = hfile_home_dir("");
    log_printf("Opening folder: (%s)", tc(homeDir));
    const char_t *ftypes[] = {"ttf", "otf", "*"};
    const char_t *filePath = comwin_open_file(app->ui.window, "Select Font File", ftypes, 3, tc(homeDir));
    if (filePath != NULL) {
        closeFont(app);

        log_printf("'%s' font file selected", filePath);
        loadFont(app, filePath);

        ViewGrid *grid = &app->grid;
        View *view = app->ui.view;
        initViewGrid(grid);
        recalculateGrid(grid, 500);
        updateViewContentSize(view, grid);
        scrollToCell(view, grid, 0);

        window_update(app->ui.window);
    }
    str_destroy(&homeDir);
}

/* -------------------------------------------------------------------------- */
static void onFileExit(App *app, Event *e) {
    unref(app);
    unref(e);
    osapp_finish();
}

/* -------------------------------------------------------------------------- */
static void onHelpAbout(App *app, Event *e) {

    uint32_t flags = ekWINDOW_TITLE | ekWINDOW_CLOSE | ekWINDOW_RETURN | ekWINDOW_ESC;
    Window *aboutDialog = window_create(flags);
    window_title(aboutDialog, "About Chasm-e-Khat");

      Panel *panel = panel_create();
        Layout *layout = layout_create(2, 1);
        layout_margin(layout, 20);
        layout_hmargin(layout, 0, 10);

          ImageView *img = imageview_create();
          imageview_image(img, (const Image*)CHASMEKHAT64_PNG);
          imageview_scale(img, ekGUI_SCALE_ASPECTDW);
          layout_imageview(layout, img, 0, 0);
          layout_hsize(layout, 0, 64);

          Label *lbl = label_create();
          label_multiline(lbl, TRUE);
          label_text(lbl, "Chasm-e-Khat\nVersion: 0.1.0 alpha\n(c) 2025 RoXimn");
          label_align(lbl, ekLEFT);
          layout_label(layout, lbl, 1, 0);
          layout_hexpand(layout, 1);

        panel_layout(panel, layout);
    window_panel(aboutDialog, panel);

    V2Df pos = window_get_origin(app->ui.window);
    S2Df s1 = window_get_size(app->ui.window);
    S2Df s2 = window_get_size(aboutDialog);
    window_origin(
        aboutDialog,
        v2df(pos.x + (s1.width - s2.width) / 2,
             pos.y + (s1.height - s2.height) / 2)
    );

    uint32_t retval = UINT32_MAX;
    retval = window_modal(aboutDialog, app->ui.window);

    window_destroy(&aboutDialog);

    unref(app);
    unref(e);
}

/*----------------------------------------------------------------------------*/
static Menu *createMenubar(App *app) {
    Menu *menu = menu_create();

    MenuItem *miFile = menuitem_create();
    menuitem_text(miFile, "&File");

    Menu *mnuFile = menu_create();

        MenuItem *miOpen = menuitem_create();
        menuitem_text(miOpen, "&Open");
        menuitem_image(miOpen, (const Image*)FILE_OPEN_PNG);
        menuitem_key(miOpen, ekKEY_O, ekMKEY_CONTROL);
        menuitem_OnClick(miOpen, listener(app, onFileOpen, App));
        menu_add_item(mnuFile, miOpen);
        app->ui.miOpen = miOpen;

        menu_add_item(mnuFile, menuitem_separator());

        MenuItem *miRecent = menuitem_create();
        menuitem_text(miRecent, "&Recent");
        menuitem_image(miRecent, (const Image*)RECENT_PNG);
        menuitem_key(miRecent, ekKEY_R, ekMKEY_CONTROL+ekMKEY_SHIFT);
        menu_add_item(mnuFile, miRecent);
        app->ui.miRecent = miRecent;

        #if !defined(__APPLE__)
        {
            menu_add_item(mnuFile, menuitem_separator());

            MenuItem *miExit = menuitem_create();
            menuitem_text(miExit, "&Exit");
            menuitem_image(miExit, (const Image*)EXIT_PNG);
            menuitem_key(miExit, ekKEY_F4, ekMKEY_CONTROL);
            menuitem_OnClick(miExit, listener(app, onFileExit, App));
            menu_add_item(mnuFile, miExit);
            app->ui.miExit = miExit;
        }
        #endif

        menuitem_submenu(miFile, &mnuFile);

    menu_add_item(menu, miFile);

    MenuItem *miHelp = menuitem_create();
    menuitem_text(miHelp, "&Help");

    Menu *mnuHelp = menu_create();

        MenuItem *miAbout = menuitem_create();
        menuitem_text(miAbout, "&About");
        menuitem_image(miAbout, (const Image*)INFO_PNG);
        menuitem_key(miAbout, ekKEY_F1, ekMKEY_NONE);
        menuitem_OnClick(miAbout, listener(app, onHelpAbout, App));
        menu_add_item(mnuHelp, miAbout);
        app->ui.miAbout = miAbout;

        menuitem_submenu(miHelp, &mnuHelp);

    menu_add_item(menu, miHelp);

    return menu;
}

/*----------------------------------------------------------------------------*/
static App *createApp(void) {
    heap_verbose(TRUE);

    App *app = heap_new0(App);

    /* Load FreeType library */
    int err = initFontEngine(&app->fontEngine);
    cassert_fatal_msg(err == 0, "Failed to initialize FreeType library.");
    FT_Int major, minor, patch;
    FT_Library_Version(app->fontEngine.ftLibrary, &major, &minor, &patch);
    log_printf("Loaded FreeType version: %d.%d.%d", major, minor, patch);

    gui_respack(icons_respack);
    gui_language("");

    app->closure.context = (void*)&app->fontEngine;
    app->closure.render = fGlyphRenderer;

    initViewGrid(&app->grid);
    dbind(ViewGrid, uint16_t, glyphIdx);

    /* Main Window */
    app->ui.window = window_create(ekWINDOW_STDRES);
    window_title(app->ui.window, "Chasm-e-Khat");
    window_origin(app->ui.window, v2df(500, 200));
    window_OnClose(app->ui.window, listener(app, onCloseWEvent, App));

    Panel *panel = createCentralPanel(app);
    window_panel(app->ui.window, panel);
    app->ui.panel = panel;

    app->ui.menu = createMenubar(app);
    osapp_menubar(app->ui.menu, app->ui.window);

    window_show(app->ui.window);
    window_focus(app->ui.window, (GuiControl*)app->ui.view);

    return app;
}

/*----------------------------------------------------------------------------*/
static void destroyApp(App **app) {
    menu_destroy(&(*app)->ui.menu);
    window_destroy(&(*app)->ui.window);

    closeFont(*app);
    closeFontEngine(&(*app)->fontEngine);

    heap_delete(app, App);
}

/*----------------------------------------------------------------------------*/
static void updateApp(App *app, const real64_t prTime, const real64_t cTime) {
    unref(prTime);
    unref(cTime);
    unref(app);
}

/*----------------------------------------------------------------------------*/
#include <osapp/osmain.h>
osmain_sync(0.1, createApp, destroyApp, updateApp, "", App);

/*----------------------------------------------------------------------------*/
