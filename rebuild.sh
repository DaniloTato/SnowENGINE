rm -rf build
cmake -B build -S .
cmake --build build --parallel 8