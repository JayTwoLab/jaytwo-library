#include <iostream>
#include <j2_library/j2_library.hpp>

int main()
{
    std::cout << "j2 library version: " << j2::VersionString << std::endl;

    // J2LIB_API class usage
    j2::bit::bit_array src({ 0b11000011, 0b10100110 }, 16);
    auto sub1 = src.get(4, 8).to_array();

    return 0;
}
