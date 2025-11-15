#include "j2_library/network/tcp/tcp_server.hpp"

namespace j2::network::tcp {

tcp_server::tcp_server() : is_running(false), server_socket(-1) {}

tcp_server::~tcp_server() {
    quit();
}
 
tcp_server::StartResult tcp_server::start(const std::string& ip, int port) {
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);
    int ret = getaddrinfo(ip.empty() ? nullptr : ip.c_str(), port_str.c_str(), &hints, &res);
    if (ret != 0 || !res) {
        return StartResult::SocketCreationFailed;
    }

    server_socket = -1;
    struct addrinfo* p;
    for (p = res; p != nullptr; p = p->ai_next) {
        server_socket = static_cast<int>(socket(p->ai_family, p->ai_socktype, p->ai_protocol));
        if (server_socket == -1) continue;

        int opt = 1;
#ifdef _WIN32
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        if (bind(server_socket, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0) {
            address_family = p->ai_family; // Store the family
            break; // Success
        }
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        server_socket = -1;
    }
    freeaddrinfo(res);

    if (server_socket == -1) return StartResult::BindFailed;
    if (listen(server_socket, SOMAXCONN) == -1) {
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        return StartResult::ListenFailed;
    }

    is_running = true;
    server_thread = std::thread(&tcp_server::acceptLoop, this);
    return StartResult::Success;
}

void tcp_server::setOnConnectCallback(Callback cb) {
    on_connect = std::move(cb);
}

void tcp_server::setOnReceiveCallback(Callback cb) {
    on_receive = std::move(cb);
}

void tcp_server::setOnCloseCallback(Callback cb) {
    on_close = std::move(cb);
}

// Changed: Return type is int, returns the return value of send()
int tcp_server::sendToClient(int client_socket, const std::string& message) {
    std::lock_guard<std::mutex> lock(send_mutex);
    return send(client_socket, message.c_str(), static_cast<int>(message.size()), 0); // If the return value is 0 or more, it is success; if negative, it is failure
}

std::vector<int> tcp_server::broadcastToClients(const std::string& message) {
    std::lock_guard<std::mutex> lock(send_mutex);
    std::vector<int> failed_clients;
    for (int client_socket : client_sockets) {
        if (send(client_socket, message.c_str(), static_cast<int>(message.size()), 0) < 0) {
            failed_clients.push_back(client_socket);
        }
    }
    return failed_clients;
}

void tcp_server::closeClient(int client_socket) {
    std::lock_guard<std::mutex> lock(send_mutex);
#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
    if (on_close) {
        on_close(client_socket, "Client disconnected");
    }
}

void tcp_server::quit() {
    if (is_running) {
        is_running = false;
        if (server_thread.joinable()) {
            server_thread.join();
        }
        for (int client : client_sockets) {
#ifdef _WIN32
            closesocket(client);
#else
            close(client);
#endif
        }
        client_sockets.clear();
        if (server_socket >= 0) {
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            server_socket = -1;
        }
    }
}

std::vector<int> tcp_server::getClientSockets() {
    std::lock_guard<std::mutex> lock(send_mutex);
    return client_sockets;
}

void tcp_server::acceptLoop() {
    while (is_running) {
        sockaddr_storage client_addr{};  
        socklen_t client_len = sizeof(client_addr);
#ifdef _WIN32
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
#else
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
#endif
        if (client_socket < 0) {
            if (is_running) {
#ifdef _WIN32
                std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
#else
                perror("Accept failed");
#endif
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(send_mutex);
            client_sockets.push_back(static_cast<int>(client_socket));
        }

        if (on_connect) {
            on_connect(static_cast<int>(client_socket), "New client connected");
        }

        std::thread(&tcp_server::clientHandler, this, static_cast<int>(client_socket)).detach();
    }
}

void tcp_server::clientHandler(int client_socket) {
    char buffer[BUFFER_SIZE] = {};
    while (is_running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
#ifdef _WIN32
            if (bytes_received == SOCKET_ERROR) {
                std::cerr << "Recv failed: " << WSAGetLastError() << "\n";
            }
#endif
            closeClient(client_socket);
            break;
        }
        if (on_receive) {
            on_receive(client_socket, std::string(buffer));
        }
    }
}

} // namespace j2::network::tcp
