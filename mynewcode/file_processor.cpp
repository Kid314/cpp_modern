// file_processor.cpp
#include "parallel_file_processor.h"
#include <iostream>
#include <algorithm>
#include <chrono>

FileProcessor::FileProcessor(size_t maxThreads) : maxThreads(maxThreads) {
}

FileProcessor::~FileProcessor() {
}

bool FileProcessor::setDirectory(const std::string& dirPath) {
    fileList.clear();
    results.clear();
    
    fileList = DirectoryHelper::getFilesInDirectory(dirPath);
    return !fileList.empty();
}

bool FileProcessor::getFilesInDirectory() {
    return !fileList.empty();
}

FileStats FileProcessor::processFile(const std::string& filepath) {
    FileStats stats;
    stats.filename = filepath;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return stats;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        stats.lineCount++;
        stats.charCount += line.length() + 1;  // +1 for newline
        
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            stats.wordCount++;
            stats.wordFrequency[word]++;
        }
    }
    
    return stats;
}

void FileProcessor::mergeResults(const FileStats& stats) {
    std::lock_guard<std::mutex> lock(resultsMutex);
    results.push_back(stats);
}

void FileProcessor::processFilesWithThreads() {
    results.clear();
    
    ThreadPool pool(maxThreads);
    std::vector<std::future<FileStats>> futures;
    
    for (const auto& file : fileList) {
        futures.emplace_back(
            pool.enqueue([this, file] {
                return processFile(file);
            })
        );
    }
    
    for (auto& future : futures) {
        auto stats = future.get();
        mergeResults(stats);
    }
}

void FileProcessor::processFilesWithProcesses() {
    results.clear();
    
    // 计算每个进程处理的文件数量
    size_t numFiles = fileList.size();
    size_t numProcesses = std::min(maxThreads, numFiles);
    size_t filesPerProcess = numFiles / numProcesses;
    size_t remainingFiles = numFiles % numProcesses;
    
#ifdef _WIN32
    // Windows版本的多进程实现
    for (size_t i = 0; i < numProcesses; ++i) {
        // 计算该进程需要处理的文件范围
        size_t startIdx = i * filesPerProcess + std::min(i, remainingFiles);
        size_t endIdx = startIdx + filesPerProcess + (i < remainingFiles ? 1 : 0);
        
        // 创建匿名管道
        HANDLE readPipe, writePipe;
        SECURITY_ATTRIBUTES saAttr;
        
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;
        
        if (!CreatePipe(&readPipe, &writePipe, &saAttr, 0)) {
            std::cerr << "Failed to create pipe" << std::endl;
            continue;
        }
        
        // 设置管道句柄不被子进程继承
        SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
        
        // 创建子进程
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        
        // 将子进程的标准输出设置为管道的写入端
        si.hStdOutput = writePipe;
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        si.dwFlags |= STARTF_USESTDHANDLES;
        
        // 创建进程命令行
        std::string cmdLine = "child_process.exe ";
        for (size_t j = startIdx; j < endIdx; ++j) {
            cmdLine += "\"" + fileList[j] + "\" ";
        }
        
        // 创建子进程
        if (!CreateProcess(
                NULL,                   // 应用程序名称
                const_cast<char*>(cmdLine.c_str()),  // 命令行
                NULL,                   // 进程安全属性
                NULL,                   // 线程安全属性
                TRUE,                   // 继承句柄
                0,                      // 创建标志
                NULL,                   // 环境块
                NULL,                   // 当前目录
                &si,                    // 启动信息
                &pi)                    // 进程信息
            ) {
            std::cerr << "Failed to create process" << std::endl;
            CloseHandle(readPipe);
            CloseHandle(writePipe);
            continue;
        }
        
        // 关闭不需要的句柄
        CloseHandle(writePipe);
        
        // 等待子进程完成
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        // 从管道读取结果
        const int BUFSIZE = 4096;
        CHAR buffer[BUFSIZE];
        DWORD bytesRead;
        std::string resultData;
        
        while (ReadFile(readPipe, buffer, BUFSIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            resultData += buffer;
        }
        
        // 处理结果
        FileStats stats = IPC::deserializeFileStats(resultData);
        mergeResults(stats);
        
        // 清理
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(readPipe);
    }
#else
    // POSIX版本的多进程实现
    for (size_t i = 0; i < numProcesses; ++i) {
        // 计算该进程需要处理的文件范围
        size_t startIdx = i * filesPerProcess + std::min(i, remainingFiles);
        size_t endIdx = startIdx + filesPerProcess + (i < remainingFiles ? 1 : 0);
        
        // 创建管道
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            std::cerr << "Failed to create pipe" << std::endl;
            continue;
        }
        
        // 创建子进程
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Failed to fork" << std::endl;
            close(pipefd[0]);
            close(pipefd[1]);
            continue;
        }
        
        if (pid == 0) {
            // 子进程
            close(pipefd[0]);  // 关闭读端
            
            FileStats combinedStats;
            combinedStats.filename = "Process_" + std::to_string(i);
            
            // 处理分配的文件
            for (size_t j = startIdx; j < endIdx; ++j) {
                FileStats stats = processFile(fileList[j]);
                
                // 合并结果
                combinedStats.lineCount += stats.lineCount;
                combinedStats.wordCount += stats.wordCount;
                combinedStats.charCount += stats.charCount;
                
                for (const auto& pair : stats.wordFrequency) {
                    combinedStats.wordFrequency[pair.first] += pair.second;
                }
            }
            
            // 序列化结果并写入管道
            std::string serialized = IPC::serializeFileStats(combinedStats);
            write(pipefd[1], serialized.c_str(), serialized.size());
            
            close(pipefd[1]);
            _exit(0);
        } else {
            // 父进程
            close(pipefd[1]);  // 关闭写端
            
            // 读取子进程的结果
            std::string resultData;
            char buffer[4096];
            ssize_t bytesRead;
            
            while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytesRead] = '\0';
                resultData += buffer;
            }
            
            close(pipefd[0]);
            
            // 等待子进程结束
            int status;
            waitpid(pid, &status, 0);
            
            // 处理结果
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0 && !resultData.empty()) {
                FileStats stats = IPC::deserializeFileStats(resultData);
                mergeResults(stats);
            }
        }
    }
#endif
}

std::vector<FileStats> FileProcessor::getResults() const {
    return results;
}

void FileProcessor::saveResultsToFile(const std::string& outputFile) const {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return;
    }
    
    for (const auto& stats : results) {
        out << "File: " << stats.filename << std::endl;
        out << "Lines: " << stats.lineCount << std::endl;
        out << "Words: " << stats.wordCount << std::endl;
        out << "Characters: " << stats.charCount << std::endl;
        out << "Top 10 words:" << std::endl;
        
        // 排序单词频率
        std::vector<std::pair<std::string, int>> sortedWords(
            stats.wordFrequency.begin(), stats.wordFrequency.end());
        
        std::sort(sortedWords.begin(), sortedWords.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });
        
        int count = 0;
        for (const auto& pair : sortedWords) {
            if (count++ >= 10) break;
            out << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        out << std::endl;
    }
}

void FileProcessor::printResults() const {
    for (const auto& stats : results) {
        std::cout << "File: " << stats.filename << std::endl;
        std::cout << "Lines: " << stats.lineCount << std::endl;
        std::cout << "Words: " << stats.wordCount << std::endl;
        std::cout << "Characters: " << stats.charCount << std::endl;
        std::cout << "Top 10 words:" << std::endl;
        
        // 排序单词频率
        std::vector<std::pair<std::string, int>> sortedWords(
            stats.wordFrequency.begin(), stats.wordFrequency.end());
        
        std::sort(sortedWords.begin(), sortedWords.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });
        
        int count = 0;
        for (const auto& pair : sortedWords) {
            if (count++ >= 10) break;
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        std::cout << std::endl;
    }
}