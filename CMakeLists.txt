cmake_minimum_required(VERSION 3.14)
project(Korlow LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(OUT_DIR "bin")

set(KORLOW_SOURCES
	src/main.cpp
	src/emulator.cpp
	src/window.cpp
	src/mmu.cpp
	src/cpu.cpp
	src/gpu.cpp
	src/cpu_instructions.cpp
)

set(KORLOW_HEADERS
	src/args.hxx
	src/emulator.h
	src/window.h
	src/mmu.h
	src/cpu.h
	src/gpu.h
	src/cpu_instructions.h
	src/types.h
)

add_executable(app
	${KORLOW_SOURCES}
	${KORLOW_HEADERS}
)

find_package(GLFW3 REQUIRED)
find_package(epoxy REQUIRED)

target_link_libraries(
	app
	PRIVATE
		GLFw3
		epoxy
)

set_target_properties(app
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY 
			${OUT_DIR}
)
