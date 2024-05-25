gcc ./src/*.c -o ./build/arh
chcp 65001
cd build
arh -c ../test.txt ../outtest --full