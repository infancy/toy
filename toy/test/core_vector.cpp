#include <gtest/gtest.h>

#include "toy/core/initializer_list.h"

// using namespace toy; 

// -----------------------------------------------------------------------------

TEST(initializer_list_test, empty_vector)
{

	toy::initializer_list<int> il;

	ASSERT_EQ(0, il.size());

	ASSERT_EQ(nullptr, il.begin());
	ASSERT_EQ(nullptr, il.end());

	ASSERT_EQ(nullptr, begin(il));
	ASSERT_EQ(nullptr, end(il));
}