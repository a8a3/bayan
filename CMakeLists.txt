cmake_minimum_required(VERSION 3.2)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(bayan VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(bayan VERSION 0.0.1)
endif()

find_package(Threads REQUIRED)
find_package(GTest  REQUIRED)
find_package(Boost  REQUIRED)

add_executable(bayan       sources/bayan.cpp)
add_executable(bayan_gtest tests/bayan_gtest.cpp)

set_target_properties(bayan PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(bayan_gtest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(bayan_gtest
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
)

if (MSVC)
    target_compile_options(bayan PRIVATE
        /W4 -O3
    )
    target_compile_options(bayan_gtest PRIVATE
        /W4 -O3
    )
else ()
    target_compile_options(bayan PRIVATE
        -Wall -Wextra -pedantic -Werror -O3
    )
    target_compile_options(bayan_gtest PRIVATE
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

add_test(bayan_gtest bayan_gtest)

add_custom_command(
    TARGET bayan_gtest
    POST_BUILD 
    COMMAND ./bayan_gtest
    COMMENT "Run bayan test..."
)