// directory_helper.cpp
#include "parallel_file_processor.h"

std::vector<std::string> DirectoryHelper::getFilesInDirectory(const std::string& dirPath) {
    std::vector<std::string> fileList;
    
#ifdef _WIN32
    WIN32_FIND_DATA ffd;
    std::string searchPath = dirPath + "\\*";
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &ffd);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        return fileList;
    }
    
    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            fileList.push_back(dirPath + "\\" + ffd.cFileName);
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    
    FindClose(hFind);
#else
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        return fileList;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            std::string fullPath = dirPath + "/" + name;
            
            struct stat statbuf;
            if (stat(fullPath.c_str(), &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
                fileList.push_back(fullPath);
            }
        }
    }
    
    closedir(dir);
#endif
    
    return fileList;
}

bool DirectoryHelper::isDirectory(const std::string& path) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributes(path.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            (attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
#endif
}

bool DirectoryHelper::fileExists(const std::string& filepath) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributes(filepath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && 
           !(attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat statbuf;
    if (stat(filepath.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISREG(statbuf.st_mode);
#endif
}