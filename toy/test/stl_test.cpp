#include <gtest/gtest.h>

#include "toy/core/initializer_list.h"

// using namespace toy; 

// -----------------------------------------------------------------------------

TEST(initializer_list_test, empty_initializer_list)
{
	toy::initializer_list<int> il;

	ASSERT_EQ(0, il.size());

	ASSERT_EQ(nullptr, il.begin());
	ASSERT_EQ(nullptr, il.end());

	ASSERT_EQ(nullptr, begin(il));
	ASSERT_EQ(nullptr, end(il));
}

// -----------------------------------------------------------------------------

GTEST_API_ int main(int argc, char **argv)
{
	// printf("Running main() from gtest_main.cc\n");
	testing::InitGoogleTest(&argc, argv);
	auto result = RUN_ALL_TESTS();
	getchar();
	return result;
}