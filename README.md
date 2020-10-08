# game_project

KMU fall's semester game project

## Installing

```sh
# Cloning the repository
$> git clone git@github.com:Pierre-Genest/game_project.git

# Installing the required environment (partial)
$> ./tools/install.sh
```

### Requirements

* Unix systems

    * g++>=10
    * libstdc++11
    * cmake>=3.9.0
    * python>=3.8
    * conan>=1.29

* Windows

    * Visual Studio 2019>=16.7
    * conan>=1.29

The build require an internet connection (for the download of dependencies)

### Recommended environment

* Unix systems

    * [ccache](https://ccache.dev/)
    * [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

* Windows

    * [VS conan extension](https://blog.conan.io/2019/06/17/Conan-extension-for-Visual-Studio.html)

## Build and Run

![Build](https://github.com/Pierre-Genest/game_project/workflows/C++%20CMake%20Build/badge.svg)

* Unix systems

    ```sh
    # Generate the cmake project
    $> ./tools/generate.sh

    # Build all the target
    $> ./tools/build.sh

    # Run the executable
    $> ./build/${build_type}/${arch}/out/app
    ```

### Package Manager

[conan](https://conan.io/) - [documentation](https://docs.conan.io/en/latest/)

## Testing

## Acknowledgement

* Jason Turner's [Weekly Game Project](https://github.com/lefticus/cpp_weekly_game_project)
