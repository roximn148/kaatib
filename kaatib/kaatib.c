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
/* #include "icons.h" */

/* -------------------------------------------------------------------------- */
static void onWindowClose(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onFileClose clicked");
    osapp_finish();
}

/* -------------------------------------------------------------------------- */
static Panel *createCentralPanel(App *app) {
    TextView *text = textview_create();
    textview_family(text, "Calibri");
    textview_fsize(text, 24);
    textview_halign(text, ekRIGHT);
    textview_lspacing(text, 1.2);
    textview_editable(text, TRUE);
    app->textview = text;
    
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
Menu *createKaatibMenubar(App *app) {
    Menu *menu = menu_create();

    menu_item(menu, createFileMenu(app));
    menu_item(menu, createEditMenu(app));
    menu_item(menu, createViewMenu(app));
    menu_item(menu, createHelpMenu(app));

    return menu;

    /* File Menu **************************************************************/
    /* MenuItem *miFile = menuitem_create();
    menuitem_text(miFile, "&File");
    menu_item(menu, miFile);

    Menu *mnuFile = menu_create();
        
        MenuItem *miNew = menuitem_create();
        menuitem_text(miNew, "&New");
        menuitem_image(miNew, (const Image*)FILE_NEW_PNG);
        menuitem_key(miNew, ekKEY_N, ekMKEY_CONTROL);
        menuitem_OnClick(miNew, listener(app, onFileNew, App));
        menu_item(mnuFile, miNew);

        MenuItem *miOpen = menuitem_create();
        menuitem_text(miOpen, "&Open");
        menuitem_image(miOpen, (const Image*)FILE_OPEN_PNG);
        menuitem_key(miOpen, ekKEY_O, ekMKEY_CONTROL);
        menuitem_OnClick(miOpen, listener(app, onFileOpen, App));
        menu_item(mnuFile, miOpen);

        MenuItem *miSave = menuitem_create();
        menuitem_text(miSave, "&Save");
        menuitem_image(miSave, (const Image*)SAVE_PNG);
        menuitem_key(miSave, ekKEY_S, ekMKEY_CONTROL);
        menuitem_OnClick(miSave, listener(app, onFileSave, App));
        menu_item(mnuFile, miSave);

        MenuItem *miRevert = menuitem_create();
        menuitem_text(miRevert, "&Revert");
        menuitem_image(miRevert, (const Image*)REVERT_PNG);
        menuitem_key(miRevert, ekKEY_R, ekMKEY_CONTROL+ekMKEY_SHIFT);
        menuitem_OnClick(miRevert, listener(app, onFileRevert, App));
        menu_item(mnuFile, miRevert);
        menu_item(mnuFile, menuitem_separator());

        MenuItem *miRecent = menuitem_create();
        menuitem_text(miRecent, "&Recent");
        menuitem_image(miRecent, (const Image*)RECENT_PNG);
        menuitem_key(miRecent, ekKEY_R, ekMKEY_CONTROL+ekMKEY_SHIFT);
        menu_item(mnuFile, miRecent);

        
        #if !defined(__APPLE__)
        {
            menu_item(mnuFile, menuitem_separator());

            MenuItem *miExit = menuitem_create();
            menuitem_text(miExit, "&Exit");
            menuitem_image(miExit, (const Image*)EXIT_PNG);
            menuitem_key(miExit, ekKEY_F4, ekMKEY_CONTROL);
            menuitem_OnClick(miExit, listener(app, onFileClose, App));
            menu_item(mnuFile, miExit);
        }
        #endif
       
        menuitem_submenu(miFile, &mnuFile); */

    /* Edit Menu **************************************************************/
    /* MenuItem *miEdit = menuitem_create();
    menuitem_text(miEdit, "&Edit");
    menu_item(menu, miEdit);

    Menu *mnuEdit = menu_create();
        
        MenuItem *miUndo = menuitem_create();
        menuitem_text(miUndo, "&Undo");
        menuitem_image(miUndo, (const Image*)UNDO_PNG);
        menuitem_key(miUndo, ekKEY_Z, ekMKEY_CONTROL);
        menuitem_OnClick(miUndo, listener(app, onEditUndo, App));
        menu_item(mnuEdit, miUndo);
        MenuItem *miRedo = menuitem_create();
        menuitem_text(miRedo, "&Redo");
        menuitem_image(miRedo, (const Image*)REDO_PNG);
        menuitem_key(miRedo, ekKEY_Z, ekMKEY_CONTROL+ekMKEY_SHIFT);
        menuitem_OnClick(miRedo, listener(app, onEditRedo, App));
        menu_item(mnuEdit, miRedo);

        MenuItem *miCopy = menuitem_create();
        menuitem_text(miCopy, "&Copy");
        menuitem_image(miCopy, (const Image*)COPY_PNG);
        menuitem_key(miCopy, ekKEY_C, ekMKEY_CONTROL);
        menuitem_OnClick(miCopy, listener(app, onEditCopy, App));
        menu_item(mnuEdit, miCopy);
        MenuItem *miCut = menuitem_create();
        menuitem_text(miCut, "&Cut");
        menuitem_image(miCut, (const Image*)CUT_PNG);
        menuitem_key(miCut, ekKEY_X, ekMKEY_CONTROL);
        menuitem_OnClick(miCut, listener(app, onEditCut, App));
        menu_item(mnuEdit, miCut);
        MenuItem *miPaste = menuitem_create();
        menuitem_text(miPaste, "&Paste");
        menuitem_image(miPaste, (const Image*)PASTE_PNG);
        menuitem_key(miPaste, ekKEY_V, ekMKEY_CONTROL);
        menuitem_OnClick(miPaste, listener(app, onEditPaste, App));
        menu_item(mnuEdit, miPaste);
        MenuItem *miSelectAll = menuitem_create();
        menuitem_text(miSelectAll, "&Select All");
        menuitem_image(miSelectAll, (const Image*)COPY_ALL_PNG);
        menuitem_key(miSelectAll, ekKEY_A, ekMKEY_CONTROL);
        menuitem_OnClick(miSelectAll, listener(app, onEditSelectAll, App));
        menu_item(mnuEdit, miSelectAll);

        menuitem_submenu(miEdit, &mnuEdit); */

    /* View Menu **************************************************************/
    /* MenuItem *miView = menuitem_create();
    menuitem_text(miView, "&View");
    menu_item(menu, miView);

    Menu *mnuView = menu_create();
        
        MenuItem *miWhitespace = menuitem_create();
        menuitem_text(miWhitespace, "&Whitespace");
        menuitem_image(miWhitespace, (const Image*)WHITESPACE_PNG);
        menuitem_key(miWhitespace, ekKEY_F3, ekMKEY_NONE);
        menuitem_OnClick(miWhitespace, listener(app, onViewWhitespace, App));
        menu_item(mnuView, miWhitespace);

        MenuItem *miKeyboard = menuitem_create();
        menuitem_text(miKeyboard, "&Keyboard");
        menuitem_image(miKeyboard, (const Image*)KEYBOARD_PNG);
        menuitem_key(miKeyboard, ekKEY_F8, ekMKEY_NONE);
        menuitem_OnClick(miKeyboard, listener(app, onViewKeyboard, App));
        menu_item(mnuView, miKeyboard);

        menuitem_submenu(miView, &mnuView); */

    /* Help Menu **************************************************************/
    /* MenuItem *miHelp = menuitem_create();
    menuitem_text(miHelp, "&Help");
    menu_item(menu, miHelp);

    Menu *mnuHelp = menu_create();
        
        MenuItem *miAbout = menuitem_create();
        menuitem_text(miAbout, "&About");
        menuitem_image(miAbout, (const Image*)INFO_PNG);
        menuitem_key(miAbout, ekKEY_F1, ekMKEY_NONE);
        menuitem_OnClick(miAbout, listener(app, onHelpAbout, App));
        menu_item(mnuHelp, miAbout);

        menuitem_submenu(miHelp, &mnuHelp); */

    
}

/* -------------------------------------------------------------------------- */
Window *createKaatibWindow(App *app) {


    Panel *panel = createCentralPanel(app);
    Window *window = window_create(ekWINDOW_STDRES);
    window_panel(window, panel);
    window_title(window, "Kaatib");
    window_OnClose(window, listener(app, onWindowClose, App));

    return window;
}

/* -------------------------------------------------------------------------- */
