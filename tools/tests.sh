./tools/generate.sh && ./tools/build.sh || exit 1

make -C ./build/Debug/x64 coverage

# cd src && lcov --directory . -c -o ./gen/rapport.info
#
# genhtml -o ./gen/rapport -t "couverture de code des tests" ./gen/rapport.info
