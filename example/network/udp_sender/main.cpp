
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "j2_library/j2_library.hpp"

std::string get_current_time_string();

void udp_sender_thread(j2::network::udp::udp_sender& sender, std::atomic<bool>& running_flag) {
    while (running_flag.load()) { // Loop until the running_flag is set to false

        // NOTE: send_data_to does not use setting of server ip and port. only use parameters.
        sender.send_data_to("Hello Unicast", "127.0.0.1", 12346); // Send unicast
        std::cout << "unicast sent at 127.0.0.1:12346" << std::endl;

        sender.set_multicast_ttl(2); // Multicast Egress TTL Settings (Optional)
        sender.send_data_to("Hello Multicast", "239.255.255.250", 12347); // Send multicast 
        std::cout << "multicast sent at 239.255.255.250:12347" << std::endl;

        sender.send_data_to("Hello Broadcast", "255.255.255.255", 12349); // Broadcast transmission
        std::cout << "broadcast sent at 255.255.255.255:12349" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return (-1);
    }
#endif

    j2::network::udp::udp_sender sender;

    sender.setServer("127.0.0.1", 12345);

    if (!sender.create()) {
        std::cerr << "Failed to create UDP sender" << std::endl;
        return 1;
    }

    std::atomic<bool> running_flag(true);
    std::thread sender_thread(udp_sender_thread, std::ref(sender), std::ref(running_flag));

    // Main thread loop with 1-second sleep
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait a bit
        // std::cout << "Main thread is running..." << std::endl;

        // It can be set to stop sending when certain conditions are met.
        // example> if (condition) { break; }
    }

    // Processing at the end of the program
    running_flag.store(false);
    if (sender_thread.joinable()) {
        sender_thread.join();
    }

    sender.stop();

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

std::string get_current_time_string()
{
    auto now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm{};
#ifdef _WIN32
    localtime_s(&local_tm, &now_time_t);  // Windows
#else
    localtime_r(&now_time_t, &local_tm);  // Linux
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");  

    return oss.str();
}
