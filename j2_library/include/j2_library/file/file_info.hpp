#pragma once

#include <string>
#include <filesystem>

#include "j2_library/export.hpp"

namespace j2::file { 

    class J2LIB_API file_info {
    public:
        explicit file_info(const std::string& path);

        bool exists() const;
        bool is_file() const;
        bool is_dir() const;

        std::string file_name() const;
        std::string extension() const;
        std::string parent_path() const;
        std::string absolute_path() const;

        std::uintmax_t file_size() const;

    private:
        std::filesystem::path path_;
    };

} // namespace j2::file
