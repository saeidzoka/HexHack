#include "HexEditor.h"
#include "imgui.h"
#include <fstream>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#endif


HexEditor::HexEditor()
    : m_IsModified(false)
    , m_SelectedByte(0)
    , m_EditingNibble(false)
    , m_BytesPerRow(16)
    , m_AddressOffset(0)
    {
        NewFile(256);
    }

HexEditor::~HexEditor(){

}

void HexEditor::Render(){
    RenderOffsetPanel();
    RenderHexView();
}

void HexEditor::RenderOffsetPanel() {
    ImGui::BeginChild("OffsetPanel", ImVec2(0, ImGui::GetFrameHeightWithSpacing()), false);
    // Show offset in hex and decimal; both edit the same value.
    ImGui::Text("Address Offset:");
    ImGui::SameLine();
    ImGui::PushItemWidth(150);
    ImGui::InputScalar("##offset_hex", ImGuiDataType_U64, &m_AddressOffset, nullptr, nullptr, "%08llX");
    ImGui::SameLine();
    ImGui::InputScalar("##offset_dec", ImGuiDataType_U64, &m_AddressOffset, nullptr, nullptr, "%llu");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        m_AddressOffset = 0;
    }
    ImGui::EndChild();
}

void HexEditor::RenderMenuBar() {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            NewFile();
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            std::string path = ShowOpenFileDialog();
            if (!path.empty()) {
                LoadFile(path);
            }
        }
        if (ImGui::MenuItem("Save", "Ctrl+S", false, m_IsModified)) {
            if (!m_CurrentFile.empty()) {
                SaveFile(m_CurrentFile);
            }
        }
        if (ImGui::MenuItem("Save As")) {
            std::string path = ShowSaveFileDialog(m_CurrentFile.empty() ? "untitled.bin" : m_CurrentFile.c_str());
            if (!path.empty()) {
                SaveFile(path);
            }
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) {
            // Handle exit
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {
            // TODO: Implement undo
        }
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {
            // TODO: Implement redo
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("8 bytes per row", nullptr, m_BytesPerRow == 8)) {
            m_BytesPerRow = 8;
        }
        if (ImGui::MenuItem("16 bytes per row", nullptr, m_BytesPerRow == 16)) {
            m_BytesPerRow = 16;
        }
        if (ImGui::MenuItem("32 bytes per row", nullptr, m_BytesPerRow == 32)) {
            m_BytesPerRow = 32;
        }
        ImGui::EndMenu();
    }
}

void HexEditor::RenderHexView() {
    if (m_Data.empty()) {
        ImGui::Text("No data loaded");
        return;
    }
    
    ImGuiStyle& style = ImGui::GetStyle();
    float glyphWidth = ImGui::CalcTextSize("F").x;
    float cellWidth = glyphWidth * 2.5f;
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
    
    // Calculate number of rows
    size_t numRows = (m_Data.size() + m_BytesPerRow - 1) / m_BytesPerRow;
    
    ImGuiListClipper clipper;
    clipper.Begin(numRows);
    
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            size_t offset = row * m_BytesPerRow;
            
            // Address column (apply offset)
            unsigned long long displayAddr = (unsigned long long)(m_AddressOffset + offset);
            ImGui::Text("%08llX:", displayAddr);
            ImGui::SameLine();
            
            // Hex bytes
            for (int col = 0; col < m_BytesPerRow; col++) {
                size_t index = offset + col;
                
                if (index >= m_Data.size()) {
                    ImGui::Text("  ");
                } else {
                    ImGui::PushID(index);

                    // Highlight selected byte. Track whether we pushed so we only pop when appropriate.
                    bool pushedStyle = false;
                    if (index == m_SelectedByte) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                        pushedStyle = true;
                    }

                    char buf[3];
                    snprintf(buf, sizeof(buf), "%02X", m_Data[index]);

                    if (ImGui::Button(buf, ImVec2(cellWidth, 0))) {
                        m_SelectedByte = index;
                    }

                    if (pushedStyle) {
                        ImGui::PopStyleColor();
                    }

                    ImGui::PopID();
                }
                
                // Add spacing every 8 bytes
                if ((col + 1) % 8 == 0 && col + 1 < m_BytesPerRow) {
                    ImGui::SameLine(0, 12);
                } else {
                    ImGui::SameLine();
                }
            }
            
            // ASCII representation
            ImGui::SameLine(0, 20);
            ImGui::Text("|");
            ImGui::SameLine();
            
            for (int col = 0; col < m_BytesPerRow; col++) {
                size_t index = offset + col;
                if (index >= m_Data.size()) break;
                
                uint8_t byte = m_Data[index];
                char c = (byte >= 32 && byte < 127) ? byte : '.';
                ImGui::Text("%c", c);
                ImGui::SameLine(0, 0);
            }
            ImGui::Text("|");
        }
    }
    
    ImGui::PopStyleVar();
}

void HexEditor::RenderStatusBar() {
    if (!m_Data.empty()) {
        unsigned long long selectedAddr = (unsigned long long)(m_AddressOffset + m_SelectedByte);
        ImGui::Text("Size: %zu bytes | Selected: 0x%08llX | Value: 0x%02X (%d) '%c'", 
            m_Data.size(), 
            selectedAddr,
            m_SelectedByte < m_Data.size() ? m_Data[m_SelectedByte] : 0,
            m_SelectedByte < m_Data.size() ? m_Data[m_SelectedByte] : 0,
            (m_SelectedByte < m_Data.size() && m_Data[m_SelectedByte] >= 32 && m_Data[m_SelectedByte] < 127) 
                ? (char)m_Data[m_SelectedByte] : '.');
    }
    
    if (m_IsModified) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), " [Modified]");
    }
}

bool HexEditor::LoadFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    m_Data.resize(fileSize);
    file.read(reinterpret_cast<char*>(m_Data.data()), fileSize);
    file.close();
    
    m_CurrentFile = filepath;
    m_IsModified = false;
    m_SelectedByte = 0;
    
    return true;
}

bool HexEditor::SaveFile(const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(m_Data.data()), m_Data.size());
    file.close();
    
    m_IsModified = false;
    m_CurrentFile = filepath;
    
    return true;
}

void HexEditor::NewFile(size_t size) {
    m_Data.clear();
    m_Data.resize(size, 0);
    m_CurrentFile.clear();
    m_IsModified = false;
    m_SelectedByte = 0;
    m_AddressOffset = 0;
}

char HexEditor::NibbleToChar(uint8_t nibble) {
    return nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
}

uint8_t HexEditor::CharToNibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

std::string HexEditor::ShowOpenFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    CHAR szFile[MAX_PATH] = {0};
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Binary and HEX Files\0*.bin;*.hex\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }
#endif
    return std::string();
}

std::string HexEditor::ShowSaveFileDialog(const char* defaultName) {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    CHAR szFile[MAX_PATH] = {0};
    if (defaultName) {
        strncpy_s(szFile, defaultName, _TRUNCATE);
    }
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Binary Files\0*.bin\0HEX Files\0*.hex\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileNameA(&ofn)) {
        return std::string(szFile);
    }
#endif
    return std::string();
}