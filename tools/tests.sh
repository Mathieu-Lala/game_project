./tools/generate.sh -- -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON && ./tools/build.sh || exit 1

make -C ./build/Debug/x64 generated_coverage
