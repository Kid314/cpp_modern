// parallel_file_processor.h
#ifndef PARALLEL_FILE_PROCESSOR_H
#define PARALLEL_FILE_PROCESSOR_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <memory>
#include <fstream>
#include <sstream>

// 跨平台处理
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

// 文件统计结果结构
struct FileStats {
    std::string filename;
    size_t lineCount = 0;
    size_t wordCount = 0;
    size_t charCount = 0;
    std::map<std::string, int> wordFrequency;
};

// 线程池类
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // 提交任务到线程池
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // 工作线程
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<std::function<void()>> tasks;
    
    // 同步
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// 文件处理类
class FileProcessor {
public:
    FileProcessor(size_t maxThreads);
    ~FileProcessor();

    // 设置要处理的目录
    bool setDirectory(const std::string& dirPath);
    
    // 使用多线程处理文件
    void processFilesWithThreads();
    
    // 使用多进程处理文件
    void processFilesWithProcesses();
    
    // 获取结果
    std::vector<FileStats> getResults() const;
    
    // 将结果保存到文件
    void saveResultsToFile(const std::string& outputFile) const;
    
    // 打印结果到控制台
    void printResults() const;

private:
    // 目录中的文件列表
    std::vector<std::string> fileList;
    
    // 处理文件的线程数
    size_t maxThreads;
    
    // 结果存储
    std::vector<FileStats> results;
    std::mutex resultsMutex;
    
    // 获取目录下所有文件
    bool getFilesInDirectory();
    
    // 处理单个文件并返回统计信息
    FileStats processFile(const std::string& filepath);
    
    // 合并统计结果
    void mergeResults(const FileStats& stats);
};

// 进程间通信辅助类
class IPC {
public:
    static bool createPipe(int& readFd, int& writeFd);
    static bool writeToPipe(int fd, const void* data, size_t size);
    static bool readFromPipe(int fd, void* data, size_t size);
    static void closeFd(int fd);
    
    // 序列化和反序列化 FileStats 对象
    static std::string serializeFileStats(const FileStats& stats);
    static FileStats deserializeFileStats(const std::string& data);
};

// 跨平台目录操作封装
class DirectoryHelper {
public:
    static std::vector<std::string> getFilesInDirectory(const std::string& dirPath);
    static bool isDirectory(const std::string& path);
    static bool fileExists(const std::string& filepath);
};

#include "thread_pool_impl.h" // 线程池模板实现

#endif // PARALLEL_FILE_PROCESSOR_H
