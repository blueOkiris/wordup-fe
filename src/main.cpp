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
#include <natevolve.hpp>
#include <wordup.hpp>

static const size_t g_width = 1280;
static const size_t g_height = 720;
static const char *const g_title = "Wordup FE";
static const int g_fps = 60;

int main() {
    // Initialize SFML and ImGui and various other tools and settings
    sf::RenderWindow win(sf::VideoMode(g_width, g_height), g_title);
    win.setFramerateLimit(g_fps);
    if (!ImGui::SFML::Init(win)) {
        std::cout << "Failed to init SFML." << std::endl;
    }
    win.resetGLStates();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = false;
    natevolve::enableUtf8();

    // Initialize app state
    std::optional<natevolve::wordup::Generator> gen;
    std::optional<std::string> fileName;

    // Main loop
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

        if (!fileName.has_value()) {
            // Need to open a file first
            ImGui::Begin("Open File");
            if (ImGui::Button("New File")) {
                std::map<std::wstring, std::vector<std::wstring>> defCats;
                std::vector<std::wstring> defVwls;
                std::vector<std::vector<std::wstring>> defOnsets;
                std::vector<std::vector<std::wstring>> defCodas;
                const natevolve::wordup::Generator defGen(defCats, defVwls, defOnsets, defCodas);
                gen.emplace(defGen);
            }
            ImGui::End();
        } else {

        }

        win.clear();
        ImGui::SFML::Render(win);
        win.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

