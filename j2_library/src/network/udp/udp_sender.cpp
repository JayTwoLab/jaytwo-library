#include "j2_library/network/udp/udp_sender.hpp"
#include <cstring>
#include <iostream>

namespace j2::network::udp {

bool udp_sender::fill_sockaddr(const std::string& ip, unsigned short port, sockaddr_storage& addr, socklen_t& addr_len, int& family) {
    memset(&addr, 0, sizeof(addr));
    sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(&addr);
    if (inet_pton(AF_INET, ip.c_str(), &(addr4->sin_addr)) == 1) {
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(port);
        addr_len = sizeof(sockaddr_in);
        family = AF_INET;
        return true;
    }
    sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(&addr);
    if (inet_pton(AF_INET6, ip.c_str(), &(addr6->sin6_addr)) == 1) {
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(port);
        addr_len = sizeof(sockaddr_in6);
        family = AF_INET6;
        return true;
    }
    return false;
}

udp_sender::udp_sender()
    : server_ip(""), server_port(0), stop_flag(false) {}

udp_sender::~udp_sender() {
    stop();
}

void udp_sender::setServer(const std::string& ip, unsigned short port) {
    std::lock_guard<std::mutex> lock(send_mutex);
    server_ip = ip;
    server_port = port;
    fill_sockaddr(server_ip, server_port, server_addr, server_addr_len, address_family);
}

bool udp_sender::create() {
    int family = AF_INET;
    sockaddr_storage tmp_addr{};
    socklen_t tmp_len = 0;
    if (!fill_sockaddr(server_ip, server_port, tmp_addr, tmp_len, family)) {
        std::cerr << "Invalid server address" << std::endl;
        return false;
    }
    address_family = family;

    socket_fd = socket(address_family, SOCK_DGRAM, 0);
#ifdef _WIN32
    if (socket_fd == INVALID_SOCKET) {
#else
    if (socket_fd == -1) {
#endif
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    int broadcastEnable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST,
#ifdef _WIN32
        (const char*)&broadcastEnable,
#else
        &broadcastEnable,
#endif
        sizeof(broadcastEnable));

    server_addr = tmp_addr;
    server_addr_len = tmp_len;

    return true;
}

ssize_t udp_sender::send_data(const std::string& data) {
    std::lock_guard<std::mutex> lock(send_mutex);
    ssize_t ret = 0;
#ifdef _WIN32
    if (socket_fd != INVALID_SOCKET) {
#else
    if (socket_fd != -1) {
#endif
        ret = sendto(socket_fd,
            data.c_str(),
            static_cast<int>(data.size()),
            0,
            reinterpret_cast<const sockaddr*>(&server_addr),
            server_addr_len);
    }
    return ret;
}

ssize_t udp_sender::send_data_to(const std::string& data, const std::string& ip, unsigned short port) {
    std::lock_guard<std::mutex> lock(send_mutex);

    ssize_t ret = 0;
    sockaddr_storage dest_addr{};
    socklen_t dest_addr_len = 0;
    int dest_family = AF_INET;
    if (!fill_sockaddr(ip, port, dest_addr, dest_addr_len, dest_family)) {
        std::cerr << "Invalid destination address" << std::endl;
        return ret;
    }

#ifdef _WIN32
    if (socket_fd != INVALID_SOCKET) {
#else
    if (socket_fd != -1) {
#endif
        ret = sendto(socket_fd,
            data.c_str(),
            static_cast<int>(data.size()),
            0,
            reinterpret_cast<const sockaddr*>(&dest_addr),
            dest_addr_len);
    }
    return ret;
}

void udp_sender::set_multicast_ttl(int ttl) {
    if (address_family == AF_INET) {
        setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL,
#ifdef _WIN32
            (const char*)&ttl,
#else
            &ttl,
#endif
            sizeof(ttl));
    } else if (address_family == AF_INET6) {
        setsockopt(socket_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
#ifdef _WIN32
            (const char*)&ttl,
#else
            &ttl,
#endif
            sizeof(ttl));
    }
}

void udp_sender::stop() {
    stop_flag = true;
#ifdef _WIN32
    if (socket_fd != INVALID_SOCKET) {
        closesocket(socket_fd);
        socket_fd = INVALID_SOCKET;
    }
#else
    if (socket_fd != -1) {
        close(socket_fd);
        socket_fd = -1;
    }
#endif
}

} // namespace j2::network::udp
