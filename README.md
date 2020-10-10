# Game Project - The PURGE

Keimyung University - Mobile Game Development - 2020 Fall Semester

This project is a video game for PC. See the [GDD](doc/GDD_ten_page.pdf) for more details.

This repository contains the source code of the game and his engine.

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

* Windows

    * Visual Studio 2019>=16.7
    * Linux CMake extension (see [the documentation](https://docs.microsoft.com/en-us/cpp/linux/cmake-linux-configure?view=vs-2019))
    * python>=3.8

The build **require** an internet connection (download of dependencies if missing).

### Recommended environment

* Unix systems

    * [ccache](https://ccache.dev/)
    * [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

* Windows

    * [VS conan extension](https://blog.conan.io/2019/06/17/Conan-extension-for-Visual-Studio.html)

## Build and Run

![Build Status](https://github.com/Pierre-Genest/game_project/workflows/C++%20CMake%20Build/badge.svg)

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

![Dependencies](doc/conan_dependencies.png)

## Testing

## Acknowledgement

* Jason Turner's [Weekly Game Project](https://github.com/lefticus/cpp_weekly_game_project)

## Initial Authors

* Mathieu LALA - [github](https://github.com/Mathieu-Lala)
* Benjamin PRAUD - [github](https://github.com/BenjaminPraud)
* Yanis FOUREL - [github](https://github.com/Yanis-F)
* Adil RAJABALY - [github](https://github.com/ADeal24)
* Pierre-Genest ESTRADE - [github](https://github.com/Pierre-Genest)
