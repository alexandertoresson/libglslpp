
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <gtest/gtest.h>

#include "glsl++.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <GL/glext.h>

#define glsl_prelude "\n"\
"#version 150\n"\
"bool float_eq(float tmp_e, float tmp_a)\n"\
"{\n"\
"	return abs(tmp_e - tmp_a) <= (max(abs(tmp_e), abs(tmp_a)) * 1.192092896e-7f * 40.0f);\n"\
"}\n"\
"#define _EXPECT_EQ(expected, actual) "\
"do {"\
	"if (gl_FragCoord.x == _libglslpp_counter_)"\
		"gl_FragColor = (expected) == (actual) ? vec4(1.0f) : vec4(0.5f);"\
	"_libglslpp_counter_++;"\
"} while (false)\n"\
"#define _EXPECT_FLOAT_EQ(expected, actual) "\
"do {"\
	"float tmp_e = (expected), tmp_a = (actual);"\
	"if (gl_FragCoord.x == _libglslpp_counter_)"\
		"gl_FragColor = float_eq(tmp_e, tmp_a) ? vec4(1.0f) : vec4(0.5f);"\
	"_libglslpp_counter_++;"\
"} while (false)\n"\
"#define _EXPECT_VEC3_EQ(expected, actual) "\
"do {"\
	"vec3 tmp_e = (expected), tmp_a = (actual);"\
	"if (gl_FragCoord.x == _libglslpp_counter_)"\
		"gl_FragColor = (float_eq(tmp_e.x, tmp_a.x) && float_eq(tmp_e.y, tmp_a.y) && float_eq(tmp_e.z, tmp_a.z)) ? vec4(1.0f) : vec4(0.5f);"\
	"_libglslpp_counter_++;"\
"} while (false)\n"\
"#define _EXPECT_VEC4_EQ(expected, actual) "\
"do {"\
	"vec4 tmp_e = (expected), tmp_a = (actual);"\
	"if (gl_FragCoord.x == _libglslpp_counter_)"\
		"gl_FragColor = (float_eq(tmp_e.x, tmp_a.x) && float_eq(tmp_e.y, tmp_a.y) && float_eq(tmp_e.z, tmp_a.z) && float_eq(tmp_e.w, tmp_a.w)) ? vec4(1.0f) : vec4(0.5f);"\
	"_libglslpp_counter_++;"\
"} while (false)\n"\
"layout(pixel_center_integer) in vec4 gl_FragCoord;"\
"out vec4 gl_FragColor;"

#define GLSLTEST(Test, TestCase, ...) \
TEST(Test, TestCase) {\
	_test_type_list_ _list_;\
	char _program_[] = glsl_prelude\
	"void main(void) {"\
		"uint _libglslpp_counter_ = 0u;"\
		#__VA_ARGS__\
	"}"; /* yo dawg, I herd u liek tests, so I put a test in ur test, so u can test while u test */ \
	__VA_ARGS__ \
	EXPECT_EQ(true, _test_shader_(_program_, _list_));\
}

typedef std::vector<unsigned> _test_type_list_;

void _add_test_type_info_(const float& e, _test_type_list_& list) {
	list.push_back(1);
}

template <typename T, unsigned n, template <typename T, unsigned n> class C>
void _add_test_type_info_(const C<T, n>& e, _test_type_list_& list) {
	list.push_back(n);
}

template <typename T, unsigned n, unsigned m>
void _add_test_type_info_(const glsl::mat<T, n, m>& e, _test_type_list_& list) {
	list.push_back(n*m);
}

#define _EXPECT_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_EQ(tmp_e, tmp_a); \
	_add_test_type_info_(tmp_e, _list_); \
} while (0)

#define _EXPECT_FLOAT_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_FLOAT_EQ(tmp_e, tmp_a); \
	_add_test_type_info_(tmp_e, _list_); \
} while (0)

#define _EXPECT_VEC3_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_FLOAT_EQ(tmp_e.x, tmp_a.x); \
	EXPECT_FLOAT_EQ(tmp_e.y, tmp_a.y); \
	EXPECT_FLOAT_EQ(tmp_e.z, tmp_a.z); \
	_add_test_type_info_(tmp_e, _list_); \
} while (0)

#define _EXPECT_VEC4_EQ(expected, actual) do { \
	auto tmp_e = expected; \
	auto tmp_a = actual; \
	EXPECT_FLOAT_EQ(tmp_e.x, tmp_a.x); \
	EXPECT_FLOAT_EQ(tmp_e.y, tmp_a.y); \
	EXPECT_FLOAT_EQ(tmp_e.z, tmp_a.z); \
	EXPECT_FLOAT_EQ(tmp_e.w, tmp_a.w); \
	_add_test_type_info_(tmp_e, _list_); \
} while (0)

#define checkGLErrors() _checkGLErrors(__LINE__)

void _checkGLErrors(int line) {
	GLenum err;
	while ((err = glGetError())) {
		printf("GL error: %d@%d", err, line);
	}
}

int GetShaderCompileError(GLuint handle, GLenum boolean) {
	GLint succeeded = 0;
	char errorbuffer[10000];
	GLsizei errorbufferlen = 0;

	glGetObjectParameterivARB(handle, boolean, &succeeded);

	glGetInfoLogARB(handle, 9999, &errorbufferlen, errorbuffer);
	errorbuffer[errorbufferlen] = 0;
	printf(errorbuffer);

	if (!succeeded) {
		return 1;
	}

	return 0;
}

GLuint ParseShader(char* fshader) {
	GLuint program;
	const GLchar** d = (const GLchar**) &fshader;
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragShader, 1, d, NULL);
	glCompileShader(fragShader);

	if (GetShaderCompileError(fragShader, GL_OBJECT_COMPILE_STATUS_ARB)) {
		puts(fshader);
		glDeleteShader(fragShader);
		fragShader = 0;
		return 0;
	}

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	if (GetShaderCompileError(program, GL_OBJECT_LINK_STATUS_ARB)) {
		puts(fshader);
		glDeleteProgram(program);
		program = 0;
		glDeleteShader(fragShader);
		return 0;
	}
	return program;
}

GLuint textureId;
GLuint fboId;

bool _test_shader_(char *shader, const _test_type_list_& list) {
	GLuint program = ParseShader(shader);
	if (program) {

		glUseProgram(program);

		glBegin(GL_QUADS);

		glVertex2s(-1, -1);

		glVertex2s(1, -1);

		glVertex2s(1, 1);

		glVertex2s(-1, 1);

		glEnd();

		checkGLErrors();

		glFlush();

		for (unsigned x = 0; x < list.size(); ++x) {
			float pixel[4];
			glReadPixels(x, 0, 1, 1, GL_RGBA, GL_FLOAT, pixel);
			if (pixel[0] != 1.0f) {
				std::cout << "OpenGL GLSL subtest #" << x << " failed" << std::endl;
				return false;
			}
		}
		return true;
	}
	return false;
}

namespace glsl {

	GLSLTEST(VecTest, HandlesConstructors,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		vec4 b = a;

		b.x = 0.0f;

		_EXPECT_EQ(1.0f, a[0]); _EXPECT_EQ(2.0f, a[1]); _EXPECT_EQ(3.0f, a[2]); _EXPECT_EQ(4.0f, a[3]);
		_EXPECT_EQ(vec2(1.0f, 1.0f), vec2(1.0f));
		_EXPECT_EQ(a, vec4(a));
		_EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), vec3(vec2(1.0f, 2.0f), 3.0f));
		_EXPECT_EQ(a, vec4(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)));
		_EXPECT_EQ(a, vec4(1.0f, vec3(2.0f, 3.0f, 4.0f)));
	)
	
	GLSLTEST(VecTest, HandlesComparison,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f), b = vec4(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f)), c = vec4(5.0f, 6.0f, 7.0f, 8.0f);
		_EXPECT_EQ(true, a == b);
		_EXPECT_EQ(true, !(a != b));
		_EXPECT_EQ(true, !(a == c));
		_EXPECT_EQ(true, a != c);
	)

	GLSLTEST(VecTest, HandlesAssignment,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f), b = a;
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
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		vec3 b, c;
		mat3 d = mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		vec3 e = vec3(d[2]);

		_EXPECT_EQ(7.0f, e.x); _EXPECT_EQ(8.0f, e.y); _EXPECT_EQ(9.0f, e.z);
		_EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), a.xyz);
		_EXPECT_EQ(a.xyz, vec3(a.xyz));
		_EXPECT_EQ(vec4(2.0f, 2.0f, 4.0f, 4.0f), a.yyww);
		_EXPECT_EQ(vec2(1.0f, 3.0f), a.xyz.xz);
	)

	GLSLTEST(VecTest, HandlesAddition,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f), b = vec4(2.0f, 3.0f, 4.0f, 5.0f);

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
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f), b = vec4(2.0f, 4.0f, 6.0f, 8.0f);

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
	
	GLSLTEST(VecTest, HandlesNegation,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);

		_EXPECT_EQ(vec4(-1.0f, -2.0f, -3.0f, -4.0f), -a);
	)

	GLSLTEST(VecTest, HandlesTrigonometryFunctions,
		float pi=3.1415926535f;
		vec4 a = vec4(0, pi/6, pi/4, pi/3);
		vec4 res_cos = vec4(1.0, sqrt(3.0)/2, sqrt(2.0)/2, 1.0/2);
		vec4 res_sin = vec4(0.0, 1.0/2, sqrt(2.0)/2, sqrt(3.0)/2);

		// _EXPECT_FLOAT_EQ can't take vecs as arguments, 'pre-process' with length()
		_EXPECT_FLOAT_EQ(length(res_cos), length(cos(a)));
		_EXPECT_FLOAT_EQ(length(res_sin), length(sin(a)));
	)

	GLSLTEST(VecTest, HandlesExponentialFunctions,
		vec4 a = vec4(1.0f, 1.1f, 1.0f, 1.1f), b = vec4(1.0f, 1.0f, 1.1f, 1.1f);
		vec4 res_pow = vec4(1.0f, 1.1f, 1.0f, 1.110534241054576f);
		_EXPECT_FLOAT_EQ(length(res_pow), length(pow(a, b)));
	)

	GLSLTEST(VecTest, HandlesCommonFunctions,
		vec3 a = vec3(1.0f, 2.0f, 3.0f), b = vec3(4.0f, 5.0f, 6.0f);

		_EXPECT_EQ(2.0f, max(2.0f, 1.0f));
		_EXPECT_EQ(b, max(a, b));
		_EXPECT_EQ(1.0f, min(2.0f, 1.0f));
		_EXPECT_EQ(a, min(a, b));
		_EXPECT_EQ(1.0f, abs(1.0f));
		_EXPECT_EQ(1.0f, abs(-1.0f));
		_EXPECT_EQ(a, abs(a));
		_EXPECT_EQ(a, abs(-a));
		_EXPECT_EQ(1.0f, floor(1.5f));
		_EXPECT_EQ(vec3(0.0f, 1.0f, 1.0f), floor(a*0.6));
		_EXPECT_EQ(0.5f, fract(1.5f));
		_EXPECT_VEC3_EQ(vec3(0.6f, 0.2f, 0.8f), fract(a*0.6));
		_EXPECT_EQ(vec3(-1.0f, 0.0f, 1.0f), sign(a-2.0f));
		_EXPECT_EQ(0.0f, clamp(-1.0f, 0.0f, 1.0f));
		_EXPECT_EQ(0.5f, clamp(0.5f, 0.0f, 1.0f));
		_EXPECT_EQ(1.0f, clamp(1.5f, 0.0f, 1.0f));
		_EXPECT_EQ(vec3(0.1f, 0.5f, 0.9f), clamp(vec3(-1.0f, 0.5f, 1.5f), vec3(0.1f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.9f)));
		_EXPECT_EQ(vec3(0.0f, 0.5f, 1.0f), clamp(vec3(-1.0f, 0.5f, 1.5f), 0.0f, 1.0f));
		_EXPECT_FLOAT_EQ(6.0f, mix(0.0f, 10.0f, 0.6f));
		_EXPECT_VEC3_EQ(vec3(2.5f, 3.5f, 4.5f), mix(a, b, 0.5f));
	)

	GLSLTEST(VecTest, HandlesGeometricFunctions,
		vec3 a = vec3(1.0f, 2.0f, 3.0f), b = vec3(4.0f, 5.0f, 6.0f);

		_EXPECT_EQ(vec3(-3.0f, 6.0f, -3.0f), cross(a, b));
		_EXPECT_EQ(32.0f, dot(a, b));
		_EXPECT_EQ(vec3(-255.0f, -318.0f, -381.0f), reflect(a, b));
		_EXPECT_FLOAT_EQ(sqrt(14.0f), length(a));
		_EXPECT_FLOAT_EQ(sqrt(27.0f), distance(a, b));
		_EXPECT_FLOAT_EQ(length(a/sqrt(14.0f)), length(normalize(a)));
	)

	GLSLTEST(VecTest, HandlesMatrixFunctions,
		vec3 a = vec3(1.0f, 2.0f, 3.0f);
		vec4 b = vec4(4.0f, 5.0f, 6.0f, 7.0f);
		mat4x3 res = mat4x3(4.0f, 8.0f, 12.0f, 5.0f, 10.0f, 15.0f, 6.0f, 12.0f, 18.0f, 7.0f, 14.0f, 21.0f);

		_EXPECT_EQ(res, outerProduct(a, b));
	)

	GLSLTEST(VecTest, HandlesVectorRelationalFunctions,
		vec3 a = vec3(1.0f, 2.0f, 3.0f), b = vec3(4.0f, 5.0f, 6.0f);
		_EXPECT_EQ(bvec3(true, true, true), greaterThan(b, a));
	)

	GLSLTEST(MatTest, HandlesConstructors,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b = mat4(1.0f), c = mat4(a, a, a, a), d = mat4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

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
		mat2x3 a = mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f),
		       b = mat2x3(vec3(1.0f, 2.0f, 3.0f), vec3(4.0f, 5.0f, 6.0f)),
		       c = mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f);
		_EXPECT_EQ(true, a == b);
		_EXPECT_EQ(true, !(a != b));
		_EXPECT_EQ(true, !(a == c));
		_EXPECT_EQ(true, a != c);
	)

	GLSLTEST(MatTest, HandlesAddition,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b = mat4(a, a, a, a), c = mat4(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res = mat4(2*a,2*a+1.0f,2*a+2.0f,2*a+3.0f);

		_EXPECT_EQ(res, b+c);

		_EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), b+1.0f);
		_EXPECT_EQ(mat4(a+1.0f, a+1.0f, a+1.0f, a+1.0f), 1.0f+b);

		b += c;
		_EXPECT_EQ(res, b);

		b += 1.0f;
		_EXPECT_EQ(res+1.0f, b);
	)

	GLSLTEST(MatTest, HandlesSubtraction,
		vec4 a = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		mat4 b = mat4(a, a, a, a), c = mat4(a, a+1.0f, a+2.0f, a+3.0f);
		mat4 res = mat4(vec4(0.0f), vec4(-1.0f), vec4(-2.0f), vec4(-3.0f));

		_EXPECT_EQ(res, b-c);

		_EXPECT_EQ(mat4(a-1.0f, a-1.0f, a-1.0f, a-1.0f), b-1.0f);
		_EXPECT_EQ(mat4(1.0f-a, 1.0f-a, 1.0f-a, 1.0f-a), 1.0f-b);

		b -= c;
		_EXPECT_EQ(res, b);

		b -= 1.0f;
		_EXPECT_EQ(res-1.0f, b);
	)

	GLSLTEST(MatTest, HandlesMultiplication,
		vec3 a = vec3(1.0f, 2.0f, 3.0f);
		vec4 b = vec4(1.0f, 2.0f, 3.0f, 4.0f);
		mat3x4 c = mat3x4(1.0f, 4.0f, 7.0f, 10.0f, 2.0f, 5.0f, 8.0f, 11.0f, 3.0f, 6.0f, 9.0f, 12.0f);
		mat4x3 d = mat4x3(1.0f, 5.0f, 9.0f, 2.0f, 6.0f, 10.0f, 3.0f, 7.0f, 11.0f, 4.0f, 8.0f, 12.0f);
		mat4x4 res_mm = mat4x4(38.0f, 83.0f, 128.0f, 173.0f, 44.0f, 98.0f, 152.0f, 206.0f, 50.0f, 113.0f, 176.0f, 239.0f, 56.0f, 128.0f, 200.0f, 272.0f);
		mat3 res_mm2 = mat3(30.0f, 66.0f, 102.0f, 36.0f, 81.0f, 126.0f, 42.0f, 96.0f, 150.0f);
		mat3 e = mat3(1.0f, 4.0f, 7.0f, 2.0f, 5.0f, 8.0f, 3.0f, 6.0f, 9.0f);
		vec4 res_vm = vec4(38.0f, 44.0f, 50.0f, 56.0f);
		vec3 res_mv = vec3(30.0f, 70.0f, 110.0f);
		vec3 res_vm2 = vec3(30.0f, 36.0f, 42.0f);

		_EXPECT_EQ(res_mm, c * d);
		_EXPECT_EQ(res_vm, a * d);
		_EXPECT_EQ(res_mv, d * b);

		a *= e;
		_EXPECT_EQ(res_vm2, a);
		e *= e;
		_EXPECT_EQ(res_mm2, e);
	)

	GLSLTEST(MatTest, HandlesMatrixFunctions,
		mat4x3 a = mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
		mat3x4 res = mat3x4(1.0f, 4.0f, 7.0f, 10.0f, 2.0f, 5.0f, 8.0f, 11.0f, 3.0f, 6.0f, 9.0f, 12.0f);

		_EXPECT_EQ(res, transpose(a));
	)

	// C++-specific test
	TEST(VecTest, HandlesOutputStreaming) {
		vec<int, 4> a(1, 2, 3, 4);
		std::stringstream sstream;
		std::string s;
		sstream << a;
		std::getline(sstream, s);
		EXPECT_EQ("(1, 2, 3, 4)", s);
	}

	// Additional possible C++ tests, TODO:
	// * Constructor on declaration, ie vec4 a(1.0f);
	// * Copy constructors

}

int main(int argc, char **argv) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface *screen = SDL_SetVideoMode( 40, 40, 0, SDL_OPENGL);

	glDisable(GL_DEPTH_TEST);

	// create a texture object
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 16, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// attach the texture to FBO color attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, textureId, 0);

	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();

	SDL_Quit();

	return ret;
}
