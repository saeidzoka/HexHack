#pragma once
#include <cstdint>
#include <string>
#include <vector>

class HexEditor
{
public:
    HexEditor();
    ~HexEditor();

    void Render();
    void RenderMenuBar();
    void RenderStatusBar();
    void RenderOffsetPanel();

    bool LoadFile(const std::string &filepath);
    bool SaveFile(const std::string &filepath);
    void NewFile(size_t size = 256);

private:
    std::vector<uint8_t> m_Data;
    std::string m_CurrentFile;
    bool m_IsModified;

    size_t m_SelectedByte;
    bool m_EditingNibble;
    int m_BytesPerRow;
    uint64_t m_AddressOffset;

    void RenderHexView();
    void RenderASCIIView();

    char NibbleToChar(uint8_t nibble);
    uint8_t CharToNibble(char c);

    // Show native file dialogs (Windows)
    std::string ShowOpenFileDialog();
    std::string ShowSaveFileDialog(const char* defaultName = nullptr);

};