set(KORLOW_TEST_SOURCES
	main.cpp
	rotation.cpp
	addition.cpp
	subtraction.cpp
	decrement.cpp
	increment.cpp
	load.cpp
	flow.cpp
	stack.cpp
)

set(KORLOW_TEST_HEADERS
	doctest.h
)

add_executable(test_app
	${KORLOW_TEST_SOURCES}
	${KORLOW_TEST_HEADERS}
)

target_link_libraries(test_app
	PRIVATE
		Korlow
		epoxy
)

set_target_properties(test_app
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY 
			${OUT_DIR}
)

target_compile_features(test_app PRIVATE cxx_std_17)

include(doctest)
doctest_discover_tests(test_app)
