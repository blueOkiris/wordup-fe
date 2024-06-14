// Spawn Wordup-FE subwindows. State is created and passed in from main()

#pragma once

#include <optional>
#include <natevolve.hpp>
#include <wordup.hpp>

namespace subwin {
    void errorPopup(sf::RenderWindow &win, std::optional<std::wstring> &errMessage);
    void fileOpen(
        sf::RenderWindow &win,
        std::optional<std::wstring> &errMessage,
        std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen,
        imgui_addons::ImGuiFileBrowser &fileDialog,
        bool &spawnNewFilePopup, bool &spawnOpenFilePopup
    );
    bool fileOpenedCanClose(
        sf::RenderWindow &win,
        std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen
    );
}

