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

static void initSys(sf::RenderWindow &win);
static void handleEvents(sf::RenderWindow &win, sf::Event &ev);
static void subwinErrorPopup(sf::RenderWindow &win, std::optional<std::wstring> &errMessage);
static void subwinFileOpen(
    sf::RenderWindow &win,
    std::optional<std::wstring> &errMessage,
    std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen,
    imgui_addons::ImGuiFileBrowser &fileDialog, bool &spawnNewFilePopup, bool &spawnOpenFilePopup
);
static bool subwinFileOpenedCanClose(
    sf::RenderWindow &win,
    std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen
);

int main() {
    // -------- App State --------

    sf::RenderWindow win(sf::VideoMode(g_width, g_height), g_title);

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
            subwinErrorPopup(win, errMessage);
        } else if (!fileName.has_value()) {
            subwinFileOpen(
                win,
                errMessage,
                fileName, gen,
                fileDialog, spawnNewFilePopup, spawnOpenFilePopup
            );
        } else {
            if (subwinFileOpenedCanClose(win, fileName, gen)) {
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
    win.setFramerateLimit(g_fps);
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

// Show a popup with an error message
static void subwinErrorPopup(sf::RenderWindow &win, std::optional<std::wstring> &errMessage) {
    ImGui::Begin("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (errMessage.has_value()) {
        ImGui::Text("Error: %ls\n", errMessage.value().c_str());
        if (ImGui::Button("Close")) {
            errMessage.reset();
        }
    }
    ImGui::End();
}

// Show a window that lets you open files or create a new one
static void subwinFileOpen(
        sf::RenderWindow &win,
        std::optional<std::wstring> &errMessage,
        std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen,
        imgui_addons::ImGuiFileBrowser &fileDialog,
        bool &spawnNewFilePopup, bool &spawnOpenFilePopup) {
    // Need to open a file first
    ImGui::Begin("Choose File", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("New File")) {
        spawnNewFilePopup = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Open File")) {
        spawnOpenFilePopup = true;
    }

    ImGui::End();

    if (spawnNewFilePopup) {
        ImGui::OpenPopup("Save File");
    }
    if (spawnOpenFilePopup) {
        ImGui::OpenPopup("Open File");
    }

    if (fileDialog.showFileDialog(
        "Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
        ImVec2(g_width, g_height), ".wu"
    )) {
        spawnNewFilePopup = false;

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
            errMessage.emplace(
                L"Failed to save new file '" + natevolve::toWstr(fileDialog.selected_fn)
                    + L"'\nErr: " + saveRes.value().message
            );
        }
    }

    if (fileDialog.showFileDialog(
        "Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
        ImVec2(g_width, g_height), ".wu"
    )) {
        spawnOpenFilePopup = false;
        const auto readGen = natevolve::wordup::Generator::fromFile(
            fileDialog.selected_fn.c_str()
        );

        if (!natevolve::isErr(readGen)) {
            fileName.emplace(fileDialog.selected_fn);
            gen.emplace(natevolve::ok(readGen));
        } else {
            errMessage.emplace(
                L"Failed to open file '" + natevolve::toWstr(fileDialog.selected_fn)
                    + L"'\nErr: " + natevolve::err(readGen).message
            );
        }
    }

    if (!ImGui::IsPopupOpen("New File")) {
        // Cancel
        spawnNewFilePopup = false;
    }
    if (!ImGui::IsPopupOpen("Open File")) {
        // Cancel
        spawnOpenFilePopup = false;
    }
}


// Show the currently opened file and a button to close it
static bool subwinFileOpenedCanClose(
        sf::RenderWindow &win,
        std::optional<std::string> &fileName, std::optional<natevolve::wordup::Generator> &gen) {
    ImGui::Begin(
        "Current File Opened", nullptr,
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize
    );
    ImGui::Text("Name: %s", fileName.value().c_str());
    ImGui::SameLine();
    bool closeFile = false;
    if (ImGui::Button("Close File")) {
        closeFile = true;
    }
    ImGui::End();

    if (closeFile) {
        fileName.reset();
        gen.reset();
        win.clear();
        ImGui::SFML::Render(win);
        win.display();
    }
    return !closeFile;
}

