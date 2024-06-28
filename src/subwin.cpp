// Implement subwindow functionality

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
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
#define IPA_NUM_EXTRA       10
#define IPA_VOWELS_NCOLS    5
#define IPA_VOWELS_NROWS    7

static const ImVec2 canClosePos(60, 60);
static const ImVec2 inventoryPos(60, 160);
static const ImVec2 categoryPos(60, 220);
static const ImVec2 onsetPos(60, 280);
static const ImVec2 codaPos(60, 340);
static const ImVec2 genPos(60, 400);
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
static const wchar_t *ipaConsExtra[IPA_NUM_EXTRA] = {
    L"w", L"ʍ", L"ɥ", L"ʜ", L"ʢ", L"ʡ", L"ɕ", L"ʑ", L"ɹ", L"ɧ"
};
static const char *ipaVowelColHdrs[IPA_VOWELS_NCOLS] = {
    "Front", "", "Central", "", "Back"
};
static const char *ipaVowelRowHdrs[IPA_VOWELS_NROWS] = {
    "Close", "", "Close-Mid", "", "Open-Mid", "", "Open"
};
static const wchar_t *ipaVowels[IPA_VOWELS_NROWS][IPA_VOWELS_NCOLS][2] = {
    { { L"i", L"y" },   { L"", L"" },   { L"ɨ", L"ʉ" }, { L"", L"" },   { L"ɯ", L"u" } },
    { { L"", L"" },     { L"ɪ", L"ʏ" }, { L"", L"" },   { L"ʊ", L"" },  { L"", L"" } },
    { { L"e", L"ø"},    { L"", L"" },   { L"ɘ", L"ɵ" }, { L"", L"" },   { L"ɤ", L"o" } },
    { { L"", L"" },     { L"", L"" },   { L"ə", L"" },  { L"", L"" },   { L"", L"" } },
    { { L"ɛ", L"œ" },   { L"", L"" },   { L"ɜ", L"ɞ" }, { L"", L"" },   { L"ʌ", L"ɔ" } },
    { { L"æ", L"" },    { L"", L"" },   { L"ɐ", L"" },  { L"", L"" },   { L"", L"" } },
    { { L"a", L"ɶ" },   { L"", L"" },   { L"", L"" },   { L"", L"" },   { L"ɑ", L"ɒ" } }
};

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
    ImGui::Spacing();
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
        ImGui::SetWindowCollapsed(true);
    }

    bool changed = false;

    // -------- Main Consonant Table --------

    const auto cons = state.gen.value().categories.at(L"C"); // No error handl should be needed
    std::vector<std::wstring> newCons;
    if (ImGui::BeginTable("Consonants", IPA_CONS_NCOLS + 1, ImGuiTableFlags_Borders)) {
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
                    if ((inCons && !toggle) || (!inCons && toggle)) { // Untouch or switch to prssd
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
                    if ((inCons && !toggle) || (!inCons && toggle)) { // Untouch or switch to prssd
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
    }

    // -------- Extra Symbols --------

    ImGui::Separator();
    ImGui::Text("Extra Consonants:");
    ImGui::PushFont(global::fontCharisSil);
    for (size_t col = 0; col < IPA_NUM_EXTRA; col++) {
        ImGui::SameLine();
        const auto find = std::find(cons.begin(), cons.end(), ipaConsExtra[col]);
        const auto inCons = find != cons.end();
        const auto toggle = ImGui::Selectable(
            natevolve::fromWstr(ipaConsExtra[col]).c_str(), inCons,
            0, ImVec2(IPA_BTN_WIDTH, 0)
        );
        if ((inCons && !toggle) || (!inCons && toggle)) { // Untouched or switched to prssd
            newCons.push_back(ipaConsExtra[col]);
        }
        if (toggle) {
            changed = true;
        }
    }
    ImGui::PopFont();

    // -------- Vowels --------

    ImGui::Separator();
    const auto vowels = state.gen.value().vowels;
    std::vector<std::wstring> newVwls;
    if (ImGui::BeginTable("Vowels", IPA_VOWELS_NCOLS + 1, ImGuiTableFlags_Borders)) {
        ImGui::TableNextColumn();
        ImGui::Text(" ");
        for (size_t col = 0; col < IPA_VOWELS_NCOLS; col++) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", ipaVowelColHdrs[col]);
        }
        for (size_t row = 0; row < IPA_VOWELS_NROWS; row++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", ipaVowelRowHdrs[row]);
            ImGui::PushFont(global::fontCharisSil);
            for (size_t col = 0; col < IPA_VOWELS_NCOLS; col++) {
                ImGui::TableNextColumn();
                if (ipaVowels[row][col][0] != std::wstring(L"")) {
                    const auto find =
                        std::find(vowels.begin(), vowels.end(), ipaVowels[row][col][0]);
                    const auto inVwls = find != vowels.end();
                    const auto toggle = ImGui::Selectable(
                        natevolve::fromWstr(ipaVowels[row][col][0]).c_str(), inVwls,
                        0, ImVec2(IPA_BTN_WIDTH, 0)
                    );
                    if ((inVwls && !toggle) || (!inVwls && toggle)) { // Untouch or switch to prssd
                        newVwls.push_back(ipaVowels[row][col][0]);
                    }
                    if (toggle) {
                        changed = true;
                    }
                }
                if (ipaVowels[row][col][1] != std::wstring(L"")) {
                    ImGui::SameLine();
                    const auto find =
                        std::find(vowels.begin(), vowels.end(), ipaVowels[row][col][1]);
                    const auto inVwls = find != vowels.end();
                    const auto toggle = ImGui::Selectable(
                        natevolve::fromWstr(ipaVowels[row][col][1]).c_str(), inVwls,
                        0, ImVec2(IPA_BTN_WIDTH, 0)
                    );
                    if ((inVwls && !toggle) || (!inVwls && toggle)) { // Untouch or switch to prssd
                        newVwls.push_back(ipaVowels[row][col][1]);
                    }
                    if (toggle) {
                        changed = true;
                    }
                }
            }
            ImGui::PopFont();
        }
        ImGui::EndTable();
    }

    ImGui::End();

    // Update if changed
    if (changed) {
        auto categories = state.gen.value().categories;
        categories[L"C"] = newCons;
        auto newGen = natevolve::wordup::Generator(
            categories,
            newVwls,
            state.gen.value().onsetOptions,
            state.gen.value().codaOptions
        );
        newGen.toFile(state.fileName.value().c_str());
        state.gen.emplace(newGen);
    }
}

void subwin::categoryMaker(AppState &state) {
    ImGui::Begin("Consonant Groupings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(categoryPos);
        ImGui::SetWindowCollapsed(true);
    }

    if (state.gen.value().categories.size() > 1) {
        std::vector<std::wstring> catsNoC;
        for (const auto &cat : state.gen.value().categories) {
            if (cat.first == L"C") {
                continue;
            }
            catsNoC.push_back(cat.first);
        }
        if (state.selCat == L"") {
            state.selCat = catsNoC[0];
        }

        // -------- Current Category --------

        if (ImGui::BeginCombo("Category", natevolve::fromWstr(state.selCat).c_str())) {
            for (const auto &cat : catsNoC) {
                if (ImGui::Selectable(natevolve::fromWstr(cat).c_str(), state.selCat == cat)) {
                    state.selCat = cat;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        auto shouldRemove = ImGui::Button("Remove");

        if (shouldRemove) {
            auto categories = state.gen.value().categories;
            categories.erase(state.selCat);
            state.selCat = L"";
            auto newGen = natevolve::wordup::Generator(
                categories,
                state.gen.value().vowels,
                state.gen.value().onsetOptions,
                state.gen.value().codaOptions
            );
            newGen.toFile(state.fileName.value().c_str());
            state.gen.emplace(newGen);
        } else {
            // -------- Buttons to add consonants to a category --------

            bool changed = false;
            std::vector<std::wstring> newCons;
            ImGui::Text("Available:");
            ImGui::PushFont(global::fontCharisSil);
            const auto curr = state.gen.value().categories.at(state.selCat);
            for (const auto &cons : state.gen.value().categories.at(L"C")) {
                ImGui::SameLine();
                const auto find = std::find(curr.begin(), curr.end(), cons);
                const auto inCurr = find != curr.end();
                const auto toggle = ImGui::Selectable(
                    natevolve::fromWstr(cons).c_str(), inCurr,
                    0, ImVec2(IPA_BTN_WIDTH, 0)
                );
                if ((inCurr && !toggle) || (!inCurr && toggle)) { // Untouch or switch to prssd
                    newCons.push_back(cons);
                }
                if (toggle) {
                    changed = true;
                }
            }
            ImGui::PopFont();

            if (changed) {
                auto categories = state.gen.value().categories;
                categories[state.selCat] = newCons;
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
    }

    // -------- New Category --------

    char catNameBuff[5] = { 0 };
    const auto textSubmit = ImGui::InputTextWithHint(
        "New Category Name", "<=4 chars, type and press Enter", catNameBuff, 5,
        ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue 
    );
    if (textSubmit && std::string(catNameBuff) != "") {
        auto newCats = state.gen.value().categories;
        newCats[natevolve::toWstr(std::string(catNameBuff))] = std::vector<std::wstring>({});
        auto newGen = (natevolve::wordup::Generator) {
            newCats,
            state.gen.value().vowels,
            state.gen.value().onsetOptions,
            state.gen.value().codaOptions
        };
        newGen.toFile(state.fileName.value().c_str());
        state.gen.emplace(newGen);
    }

    ImGui::End();
}

void subwin::onsetMaker(AppState &state) {
    ImGui::Begin("Onsets", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(onsetPos);
        ImGui::SetWindowCollapsed(true);
    }

    if (state.gen.value().onsetOptions.size() > 1) {
        for (size_t i = 1; i < state.gen.value().onsetOptions.size(); i++) {
            bool changed = false;
            auto onset = state.gen.value().onsetOptions[i];
            for (size_t j = 0; j < onset.size(); j++) {
                if (j != 0) {
                    ImGui::SameLine();
                }
                ImGui::Text("%s", natevolve::fromWstr(onset[j]).c_str());
            }
            bool same = onset.size() > 0;
            for (const auto &cat : state.gen.value().categories) {
                if (!same) {
                    same = true;
                } else {
                    ImGui::SameLine();
                }
                ImGui::PushID(i * 4);
                if (ImGui::Button(("+" + natevolve::fromWstr(cat.first)).c_str())) {
                    onset.push_back(cat.first);
                    changed = true;
                }
                ImGui::PopID();
            }
            ImGui::SameLine();
            ImGui::PushID(i * 4 + 1);
            if (ImGui::Button("-")) {
                onset.pop_back();
                changed = true;
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(i * 4 + 2);
            if (ImGui::Button("Remove")) {
                auto onsets = state.gen.value().onsetOptions;
                onsets.erase(onsets.begin() + i);
                auto newGen = (natevolve::wordup::Generator) {
                    state.gen.value().categories,
                    state.gen.value().vowels,
                    onsets,
                    state.gen.value().codaOptions
                };
                newGen.toFile(state.fileName.value().c_str());
                state.gen.emplace(newGen);
            }
            ImGui::PopID();
            ImGui::PushID(i * 4 + 3);
            if (changed) {
                auto onsets = state.gen.value().onsetOptions;
                onsets[i] = onset;
                auto newGen = (natevolve::wordup::Generator) {
                    state.gen.value().categories,
                    state.gen.value().vowels,
                    onsets,
                    state.gen.value().codaOptions
                };
                newGen.toFile(state.fileName.value().c_str());
                state.gen.emplace(newGen);
            }
            ImGui::PopID();
        }
    }

    // -------- Onset Option --------

    if (ImGui::Button("New Onset")) {
        auto onsets = state.gen.value().onsetOptions;
        onsets.push_back(std::vector<std::wstring>({ L"C" }));
        auto newGen = (natevolve::wordup::Generator) {
            state.gen.value().categories,
            state.gen.value().vowels,
            onsets,
            state.gen.value().codaOptions
        };
        newGen.toFile(state.fileName.value().c_str());
        state.gen.emplace(newGen);
    }

    ImGui::End();
}

void subwin::codaMaker(AppState &state) {
    ImGui::Begin("Codas", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(codaPos);
        ImGui::SetWindowCollapsed(true);
    }

    if (state.gen.value().codaOptions.size() > 1) {
        for (size_t i = 1; i < state.gen.value().codaOptions.size(); i++) {
            bool changed = false;
            auto coda = state.gen.value().codaOptions[i];
            for (size_t j = 0; j < coda.size(); j++) {
                if (j != 0) {
                    ImGui::SameLine();
                }
                ImGui::Text("%s", natevolve::fromWstr(coda[j]).c_str());
            }
            bool same = coda.size() > 0;
            for (const auto &cat : state.gen.value().categories) {
                if (!same) {
                    same = true;
                } else {
                    ImGui::SameLine();
                }
                ImGui::PushID(i * 4);
                if (ImGui::Button(("+" + natevolve::fromWstr(cat.first)).c_str())) {
                    coda.push_back(cat.first);
                    changed = true;
                }
                ImGui::PopID();
            }
            ImGui::SameLine();
            ImGui::PushID(i * 4 + 1);
            if (ImGui::Button("-")) {
                coda.pop_back();
                changed = true;
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(i * 4 + 2);
            if (ImGui::Button("Remove")) {
                auto codas = state.gen.value().codaOptions;
                codas.erase(codas.begin() + i);
                auto newGen = (natevolve::wordup::Generator) {
                    state.gen.value().categories,
                    state.gen.value().vowels,
                    state.gen.value().onsetOptions,
                    codas
                };
                newGen.toFile(state.fileName.value().c_str());
                state.gen.emplace(newGen);
            }
            ImGui::PopID();
            ImGui::PushID(i * 4 + 3);
            if (changed) {
                auto codas = state.gen.value().codaOptions;
                codas[i] = coda;
                auto newGen = (natevolve::wordup::Generator) {
                    state.gen.value().categories,
                    state.gen.value().vowels,
                    state.gen.value().onsetOptions,
                    codas
                };
                newGen.toFile(state.fileName.value().c_str());
                state.gen.emplace(newGen);
            }
            ImGui::PopID();
        }
    }

    // -------- Onset Option --------

    if (ImGui::Button("New Coda")) {
        auto codas = state.gen.value().codaOptions;
        codas.push_back(std::vector<std::wstring>({ L"C" }));
        auto newGen = (natevolve::wordup::Generator) {
            state.gen.value().categories,
            state.gen.value().vowels,
            state.gen.value().onsetOptions,
            codas
        };
        newGen.toFile(state.fileName.value().c_str());
        state.gen.emplace(newGen);
    }

    ImGui::End();
}

void subwin::generate(AppState &state) {
    ImGui::Begin("Generate", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (!std::filesystem::exists("imgui.ini")) {
        // Give it a non-overlapping default pos
        ImGui::SetWindowPos(genPos);
        ImGui::SetWindowCollapsed(true);
    }

    ImGui::Text("Generated Word: ");
    ImGui::SameLine();
    ImGui::PushFont(global::fontCharisSil);
    ImGui::Text("%ls", state.newWord.c_str());
    ImGui::PopFont();

    ImGui::Text("Num Syllables: %lu", state.numSyllables);
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        state.numSyllables++;
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) {
        if (state.numSyllables > 0) {
            state.numSyllables--;
        }
    }

    if (ImGui::Button("Generate")) {
        std::wstringstream newWord;
        for (size_t i = 0; i < state.numSyllables; i++) {
            const auto result = state.gen.value().generate();
            if (natevolve::isErr(result)) {
                state.errMessage.emplace(
                    L"Failed to generate a new word. Err: " + natevolve::err(result).message
                );
                break;
            } else {
                if (newWord.str() != L"") {
                    newWord << L".";
                }
                newWord << natevolve::ok(result);
            }
        }
        if (!state.errMessage.has_value()) {
            state.newWord = newWord.str();
        }
    }

    ImGui::End();
}

