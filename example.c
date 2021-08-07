#include "corn_test.h"

TEST(foo)
{
	ASSERT(4 == 3, "err!");
}

TEST(bar)
{
	ASSERT_EQ(1, 2 - 1);
	ASSERT_EQ(3, 4);
}

TEST_MAIN()
