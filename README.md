# Requirements
- Conan

# Build

## Install dependencies
`conan install . -if build -s build_type=Debug`

Downloads and installs dependencies into `build`.

## Configure CMake
`cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug`

## Build sources
`cmake --build .`

![Screenshot](screenshot.png?raw=true)
