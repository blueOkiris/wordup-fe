// Implement state initialization

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <ImGuiFileBrowser.h>
#include <optional>
#include <natevolve.hpp>
#include <wordup.hpp>
#include <global.hpp>
#include <state.hpp>

AppState::AppState():
    win(sf::VideoMode(global::width, global::height), global::title),
    errMessage(std::nullopt),
    fileName(std::nullopt), gen(std::nullopt),
    fileDialog(), spawnNewFilePopup(false), spawnOpenFilePopup(false) {}

