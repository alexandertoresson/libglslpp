
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <gtest/gtest.h>

#include "glsl++.h"

#define glsl_update_counter \
"#ifdef __LIBGLSLPP_COUNTER_DECLARED__\n"\
"__libglslpp_counter__++;\n"\
"#else\n"\
"int __libglslpp_counter__ = 0;\n"\
"#define __LIBGLSLPP_COUNTER_DECLARED__\n"\
"#endif\n";

#define glsl_prelude "\n"\
"#define _EXPECT_EQ(expected, actual)\n"\
glsl_update_counter\
"if (gl_FragCoord.x == __libglslpp_counter__)"\
"gl_FragColor = expected == actual ? vec4(1.0f) : vec4(0.0f)"\
"#define _EXPECT_FLOAT_EQ(expected, actual)\n"\
glsl_update_counter\
"if (gl_FragCoord.x == __libglslpp_counter__)"\
"gl_FragColor = expected == actual ? vec4(1.0f) : vec4(0.0f)" // TODO

#define GLSLTEST(Test, TestCase, ...) \
TEST(Test, TestCase) {\
	test_type_list __list__;\
	char program[] = #__VA_ARGS__; /* yo dawg, I herd u liek tests, so I put a test in ur test, so u can test while u test */ \
__VA_ARGS__ \
}

typedef std::vector<int> test_type_list;

void __add_test_type_info__(const float& e, test_type_list& list) {
	list.push_back(1);
}

template <typename T, unsigned n, template <typename T, unsigned n> class C>
void __add_test_type_info__(const C<T, n>& e, test_type_list& list) {
	list.push_back(n);
}

template <typename T, unsigned n, unsigned m>
void __add_test_type_info__(const glsl::mat<T, n, m>& e, test_type_list& list) {
	list.push_back(n*m);
}

#define _EXPECT_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_EQ(tmp_e, tmp_a); \
	__add_test_type_info__(tmp_e, __list__); \
} while (0)

#define _EXPECT_FLOAT_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_FLOAT_EQ(tmp_e, tmp_a); \
	__add_test_type_info__(tmp_e, __list__); \
} while (0)

namespace glsl {

	GLSLTEST(VecTest, HandlesConstructors,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		vec4 b(a);

		b.x = 0.0f;

		_EXPECT_EQ(1.0f, a[0]); _EXPECT_EQ(2.0f, a[1]); _EXPECT_EQ(3.0f, a[2]); _EXPECT_EQ(4.0f, a[3]);
		_EXPECT_EQ(vec2(1.0f, 1.0f), vec2(1.0f));
		_EXPECT_EQ(a, vec4(a));
		_EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), vec3(vec2(1.0f, 2.0f), 3.0f));
		_EXPECT_EQ(a, vec4(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)));
		_EXPECT_EQ(a, vec4(1.0f, vec3(2.0f, 3.0f, 4.0f)));
	)
	
	GLSLTEST(VecTest, HandlesComparison,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)), c(5.0f, 6.0f, 7.0f, 8.0f);
		_EXPECT_EQ(true, a == b);
		_EXPECT_EQ(true, !(a != b));
		_EXPECT_EQ(true, !(a == c));
		_EXPECT_EQ(true, a != c);
	)

	GLSLTEST(VecTest, HandlesAssignment,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b = a;
		vec3 c;

		_EXPECT_EQ(a, b);

		c = a.yzw;
		_EXPECT_EQ(vec3(2.0f, 3.0f, 4.0f), c);
		c.xy = vec2(1.0f, 2.0f);
		_EXPECT_EQ(vec3(1.0f, 2.0f, 4.0f), c);
		c.xy = a.yz;
		_EXPECT_EQ(vec3(2.0f, 3.0f, 4.0f), c);
		c.zyx.xy = a.zyx.xy;
		_EXPECT_EQ(vec3(2.0f, 2.0f, 3.0f), c);

		b.x = 0.0f;

		_EXPECT_EQ(vec4(1.0f, 2.0f, 3.0f, 4.0f), a);
	)

	GLSLTEST(VecTest, HandlesSwizzles,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		vec3 b, c;
		mat3 d(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		vec3 e(d[2]);

		_EXPECT_EQ(7.0f, e.x); _EXPECT_EQ(8.0f, e.y); _EXPECT_EQ(9.0f, e.z);
		_EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), a.xyz);
		_EXPECT_EQ(a.xyz, vec3(a.xyz));
		_EXPECT_EQ(vec4(2.0f, 2.0f, 4.0f, 4.0f), a.yyww);
		_EXPECT_EQ(vec2(1.0f, 3.0f), a.xyz.xz);
	)

	GLSLTEST(VecTest, HandlesAddition,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(2.0f, 3.0f, 4.0f, 5.0f);

		_EXPECT_EQ(vec4(3.0f, 5.0f, 7.0f, 9.0f), a+b);
		_EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), a+1.0f);
		_EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), 1.0f+a);
		a += 1.0f;
		_EXPECT_EQ(vec4(2.0f, 3.0f, 4.0f, 5.0f), a);
		a += b;
		_EXPECT_EQ(vec4(4.0f, 6.0f, 8.0f, 10.0f), a);
		a.xy += 1.0f;
		_EXPECT_EQ(vec4(5.0f, 7.0f, 8.0f, 10.0f), a);
		a.xy += vec2(1.0f, 2.0f);
		_EXPECT_EQ(vec4(6.0f, 9.0f, 8.0f, 10.0f), a);
		a += b.xyzw;
		_EXPECT_EQ(vec4(8.0f, 12.0f, 12.0f, 15.0f), a);
		a.xy += b.xy;
		_EXPECT_EQ(vec4(10.0f, 15.0f, 12.0f, 15.0f), a);
	)

	GLSLTEST(VecTest, HandlesSubtraction,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f), b(2.0f, 4.0f, 6.0f, 8.0f);

		_EXPECT_EQ(vec4(-1.0f, -2.0f, -3.0f, -4.0f), a-b);
		_EXPECT_EQ(vec4(0.0f, 1.0f, 2.0f, 3.0f), a-1.0f);
		_EXPECT_EQ(vec4(0.0f, -1.0f, -2.0f, -3.0f), 1.0f-a);
		a -= 1.0f;
		_EXPECT_EQ(vec4(0.0f, 1.0f, 2.0f, 3.0f), a);
		a -= b;
		_EXPECT_EQ(vec4(-2.0f, -3.0f, -4.0f, -5.0f), a);
		a.xy -= 1.0f;
		_EXPECT_EQ(vec4(-3.0f, -4.0f, -4.0f, -5.0f), a);
		a.xy -= vec2(1.0f, 2.0f);
		_EXPECT_EQ(vec4(-4.0f, -6.0f, -4.0f, -5.0f), a);
		a -= b.xyzw;
		_EXPECT_EQ(vec4(-6.0f, -10.0f, -10.0f, -13.0f), a);
		a.xy -= b.xy;
		_EXPECT_EQ(vec4(-8.0f, -14.0f, -10.0f, -13.0f), a);
	)

	GLSLTEST(VecTest, HandlesTrigonometryFunctions,
		float pi=3.1415926535f;
		vec4 a(0, pi/6, pi/4, pi/3);
		vec4 res_cos(1.0, sqrt(3)/2, sqrt(2)/2, 1.0/2);
		vec4 res_sin(0.0, 1.0/2, sqrt(2)/2, sqrt(3)/2);

		// _EXPECT_FLOAT_EQ can't take vecs as arguments, 'pre-process' with length()
		_EXPECT_FLOAT_EQ(length(res_cos), length(cos(a)));
		_EXPECT_FLOAT_EQ(length(res_sin), length(sin(a)));
	)

	GLSLTEST(VecTest, HandlesExponentialFunctions,
		vec4 a(1.0f, 1.1f, 1.0f, 1.1f), b(1.0f, 1.0f, 1.1f, 1.1f);
		vec4 res_pow(1.0f, 1.1f, 1.0f, 1.110534241054576f);
		_EXPECT_FLOAT_EQ(length(res_pow), length(pow(a, b)));
	)

	GLSLTEST(VecTest, HandlesCommonFunctions,
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);

		_EXPECT_EQ(2.0f, max(2.0f, 1.0f));
		_EXPECT_EQ(b, max(a, b));
		_EXPECT_EQ(1.0f, min(2.0f, 1.0f));
		_EXPECT_EQ(a, min(a, b));
		_EXPECT_EQ(1.0f, abs(1.0f));
		_EXPECT_EQ(1.0f, abs(-1.0f));
		_EXPECT_EQ(a, abs(a));
		_EXPECT_EQ(a, abs(0.0f-a)); // TODO: Change to -a when negation is implemented
		_EXPECT_EQ(0.0f, clamp(-1.0f, 0.0f, 1.0f));
		_EXPECT_EQ(0.5f, clamp(0.5f, 0.0f, 1.0f));
		_EXPECT_EQ(1.0f, clamp(1.5f, 0.0f, 1.0f));
		_EXPECT_EQ(vec3(0.1f, 0.5f, 0.9f), clamp(vec3(-1.0f, 0.5f, 1.5f), vec3(0.1f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.9f)));
		_EXPECT_EQ(vec3(0.0f, 0.5f, 1.0f), clamp(vec3(-1.0f, 0.5f, 1.5f), 0.0f, 1.0f));
	)

	GLSLTEST(VecTest, HandlesGeometricFunctions,
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);

		_EXPECT_EQ(vec3(-3.0f, 6.0f, -3.0f), cross(a, b));
		_EXPECT_EQ(32.0f, dot(a, b));
		_EXPECT_EQ(vec3(-255.0f, -318.0f, -381.0f), reflect(a, b));
		_EXPECT_FLOAT_EQ(sqrt(14.0f), length(a));
		_EXPECT_FLOAT_EQ(sqrt(27.0f), distance(a, b));
		_EXPECT_FLOAT_EQ(length(a/sqrt(14.0f)), length(normalize(a)));
	)

	GLSLTEST(VecTest, HandlesMatrixFunctions,
		vec3 a(1.0f, 2.0f, 3.0f);
		vec4 b(4.0f, 5.0f, 6.0f, 7.0f);
		mat4x3 res(4.0f, 8.0f, 12.0f, 5.0f, 10.0f, 15.0f, 6.0f, 12.0f, 18.0f, 7.0f, 14.0f, 21.0f);

		_EXPECT_EQ(res, outerProduct(a, b));
	)

	GLSLTEST(VecTest, HandlesVectorRelationalFunctions,
		vec3 a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);
		_EXPECT_EQ(bvec3(true, true, true), greaterThan(b, a));
	)

	GLSLTEST(MatTest, HandlesConstructors,
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(1.0f), c(a, a, a, a), d(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

		_EXPECT_EQ(1.0f, d[0][0]); _EXPECT_EQ(2.0f, d[0][1]); _EXPECT_EQ(3.0f, d[0][2]); _EXPECT_EQ(4.0f, d[0][3]);
		_EXPECT_EQ(5.0f, d[1][0]); _EXPECT_EQ(6.0f, d[1][1]); _EXPECT_EQ(7.0f, d[1][2]); _EXPECT_EQ(8.0f, d[1][3]);
		_EXPECT_EQ(9.0f, d[2][0]); _EXPECT_EQ(10.0f, d[2][1]); _EXPECT_EQ(11.0f, d[2][2]); _EXPECT_EQ(12.0f, d[2][3]);
		_EXPECT_EQ(13.0f, d[3][0]); _EXPECT_EQ(14.0f, d[3][1]); _EXPECT_EQ(15.0f, d[3][2]); _EXPECT_EQ(16.0f, d[3][3]);
		_EXPECT_EQ(mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f), b);
		_EXPECT_EQ(c[0], a); _EXPECT_EQ(c[1], a); _EXPECT_EQ(c[2], a); _EXPECT_EQ(c[3], a);
		_EXPECT_EQ(d, mat4(d));

		// TODO: Constructors from arbitrary combinations of vectors and scalars,
		//       constructors from matrices of other sizes.
	)

	GLSLTEST(MatTest, HandlesComparison,
		mat2x3 a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f),
		       b(vec3(1.0f, 2.0f, 3.0f), vec3(4.0f, 5.0f, 6.0f)),
		       c(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f);
		_EXPECT_EQ(true, a == b);
		_EXPECT_EQ(true, !(a != b));
		_EXPECT_EQ(true, !(a == c));
		_EXPECT_EQ(true, a != c);
	)

	GLSLTEST(MatTest, HandlesAddition, {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(a, a, a, a), c(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res(2*a,2*a+1.0f,2*a+2.0f,2*a+3.0f);

		_EXPECT_EQ(res, b+c);

		_EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), b+1.0f);
		_EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), 1.0f+b);

		b += c;
		_EXPECT_EQ(res, b);

		b += 1.0f;
		_EXPECT_EQ(res+1.0f, b);
	})

	GLSLTEST(MatTest, HandlesSubtraction, {
		vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b(a, a, a, a), c(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res(vec4(0.0f), vec4(-1.0f), vec4(-2.0f), vec4(-3.0f));

		_EXPECT_EQ(res, b-c);

		_EXPECT_EQ(mat4(a-1.0f, a-1.0f, a-1.0f, a-1.0f), b-1.0f);
		_EXPECT_EQ(mat4(1.0f-a, 1.0f-a, 1.0f-a, 1.0f-a), 1.0f-b);

		b -= c;
		_EXPECT_EQ(res, b);

		b -= 1.0f;
		_EXPECT_EQ(res-1.0f, b);
	})

	GLSLTEST(MatTest, HandlesMultiplication, {
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

		_EXPECT_EQ(res_mm, c * d);
		_EXPECT_EQ(res_vm, a * d);
		_EXPECT_EQ(res_mv, d * b);

		a *= e;
		_EXPECT_EQ(res_vm2, a);
		e *= e;
		_EXPECT_EQ(res_mm2, e);
	})

	GLSLTEST(MatTest, HandlesMatrixFunctions, {
		mat4x3 a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
		mat3x4 res(1.0f, 4.0f, 7.0f, 10.0f, 2.0f, 5.0f, 8.0f, 11.0f, 3.0f, 6.0f, 9.0f, 12.0f);

		_EXPECT_EQ(res, transpose(a));
	})

	// C++-specific test
	TEST(VecTest, HandlesOutputStreaming) {
		vec<int, 4> a(1, 2, 3, 4);
		std::stringstream sstream;
		std::string s;
		sstream << a;
		std::getline(sstream, s);
		EXPECT_EQ("(1, 2, 3, 4)", s);
	}

}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
