make
make run
llc test.S
clang -o main main.c test.S.s
./main
rm -rf main test.S test.S.s