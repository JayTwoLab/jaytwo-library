
#include "j2_library/network/tcp/tcp_client.hpp"

namespace j2::network::tcp {

tcp_client::tcp_client()
#ifdef _WIN32
    : socket_fd(INVALID_SOCKET)
#else
    : socket_fd(-1)
#endif
    , address_family(AF_INET)
{
    stop_flag = false;
    server_ip.clear();
    server_port = 0;
}

tcp_client::~tcp_client() {
    stop();
}

void tcp_client::setServer(const std::string& ip, unsigned short port, int family) {
    server_ip = ip;
    server_port = port;
    address_family = family;
}

void tcp_client::set_on_connect(Callback cb) { on_connect = std::move(cb); }
void tcp_client::set_on_close(Callback cb) { on_close = std::move(cb); }
void tcp_client::set_on_receive(ReceiveCallback cb) { on_receive = std::move(cb); }

bool tcp_client::start(std::chrono::seconds sleep_time) {
    if (server_ip.length() == 0) {
        std::cerr << " server ip is empty. " << std::endl;
        return false;
    }

    if (server_port == 0) {
        std::cerr << " server port is zero. " << std::endl;
        return false;
    }

    stop_flag = false;
    client_thread = std::thread(&tcp_client::connect_to_server, this, sleep_time);

    return true;
}

void tcp_client::stop() {
    stop_flag = true;
    close_connection();
    if (client_thread.joinable()) {
        client_thread.join();
    }
}

int tcp_client::send_data(const std::string& data) {
    std::lock_guard<std::mutex> lock(send_mutex);
#ifdef _WIN32
    if (socket_fd != INVALID_SOCKET) {
#else
    if (socket_fd != -1) {
#endif
        return ::send(socket_fd, data.c_str(), static_cast<int>(data.size()), 0);
    }
    return -1; // Not connected or invalid socket
}

void tcp_client::close_connection() {
    std::lock_guard<std::mutex> lock(send_mutex);

#ifdef _WIN32
    if (socket_fd != INVALID_SOCKET) {
#else
    if (socket_fd != -1) {
#endif

#ifdef _WIN32
        closesocket(socket_fd);
        socket_fd = INVALID_SOCKET;
#else
        close(socket_fd);
        socket_fd = -1;
#endif

        if (on_close) on_close();
    }
}

bool tcp_client::is_connected() const {
#ifdef _WIN32
    return socket_fd != INVALID_SOCKET;
#else
    return socket_fd != -1;
#endif
}

void tcp_client::connect_to_server(std::chrono::seconds sleep_time) {
    while (!stop_flag) {
        socket_fd = ::socket(address_family, SOCK_STREAM, 0);

#ifdef _WIN32
        if (socket_fd == INVALID_SOCKET) {
#else
        if (socket_fd == (-1)) {
#endif
            std::this_thread::sleep_for(sleep_time);
            continue;
        }

        sockaddr_storage server_addr_storage;
        memset(&server_addr_storage, 0, sizeof(server_addr_storage));
        socklen_t addr_len = 0;

        if (address_family == AF_INET) {
            sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(&server_addr_storage);
            addr4->sin_family = AF_INET;
            addr4->sin_port = htons(server_port);
#ifdef _WIN32
            inet_pton(AF_INET, server_ip.c_str(), &addr4->sin_addr.s_addr);
#else
            inet_pton(AF_INET, server_ip.c_str(), &addr4->sin_addr);
#endif
            addr_len = sizeof(sockaddr_in);
        } else if (address_family == AF_INET6) {
            sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(&server_addr_storage);
            addr6->sin6_family = AF_INET6;
            addr6->sin6_port = htons(server_port);
            inet_pton(AF_INET6, server_ip.c_str(), &addr6->sin6_addr);
            addr_len = sizeof(sockaddr_in6);
        } else {
            std::cerr << "Unsupported address family." << std::endl;
#ifdef _WIN32
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
#else
            close(socket_fd);
            socket_fd = -1;
#endif
            std::this_thread::sleep_for(sleep_time);
            continue;
        }

        std::cout << "   try to connect..." << std::endl;

        if (::connect(socket_fd, reinterpret_cast<sockaddr*>(&server_addr_storage), addr_len) == 0) {
            if (on_connect) on_connect();
            receive_loop();
        }
        else {
#ifdef _WIN32
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
#else
            close(socket_fd);
            socket_fd = -1;
#endif
        }

        std::this_thread::sleep_for(sleep_time);
    }
}

void tcp_client::receive_loop() {
    char buffer[BUFFER_SIZE];

    while (!stop_flag) {
        int bytes_received = ::recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            if (on_receive) {
                on_receive(std::string(buffer, bytes_received));
            }
        }
        else {
            close_connection();
            break;
        }
    }
}

} // namespace j2::network::tcp
