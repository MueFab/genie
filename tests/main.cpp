#ifdef NDEBUG
    #error "Macro NDEBUG is defined, but tests need assert() to be enabled."
#endif


#include "gtest/gtest.h"


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
