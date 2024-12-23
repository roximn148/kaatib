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
static void onFileNew(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onFileNew clicked");
}

/* -------------------------------------------------------------------------- */
static void onFileOpen(App *app, Event *e) {
    unref(e);

    log_printf("onFileOpen clicked");
    
    String *homeDir = hfile_home_dir("");
    log_printf("Opening folder: (%s)", tc(homeDir));
    const char_t *ftypes[] = {"txt", "*"};
    const char_t *filePath = comwin_open_file(
        app->window,
        ftypes, 2,
        tc(homeDir));
    if (filePath != NULL) {
        log_printf("Selected File: (%s)", filePath);
    } else {
        log_printf("No file selected");
    }
    str_destroy(&homeDir);
}

/* -------------------------------------------------------------------------- */
static void onFileSave(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onFileSave clicked");
}

/* -------------------------------------------------------------------------- */
static void onFileRevert(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onFileRevert clicked");
}

/* -------------------------------------------------------------------------- */
static void onFileClose(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onFileClose clicked");
    osapp_finish();
}

/* -------------------------------------------------------------------------- */
/* File Menu **************************************************************** */
MenuItem *createFileMenu(App *app) {

    MenuItem *miFile = menuitem_create();
    menuitem_text(miFile, "&File");

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
       
        menuitem_submenu(miFile, &mnuFile);

    return miFile;
}

/* -------------------------------------------------------------------------- */
static void onEditUndo(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditUndo clicked");
}

/* -------------------------------------------------------------------------- */
static void onEditRedo(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditRedo clicked");
}

/* -------------------------------------------------------------------------- */
static void onEditCopy(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditCopy clicked");
}

/* -------------------------------------------------------------------------- */
static void onEditCut(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditCut clicked");
}

/* -------------------------------------------------------------------------- */
static void onEditPaste(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditPaste clicked");
}

/* -------------------------------------------------------------------------- */
static void onEditSelectAll(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onEditSelectAll clicked");
}

/* -------------------------------------------------------------------------- */
/* Edit Menu **************************************************************** */
MenuItem *createEditMenu(App *app) {

    MenuItem *miEdit = menuitem_create();
    menuitem_text(miEdit, "&Edit");

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

        menuitem_submenu(miEdit, &mnuEdit);
    
    return miEdit;
}

/* -------------------------------------------------------------------------- */
static void onViewWhitespace(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onViewWhitespace clicked");
}

/* -------------------------------------------------------------------------- */
static void onViewKeyboard(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onViewKeyboard clicked");
}

/* -------------------------------------------------------------------------- */
/* View Menu **************************************************************** */
MenuItem *createViewMenu(App *app) {

    MenuItem *miView = menuitem_create();
    menuitem_text(miView, "&View");

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

        menuitem_submenu(miView, &mnuView);

    return miView;
}

/* -------------------------------------------------------------------------- */
static void onHelpAbout(App *app, Event *e) {
    unref(app);
    unref(e);
    log_printf("onHelpAbout clicked");
}

/* -------------------------------------------------------------------------- */
/* Help Menu **************************************************************** */
MenuItem *createHelpMenu(App *app) {

    MenuItem *miHelp = menuitem_create();
    menuitem_text(miHelp, "&Help");

    Menu *mnuHelp = menu_create();
        
        MenuItem *miAbout = menuitem_create();
        menuitem_text(miAbout, "&About");
        menuitem_image(miAbout, (const Image*)INFO_PNG);
        menuitem_key(miAbout, ekKEY_F1, ekMKEY_NONE);
        menuitem_OnClick(miAbout, listener(app, onHelpAbout, App));
        menu_item(mnuHelp, miAbout);

        menuitem_submenu(miHelp, &mnuHelp);

    return miHelp;
}

/* -------------------------------------------------------------------------- */
