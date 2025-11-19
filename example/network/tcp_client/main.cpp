
#include "j2_library/j2_library.hpp"

#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

// Custom handler class for TCP client events
class MyTcpClientHandler {
public:
    void on_connect() {
        std::cout << " Connected to server!" << std::endl;
    }

    void on_close() {
        std::cout << "Disconnected!" << std::endl;
    }

    void on_receive(const std::string& data) {
        std::cout << " Received: " << data << std::endl;
        std::vector<uint8_t> byte_array(data.begin(), data.end()); // Convert string to byte array
    }
};

// Example for both IPv4 and IPv6
int main() {

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif

    // ----------- IPv4 Example -----------
    {
        std::cout << "[IPv4 Example]" << std::endl;
        j2::network::tcp::tcp_client client;
        client.setServer("127.0.0.1", 12345, AF_INET); // Set server IP and port, IPv4

        auto handler = std::make_shared<MyTcpClientHandler>();
        client.set_on_connect([handler]() { handler->on_connect(); });
        client.set_on_close([handler]() { handler->on_close(); });
        client.set_on_receive([handler](const std::string& data) { handler->on_receive(data); });

        auto sleep_duration = std::chrono::seconds(60); // Sleep duration between connection attempts
        if (!client.start(sleep_duration)) {
            return 1;
        }

        // ========================================
        //  TCP kernel parameter status check
        // ========================================
        // 
        // tcp_fin_timeout                : net.ipv4.tcp_fin_timeout = 60
        //   Description: Time (seconds) to keep TIME_WAIT after FIN-based close. Default 60s. Prevents socket resource delay.
        // 
        // tcp_tw_reuse                   : net.ipv4.tcp_tw_reuse = 2
        //   Description: Whether to reuse TIME_WAIT sockets. Value 2 in recent kernels means safe reuse for outbound connections.
        // 
        // tcp_max_tw_buckets             : net.ipv4.tcp_max_tw_buckets = 131072
        //   Description: Max number of TIME_WAIT sockets. If exceeded, kernel forcibly cleans up old TIME_WAITs to prevent resource exhaustion.
        // 
        // tcp_max_syn_backlog            : net.ipv4.tcp_max_syn_backlog = 2048
        //   Description: Size of the queue for initial SYN requests. Larger value increases concurrent connection handling.
        // 
        // somaxconn                      : net.core.somaxconn = 4096
        //   Description: Maximum backlog for listen(). Directly affects server concurrent connection capacity.
        // 
        // ip_local_port_range            : net.ipv4.ip_local_port_range = 9000    65535
        //   Description: Ephemeral port range for client outbound connections. Directly related to port exhaustion.

        // [Test 1] Simulate the main thread handling its own loop
        for (int i = 0; i < 3; ++i) { // Run 3 times for demonstration
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (client.is_connected()) {
                // Get current time as string
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                std::tm tm;
#ifdef _WIN32
                localtime_s(&tm, &now_c);
#else
                localtime_r(&now_c, &tm);
#endif
                std::ostringstream oss;
                oss << "hello (IPv4) " << std::put_time(&tm, "%H:%M:%S");
                if (client.send_data(oss.str()) < 0) { // Send data to server
                    std::cerr << " Failed to send data." << std::endl;
                }
            }
        }

        // [Test 2] Send binary data
        // std::vector<uint8_t> bin = { 0x01, 0x02, 0x00, 0xFF };
        // std::string bin_str(reinterpret_cast<const char*>(bin.data()), bin.size());
        // client.send_data(bin_str);

        client.stop();
    }

    // ----------- IPv6 Example -----------
    {
        std::cout << "[IPv6 Example]" << std::endl;
        j2::network::tcp::tcp_client client;
        client.setServer("::1", 12346, AF_INET6); // Set server IP and port, IPv6

        auto handler = std::make_shared<MyTcpClientHandler>();
        client.set_on_connect([handler]() { handler->on_connect(); });
        client.set_on_close([handler]() { handler->on_close(); });
        client.set_on_receive([handler](const std::string& data) { handler->on_receive(data); });

        auto sleep_duration = std::chrono::seconds(60); // Sleep duration between connection attempts
        if (!client.start(sleep_duration)) {
            return 1;
        }

        // [Test 1] Simulate the main thread handling its own loop
        for (int i = 0; i < 3; ++i) { // Run 3 times for demonstration
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (client.is_connected()) {
                // Get current time as string
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                std::tm tm;
#ifdef _WIN32
                localtime_s(&tm, &now_c);
#else
                localtime_r(&now_c, &tm);
#endif
                std::ostringstream oss;
                oss << "hello (IPv6) " << std::put_time(&tm, "%H:%M:%S");
                if (client.send_data(oss.str()) < 0) { // Send data to server
                    std::cerr << " Failed to send data." << std::endl;
                }
            }
        }

        client.stop();
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
