// A data structure representing application state. Simplifies subwin functions

#pragma once

#include <string>
#include <optional>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <ImGuiFileBrowser.h>
#include <natevolve.hpp>
#include <wordup.hpp>

struct AppState {
    // -------- Functions --------

    AppState();

    // -------- Members --------

    // The main rendering window
    sf::RenderWindow win;

    // Error related stuff
    std::optional<std::wstring> errMessage;

    // The actual data we're modifying
    std::optional<std::string> fileName;
    std::optional<natevolve::wordup::Generator> gen;

    // Flags and such for the file opening
    imgui_addons::ImGuiFileBrowser fileDialog;
    bool spawnNewFilePopup;
    bool spawnOpenFilePopup;

    // Which category is being edited in the category maker
    std::wstring selCat;

    // How many syllables to generate
    size_t numSyllables;

    // The last word generated
    std::wstring newWord;
};

