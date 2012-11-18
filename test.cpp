
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
	
	TEST(VecTest, HandlesComparison) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)), c(5.0f, 6.0f, 7.0f, 8.0f);
		EXPECT_EQ(true, a == b);
		EXPECT_EQ(true, !(a != b));
		EXPECT_EQ(true, !(a == c));
		EXPECT_EQ(true, a != c);
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

	TEST(VecTest, HandlesTrigonometryFunctions) {
		float pi=3.1415926535f;
		vec4 a(0, pi/6, pi/4, pi/3);
		vec4 res_cos(1.0, sqrt(3)/2, sqrt(2)/2, 1.0/2);
		vec4 res_sin(0.0, 1.0/2, sqrt(2)/2, sqrt(3)/2);

		// EXPECT_FLOAT_EQ can't take vecs as arguments, 'pre-process' with length()
		EXPECT_FLOAT_EQ(length(res_cos), length(cos(a)));
		EXPECT_FLOAT_EQ(length(res_sin), length(sin(a)));
	}

	TEST(VecTest, HandlesExponentialFunctions) {
		vec4 a(1.0f, 1.1f, 1.0f, 1.1f), b(1.0f, 1.0f, 1.1f, 1.1f);
		vec4 res_pow(1.0f, 1.1f, 1.0f, 1.110534241054576f);
		EXPECT_FLOAT_EQ(length(res_pow), length(pow(a, b)));
	}

	TEST(VecTest, HandlesCommonFunctions) {
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);

		EXPECT_EQ(2.0f, max(2.0f, 1.0f));
		EXPECT_EQ(b, max(a, b));
		EXPECT_EQ(1.0f, min(2.0f, 1.0f));
		EXPECT_EQ(a, min(a, b));
		EXPECT_EQ(1.0f, abs(1.0f));
		EXPECT_EQ(1.0f, abs(-1.0f));
		EXPECT_EQ(a, abs(a));
		EXPECT_EQ(a, abs(0.0f-a)); // TODO: Change to -a when negation is implemented
		EXPECT_EQ(0.0f, clamp(-1.0f, 0.0f, 1.0f));
		EXPECT_EQ(0.5f, clamp(0.5f, 0.0f, 1.0f));
		EXPECT_EQ(1.0f, clamp(1.5f, 0.0f, 1.0f));
		EXPECT_EQ(vec3(0.1f, 0.5f, 0.9f), clamp(vec3(-1.0f, 0.5f, 1.5f), vec3(0.1f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.9f)));
		EXPECT_EQ(vec3(0.0f, 0.5f, 1.0f), clamp(vec3(-1.0f, 0.5f, 1.5f), 0.0f, 1.0f));
	}

	TEST(VecTest, HandlesGeometricFunctions) {
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);

		EXPECT_EQ(vec3(-3.0f, 6.0f, -3.0f), cross(a, b));
		EXPECT_EQ(32.0f, dot(a, b));
		EXPECT_EQ(vec3(-255.0f, -318.0f, -381.0f), reflect(a, b));
		EXPECT_FLOAT_EQ(sqrt(14.0f), length(a));
		EXPECT_FLOAT_EQ(sqrt(27.0f), distance(a, b));
		EXPECT_FLOAT_EQ(length(a/sqrt(14.0f)), length(normalize(a)));
	}

	TEST(VecTest, HandlesMatrixFunctions) {
		vec3 a(1.0f, 2.0f, 3.0f);
		vec4 b(4.0f, 5.0f, 6.0f, 7.0f);
		mat4x3 res(4.0f, 8.0f, 12.0f, 5.0f, 10.0f, 15.0f, 6.0f, 12.0f, 18.0f, 7.0f, 14.0f, 21.0f);

		EXPECT_EQ(res, outerProduct(a, b));
	}

	TEST(VecTest, HandlesVectorRelationalFunctions) {
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);
		EXPECT_EQ(bvec3(true, true, true), greaterThan(b, a));
	}

	TEST(MatTest, HandlesConstructors) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(1.0f), c(a, a, a, a), d(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

		EXPECT_EQ(1.0f, d[0][0]); EXPECT_EQ(2.0f, d[0][1]); EXPECT_EQ(3.0f, d[0][2]); EXPECT_EQ(4.0f, d[0][3]);
		EXPECT_EQ(5.0f, d[1][0]); EXPECT_EQ(6.0f, d[1][1]); EXPECT_EQ(7.0f, d[1][2]); EXPECT_EQ(8.0f, d[1][3]);
		EXPECT_EQ(9.0f, d[2][0]); EXPECT_EQ(10.0f, d[2][1]); EXPECT_EQ(11.0f, d[2][2]); EXPECT_EQ(12.0f, d[2][3]);
		EXPECT_EQ(13.0f, d[3][0]); EXPECT_EQ(14.0f, d[3][1]); EXPECT_EQ(15.0f, d[3][2]); EXPECT_EQ(16.0f, d[3][3]);
		EXPECT_EQ(mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f), b);
		EXPECT_EQ(c[0], a); EXPECT_EQ(c[1], a); EXPECT_EQ(c[2], a); EXPECT_EQ(c[3], a);
		EXPECT_EQ(d, mat4(d));

		// TODO: Constructors from arbitrary combinations of vectors and scalars,
		//       constructors from matrices of other sizes.
	}

	TEST(MatTest, HandlesComparison) {
		mat2x3 a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f),
		       b(vec3(1.0f, 2.0f, 3.0f), vec3(4.0f, 5.0f, 6.0f)),
		       c(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f);
		EXPECT_EQ(true, a == b);
		EXPECT_EQ(true, !(a != b));
		EXPECT_EQ(true, !(a == c));
		EXPECT_EQ(true, a != c);
	}

	TEST(MatTest, HandlesAddition) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(a, a, a, a), c(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res(2*a,2*a+1.0f,2*a+2.0f,2*a+3.0f);

		EXPECT_EQ(res, b+c);

		EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), b+1.0f);
		EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), 1.0f+b);

		b += c;
		EXPECT_EQ(res, b);

		b += 1.0f;
		EXPECT_EQ(res+1.0f, b);
	}

	TEST(MatTest, HandlesSubtraction) {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(a, a, a, a), c(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res(vec4(0.0f), vec4(-1.0f), vec4(-2.0f), vec4(-3.0f));

		EXPECT_EQ(res, b-c);

		EXPECT_EQ(mat4(a-1.0f, a-1.0f, a-1.0f, a-1.0f), b-1.0f);
		EXPECT_EQ(mat4(1.0f-a, 1.0f-a, 1.0f-a, 1.0f-a), 1.0f-b);

		b -= c;
		EXPECT_EQ(res, b);

		b -= 1.0f;
		EXPECT_EQ(res-1.0f, b);
	}

	TEST(MatTest, HandlesMultiplication) {
		vec3 a(1.0f, 2.0f, 3.0f);
		vec4 b(1.0f, 2.0f, 3.0f, 4.0f);
		mat3x4 c(1.0f, 4.0f, 7.0f, 10.0f, 2.0f, 5.0f, 8.0f, 11.0f, 3.0f, 6.0f, 9.0f, 12.0f);
		mat4x3 d(1.0f, 5.0f, 9.0f, 2.0f, 6.0f, 10.0f, 3.0f, 7.0f, 11.0f, 4.0f, 8.0f, 12.0f);
		mat4x4 res_mm(38.0f, 83.0f, 128.0f, 173.0f, 44.0f, 98.0f, 152.0f, 206.0f, 50.0f, 113.0f, 176.0f, 239.0f, 56.0f, 128.0f, 200.0f, 272.0f);
		mat3 res_mm2(30.0f, 66.0f, 102.0f, 36.0f, 81.0f, 126.0f, 42.0f, 96.0f, 150.0f);
		mat3 e(1.0f, 4.0f, 7.0f, 2.0f, 5.0f, 8.0f, 3.0f, 6.0f, 9.0f);
		vec4 res_vm(38.0f, 44.0f, 50.0f, 56.0f);
		vec3 res_mv(30.0f, 70.0f, 110.0f);
		vec3 res_vm2(30.0f, 36.0f, 42.0f);

		EXPECT_EQ(res_mm, c * d);
		EXPECT_EQ(res_vm, a * d);
		EXPECT_EQ(res_mv, d * b);

		a *= e;
		EXPECT_EQ(res_vm2, a);
		e *= e;
		EXPECT_EQ(res_mm2, e);
	}

	TEST(MatTest, HandlesMatrixFunctions) {
		mat4x3 a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
		mat3x4 res(1.0f, 4.0f, 7.0f, 10.0f, 2.0f, 5.0f, 8.0f, 11.0f, 3.0f, 6.0f, 9.0f, 12.0f);

		EXPECT_EQ(res, transpose(a));
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
