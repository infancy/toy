#include <vector>

#include <gtest/gtest.h>

#include "toy/core/initializer_list.h"
#include "toy/core/type_traits.h"
#include "toy/core/utility.h"

// using namespace toy; 

// test initializer_list -------------------------------------------------------

TEST(initializer_list_test, empty_initializer_list)
{
	toy::initializer_list<int> il;

	ASSERT_EQ(0, il.size());

	ASSERT_EQ(nullptr, il.begin());
	ASSERT_EQ(nullptr, il.end());

	ASSERT_EQ(nullptr, begin(il));
	ASSERT_EQ(nullptr, end(il));
}

// test move() -----------------------------------------------------------------

TEST(type_traits_test, move)
{
	std::vector<int> v1{ 1 };
	std::vector<int> v2{ 2 };

	v1 = toy::move(v2);

	ASSERT_EQ(2, v1[0]);
	ASSERT_EQ(0, v2.size());
}

// test swap() -----------------------------------------------------------------

TEST(utility_test, swap)
{
	std::vector<int> v1{ 1 };
	std::vector<int> v2{ 2 };

	toy::swap(v1, v2);

	ASSERT_EQ(2, v1[0]);
	ASSERT_EQ(1, v2[0]);
}

TEST(utility_test, swap_array)
{
	int v1[]{ 1 };
	int v2[]{ 2 };

	toy::swap(v1, v2);

	ASSERT_EQ(2, v1[0]);
	ASSERT_EQ(1, v2[0]);
}

// test pair -------------------------------------------------------------------

TEST(utility_test, pair)
{
	toy::pair<int, float> p1{1, 1.f};
	toy::pair<int, float> p2{2, 2.f};
	auto p3{p1};
	auto p4 = toy::make_pair(2, 2.f);

	ASSERT_EQ(true, p1 < p2);
	ASSERT_EQ(true, p1 == p3);
	ASSERT_EQ(true, p2 == p4);

	toy::swap(p1, p2);

	ASSERT_EQ(true, p1.first == 2 && p1.second == 2.f);
	ASSERT_EQ(true, p2 == p3);
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