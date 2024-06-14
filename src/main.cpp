// Entry point for Wordup-FE

#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

static const size_t g_width = 1280;
static const size_t g_height = 720;
static const char *const g_title = "Wordup FE";
static const int g_fps = 200;

int main() {
    sf::RenderWindow win(sf::VideoMode(g_width, g_height), g_title);
    win.setFramerateLimit(g_fps);
    if (!ImGui::SFML::Init(win)) {
        std::cout << "Failed to init SFML." << std::endl;
    }
    win.resetGLStates();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;

    sf::Clock deltaClock;
    while (win.isOpen()) {
        sf::Event event;
        while (win.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(win, event);
            switch (event.type) {
                case sf::Event::Closed:
                    win.close();
                    break;
                default:
                    break;
            }
        }

        ImGui::SFML::Update(win, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        win.clear();
        ImGui::SFML::Render(win);
        win.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

