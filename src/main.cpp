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
#include <state.hpp>

static void initSys(AppState &state);
static void handleEvents(AppState &state, sf::Event &ev);

int main() {
    AppState state;

    initSys(state);

    // -------- Main loop --------

    sf::Clock deltaClock;
    while (state.win.isOpen()) {
        sf::Event event;
        while (state.win.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(state.win, event);
            handleEvents(state, event);
        }

        ImGui::SFML::Update(state.win, deltaClock.restart());

        if (state.errMessage.has_value()) {
            subwin::errorPopup(state);
        } else if (!state.fileName.has_value()) {
            subwin::fileOpen(state);
        } else {
            subwin::ipaSelect(state);
            subwin::categoryMaker(state);
            subwin::onsetMaker(state);
            subwin::codaMaker(state);
            subwin::generate(state);

            // Must be last bc can erase fileName
            subwin::fileOpenedCanClose(state);
        }

        state.win.clear();
        ImGui::SFML::Render(state.win);
        state.win.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

// Initialize SFML and ImGui and various other tools and settings
static void initSys(AppState &state) {
    state.win.setFramerateLimit(global::fps);
    if (!ImGui::SFML::Init(state.win, false)) {
        std::cout << "Failed to init SFML." << std::endl;
        exit(1);
    }
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDrawCursor = false;
    io.Fonts->AddFontFromFileTTF("fonts/ubuntu-font-family/Ubuntu-R.ttf", 18.0f);
    global::fontCharisSil = io.Fonts->AddFontFromFileTTF(
        "fonts/charis-sil/CharisSIL-Regular.ttf", 26.0f, nullptr, global::ipaRanges
    );
    if (!ImGui::SFML::UpdateFontTexture()) {
        std::cout << "Failed to update font texture." << std::endl;
        exit(1);
    }
    state.win.resetGLStates();
    natevolve::enableUtf8();
}

// Handle things like resize and close
static void handleEvents(AppState &state, sf::Event &ev) {
    switch (ev.type) {
        case sf::Event::Closed:
            state.win.close();
            break;
        default:
            break;
    }
}

