#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <typeinfo>
#include <type_traits>
#include <limits>

namespace j2::core {

    template <typename T>
    std::string to_string(typename std::enable_if< std::is_floating_point<T>::value, T >::type value)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(std::numeric_limits<T>::max_digits10) << value;
        std::string ret = oss.str();
        return ret;
    }

    template <typename T>
    std::string to_string(typename std::enable_if< std::is_floating_point<T>::value, T >::type value, long long precision)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        std::string ret = oss.str();
        return ret;
    }

    template <typename T>
    bool is_equal(T value1, T value2, long long precision)
    {
        std::string str1 = to_string<T>(value1, precision);
        std::string str2 = to_string<T>(value2, precision);

        if (str1 == str2) {
            return true;
        }

        return false;
    }

    template <typename T>
    bool is_equal(T value1, T value2)
    {
        std::string str1 = to_string<T>(value1);
        std::string str2 = to_string<T>(value2);

        if (str1 == str2) {
            return true;
        }

        return false;
    }

} // namespace j2::core
