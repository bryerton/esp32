cd thirdparty/cppcheck
mkdir build
cd build
cmake .. -DUSE_MATCHCOMPILER=ON -DHAVE_RULES=ON
cmake --build .
sudo make install
