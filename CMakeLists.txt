cmake_minimum_required(VERSION 3.14)
project(Korlow LANGUAGES CXX)

option(DO_TESTS "Enable testing" ON)

# Used for doctest and conan
list(APPEND
	CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/cmake
	${CMAKE_SOURCE_DIR}
)

set(KORLOW_SRC_SOURCES
	src/main.cpp
	src/fs.cpp
	src/mmu.cpp
	src/ppu.cpp
	src/rom_util.cpp
	src/cpu/cpu.cpp
	src/cpu/cpu_base.cpp
	src/cpu/cpu_instructions.cpp
	src/cpu/inst_data.cpp
	src/render/gl_shader.cpp
	src/render/gl_texture.cpp
	src/render/gl_rect.cpp
	src/render/sdl.cpp
	src/render/message_queue.cpp
	src/render/image_window.cpp
	src/render/tiles_window.cpp
	src/render/map_window.cpp
)

set(KORLOW_LIB_SOURCES
	lib/glad/glad.cpp
	lib/imgui/imgui_impl_opengl3.cpp
	lib/imgui/imgui_impl_sdl.cpp
)

add_executable(app
	${KORLOW_SRC_SOURCES}
	${KORLOW_LIB_SOURCES}
)

target_include_directories(app PRIVATE
	src

	# My includes
	include

	# Includes from external projects
	include

	# Need to include this one specifically because the files there use `#include <file.h>` and not
	# `include <imgui/file.h>`
	include/imgui
)

target_include_directories(app
	INTERFACE
		${CMAKE_SOURCE_DIR}/src
)

set_property(TARGET app PROPERTY CXX_STANDARD 20)
set_property(TARGET app PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

target_link_libraries(
	app
	PRIVATE
		${CONAN_LIBS}
)

add_custom_command(TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
              ${CMAKE_CURRENT_LIST_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

# Copy the DLL's built by Conan to the output directory
# Note: Conan places them in build/bin/, as configured in conanfile.txt
add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/bin/
        $<TARGET_FILE_DIR:app>
)

if (DO_TESTS)
	set(KORLOW_TEST_SOURCES
		src/cpu/inst_data.cpp
		src/cpu/cpu_base.cpp
		src/cpu/cpu_instructions.cpp
		src/cpu/cpu.cpp
		src/mmu.cpp
		src/ppu.cpp
		tests/main.cpp
		#tests/rotation.cpp
		#tests/addition.cpp
		#tests/subtraction.cpp
		#tests/decrement.cpp
		#tests/increment.cpp
		tests/load.cpp
		#tests/flow.cpp
		#tests/stack.cpp
		#tests/logic.cpp
	)

	add_executable(test_app
		${KORLOW_TEST_SOURCES}
	)

	target_include_directories(test_app PRIVATE src include include/lib)

	target_link_libraries(test_app PRIVATE CONAN_PKG::doctest)

	set_property(TARGET test_app PROPERTY CXX_STANDARD 20)
	set_property(TARGET test_app PROPERTY CXX_STANDARD_REQUIRED ON)

	if (MSVC_VERSION GREATER_EQUAL "1900")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
		if (_cpp_latest_flag_supported)
			add_compile_options("/std:c++latest")
		endif()
	endif()

	include(doctest)
	doctest_discover_tests(test_app)
endif (DO_TESTS)
