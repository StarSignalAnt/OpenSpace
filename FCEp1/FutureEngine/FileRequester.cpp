#include "FileRequester.h"

#ifdef _WIN32
#include <windows.h>
#include <Commdlg.h>
#include <shlobj.h>
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "Shell32.lib")
#elif defined(__APPLE__)
#include <Carbon/Carbon.h>
#else
// Linux implementation would typically use GTK or Qt
// For simplicity, we'll assume a basic implementation here
#include <iostream>
#endif

std::string FileRequester::OpenFile(const std::string& title, const std::vector<FileFilter>& filters, const std::string& initialDir)
{
#ifdef _WIN32
    char filename[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);

    std::string filterStr = BuildFilterString(filters);
    ofn.lpstrFilter = filterStr.c_str();

    ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        return filename;
    }
    return "";
#elif defined(__APPLE__)
    // macOS implementation would go here
    std::cout << "Opening file dialog on macOS is not implemented in this example." << std::endl;
    return "";
#else
    // Linux implementation would go here
    std::cout << "Opening file dialog on Linux is not implemented in this example." << std::endl;
    return "";
#endif
}

std::string FileRequester::SaveFile(const std::string& title, const std::vector<FileFilter>& filters,
    const std::string& defaultName, const std::string& initialDir)
{
#ifdef _WIN32
    char filename[MAX_PATH] = "";

    // Copy default name to filename buffer if provided
    if (!defaultName.empty()) {
        strncpy_s(filename, defaultName.c_str(), MAX_PATH - 1);
    }

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);

    std::string filterStr = BuildFilterString(filters);
    ofn.lpstrFilter = filterStr.c_str();

    ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn)) {
        // Check if extension was added, if not and a filter was selected, add it
        std::string result(filename);
        if (!filters.empty() && ofn.nFilterIndex > 0 && ofn.nFilterIndex <= filters.size()) {
            std::string ext = filters[ofn.nFilterIndex - 1].Extension;

            // Remove leading wildcard if present
            if (!ext.empty() && ext[0] == '*') {
                ext = ext.substr(1);
            }

            // Check if filename already has this extension
            if (!ext.empty() && result.length() < ext.length() ||
                result.substr(result.length() - ext.length()) != ext) {
                result += ext;
            }
        }
        return result;
    }
    return "";
#elif defined(__APPLE__)
    // macOS implementation would go here
    std::cout << "Save file dialog on macOS is not implemented in this example." << std::endl;
    return "";
#else
    // Linux implementation would go here
    std::cout << "Save file dialog on Linux is not implemented in this example." << std::endl;
    return "";
#endif
}

std::string FileRequester::SelectFolder(const std::string& title, const std::string& initialDir)
{
#ifdef _WIN32
    char folderPath[MAX_PATH] = "";

    BROWSEINFOA bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    // Set initial directory if provided
    LPITEMIDLIST pidl = NULL;
    if (!initialDir.empty()) {
        LPSHELLFOLDER pDesktopFolder;
        if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder))) {
            OLECHAR olePath[MAX_PATH];
            MultiByteToWideChar(CP_ACP, 0, initialDir.c_str(), -1, olePath, MAX_PATH);
            ULONG chEaten;
            ULONG dwAttributes;
            pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidl, &dwAttributes);
            pDesktopFolder->Release();
        }
    }

    bi.pidlRoot = pidl;

    LPITEMIDLIST pidlSelected = SHBrowseForFolderA(&bi);
    if (pidlSelected) {
        SHGetPathFromIDListA(pidlSelected, folderPath);
        CoTaskMemFree(pidlSelected);
        if (pidl) {
            CoTaskMemFree(pidl);
        }
        return folderPath;
    }

    if (pidl) {
        CoTaskMemFree(pidl);
    }
    return "";
#elif defined(__APPLE__)
    // macOS implementation would go here
    std::cout << "Folder selection dialog on macOS is not implemented in this example." << std::endl;
    return "";
#else
    // Linux implementation would go here
    std::cout << "Folder selection dialog on Linux is not implemented in this example." << std::endl;
    return "";
#endif
}

std::string FileRequester::BuildFilterString(const std::vector<FileFilter>& filters)
{
#ifdef _WIN32
    // On Windows, filter string format is:
    // "Description\0*.ext\0Description2\0*.ext2\0\0"
    // Double null at the end

    if (filters.empty()) {
        return "All Files\0*.*\0\0";
    }

    std::string result;
    for (const auto& filter : filters) {
        // Add description
        result += filter.Description;
        result.push_back('\0');

        // Add extension pattern (make sure it has a * prefix)
        std::string ext = filter.Extension;
        if (!ext.empty() && ext[0] != '*') {
            ext = "*" + ext;
        }
        result += ext;
        result.push_back('\0');
    }

    // Add All Files option at the end
    result += "All Files";
    result.push_back('\0');
    result += "*.*";
    result.push_back('\0');

    // Final null character
    result.push_back('\0');

    return result;
#else
    // For non-Windows platforms, this would be implemented differently
    // Just a placeholder for now
    return "";
#endif
}
#include "FileRequester.h"
#include <string>
#include <vector>

/**
 * Extension to FileRequester that supports multiple file selection
 * This would need to be integrated with your FileRequester class
 */
std::vector<std::string> FileRequester::OpenMultipleFiles(
    const std::string& title,
    const std::vector<FileRequester::FileFilter>& filters,
    const std::string& initialDir
) {
    std::vector<std::string> result;

#ifdef _WIN32
    char filename[32768] = ""; // Large buffer for multiple filenames

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);

    std::string filterStr = FileRequester::BuildFilterString(filters);
    ofn.lpstrFilter = filterStr.c_str();

    ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        // Parse the multiple file selection result
        if (filename[ofn.nFileOffset - 1] != '\0') {
            // Only one file was selected
            result.push_back(filename);
        }
        else {
            // Multiple files were selected
            std::string directory = filename;
            char* p = filename + directory.size() + 1;

            while (*p) {
                std::string file = p;
                result.push_back(directory + "\\" + file);
                p += file.size() + 1;
            }
        }
    }
#else
    // Placeholder for non-Windows implementation
    // For now, fall back to single selection
    std::string singleFile = FileRequester::OpenFile(title, filters, initialDir);
    if (!singleFile.empty()) {
        result.push_back(singleFile);
    }
#endif

    return result;
}
