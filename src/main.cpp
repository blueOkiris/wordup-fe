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
    imgui_addons::ImGuiFileBrowser fileDialog;
    std::optional<natevolve::wordup::Generator> gen;
    std::optional<std::string> fileName;
    bool createNewFile = false;
    bool openFile = false;

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
            ImGui::Begin("Open File", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Button("New File")) {
                createNewFile = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File")) {
                openFile = true;
            }

            if (createNewFile) {
                ImGui::OpenPopup("Save File");
            }
            if (openFile) {
                ImGui::OpenPopup("Open File");
            }

            if (fileDialog.showFileDialog(
                "Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
                ImVec2(g_width, g_height), ".wu"
            )) {
                createNewFile = false;

                std::map<std::wstring, std::vector<std::wstring>> defCats;
                std::vector<std::wstring> defVwls;
                std::vector<std::vector<std::wstring>> defOnsets;
                std::vector<std::vector<std::wstring>> defCodas;
                const natevolve::wordup::Generator defGen(
                    defCats, defVwls, defOnsets, defCodas
                );
                const auto saveRes = defGen.toFile(fileDialog.selected_fn.c_str());
                if (!saveRes.has_value()) {
                    fileName.emplace(fileDialog.selected_fn);
                    gen.emplace(defGen);
                } else {
                    std::cout << "Todo: Implement errors" << std::endl;
                }
            }

            if (fileDialog.showFileDialog(
                "Open File", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,
                ImVec2(g_width, g_height), ".wu"
            )) {
                openFile = false;
                const auto readGen = natevolve::wordup::Generator::fromFile(
                    fileDialog.selected_fn.c_str()
                );

                if (!natevolve::isErr(readGen)) {
                    fileName.emplace(fileDialog.selected_fn);
                    gen.emplace(natevolve::ok(readGen));
                } else {
                    std::cout << "Todo: Implement errors" << std::endl;
                }
            }

            if (!ImGui::IsPopupOpen("New File")) {
                // Cancel
                createNewFile = false;
            }
            if (!ImGui::IsPopupOpen("Open File")) {
                // Cancel
                openFile = false;
            }

            ImGui::End();
        } else {
            // Show the currently opened file and a button to close it
            ImGui::Begin(
                " ", nullptr,
                ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize
            );
            ImGui::BeginMenuBar();
            ImGui::Text("Opened: %s", fileName.value().c_str());
            bool closeFile = false;
            if (ImGui::Button("Close File")) {
                closeFile = true;
            }
            ImGui::EndMenuBar();
            ImGui::End();
            if (closeFile) {
                fileName.reset();
                gen.reset();
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

