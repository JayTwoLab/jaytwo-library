#include <iostream>
#include <j2_library/j2_library.hpp>

int main()
{
    std::cout << "j2 library version: " << j2::VersionString << std::endl;

    j2::bit::bit_array bits1; // J2LIB_API class usage
    bits1.set(0);
    bits1.set(2);
    bits1.set(4);
    std::cout << "bits1: " << bits1.to_string() << std::endl;

    return 0;
}
