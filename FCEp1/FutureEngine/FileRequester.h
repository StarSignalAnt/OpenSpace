#pragma once
#include <string>
#include <vector>

// Simple helper class for opening file dialogs
class FileRequester
{
public:
    // Structure to define file filters
    struct FileFilter {
        std::string Description;
        std::string Extension;

        FileFilter(const std::string& desc, const std::string& ext)
            : Description(desc), Extension(ext) {
        }
    };

    // Opens a file open dialog and returns the selected file path
    // Returns empty string if canceled
    static std::string OpenFile(
        const std::string& title = "Open File",
        const std::vector<FileFilter>& filters = {},
        const std::string& initialDir = ""
    );

    // Opens a file save dialog and returns the selected file path
    // Returns empty string if canceled
    static std::string SaveFile(
        const std::string& title = "Save File",
        const std::vector<FileFilter>& filters = {},
        const std::string& defaultName = "",
        const std::string& initialDir = ""
    );

    // Opens a folder selection dialog and returns the selected folder path
    // Returns empty string if canceled
    static std::string SelectFolder(
        const std::string& title = "Select Folder",
        const std::string& initialDir = ""
    );
    static std::vector<std::string> OpenMultipleFiles(
        const std::string& title,
        const std::vector<FileRequester::FileFilter>& filters,
        const std::string& initialDir = ""
    );

private:
    // Helper function to construct filter string in the format needed by the platform
    static std::string BuildFilterString(const std::vector<FileFilter>& filters);
};