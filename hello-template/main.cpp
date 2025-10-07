#include <iostream>
#include "j2_library/j2_library.hpp"

int main()
{
    std::cout << "j2_library version: " << j2::VersionString << "\n";
    std::cout << "3 + 4 = " << j2::add(3, 4) << "\n";

    return 0;
}
