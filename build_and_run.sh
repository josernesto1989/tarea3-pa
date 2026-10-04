mkdir -p build
g++ -O2 -std=c++11 -mavx main.cpp -o build/main.o 
./build/main.o