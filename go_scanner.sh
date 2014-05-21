re2c -o scannerTest.cpp scannerTestRe2c.cpp
g++ scannerTest.cpp -std=c++11 -O0 -o scannerTest
./scannerTest