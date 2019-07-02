#ifdef NDEBUG
    #error "Macro NDEBUG must not be defined for our tests"
#endif


#include "gtest/gtest.h"


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
