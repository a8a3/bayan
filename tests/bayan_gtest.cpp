#include <gtest/gtest.h>
#include <matrix_impl.hpp>
#include <memory>

// ------------------------------------------------------------------
TEST(bayan_test, bayan_test) {
    ASSERT_EQ(1, 1);
}

// ------------------------------------------------------------------
int main( int argc, char* argv[] ) {
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}