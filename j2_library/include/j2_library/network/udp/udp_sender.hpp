#pragma once

#include "j2_library/export.hpp"
#include "j2_library/network/ethernet.hpp"
#include <string>
#include <atomic>
#include <mutex>

namespace j2::network::udp {

class J2LIB_API udp_sender {
protected:
    std::string server_ip;
    unsigned short server_port;
    int address_family = AF_INET;
#ifdef _WIN32
    SOCKET socket_fd = INVALID_SOCKET;
#else
    int socket_fd = -1;
#endif
    sockaddr_storage server_addr{};
    socklen_t server_addr_len = 0;
    std::atomic<bool> stop_flag;
    std::mutex send_mutex;

    static bool fill_sockaddr(const std::string& ip, unsigned short port, sockaddr_storage& addr, socklen_t& addr_len, int& family);

public:
    udp_sender();
    ~udp_sender();

    void setServer(const std::string& ip, unsigned short port);
    bool create();
    ssize_t send_data(const std::string& data);
    ssize_t send_data_to(const std::string& data, const std::string& ip, unsigned short port);
    void set_multicast_ttl(int ttl);
    void stop();
};

} // namespace j2::network::udp
