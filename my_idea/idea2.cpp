#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <string>
#include <cstring>

constexpr int PORT = 54000;
const char* SERVER_IP = "127.0.0.1"; // 本地服务器地址，修改为服务器实际IP

using socket_t = int;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close

int main()
{
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "Invalid server IP address\n";
        CLOSESOCKET(sock);
        return 2;
    }

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server\n";
        CLOSESOCKET(sock);
        return 3;
    }

    std::cout << "Connected to server " << SERVER_IP << ":" << PORT << std::endl;
    std::cout << "Enter commands (like 'fib 10' or 'quit'):" << std::endl;

    std::string input;
    char buffer[1024];

    while (true)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, input))
            break;

        if (input.empty())
            continue;

        // 发送数据到服务器，自动添加换行
        std::string toSend = input + "\n";
        ssize_t sent = send(sock, toSend.c_str(), toSend.size(), 0);
        if (sent == SOCKET_ERROR)
        {
            std::cerr << "Send failed\n";
            break;
        }

        // 收服务器回应
        ssize_t received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received == 0)
        {
            std::cout << "Server closed connection\n";
            break;
        }
        else if (received == SOCKET_ERROR)
        {
            std::cerr << "Receive failed\n";
            break;
        }

        buffer[received] = '\0';
        std::cout << "Response: " << buffer;

        if (input == "quit")
        {
            std::cout << "Quitting client.\n";
            break;
        }
    }

    CLOSESOCKET(sock);
    return 0;
}