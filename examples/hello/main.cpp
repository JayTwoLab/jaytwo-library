#include <iostream>
#include "j2_library/j2_library.hpp"

int main()
{
    namespace jc = j2::core;

    std::cout << "j2_library version: " << j2::VersionString << "\n";
    std::cout << "2 + 5 = " << jc::add(2, 5) << "\n";
    std::cout << "trim(\"  hello  \") = [" << jc::trim("  hello  ") << "]\n";

    auto parts = jc::split("a:b::c", ':');
    std::cout << "split(\"a:b::c\", ':') -> size = " << parts.size() << "\n";
    for (const auto& p : parts) std::cout << " - [" << p << "]\n";

    std::cout << "to_upper(\"Hi! j2\") = " << jc::to_upper("Hi! j2") << "\n";
    return 0;
}
