#include <iostream>
#include "j2_library/j2_library.hpp"

int main()
{
    namespace jc = j2::core;

    std::cout << "j2_library version: " << j2::VersionString << "\n";
    std::cout << "2 + 5 = " << jc::add(2, 5) << "\n";

    return 0;
}
