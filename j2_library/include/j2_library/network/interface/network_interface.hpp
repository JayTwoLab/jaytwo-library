#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "j2_library/export.hpp"

namespace j2::network::interface {

struct J2LIB_API InterfaceStats {
    uint64_t rx_bytes = 0; // 누적 수신 바이트
    uint64_t tx_bytes = 0; // 누적 전송 바이트
};

struct J2LIB_API InterfaceInfo {
    std::string name;
    std::string ip_address;
    std::string mac_address;
    InterfaceStats stats;
    bool is_running = false;
};

class J2LIB_API InterfaceManager {
public:
    /**
        * @brief 일반 권한으로 시스템의 모든 네트워크 인터페이스 정보 및 통계를 가져옵니다.
        */
    static std::vector<InterfaceInfo> get_interfaces();
};

} // namespace j2::network::interface


