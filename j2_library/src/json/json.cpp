#include "j2_library/json/json.hpp"

namespace j2::json {

    bool exists(const nj& j, const std::string& path) noexcept {
        return j.contains(njj(path));
    }

    std::string get_string(const nj& j,
        const std::string& path,
        const std::string& defval) noexcept {
        return value_or_path(j, path, defval);
    }

    int get_int(const nj& j,
        const std::string& path,
        int defval) noexcept {
        return value_or_path<int>(j, path, defval);
    }

    bool get_bool(const nj& j,
        const std::string& path,
        bool defval) noexcept {
        return value_or_path(j, path, defval);
    }

    double get_double(const nj& j,
        const std::string& path,
        double defval) noexcept {
        return value_or_path<double>(j, path, defval);
    }

} // namespace j2::json
