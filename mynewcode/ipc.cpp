// ipc.cpp
#include "parallel_file_processor.h"
#include <cstring>

bool IPC::createPipe(int& readFd, int& writeFd) {
#ifdef _WIN32
    // Windows实现
    HANDLE readHandle, writeHandle;
    SECURITY_ATTRIBUTES saAttr;
    
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    if (!CreatePipe(&readHandle, &writeHandle, &saAttr, 0)) {
        return false;
    }
    
    // 转换为文件描述符
    readFd = _open_osfhandle((intptr_t)readHandle, 0);
    writeFd = _open_osfhandle((intptr_t)writeHandle, 0);
    
    return (readFd != -1 && writeFd != -1);
#else
    // POSIX实现
    int pipefd[2];
    int result = pipe(pipefd);
    if (result != 0) {
        return false;
    }
    
    readFd = pipefd[0];
    writeFd = pipefd[1];
    return true;
#endif
}

bool IPC::writeToPipe(int fd, const void* data, size_t size) {
#ifdef _WIN32
    // Windows实现
    HANDLE handle = (HANDLE)_get_osfhandle(fd);
    DWORD bytesWritten;
    
    if (!WriteFile(handle, data, static_cast<DWORD>(size), &bytesWritten, NULL) || 
        bytesWritten != size) {
        return false;
    }
    
    return true;
#else
    // POSIX实现
    ssize_t bytesWritten = write(fd, data, size);
    return bytesWritten == static_cast<ssize_t>(size);
#endif
}

bool IPC::readFromPipe(int fd, void* data, size_t size) {
#ifdef _WIN32
    // Windows实现
    HANDLE handle = (HANDLE)_get_osfhandle(fd);
    DWORD bytesRead;
    
    if (!ReadFile(handle, data, static_cast<DWORD>(size), &bytesRead, NULL) || 
        bytesRead != size) {
        return false;
    }
    
    return true;
#else
    // POSIX实现
    ssize_t bytesRead = read(fd, data, size);
    return bytesRead == static_cast<ssize_t>(size);
#endif
}

void IPC::closeFd(int fd) {
#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
}

std::string IPC::serializeFileStats(const FileStats& stats) {
    std::stringstream ss;
    
    ss << stats.filename << "\n";
    ss << stats.lineCount << "\n";
    ss << stats.wordCount << "\n";
    ss << stats.charCount << "\n";
    ss << stats.wordFrequency.size() << "\n";
    
    for (const auto& pair : stats.wordFrequency) {
        ss << pair.first << "\n" << pair.second << "\n";
    }
    
    return ss.str();
}

FileStats IPC::deserializeFileStats(const std::string& data) {
    FileStats stats;
    std::istringstream iss(data);
    
    std::getline(iss, stats.filename);
    
    std::string line;
    std::getline(iss, line);
    stats.lineCount = std::stoull(line);
    
    std::getline(iss, line);
    stats.wordCount = std::stoull(line);
    
    std::getline(iss, line);
    stats.charCount = std::stoull(line);
    
    std::getline(iss, line);
    size_t wordCount = std::stoull(line);
    
    for (size_t i = 0; i < wordCount; ++i) {
        std::string word, count;
        std::getline(iss, word);
        std::getline(iss, count);
        
        stats.wordFrequency[word] = std::stoi(count);
    }
    
    return stats;
}