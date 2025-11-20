# Create build directory
mkdir build 
cd build

# Run cmake (Release build, specify install path)
#  - Change the -DCMAKE_INSTALL_PREFIX value according to your desired install path.
#  - Change the -DCMAKE_BUILD_TYPE value for Debug/Release mode.
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/home/j2/dev/lib/j2_library"
# If using clang
#   cmake -S .. -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ... 

# Build the library (parallel build using number of CPU cores)
cmake --build build -j$(nproc)

# Install the library 
cmake --install build
# If the install path is a system directory like /usr/local/..., sudo (root privileges) is required for installation.
#  sudo cmake --install build
