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
	src/render/gameboy_renderer.cpp
	src/render/message_renderer.cpp
)

set(KORLOW_RENDER_HEADERS
	${KORLOW_FONT_HEADERS}
	src/render/opengl.h
	src/render/gl_util.h
	src/render/renderer.h
	src/render/gameboy_renderer.h
	src/render/message_renderer.h
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
	src/gpu.cpp
	src/fs.cpp
	src/rom_util.cpp
	src/message_manager.cpp
)

set(KORLOW_HEADERS
	${KORLOW_GUI_HEADERS}
	${KORLOW_RENDER_HEADERS}
	${KORLOW_CPU_HEADERS}
	src/args.hxx
	src/emulator.h
	src/mmu.h
	src/gpu.h
	src/types.h
	src/fs.h
	src/rom_util.h
	src/memory_map.h
	src/message_manager.h
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
