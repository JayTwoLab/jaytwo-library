#pragma once

#include "j2_library/export.hpp"
#include "j2_library/network/ethernet.hpp"

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <chrono>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cstring> // for memset
#include <cstdint>

namespace j2::network::tcp {

class J2LIB_API tcp_client {
public:
    using Callback = std::function<void()>;
    using ReceiveCallback = std::function<void(const std::string&)>;

protected:
    std::string server_ip;
    unsigned short server_port;
    int address_family; // AF_INET or AF_INET6
#ifdef _WIN32
    SOCKET socket_fd;
#else
    int socket_fd;
#endif
    std::thread client_thread;
    std::atomic<bool> stop_flag;
    std::mutex send_mutex;
    Callback on_connect;
    Callback on_close;
    ReceiveCallback on_receive;

    static constexpr int BUFFER_SIZE = 1024;

public:
    tcp_client();
    ~tcp_client();

    // address_family: AF_INET (default) or AF_INET6
    void setServer(const std::string& ip, unsigned short port, int address_family = AF_INET);

    void set_on_connect(Callback cb);
    void set_on_close(Callback cb);
    void set_on_receive(ReceiveCallback cb);

    bool start(std::chrono::seconds sleep_time = std::chrono::seconds(1));
    void stop();
    int send_data(const std::string& data);
    void close_connection();
    bool is_connected() const;

protected:
    void connect_to_server(std::chrono::seconds sleep_time = std::chrono::seconds(1));
    void receive_loop();
};

} // namespace j2::network::tcp

