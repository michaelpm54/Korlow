# Requirements
- Conan
- Qt5

# Build
`conan install . -if build -s build_type=debug`

Downloads and installs dependencies into `build`.

`cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug`

Requires Qt5. Can be provided to cmake via Qt5_DIR variables, i.e. `cmake .. -DCMAKE_BUILD_TYPE=Debug -DQt5_DIR=C:\Qt\5.13.2\msvc2017_64\lib\cmake\Qt5`.

`cmake --build .`
