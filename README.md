# Requirements
- Conan
- Qt5

# Build

## Install dependencies
`conan install . -if build -s build_type=debug`

Downloads and installs dependencies into `build`.

## Configure CMake
`cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug`

Qt5 can be provided to CMake via the Qt5_DIR variable, i.e. `cmake .. -DCMAKE_BUILD_TYPE=Debug -DQt5_DIR=C:\Qt\5.13.2\msvc2017_64\lib\cmake\Qt5`.

## Build sources
`cmake --build .`
