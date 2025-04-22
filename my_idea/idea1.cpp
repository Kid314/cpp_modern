#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sstream>
#include <string>
#include <algorithm>

constexpr int PORT = 54000;
constexpr int MAX_CONNECTIONS = 10;
constexpr int THREAD_POOL_SIZE = 4;

using socket_t = int;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close

// 设置socket为非阻塞模式（可选，示例未用）
bool setNonBlocking(socket_t sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) return false;
    flags |= O_NONBLOCK;
    return fcntl(sock, F_SETFL, flags) == 0;
}

// 计算斐波那契数
uint64_t fibonacci(int n)
{
    if (n <= 0) return 0;
    if (n == 1) return 1;
    uint64_t a = 0, b = 1, c = 0;
    for (int i = 2; i <= n; ++i)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return c;
}

// 任务结构，网络连接及收到的请求字符串
struct Task
{
    socket_t clientSocket;
    std::string request;
};

class ThreadPool
{
public:
    ThreadPool(size_t numThreads) : done(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back(&ThreadPool::workerThread, this);
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            done = true;
        }
        cv.notify_all();
        for (auto &thread : workers)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    void enqueueTask(Task task)
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.push(std::move(task));
        }
        cv.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool done;

    void workerThread()
    {
        while (!done)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]{ return done || !tasks.empty(); });
                if (done && tasks.empty())
                    return;
                task = std::move(tasks.front());
                tasks.pop();
            }

            // 处理任务
            handleTask(task);
        }
    }

    void handleTask(const Task &task)
    {
        std::istringstream iss(task.request);
        std::string cmd;
        iss >> cmd;

        std::string response;
        if (cmd == "fib")
        {
            int n;
            if (!(iss >> n))
            {
                response = "Error: fib command needs a number\n";
            }
            else
            {
                uint64_t res = fibonacci(n);
                response = "fib(" + std::to_string(n) + ") = " + std::to_string(res) + "\n";
            }
        }
        else if (cmd == "quit")
        {
            response = "Goodbye\n";
        }
        else
        {
            response = "Unknown command\n";
        }

        send(task.clientSocket, response.c_str(), (int)response.size(), 0);

        if (cmd == "quit")
        {
            CLOSESOCKET(task.clientSocket);
        }
    }
};

void clientHandler(socket_t clientSocket, ThreadPool &threadPool)
{
    char buffer[1024];

    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0)
        {
            CLOSESOCKET(clientSocket);
            std::cout << "[Thread " << std::this_thread::get_id() << "] Client disconnected or error.\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::string request(buffer);

        request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
        request.erase(std::remove(request.begin(), request.end(), '\n'), request.end());
        if (request.empty())
            continue;

        std::cout << "[Thread " << std::this_thread::get_id() << "] Received: " << request << std::endl;

        threadPool.enqueueTask({clientSocket, request});

        if (request == "quit")
            break;
    }
}

int main()
{
    socket_t listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket\n";
        return 2;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listenSock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed\n";
        CLOSESOCKET(listenSock);
        return 3;
    }

    if (listen(listenSock, MAX_CONNECTIONS) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed\n";
        CLOSESOCKET(listenSock);
        return 4;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    ThreadPool threadPool(THREAD_POOL_SIZE);
    std::vector<std::thread> clientThreads;

    while (true)
    {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        socket_t clientSocket = accept(listenSock, (sockaddr*)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        int result = getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (result == 0)
        {
            std::cout << "New connection from " << host << ":" << service << std::endl;
        }
        else
        {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << "New connection from " << host << ":" << ntohs(client.sin_port) << std::endl;
        }

        clientThreads.emplace_back(std::thread(clientHandler, clientSocket, std::ref(threadPool)));

        for (auto it = clientThreads.begin(); it != clientThreads.end(); )
        {
            if (it->joinable())
            {
                it->join();
                it = clientThreads.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    for (auto &t : clientThreads)
    {
        if (t.joinable())
            t.join();
    }

    CLOSESOCKET(listenSock);

    return 0;
}