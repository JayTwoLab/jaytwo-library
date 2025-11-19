
#include "j2_library/j2_library.hpp"


void udp_sender_thread(j2::network::udp::udp_sender& sender, std::atomic<bool>& running_flag) {
    int counter = 0;
    while (running_flag.load()) {
        sender.send_data_to("Hello, UDP Server! Message " + std::to_string(counter), "127.0.0.1", 12345);
        std::cout << " send data..." << std::endl;
        ++counter;
        std::this_thread::sleep_for(std::chrono::seconds(3));
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
    // sender.setServer("127.0.0.1", 12345);

    if (!sender.create()) {
        return 1;
    }

    sender.set_multicast_ttl(2); // Multicast Egress TTL Settings (Optional)
    sender.send_data_to("Hello Multicast", "239.0.0.1", 12345); // Send multicast

    sender.send_data_to("Hello Broadcast", "255.255.255.255", 12345); // Broadcast transmission

    std::atomic<bool> running_flag(true);
    std::thread sender_thread(udp_sender_thread, std::ref(sender), std::ref(running_flag));

    // Main thread loop with 1-second sleep
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Main thread is running..." << std::endl;

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

