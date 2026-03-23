#pragma once

#include "j2_library/export.hpp"

#include <string>
#include <map>
#include <mutex>
#include <vector>

namespace j2::config {

class config_manager {
public:
    /**
        * @brief singleton 인스턴스를 반환합니다.
        */
    static config_manager& get_instance() {
        static config_manager instance;
        return instance;
    }

    /**
        * @brief 설정 파일을 로드합니다.
        * @param file_path 파일 경로
        * @return 성공 여부
        */
    bool load(const std::string& file_path);

    /**
        * @brief 현재 설정을 파일로 저장합니다.
        * @param file_path 파일 경로
        * @return 성공 여부
        */
    bool save(const std::string& file_path);

    // 값 가져오기
    std::string get_string(const std::string& key, const std::string& default_val = "");
    int get_int(const std::string& key, int default_val = 0);
    bool get_bool(const std::string& key, bool default_val = false);

    /**
        * @brief 새로운 설정을 추가하거나 기존 값을 변경합니다.
        */
    void set(const std::string& key, const std::string& value);

    /**
        * @brief 모든 키 목록을 가져옵니다.
        */
    std::vector<std::string> get_all_keys();

private:
    config_manager() = default;
    ~config_manager() = default;
    config_manager(const config_manager&) = delete;
    config_manager& operator=(const config_manager&) = delete;

    // 내부 문자열 공백 제거 헬퍼
    void trim(std::string& s);

    std::map<std::string, std::string> m_config_map;
    std::mutex m_mutex;
};

}  // namespace j2::config
