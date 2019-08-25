cmake_minimum_required(VERSION 3.14)
project(Korlow LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(OUT_DIR "bin")

set(KORLOW_SOURCES
	src/emulator.cpp
	src/window.cpp
	src/mmu.cpp
	src/cpu.cpp
	src/gpu.cpp
	src/cpu_instructions.cpp
	src/cpu_base.cpp
	src/inst_data.cpp
)

set(KORLOW_HEADERS
	src/args.hxx
	src/emulator.h
	src/window.h
	src/mmu.h
	src/cpu.h
	src/gpu.h
	src/cpu_instructions.h
	src/cpu_base.h
	src/types.h
	src/inst_data.h
	src/memory_map.h
)

add_library(Korlow
	${KORLOW_SOURCES}
	${KORLOW_HEADERS}
)

add_executable(app
	src/main.cpp
)

target_include_directories(Korlow
	INTERFACE
		${CMAKE_SOURCE_DIR}/src
)

target_compile_features(app PRIVATE cxx_std_17)
target_compile_features(Korlow PRIVATE cxx_std_17)

find_package(GLFW3 REQUIRED)
find_package(epoxy REQUIRED)

target_link_libraries(
	app
	PRIVATE
		Korlow
		GLFw3
		epoxy
)

set_target_properties(app
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY 
			${OUT_DIR}
)

add_subdirectory(tests)
enable_testing()
add_test(NAME KorlowTest COMMAND test_app)
