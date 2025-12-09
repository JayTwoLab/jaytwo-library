// publisher example

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "j2_library/j2_library.hpp"
 
struct Person { // example struct

    std::string name;
    int age;
    std::vector<std::string> tags;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, name, age, tags) // for nlohmann::json serialization
};

std::string get_current_time_string();

int main() {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return (-1);
    }
#endif

    Person person;
    person.name = "John Doe";
    person.age = 30;
    person.tags = {"developer", "cpp", "json"};
        
    nlohmann::json json_data = person; // Serialize to JSON

    j2::network::udp::udp_sender sender; // udp sender

    std::string ip = "127.0.0.1"; // loopback address  
    uint16_t port = 54000;

    sender.setServer(ip, port);

    if (!sender.create()) {
        std::cerr << "Failed to create UDP sender" << std::endl;
        return 1;
    }

    while (true)
    {
        // wait for 5 second
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // send serialized JSON string over UDP
        sender.send_data(json_data.dump());

        std::cout << get_current_time_string() << " sent data " << std::endl;
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
