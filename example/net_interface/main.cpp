
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

#include "j2_library/network/network.hpp"

// 네임스페이스 별칭 사용 가능
namespace j2if = j2::network::interface;

// 인터페이스 상세 정보를 출력하는 함수
void print_interface_details();

// 현재 시간을 [YYYY-MM-DD HH:MM:SS] 문자열로 반환하는 헬퍼 함수
std::string get_current_timestamp();

int main() {

    // 1. 먼저 상세 목록을 한 번 출력합니다.
    print_interface_details();

    // 2. 이후 실시간 모니터링으로 넘어갑니다. 
    std::cout << "Starting Network Interface Monitor..." << std::endl;

    auto prev_data = j2if::InterfaceManager::get_interfaces();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto curr_data = j2if::InterfaceManager::get_interfaces();

        // 매 출력 시 상단에 현재 시간 표시
        std::cout << "\n" << get_current_timestamp() << " Network Status:" << std::endl;
        std::cout << std::setw(15) << "Interface"
            << std::setw(18) << "IP Address"
            << std::setw(15) << "RX (KB/s)"
            << std::setw(15) << "TX (KB/s)" << std::endl;
        std::cout << std::string(65, '-') << std::endl;

        for (size_t i = 0; i < curr_data.size(); ++i) {
            if (i >= prev_data.size()) break;

            // 초당 속도 계산
            double rx_kb = (curr_data[i].stats.rx_bytes - prev_data[i].stats.rx_bytes) / 1024.0;
            double tx_kb = (curr_data[i].stats.tx_bytes - prev_data[i].stats.tx_bytes) / 1024.0;

            std::cout << std::setw(15) << curr_data[i].name
                << std::setw(18) << curr_data[i].ip_address
                << std::fixed << std::setprecision(2)
                << std::setw(15) << rx_kb
                << std::setw(15) << tx_kb << std::endl;
        }
        prev_data = curr_data;
    }

    return 0;
}

void print_interface_details() {
    std::cout << "=====================================================" << std::endl;
    std::cout << "      System Network Interface Enumeration          " << std::endl;
    std::cout << "=====================================================" << std::endl;

    // 모든 인터페이스 정보 가져오기
    std::vector<j2if::InterfaceInfo> interfaces = j2if::InterfaceManager::get_interfaces();

    if (interfaces.empty()) {
        std::cout << "No network interfaces found." << std::endl;
        return;
    }

    for (const auto& info : interfaces) {
        std::cout << "[ Interface: " << info.name << " ]" << std::endl;
        std::cout << "  - Status:      " << (info.is_running ? "ONLINE (UP)" : "OFFLINE (DOWN)") << std::endl;
        std::cout << "  - IP Address:  " << (info.ip_address.empty() ? "N/A" : info.ip_address) << std::endl;
        std::cout << "  - MAC Address: " << info.mac_address << std::endl;

        // 초기 누적 사용량 출력
        std::cout << "  - Total RX:    " << (info.stats.rx_bytes / 1024 / 1024) << " MB" << std::endl;
        std::cout << "  - Total TX:    " << (info.stats.tx_bytes / 1024 / 1024) << " MB" << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;
    }
}

// 현재 시간을 [YYYY-MM-DD HH:MM:SS] 문자열로 반환하는 헬퍼 함수
std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "]";
    return ss.str();
}

