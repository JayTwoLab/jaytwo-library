
#include "j2_library/config/config.hpp" // 설정 관리 유틸
#include "j2_library/string/to_console_encoding.hpp" // 윈도우인 경우, UTF-8 → 콘솔(EUC-KR) 인코딩 변환 유틸

#include <iostream>

int main() {
    auto& config = j2::config::config_manager::get_instance();

    // 파일 로드 테스트
    if (config.load("app_config.conf")) {
        std::string app_name = config.get_string("app.name", "Default App");
        std::string desc = config.get_string("app.description");
        int timeout = config.get_int("network.timeout", 30);

        std::cout << "App Name: " << j2::string::to_console_encoding(app_name) << std::endl;
        std::cout << "Description: " << j2::string::to_console_encoding(desc) << std::endl;
        std::cout << "Timeout: " << timeout << "s" << std::endl;
    }

    // 값 수정 및 저장
    config.set("app.last_run", "2026-03-23");
    config.save("app_config.conf");

    return 0;
}
