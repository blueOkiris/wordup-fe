// Implement subwindow functionality

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
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
#define IPA_BTN_WIDTH       15

static const ImVec2 canClosePos(60, 60);
static const ImVec2 inventoryPos(60, 150);
static const char *ipaConsColHdrs[IPA_CONS_NCOLS] = {
    "Bilab", "Lab-Den", "Dental", "Alveolr", "Post-Alv",
    "Retflex", "Palatl", "Velar", "Uvulr", "Phryg",
    "Glottl"
};
static const char *ipaConsRowHdrs[IPA_CONS_NROWS] = {
    "Stop", "Nasal", "Trill", "Tap", "Frictv", "Lat Fric", "Affr",
    "Approx", "Lat Apx", "Implosv", "Click", "Ejectv"
};
static const wchar_t *ipaConsTbl[IPA_CONS_NROWS][IPA_CONS_NCOLS][2] = {
    {
        { L"p", L"b" }, { L"", L"" }, { L"t̪", L"d̪" }, { L"t", L"d" }, { L"t̺", L"d̺" },
        { L"ʈ", L"ɖ" }, { L"c", L"ɟ" }, { L"k", L"g" }, { L"q", L"ɢ" }, { L"", L"" },
        { L"ʔ", L"" }
    }, {
        { L"m", L"" }, { L"ɱ", L"" }, { L"", L"" }, { L"n", L"" }, { L"", L"" },
        { L"ɳ", L"" }, { L"ɲ", L"" }, { L"ŋ", L"" }, { L"ɴ", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"ʙ", L"" }, { L"", L"" }, { L"", L"" }, { L"r", L"" }, { L"", L"" },
        { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"ʀ", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"", L"" }, { L"ⱱ", L"" }, { L"", L"" }, { L"ɾ", L"" }, { L"", L"" },
        { L"ɽ", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"ɸ", L"β" }, { L"f", L"v" }, { L"θ", L"ð" }, { L"s", L"z" }, { L"ʃ", L"ʒ" },
        { L"ʂ", L"ʐ" }, { L"ç", L"ʝ" }, { L"x", L"ɣ" }, { L"χ", L"ʁ" }, { L"ħ", L"ʕ" },
        { L"h", L"ɦ" }
    }, {
        { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"ɫ", L"ɮ" }, { L"", L"" },
        { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"p͡f", L"b͡v" }, { L"", L"" }, { L"t͡θ", L"d͡ð" }, { L"t͡s", L"d͡z" }, { L"t͡ʃ", L"d͡ʒ" },
        { L"ʈ͡ʂ", L"ɖ͡ʐ" }, { L"c͡ç", L"ɟ͡ʝ" }, { L"k͡x", L"g͡ɣ" }, { L"q͡χ", L"ɢ͡ʁ" }, { L"", L"" },
        { L"ʔ͡h", L"" }
    }, {
        { L"", L"" }, { L"ʋ", L"" }, { L"", L"" }, { L"ɹ", L"" }, { L"", L"" },
        { L"ɻ", L"" }, { L"j", L"" }, { L"ɰ", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"l", L"" }, { L"", L"" },
        { L"ɭ", L"" }, { L"ʎ", L"" }, { L"ʟ", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"ɓ", L"" }, { L"", L"" }, { L"", L"" }, { L"ɗ", L"" }, { L"", L"" },
        { L"", L"" }, { L"ʄ", L"" }, { L"ɠ", L"" }, { L"ʛ", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"ʘ", L"" }, { L"", L"" }, { L"ǀ", L"" }, { L"ǃ", L"ǁ" }, { L"ǂ", L"" },
        { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
    }, {
        { L"pʼ", L"" }, { L"", L"" }, { L"", L"" }, { L"tʼ", L"" }, { L"", L"" },
        { L"", L"" }, { L"", L"" }, { L"kʼ", L"" }, { L"", L"" }, { L"", L"" },
        { L"", L"" }
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

        std::map<std::wstring, std::vector<std::wstring>> defCats({
            { L"C", std::vector<std::wstring>({}) }
        });
        std::vector<std::wstring> defVwls;
        std::vector<std::vector<std::wstring>> defOnsets({ std::vector<std::wstring>({ L"∅" }) });
        std::vector<std::vector<std::wstring>> defCodas({ std::vector<std::wstring>({ L"∅" }) });
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
            if (natevolve::ok(readGen).categories.count(L"C") < 1
                    || std::find(
                        natevolve::ok(readGen).onsetOptions.begin(),
                        natevolve::ok(readGen).onsetOptions.end(), 
                        std::vector<std::wstring>({ L"∅" })
                    ) == natevolve::ok(readGen).onsetOptions.end()
                    || std::find(
                        natevolve::ok(readGen).codaOptions.begin(),
                        natevolve::ok(readGen).codaOptions.end(), 
                        std::vector<std::wstring>({ L"∅" })
                    ) == natevolve::ok(readGen).codaOptions.end()) {
                state.errMessage.emplace(L"Corrupted file");
            } else {
                state.fileName.emplace(state.fileDialog.selected_fn);
                state.gen.emplace(natevolve::ok(readGen));
            }
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
    ImGui::Separator();
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
    ImGui::Begin(
        "Inventory", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse
    );

    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(inventoryPos);
    }

    ImGui::BeginTable(
        "Consonants", IPA_CONS_NCOLS + 1,
        ImGuiTableFlags_Borders
    );

    bool changed = false;
    const auto cons = state.gen.value().categories.at(L"C"); // No error handling should be needed
    std::vector<std::wstring> newCons;

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
        ImGui::PushFont(global::fontCharisSil);
        for (size_t col = 0; col < IPA_CONS_NCOLS; col++) {
            ImGui::TableNextColumn();
            if (ipaConsTbl[row][col][0] != std::wstring(L"")) {
                const auto find = std::find(cons.begin(), cons.end(), ipaConsTbl[row][col][0]);
                const auto inCons = find != cons.end();
                const auto toggle = ImGui::Selectable(
                    natevolve::fromWstr(ipaConsTbl[row][col][0]).c_str(), inCons,
                    0, ImVec2(IPA_BTN_WIDTH, 0)
                );
                if ((inCons && !toggle) || (!inCons && toggle)) { // Untouched or switched to prssd
                    newCons.push_back(ipaConsTbl[row][col][0]);
                }
                if (toggle) {
                    changed = true;
                }
            }
            if (ipaConsTbl[row][col][1] != std::wstring(L"")) {
                ImGui::SameLine();
                const auto find = std::find(cons.begin(), cons.end(), ipaConsTbl[row][col][1]);
                const auto inCons = find != cons.end();
                const auto toggle = ImGui::Selectable(
                    natevolve::fromWstr(ipaConsTbl[row][col][1]).c_str(), inCons,
                    0, ImVec2(IPA_BTN_WIDTH, 0)
                );
                if ((inCons && !toggle) || (!inCons && toggle)) { // Untouched or switched to prssd
                    newCons.push_back(ipaConsTbl[row][col][1]);
                }
                if (toggle) {
                    changed = true;
                }
            }
        }
        ImGui::PopFont();
    }
    ImGui::EndTable();
    ImGui::End();

    // Update if changed
    if (changed) {
        auto categories = state.gen.value().categories;
        categories[L"C"] = newCons;
        auto newGen = natevolve::wordup::Generator(
            categories,
            state.gen.value().vowels,
            state.gen.value().onsetOptions,
            state.gen.value().codaOptions
        );
        newGen.toFile(state.fileName.value().c_str());
        state.gen.emplace(newGen);
    }
}

