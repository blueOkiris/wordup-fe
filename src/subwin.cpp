// Implement subwindow functionality

#include <optional>
#include <string>
#include <vector>
#include <filesystem>
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

#define IPA_CONS_NCOLS      11
#define IPA_CONS_NROWS      12

static const ImVec2 canClosePos(60, 60);
static const ImVec2 inventoryPos(60, 150);
static const char *ipaConsColHdrs[IPA_CONS_NCOLS] = {
    "Bilab", "Labden", "Den", "Alv", "Post-Alv",
    "Rflex", "Palt", "Vel", "Uv", "Pharyn",
    "Glot"
};
static const char *ipaConsRowHdrs[IPA_CONS_NROWS] = {
    "Stop", "Nas", "Trill", "Tap", "Fric", "Lat Fric", "Affr",
    "Approx", "Lat Approx", "Impl", "Click", "Eject"
};
static const wchar_t *ipaConsTbl[IPA_CONS_NROWS][IPA_CONS_NCOLS][2] = {
    {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" },
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" },
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" },
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" },
    }, {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" },
    }
};
static const std::wstring ipaConsExtra[] = { L"w" };

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
    if (!std::filesystem::exists("imgui.ini")) {
        ImGui::SetWindowPos(canClosePos);
    }
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
    ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(inventoryPos);
    }

    ImGui::BeginTable("Consonants", IPA_CONS_NCOLS + 1);

    ImGui::TableNextColumn();
    ImGui::Text(" ");
    for (size_t col = 0; col < IPA_CONS_NCOLS; col++) {
        ImGui::TableNextColumn();
        ImGui::Text("%s", ipaConsColHdrs[col]);
    }
    for (size_t row = 0; row < IPA_CONS_NROWS; row++) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", ipaConsRowHdrs[row]);
        for (size_t col = 0; col < IPA_CONS_NCOLS; col++) {
            ImGui::TableNextColumn();
            ImGui::Text("%ls", ipaConsTbl[row][col][0]);
            ImGui::TableNextColumn();
            ImGui::Text("%ls", ipaConsTbl[row][col][0]);
        }
    }
    ImGui::EndTable();

    ImGui::End();
}

