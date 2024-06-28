// Spawn Wordup-FE subwindows. State is created and passed in from main()

#pragma once

#include <string>
#include <optional>
#include <natevolve.hpp>
#include <wordup.hpp>
#include <state.hpp>

namespace subwin {
    // Show a popup with an error message
    void errorPopup(AppState &state);

    // Show a window that lets you open files or create a new one
    void fileOpen(AppState &state);

    // Show the currently opened file and a button to close it
    void fileOpenedCanClose(AppState &state);

    // Show a table with selectable IPA flags
    void ipaSelect(AppState &state);

    // Show a window for grouping chosen sounds into categories
    void categoryMaker(AppState &state);

    // Show a window for piecing together a series of consonant categories to make an onset
    void onsetMaker(AppState &state);

    // Show a window for piecing together a series of consonant categories to make a coda
    void codaMaker(AppState &state);
}

