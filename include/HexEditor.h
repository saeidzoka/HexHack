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

    void RenderHexView();
    void RenderASCIIView();

    char NibbleToChar(uint8_t nibble);
    uint8_t CharToNibble(char c);

};