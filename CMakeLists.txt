cmake_minimum_required(VERSION 3.2)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(bayan VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(bayan VERSION 0.0.1)
endif()

find_package(Boost
        COMPONENTS
        program_options
        filesystem
        unit_test_framework
        REQUIRED)

add_executable(bayan       sources/bayan.cpp)
add_executable(bayan_tests tests/bayan_tests.cpp)

set_target_properties(bayan PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(bayan
    PRIVATE
        Boost::program_options
        Boost::filesystem
)

target_include_directories(bayan_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sources
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(bayan_tests
    PRIVATE
        Boost::unit_test_framework
)

target_compile_definitions(bayan_tests
    PRIVATE
        BOOST_TEST_DYN_LINK
)

if (MSVC)
    target_compile_options(bayan
        PRIVATE
            /W4 -O3
    )
    target_compile_options(bayan_tests
        PRIVATE
            /W4 -O3
    )
else ()
    target_compile_options(bayan
        PRIVATE
            -Wall -Wextra -pedantic -Werror -O3
    )
    target_compile_options(bayan_tests
        PRIVATE
            -Wall -Wextra -pedantic -Werror -O3
    )
endif()

install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT bayan@mail.ru)

include(CPack)

enable_testing()

add_test(test bayan_test)

add_custom_command(
    TARGET bayan_tests
    POST_BUILD 
    COMMAND ./bayan_tests
    COMMENT "Run bayan tests..."
)