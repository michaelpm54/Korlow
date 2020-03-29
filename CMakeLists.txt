cmake_minimum_required(VERSION 3.14)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(Korlow LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(OUT_DIR "bin")
set(CMAKE_AUTOMOC ON)

set(KORLOW_GUI_SOURCES
	src/gui/main_window.cpp
	src/gui/opengl_widget.cpp
)

set(KORLOW_GUI_HEADERS
	src/gui/main_window.h
	src/gui/opengl_widget.h
)

set(KORLOW_SOURCES
	src/main.cpp
	src/emulator.cpp
	src/window.cpp
	src/mmu.cpp
	src/cpu.cpp
	src/gpu.cpp
	src/cpu_instructions.cpp
	src/cpu_base.cpp
	src/inst_data.cpp
	src/font.cpp
	src/fs.cpp
	src/gl_util.cpp
	src/rom_util.cpp
	src/gameboy_renderer.cpp
	src/message_manager.cpp
	src/message_renderer.cpp
	${KORLOW_GUI_SOURCES}
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
	src/key_receiver.h
	src/font.h
	src/opengl.h
	src/fs.h
	src/gl_util.h
	src/rom_util.h
	src/renderer.h
	src/gameboy_renderer.h
	src/message_manager.h
	src/message_renderer.h
	${KORLOW_GUI_HEADERS}
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

#add_subdirectory(tests)
#enable_testing()
#add_test(NAME KorlowTest COMMAND test_app)
