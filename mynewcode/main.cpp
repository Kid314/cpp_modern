// main.cpp
#include "parallel_file_processor.h"
#include <iostream>
#include <chrono>

void printUsage() {
    std::cout << "Usage: file_processor [directory] [mode] [threads] [output]" << std::endl;
    std::cout << "  directory  Directory to process files from" << std::endl;
    std::cout << "  mode       Processing mode: 'thread' or 'process'" << std::endl;
    std::cout << "  threads    Number of threads/processes to use" << std::endl;
    std::cout << "  output     Optional output file path" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }
    
    std::string directory = argv[1];
    std::string mode = argv[2];
    size_t numThreads = std::stoul(argv[3]);
    std::string outputFile = (argc >= 5) ? argv[4] : "";
    
    // 检查目录是否存在
    if (!DirectoryHelper::isDirectory(directory)) {
        std::cerr << "Directory does not exist: " << directory << std::endl;
        return 1;
    }
    
    // 创建文件处理器
    FileProcessor processor(numThreads);
    
    // 设置目录
    if (!processor.setDirectory(directory)) {
        std::cerr << "No files found in directory: " << directory << std::endl;
        return 1;
    }
    
    // 开始计时
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 根据模式处理文件
    if (mode == "thread") {
        std::cout << "Processing files with " << numThreads << " threads..." << std::endl;
        processor.processFilesWithThreads();
    } else if (mode == "process") {
        std::cout << "Processing files with " << numThreads << " processes..." << std::endl;
        processor.processFilesWithProcesses();
    } else {
        std::cerr << "Invalid mode. Use 'thread' or 'process'." << std::endl;
        return 1;
    }
    
    // 结束计时
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // 显示结果
    std::cout << "Processing completed in " << duration << " ms" << std::endl;
    processor.printResults();
    
    // 保存到文件
    if (!outputFile.empty()) {
        processor.saveResultsToFile(outputFile);
        std::cout << "Results saved to: " << outputFile << std::endl;
    }
    
    return 0;
}