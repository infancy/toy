#include "gtest/gtest.h"

#include"math/matrix.h"

TEST(Math_Matrix_Test, Mul)
{
	toy::Matrix mat;
}

GTEST_API_ int main(int argc, char **argv)
{
	printf("Running main() from gtest_main.cc\n");
	testing::InitGoogleTest(&argc, argv);
	auto result = RUN_ALL_TESTS();
	getchar();
	return result;
}