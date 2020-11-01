./tools/generate.sh -- -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON && ./tools/build.sh || exit 1

# todo : handle msvc and document

make -C ./build/Debug/x64 coverage
