# Exports target: Epoxy::Epoxy

find_path(
	EPOXY_INCLUDE
	NAMES
		epoxy/gl.h
	PATHS
		/mingw64/include
		${EPOXY_DIR}/include
		$ENV{EPOXY_DIR}/include
		${CMAKE_SOURCE_DIR}/3rdparty/include
		${CMAKE_SOURCE_DIR}/external/include
)
mark_as_advanced(EPOXY_INCLUDE)

find_library(
	EPOXY_LIBRARY
	NAMES
		epoxy-0
		libepoxy
	HINTS
		/mingw64/lib
		${EPOXY_DIR}/bin
		$ENV{EPOXY_DIR}/bin
		${CMAKE_SOURCE_DIR}/3rdparty/bin
		${CMAKE_SOURCE_DIR}/external/bin
		${EPOXY_DIR}/lib
		$ENV{EPOXY_DIR}/lib
		${CMAKE_SOURCE_DIR}/3rdparty/lib
		${CMAKE_SOURCE_DIR}/external/lib
)
mark_as_advanced(EPOXY_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	Epoxy
	DEFAULT_MSG
	EPOXY_INCLUDE
	EPOXY_LIBRARY
)

if (NOT TARGET Epoxy::Epoxy)
	add_library(Epoxy::Epoxy UNKNOWN IMPORTED)
endif()

message(STATUS "EPOXY_INCLUDE = '${EPOXY_INCLUDE}'")
message(STATUS "EPOXY_LIBRARY = '${EPOXY_LIBRARY}'")

set_target_properties(
	Epoxy::Epoxy
	PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${EPOXY_INCLUDE}"
		IMPORTED_LINK_INTERFACE_LANGUAGES "C"
		IMPORTED_LOCATION "${EPOXY_LIBRARY}"
		LOCATION_RELEASE "${EPOXY_LIBRARY}"
)
