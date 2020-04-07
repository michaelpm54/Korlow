cmake_minimum_required(VERSION 3.14)

option(DO_TESTS "Enable testing" ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(Korlow LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(OUT_DIR "bin")
set(CMAKE_AUTOMOC ON)

set(KORLOW_GUI_SOURCES
	src/gui/opengl_widget.cpp
)

set(KORLOW_GUI_HEADERS
	src/gui/opengl_widget.h
)

set(KORLOW_FONT_SOURCES
	src/render/font/ft_font.cpp
	src/render/font/ft_util.cpp
)

set(KORLOW_FONT_HEADERS
	src/render/font/ft_font.h
	src/render/font/ft_util.h
)

set(KORLOW_RENDER_SOURCES
	${KORLOW_FONT_SOURCES}
	src/render/gl_util.cpp
	src/render/main_scene.cpp
	src/render/map_scene.cpp
	src/render/message_renderer.cpp
	src/textured_quad_gl.cpp
)

set(KORLOW_RENDER_HEADERS
	${KORLOW_FONT_HEADERS}
	src/render/opengl.h
	src/render/gl_util.h
	src/render/renderer.h
	src/render/main_scene.h
	src/render/map_scene.h
	src/render/message_renderer.h
	src/textured_quad_gl.h
)

set(KORLOW_CPU_SOURCES
	src/cpu/cpu.cpp
	src/cpu/cpu_base.cpp
	src/cpu/cpu_instructions.cpp
	src/cpu/inst_data.cpp
)

set(KORLOW_CPU_HEADERS
	src/cpu/cpu.h
	src/cpu/cpu_base.h
	src/cpu/cpu_instructions.h
	src/cpu/inst_data.h
)

set(KORLOW_SOURCES
	${KORLOW_GUI_SOURCES}
	${KORLOW_RENDER_SOURCES}
	${KORLOW_CPU_SOURCES}
	src/main.cpp
	src/emulator.cpp
	src/mmu.cpp
	src/ppu.cpp
	src/ppu_map_proxy.cpp
	src/fs.cpp
	src/rom_util.cpp
	src/message_manager.cpp
	src/gameboy.cpp
)

set(KORLOW_HEADERS
	${KORLOW_GUI_HEADERS}
	${KORLOW_RENDER_HEADERS}
	${KORLOW_CPU_HEADERS}
	src/args.hxx
	src/emulator.h
	src/mmu.h
	src/ppu.h
	src/ppu_map_proxy.h
	src/types.h
	src/fs.h
	src/rom_util.h
	src/memory_map.h
	src/message_manager.h
	src/constants.h
	src/component.h
	src/gameboy.h
)

add_executable(app
	${KORLOW_SOURCES}
	${KORLOW_HEADERS}
)

target_include_directories(app PUBLIC src)

target_include_directories(app
	INTERFACE
		${CMAKE_SOURCE_DIR}/src
)

set_property(TARGET app PROPERTY CXX_STANDARD 20)
set_property(TARGET app PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC_VERSION GREATER_EQUAL "1900")
    target_compile_features(app PRIVATE cxx_std_20)
endif()

find_package(GLEW REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Widgets Gui CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

target_link_libraries(
	app
	PRIVATE
		GLEW::GLEW
		freetype
		glm
		Qt5::Core Qt5::Widgets Qt5::Gui
)

set_target_properties(app
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY 
			${OUT_DIR}
)

add_custom_command(TARGET app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
              ${CMAKE_CURRENT_LIST_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

if (DO_TESTS)
	set(KORLOW_TEST_SOURCES
	    ${KORLOW_CPU_SOURCES}
		src/mmu.cpp
		src/ppu.cpp
		tests/main.cpp
		tests/rotation.cpp
		tests/addition.cpp
		tests/subtraction.cpp
		tests/decrement.cpp
		tests/increment.cpp
		tests/load.cpp
		tests/flow.cpp
		tests/stack.cpp
		tests/logic.cpp
	)

	set(KORLOW_TEST_HEADERS
		${KORLOW_CPU_HEADERS}
		#doctest.h
	)

	add_executable(test_app
		${KORLOW_TEST_SOURCES}
		${KORLOW_TEST_HEADERS}
	)

	target_include_directories(test_app PUBLIC src)

	target_link_libraries(test_app PRIVATE doctest::doctest)

	set_target_properties(test_app
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY
				${OUT_DIR}
	)

	set_property(TARGET test_app PROPERTY CXX_STANDARD 20)
	set_property(TARGET test_app PROPERTY CXX_STANDARD_REQUIRED ON)

	if (MSVC_VERSION GREATER_EQUAL "1900")
		target_compile_features(test_app PRIVATE cxx_std_20)
	endif()

	include(doctest)
	doctest_discover_tests(test_app)
endif (DO_TESTS)
