
#include "j2_library/j2_library.hpp"

#include <iostream>
#include <thread>
#include <string>
#include <vector>

class MyTcoServerHandler {
public:
    void onReceive(int client_socket, const std::string& message) {
        std::cout << "MyHandler::onReceive: " << client_socket << " - " << message << std::endl;
    }
    void onConnect(int client_socket, const std::string& message) {
        std::cout << "MyHandler::onConnect: " << client_socket << " - " << message << std::endl;
    }
    void onClose(int client_socket, const std::string& message) {
        std::cout << "MyHandler::onClose: " << client_socket << " - " << message << std::endl;
    }
};

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return -1;
    }
#endif

    MyTcoServerHandler handler;

    // IPv4 tcp server 
    j2::network::tcp::tcp_server server_ipv4;
    server_ipv4.setOnConnectCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onConnect(client_socket, "[IPv4] " + message);
        }
    );
    server_ipv4.setOnReceiveCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onReceive(client_socket, "[IPv4] " + message);
        }
    );
    server_ipv4.setOnCloseCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onClose(client_socket, "[IPv4] " + message);
        }
    );

    std::string ipv4_addr = "127.0.0.1";
    unsigned short port_ipv4 = 12345;

    if (server_ipv4.start(ipv4_addr, port_ipv4) != j2::network::tcp::tcp_server::StartResult::Success) {
        std::cerr << "Failed to start IPv4 server" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    // IPv6 tcp server 
    j2::network::tcp::tcp_server server_ipv6;
    server_ipv6.setOnConnectCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onConnect(client_socket, "[IPv6] " + message);
        }
    );
    server_ipv6.setOnReceiveCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onReceive(client_socket, "[IPv6] " + message);
        }
    );
    server_ipv6.setOnCloseCallback(
        [&handler](int client_socket, const std::string& message) {
            handler.onClose(client_socket, "[IPv6] " + message);
        }
    );

    std::string ipv6_addr = "::1"; // Loopback address for IPv6
    unsigned short port_ipv6 = 12346;

    if (server_ipv6.start(ipv6_addr, port_ipv6) != j2::network::tcp::tcp_server::StartResult::Success) {
        std::cerr << "Failed to start IPv6 server" << std::endl;
        server_ipv4.quit();
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    std::cout << "IPv4 server running on " << ipv4_addr << ":" << port_ipv4 << std::endl;
    std::cout << "IPv6 server running on [" << ipv6_addr << "]:" << port_ipv6 << std::endl;
    std::cout << "Type 'broadcast4' or 'broadcast6' to send a message, 'quit' to stop." << std::endl;

    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "quit") {
            break;
        }
        if (input.rfind("broadcast4 ", 0) == 0) {
            std::string msg = input.substr(11);
            std::vector<int> ret = server_ipv4.broadcastToClients(msg);
            for (auto failed : ret) {
                std::cout << "Failed to send to IPv4 client: " << failed << std::endl;
            }
        }
        else if (input.rfind("broadcast6 ", 0) == 0) {
            std::string msg = input.substr(11);
            std::vector<int> ret = server_ipv6.broadcastToClients(msg);
            for (auto failed : ret) {
                std::cout << "Failed to send to IPv6 client: " << failed << std::endl;
            }
        }
    }

    server_ipv4.quit();
    server_ipv6.quit();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}

