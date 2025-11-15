#pragma once

#include "j2_library/export.hpp"
#include "j2_library/network/ethernet.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <unistd.h>
#endif 

namespace j2::network::tcp {

class J2LIB_API tcp_server {
public:
    using Callback = std::function<void(int, const std::string&)>;

protected:
    int server_socket;
    int address_family = AF_UNSPEC; // Add this line
    std::atomic<bool> is_running;
    std::thread server_thread;
    std::mutex send_mutex;
    std::vector<int> client_sockets;

    Callback on_connect;
    Callback on_receive;
    Callback on_close;

    static constexpr int BUFFER_SIZE = 1024;

public:
    tcp_server();
    ~tcp_server();

    enum class StartResult {
        Success,                // Server started successfully
        SocketCreationFailed,   // Socket creation failed. Could be a system resource issue.
        BindFailed,             // Bind failed. The port may already be in use.
        ListenFailed            // Listen failed. Could be a network issue.
    };

    StartResult start(const std::string& ip, int port);

    void setOnConnectCallback(Callback cb);
    void setOnReceiveCallback(Callback cb);
    void setOnCloseCallback(Callback cb);

    int sendToClient(int client_socket, const std::string& message); // If the return value is 0 or greater, it is success; if negative, it is failure
    std::vector<int> broadcastToClients(const std::string& message);
    void closeClient(int client_socket);
    void quit();

    std::vector<int> getClientSockets();

protected:
    void acceptLoop();
    void clientHandler(int client_socket);
};

} // namespace j2::network::tcp









