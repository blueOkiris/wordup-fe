// Implement subwindow functionality

#include <optional>
#include <imgui.h>
#include <string>
#include <vector>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <ImGuiFileBrowser.h>
#include <natevolve.hpp>
#include <wordup.hpp>
#include <global.hpp>
#include <subwin.hpp>

// Show a popup with an error message
void subwin::errorPopup(sf::RenderWindow &win, std::optional<std::wstring> &errMessage) {
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
void subwin::fileOpen(
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
        ImVec2(global::width, global::height), ".wu"
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
        ImVec2(global::width, global::height), ".wu"
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
bool subwin::fileOpenedCanClose(
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

