#include "fileedit.h"
#include <memory>
#include <fstream>

void FileEditor::tryLoadFile()
{
    auto &file = window.file;
    if (!file.empty()) {
        std::error_code ec;
        file.assign(std::filesystem::absolute(file, ec));
        if (!ec)
            loadFile();
        else
            window.error = fmt::format("'{}' is not a valid path.", file.native());
    }
}

void FileEditor::loadFile()
{
    auto &file = window.file;
    std::ifstream f(file, ios::in | ios::binary);
    if (f) {
        bool ok;
        f.seekg(0, ios::end);
        size_t fSize = f.tellg();
        f.seekg(0);
        // Allocate 1000 extra bytes, as in SciTE.
        editor.WndProc(SCI_ALLOCATE, fSize + 1000, 0U);
        if (fSize > (1 << 20))
            // Disable word wrap on big files.
            editor.WndProc(SCI_SETWRAPMODE, SC_WRAP_NONE, 0U);
        if (fSize) {
            constexpr size_t blockSize = 1 << 20; // Read in chunks of 1 MiB.
            size_t readSize = std::min(fSize, blockSize);
            std::unique_ptr<char[]> buffer {new char[readSize]};
            sptr_t wParam = reinterpret_cast<sptr_t>(buffer.get());
            while (fSize > 0 && (ok = bool(f.read(buffer.get(), readSize)))) {
                editor.WndProc(SCI_APPENDTEXT, readSize, wParam);
                fSize -= readSize;
                if (fSize < readSize)
                    readSize = fSize;
            };
            if (!ok)
                window.error = fmt::format("An error occurred while reading file '{}'.", file.native());
        }
    } else
        window.error = fmt::format("Unable to open file '{}'.", file.native());
}
