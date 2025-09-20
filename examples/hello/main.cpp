#include <iostream>
#include "j2_library/j2_library.hpp"
#include "j2_library/version.hpp"

int main()
{
    std::cout << "j2_library version: " << j2::VersionString << "\n";
    std::cout << "2 + 5 = " << j2::add(2, 5) << "\n";
    std::cout << "trim(\"  hello  \") = [" << j2::trim("  hello  ") << "]\n";

    auto parts = j2::split("a:b::c", ':');
    std::cout << "split(\"a:b::c\", ':') -> size = " << parts.size() << "\n";
    for (const auto& p : parts) std::cout << " - [" << p << "]\n";

    std::cout << "to_upper(\"Hi! j2\") = " << j2::to_upper("Hi! j2") << "\n";
    return 0;
}
