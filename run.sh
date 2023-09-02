echo "Install dependencies & Boost"
vcpkg install boost
vcpkg install boost-asio
mkdir build
cd build
cmake ..
make
echo "Run server:"
echo "  cd build"
echo "  ./web_api:"