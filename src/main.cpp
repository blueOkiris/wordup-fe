// Entry point for Wordup-FE

#include <optional>
#include <iostream>
#include <string>
#include <vector>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <ImGuiFileBrowser.h>
#include <natevolve.hpp>
#include <wordup.hpp>
#include <global.hpp>
#include <subwin.hpp>

static void initSys(sf::RenderWindow &win);
static void handleEvents(sf::RenderWindow &win, sf::Event &ev);

int main() {
    // -------- App State --------

    sf::RenderWindow win(sf::VideoMode(global::width, global::height), global::title);

    std::optional<std::wstring> errMessage = std::nullopt;

    std::optional<std::string> fileName = std::nullopt;
    std::optional<natevolve::wordup::Generator> gen = std::nullopt;

    imgui_addons::ImGuiFileBrowser fileDialog;
    bool spawnNewFilePopup = false;
    bool spawnOpenFilePopup = false;

    initSys(win);

    // -------- Main loop --------

    sf::Clock deltaClock;
    while (win.isOpen()) {
        sf::Event event;
        while (win.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(win, event);
            handleEvents(win, event);
        }

        ImGui::SFML::Update(win, deltaClock.restart());

        if (errMessage.has_value()) {
            subwin::errorPopup(win, errMessage);
        } else if (!fileName.has_value()) {
            subwin::fileOpen(
                win,
                errMessage,
                fileName, gen,
                fileDialog, spawnNewFilePopup, spawnOpenFilePopup
            );
        } else {
            if (subwin::fileOpenedCanClose(win, fileName, gen)) {
                win.clear();
                ImGui::SFML::Render(win);
                win.display();
                continue;
            }
        }

        win.clear();
        ImGui::SFML::Render(win);
        win.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

// Initialize SFML and ImGui and various other tools and settings
static void initSys(sf::RenderWindow &win) {
    win.setFramerateLimit(global::fps);
    if (!ImGui::SFML::Init(win)) {
        std::cout << "Failed to init SFML." << std::endl;
    }
    win.resetGLStates();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = false;
    natevolve::enableUtf8();
}

// Handle things like resize and close
static void handleEvents(sf::RenderWindow &win, sf::Event &ev) {
    switch (ev.type) {
        case sf::Event::Closed:
            win.close();
            break;
        default:
            break;
    }
}

