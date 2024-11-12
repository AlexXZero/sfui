#include "UTest.hpp"
#include "sfui/UserInterface.h"

UTEST(TestExample)
{
    // Given:
    constexpr bool test = true;

    // When:

    // Then:
    UTEST_ASSERT_EQUAL(true, test);
}
