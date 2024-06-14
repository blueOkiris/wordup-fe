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
#include <state.hpp>

void subwin::errorPopup(AppState &state) {
    ImGui::Begin("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (state.errMessage.has_value()) {
        ImGui::Text("Error: %ls\n", state.errMessage.value().c_str());
        if (ImGui::Button("Close")) {
            state.errMessage.reset();
        }
    }
    ImGui::End();
}

void subwin::fileOpen(AppState &state) {
    // Need to open a file first
    ImGui::Begin("Choose File", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("New File")) {
        state.spawnNewFilePopup = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Open File")) {
        state.spawnOpenFilePopup = true;
    }

    ImGui::End();

    if (state.spawnNewFilePopup) {
        ImGui::OpenPopup("Save File");
    }
    if (state.spawnOpenFilePopup) {
        ImGui::OpenPopup("Open File");
    }

    if (state.fileDialog.showFileDialog(
        "Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
        ImVec2(global::width, global::height), ".wu"
    )) {
        state.spawnNewFilePopup = false;

        std::map<std::wstring, std::vector<std::wstring>> defCats;
        std::vector<std::wstring> defVwls;
        std::vector<std::vector<std::wstring>> defOnsets;
        std::vector<std::vector<std::wstring>> defCodas;
        const natevolve::wordup::Generator defGen(
            defCats, defVwls, defOnsets, defCodas
        );
        const auto saveRes = defGen.toFile(state.fileDialog.selected_fn.c_str());
        if (!saveRes.has_value()) {
            state.fileName.emplace(state.fileDialog.selected_fn);
            state.gen.emplace(defGen);
        } else {
            state.errMessage.emplace(
                L"Failed to save new file '" + natevolve::toWstr(state.fileDialog.selected_fn)
                    + L"'\nErr: " + saveRes.value().message
            );
        }
    }

    if (state.fileDialog.showFileDialog(
        "Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
        ImVec2(global::width, global::height), ".wu"
    )) {
        state.spawnOpenFilePopup = false;
        const auto readGen = natevolve::wordup::Generator::fromFile(
            state.fileDialog.selected_fn.c_str()
        );

        if (!natevolve::isErr(readGen)) {
            state.fileName.emplace(state.fileDialog.selected_fn);
            state.gen.emplace(natevolve::ok(readGen));
        } else {
            state.errMessage.emplace(
                L"Failed to open file '" + natevolve::toWstr(state.fileDialog.selected_fn)
                    + L"'\nErr: " + natevolve::err(readGen).message
            );
        }
    }

    if (!ImGui::IsPopupOpen("New File")) {
        // Cancel
        state.spawnNewFilePopup = false;
    }
    if (!ImGui::IsPopupOpen("Open File")) {
        // Cancel
        state.spawnOpenFilePopup = false;
    }
}

void subwin::fileOpenedCanClose(AppState &state) {
    ImGui::Begin("Current File Opened", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(canClosePos);
    ImGui::Text("Name: %s", state.fileName.value().c_str());
    ImGui::SameLine();
    bool closeFile = false;
    if (ImGui::Button("Close File")) {
        closeFile = true;
    }
    ImGui::End();

    if (closeFile) {
        state.fileName.reset();
        state.gen.reset();
    }
}

void subwin::ipaSelect(AppState &state) {
    ImGui::Begin("Inventory");
    ImGui::SetWindowPos(inventoryPos);

    ImGui::Selectable("p");

    ImGui::End();
}

