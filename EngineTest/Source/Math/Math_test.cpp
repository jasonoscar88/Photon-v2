#include <Math/Math.h>

#include <gtest/gtest.h>

#include <limits>

TEST(MathTest, AngleUnitConversion)
{
	EXPECT_FLOAT_EQ(ph::Math::toDegrees(3.14159265359f), 180.0f);
	EXPECT_FLOAT_EQ(ph::Math::toRadians(180.0f), 3.14159265359f);
}

TEST(MathTest, NumberSignExtraction)
{
	EXPECT_TRUE(ph::Math::sign(-3) == -1);
	EXPECT_TRUE(ph::Math::sign( 0) ==  0);
	EXPECT_TRUE(ph::Math::sign( 2) ==  1);

	EXPECT_TRUE(ph::Math::sign(-4.0f) == -1);
	EXPECT_TRUE(ph::Math::sign( 0.0f) ==  0);
	EXPECT_TRUE(ph::Math::sign( 7.0f) ==  1);
}

TEST(MathTest, NumberClamping)
{
	EXPECT_TRUE(ph::Math::clamp(-1, 0, 4) == 0);
	EXPECT_TRUE(ph::Math::clamp( 2, 0, 4) == 2);
	EXPECT_TRUE(ph::Math::clamp( 7, 0, 4) == 4);

	EXPECT_TRUE(ph::Math::clamp(-3.0f, 1.0f, 5.0f) == 1.0f);
	EXPECT_TRUE(ph::Math::clamp( 2.5f, 1.0f, 5.0f) == 2.5f);
	EXPECT_TRUE(ph::Math::clamp( 5.1f, 1.0f, 5.0f) == 5.0f);

	if(std::numeric_limits<float>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::Math::clamp(std::numeric_limits<float>::quiet_NaN(), 1.0f, 5.0f) == 1.0f);
	}

	if(std::numeric_limits<double>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::Math::clamp(std::numeric_limits<double>::quiet_NaN(), 1.0, 5.0) == 1.0);
	}
}