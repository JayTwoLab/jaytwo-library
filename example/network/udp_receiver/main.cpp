
#include "j2_library/j2_library.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>

// my_udp_receiver_handler class definition
class my_udp_receiver_handler {
public:
    void onReceive(const std::string& message, const std::string& ip, uint16_t port) {
        std::cout << "[my_udp_receiver_handler] Received from " << ip << ":" << port << " - " << message << std::endl;
    }
};

void run_receiver(j2::network::udp::udp_receiver& receiver, std::atomic<bool>& stop_flag) {
    while (!stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    receiver.quit();
}

int main() {

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return -1;
    }
#endif

    // IPv4 Unicast
    j2::network::udp::udp_receiver unicast_receiver;
    unicast_receiver.setIpVersion(j2::network::udp::udp_receiver::IpVersion::IPv4);
    my_udp_receiver_handler unicast_handler;
    unicast_receiver.setOnReceiveCallback(
        [&unicast_handler](const std::string& message, const std::string& ip, uint16_t port) {
            unicast_handler.onReceive(message, ip, port);
        }
    );
    if (!unicast_receiver.startUnicast("0.0.0.0", 12345)) {
        std::cerr << "Failed to start IPv4 UDP unicast receiver" << std::endl;
    }
    else {
        std::cout << "[Unicast IPv4] UDP Receiver running on 0.0.0.0:12345" << std::endl;
    }

    // IPv6 Unicast
    j2::network::udp::udp_receiver unicast6_receiver;
    unicast6_receiver.setIpVersion(j2::network::udp::udp_receiver::IpVersion::IPv6);
    my_udp_receiver_handler unicast6_handler;
    unicast6_receiver.setOnReceiveCallback(
        [&unicast6_handler](const std::string& message, const std::string& ip, uint16_t port) {
            unicast6_handler.onReceive(message, ip, port);
        }
    );
    if (!unicast6_receiver.startUnicast("::", 12346)) {
        std::cerr << "Failed to start IPv6 UDP unicast receiver" << std::endl;
    }
    else {
        std::cout << "[Unicast IPv6] UDP Receiver running on [::]:12346" << std::endl;
    }

    // IPv4 Multicast
    j2::network::udp::udp_receiver multicast_receiver;
    multicast_receiver.setIpVersion(j2::network::udp::udp_receiver::IpVersion::IPv4);
    my_udp_receiver_handler multicast_handler;
    multicast_receiver.setOnReceiveCallback(
        [&multicast_handler](const std::string& message, const std::string& ip, uint16_t port) {
            multicast_handler.onReceive(message, ip, port);
        }
    );
    if (!multicast_receiver.startMulticast("239.255.255.250", 12347)) {
        std::cerr << "Failed to start IPv4 UDP multicast receiver" << std::endl;
    }
    else {
        std::cout << "[Multicast IPv4] UDP Receiver running on 239.255.255.250:12347" << std::endl;
    }

    // IPv6 Multicast
    j2::network::udp::udp_receiver multicast6_receiver;
    multicast6_receiver.setIpVersion(j2::network::udp::udp_receiver::IpVersion::IPv6);
    my_udp_receiver_handler multicast6_handler;
    multicast6_receiver.setOnReceiveCallback(
        [&multicast6_handler](const std::string& message, const std::string& ip, uint16_t port) {
            multicast6_handler.onReceive(message, ip, port);
        }
    );
    // Use a random, valid IPv6 multicast address for demonstration
    if (!multicast6_receiver.startMulticast("ff3e::1234", 12348)) {
        std::cerr << "Failed to start IPv6 UDP multicast receiver" << std::endl;
    }
    else {
        std::cout << "[Multicast IPv6] UDP Receiver running on ff3e::1234:12348" << std::endl;
    }

    // IPv4 Broadcast
    j2::network::udp::udp_receiver broadcast_receiver;
    broadcast_receiver.setIpVersion(j2::network::udp::udp_receiver::IpVersion::IPv4);
    my_udp_receiver_handler broadcast_handler;
    broadcast_receiver.setOnReceiveCallback(
        [&broadcast_handler](const std::string& message, const std::string& ip, uint16_t port) {
            broadcast_handler.onReceive(message, ip, port);
        }
    );
    if (!broadcast_receiver.startBroadcast(12349)) {
        std::cerr << "Failed to start IPv4 UDP broadcast receiver" << std::endl;
    }
    else {
        std::cout << "[Broadcast IPv4] UDP Receiver running on port 12349" << std::endl;
    }

    // NOTE: ipv6 is not supported broadcast. So we skip ipv6 broadcast receiver.

    // Run all receivers in separate threads for demonstration
    std::atomic<bool> stop_flag(false);
    std::thread t1(run_receiver, std::ref(unicast_receiver), std::ref(stop_flag));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t2(run_receiver, std::ref(unicast6_receiver), std::ref(stop_flag));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t3(run_receiver, std::ref(multicast_receiver), std::ref(stop_flag));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t4(run_receiver, std::ref(multicast6_receiver), std::ref(stop_flag));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread t5(run_receiver, std::ref(broadcast_receiver), std::ref(stop_flag));

    std::cout << "All UDP Receivers running. Type 'quit' to stop." << std::endl;

    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "quit") break;
    }

    stop_flag = true;

    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
    if (t4.joinable()) t4.join();
    if (t5.joinable()) t5.join();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}




