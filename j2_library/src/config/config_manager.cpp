
#include "j2_library/config/config_manager.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace j2::config {

void config_manager::trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    size_t pos = s.find_last_not_of(" \t\r\n");
    if (pos != std::string::npos) s.erase(pos + 1);
}

bool config_manager::load(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(file_path);
    if (!file.is_open()) return false;

    m_config_map.clear();
    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t sep = line.find('=');
        if (sep != std::string::npos) {
            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 1);

            trim(key);
            trim(val);

            // 따옴표 및 이스케이프 처리
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.size() - 2);

                // \" -> " 처리
                size_t pos = 0;
                while ((pos = val.find("\\\"", pos)) != std::string::npos) {
                    val.replace(pos, 2, "\"");
                    pos += 1;
                }
                // \\ -> \ 처리
                pos = 0;
                while ((pos = val.find("\\\\", pos)) != std::string::npos) {
                    val.replace(pos, 2, "\\");
                    pos += 1;
                }
            }

            m_config_map[key] = val;
        }
    }
    return true;
}

bool config_manager::save(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ofstream file(file_path);
    if (!file.is_open()) return false;

    file << "# j2_library configuration file\n";
    for (const auto& [key, val] : m_config_map) {
        // 저장 시에도 따옴표 처리가 필요할 수 있으나, 여기서는 단순 저장
        file << key << "=" << val << "\n";
    }
    return true;
}

std::string config_manager::get_string(const std::string& key, const std::string& default_val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_config_map.find(key);
    if (it != m_config_map.end()) return it->second;
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
    if (val == "true" || val == "1" || val == "on" || val == "yes") return true;
    if (val == "false" || val == "0" || val == "off" || val == "no") return false;
    return default_val;
}

void config_manager::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config_map[key] = value;
}

std::vector<std::string> config_manager::get_all_keys() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> keys;
    for (const auto& [key, _] : m_config_map) {
        keys.push_back(key);
    }
    return keys;
}


} // namespace j2::config
