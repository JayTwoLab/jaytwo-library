
#include "j2_library/config/config.hpp"

#include <fstream>
#include <sstream>

namespace j2 {
    namespace config {

        void config_manager::trim(std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            size_t pos = s.find_last_not_of(" \t\r\n");
            if (pos != std::string::npos) s.erase(pos + 1);
        }

        std::string config_manager::process_escape(std::string val) {
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.size() - 2);

                // \" -> " 치환
                size_t pos = 0;
                while ((pos = val.find("\\\"", pos)) != std::string::npos) {
                    val.replace(pos, 2, "\"");
                    pos += 1;
                }
                // \\ -> \ 치환
                pos = 0;
                while ((pos = val.find("\\\\", pos)) != std::string::npos) {
                    val.replace(pos, 2, "\\");
                    pos += 1;
                }
            }
            return val;
        }

        bool config_manager::load(const std::string& file_path) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::ifstream file(file_path);
            if (!file.is_open()) return false;

            m_lines.clear();
            std::string line_text;
            while (std::getline(file, line_text)) {
                config_line node;
                node.raw_text = line_text;
                node.is_data = false;

                std::string temp = line_text;
                trim(temp);

                // 주석(#)이 아니고 빈 줄이 아닐 경우 데이터 파싱 시도
                if (!temp.empty() && temp[0] != '#') {
                    size_t sep = temp.find('=');
                    if (sep != std::string::npos) {
                        std::string k = temp.substr(0, sep);
                        std::string v = temp.substr(sep + 1);
                        trim(k);
                        trim(v);

                        node.key = k;
                        node.value = process_escape(v);
                        node.is_data = true;
                    }
                }
                m_lines.push_back(node);
            }
            return true;
        }

        bool config_manager::save(const std::string& file_path) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::ofstream file(file_path);
            if (!file.is_open()) return false;

            for (const auto& line : m_lines) {
                if (line.is_data) {
                    // 데이터 라인은 'key = value' 형식으로 갱신하여 저장
                    file << line.key << " = " << line.value << "\n";
                }
                else {
                    // 주석이나 빈 줄은 원본 raw_text 그대로 저장
                    file << line.raw_text << "\n";
                }
            }
            return true;
        }

        std::string config_manager::get_string(const std::string& key, const std::string& default_val) {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& line : m_lines) {
                if (line.is_data && line.key == key) {
                    return line.value;
                }
            }
            return default_val;
        }

        int config_manager::get_int(const std::string& key, int default_val) {
            std::string val = get_string(key, "");
            if (val.empty()) return default_val;
            try {
                return std::stoi(val);
            }
            catch (...) {
                return default_val;
            }
        }

        bool config_manager::get_bool(const std::string& key, bool default_val) {
            std::string val = get_string(key, "");
            std::transform(val.begin(), val.end(), val.begin(), ::tolower);
            if (val == "true" || val == "1" || val == "yes" || val == "on") return true;
            if (val == "false" || val == "0" || val == "no" || val == "off") return false;
            return default_val;
        }

        void config_manager::set(const std::string& key, const std::string& value) {
            std::lock_guard<std::mutex> lock(m_mutex);

            // 1. 기존 키가 리스트에 존재하는지 확인
            for (auto& line : m_lines) {
                if (line.is_data && line.key == key) {
                    line.value = value; // 존재하면 값만 업데이트 (주석 순서 보존)
                    return;
                }
            }

            // 2. 존재하지 않으면 새로운 데이터 라인을 맨 뒤에 추가
            config_line new_node;
            new_node.key = key;
            new_node.value = value;
            new_node.is_data = true;
            new_node.raw_text = key + " = " + value; // 초기 raw_text 생성
            m_lines.push_back(new_node);
        }

    } // namespace config
} // namespace j2
