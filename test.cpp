
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <gtest/gtest.h>

#include "glsl++.h"

namespace glsl {

	TEST(VecTest, HandlesConstructors) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);

		EXPECT_EQ(1.0f, a[0]); EXPECT_EQ(2.0f, a[1]); EXPECT_EQ(3.0f, a[2]); EXPECT_EQ(4.0f, a[3]);
		EXPECT_EQ(vec2(1.0f, 1.0f), vec2(1.0f));
		EXPECT_EQ(a, vec4(a));
		EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), vec3(vec2(1.0f, 2.0f), 3.0f));
		EXPECT_EQ(a, vec4(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)));
		EXPECT_EQ(a, vec4(1.0f, vec3(2.0f, 3.0f, 4.0f)));
	}
	
	TEST(VecTest, HandlesAssignment) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b = a;
		vec3 c;

		EXPECT_EQ(a, b);

		c = a.yzw;
		EXPECT_EQ(vec3(2.0f, 3.0f, 4.0f), c);
		c.xy = vec2(1.0f, 2.0f);
		EXPECT_EQ(vec3(1.0f, 2.0f, 4.0f), c);
		c.xy = a.yz;
		EXPECT_EQ(vec3(2.0f, 3.0f, 4.0f), c);
		c.zyx.xy = a.zyx.xy;
		EXPECT_EQ(vec3(2.0f, 2.0f, 3.0f), c);
	}

	TEST(VecTest, HandlesSwizzles) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		vec3 b, c;

		EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), a.xyz);
		EXPECT_EQ(a.xyz, vec3(a.xyz));
		EXPECT_EQ(vec4(2.0f, 2.0f, 4.0f, 4.0f), a.yyww);
		EXPECT_EQ(vec2(1.0f, 3.0f), a.xyz.xz);
	}

	TEST(VecTest, HandlesOutputStreaming) {
		vec<int, 4> a(1, 2, 3, 4);
		std::stringstream sstream;
		std::string s;
		sstream << a;
		std::getline(sstream, s);
		EXPECT_EQ("(1, 2, 3, 4)", s);		
	}

	TEST(VecTest, HandlesAddition) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(2.0f, 3.0f, 4.0f, 5.0f);

		EXPECT_EQ(vec4(3.0f, 5.0f, 7.0f, 9.0f), a+b);
		EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), a+1.0f);
		EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), 1.0f+a);
		a += 1.0f;
		EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), a);
		a += b;
		EXPECT_EQ(vec4(4.0f, 6.0f, 8.0f, 10.0f), a);
		a.xy += 1.0f;
		EXPECT_EQ(vec4(5.0f, 7.0f, 8.0f, 10.0f), a);
		a.xy += vec2(1.0f, 2.0f);
		EXPECT_EQ(vec4(6.0f, 9.0f, 8.0f, 10.0f), a);
		a += b.xyzw;
		EXPECT_EQ(vec4(8.0f, 12.0f, 12.0f, 15.0f), a);
		a.xy += b.xy;
		EXPECT_EQ(vec4(10.0f, 15.0f, 12.0f, 15.0f), a);
	}

	TEST(VecTest, HandlesSubtraction) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(2.0f, 4.0f, 6.0f, 8.0f);

		EXPECT_EQ(vec4(-1.0f, -2.0f, -3.0f, -4.0f), a-b);
		EXPECT_EQ(vec4(0.0f, 1.0f, 2.0f, 3.0f), a-1.0f);
		EXPECT_EQ(vec4(0.0f, -1.0f, -2.0f, -3.0f), 1.0f-a);
		a -= 1.0f;
		EXPECT_EQ(vec4(0.0f, 1.0f, 2.0f, 3.0f), a);
		a -= b;
		EXPECT_EQ(vec4(-2.0f, -3.0f, -4.0f, -5.0f), a);
		a.xy -= 1.0f;
		EXPECT_EQ(vec4(-3.0f, -4.0f, -4.0f, -5.0f), a);
		a.xy -= vec2(1.0f, 2.0f);
		EXPECT_EQ(vec4(-4.0f, -6.0f, -4.0f, -5.0f), a);
		a -= b.xyzw;
		EXPECT_EQ(vec4(-6.0f, -10.0f, -10.0f, -13.0f), a);
		a.xy -= b.xy;
		EXPECT_EQ(vec4(-8.0f, -14.0f, -10.0f, -13.0f), a);
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
