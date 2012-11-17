
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <gtest/gtest.h>

#include "glsl++.h"

namespace glsl {

	float eps = 0.0001;
	float pi = 3.1415926;
	float pi2 = pi/2.0;
	float time;
	uniform vec4 params[2];

	mat3 rm(float angle, vec3 n)
	{
		float c = cos(angle), s = sin(angle);
		n = normalize(n);
		return mat3(c,n.z*s,-n.y*s,
	            	    -n.z*s,c,n.x*s,
	            	    n.y*s,-n.x*s,c) +  outerProduct(n * vec3(1.0f-c), n);
	}

	vec3 phong(vec3 D, vec3 P, vec3 N, vec3 L, vec3 d, vec3 s, float e)
	{
		L = normalize(L - P);
		//    return d * max(dot(N, L), 0.0) + s * pow(max(dot(N, normalize(L - D)), 0.0), e); // Blinn-Phong
		return d * max(dot(N, L), 0.0f) + s * pow(max(dot(reflect(L, N), D), 0.0f), e);
	}

	float sphere(vec3 pos, vec3 origin, float radius)
	{
		return distance(pos, origin) - radius;
	}

	float cube(vec3 pos, vec3 origin, float size)
	{
		vec3 c = abs(pos - origin) - size;
		return max(max(c.x, c.y), c.z);
	}

	float torus(vec3 pos, vec3 origin, float radius1, float radius2)
	{
		return length(vec2(length((pos - origin).xz) - radius1, pos.y - origin.y)) - radius2;
	}

	vec3 eval(mat3 pos)
	{
		vec3 dist;
		vec3 p2 = pos[0] * rm((pos[0].y+pos[0].z)*(cos(time*0.73)*params[0].x + params[0].y /*0.3*/ /*+1.0*/), vec3(0.0, 0.0, 1.0) * rm(time*0.43, vec3(0.0, 1.0, 0.0))); 
		p2 += cos(p2*params[0].z /* + cos(time*0.87)*params[0].w + 1.5 */);
	//	p2 += cos(p2*params[0].z /*2.0*/ + cos(time*0.87)*params[0].w /*0.5*/ + 1.5);
		dist.x = min(-sphere(pos[0], vec3(0.0, 0.0, 0.0), 20.0), torus(p2, vec3(0.0, 0.0, 0.0), 1.0, 0.30+cos(time*0.37)*params[1].x /*0.1*/) - params[1].y /*0.15*/ * sphere(p2 * rm(time*0.57, vec3(0.0, 1.0, 0.0)) * (vec3(0.1, 1.0, 1.0)), vec3(0.0, 0.0, 0.0), 1.0));
	//	dist.x = min(-sphere(pos[0], vec3(0.0, 0.0, 0.0), 20.0), torus(pos[0], vec3(0.0, 0.0, 0.0), 1.0, 0.25));
		dist.y = -sphere(pos[1], vec3(0.0, 0.0, 0.0), 10.0);
		dist.z = -cube(pos[2] * rm((pos[2].x+pos[2].y)*cos(time*0.37)*params[1].z /*0.05*/, vec3(1.0, 1.0, 0.0)), vec3(0.0, 0.0, 0.0), 7.0);
		return dist;
	}

	vec3 cast(inout vec3& dir, inout vec3& origin)
	{
		float i = 0.0;
		vec3 dist, tdist = vec3(0.0, 0.0, 0.0);
		mat3 m_origin = mat3(origin, origin, origin);
		do
		{
			dist = eval(m_origin) * vec3(0.1, 1.0, 0.5);
			tdist += max(dist, vec3(eps));
	/*		m_origin[0] += dir * max(dist.x, eps);
			m_origin[1] += dir * max(dist.y, eps);
			m_origin[2] += dir * max(dist.z, eps);
			m_origin[3] += dir * max(dist.w, eps);*/
			m_origin += outerProduct(dir, max(dist, vec3(eps)));
			i += 1.0;
	//	} while (i < 400.0 && greaterThan(dist, vec3(eps)) != bvec3(false, false, false));
		} while (i < 300.0 && (dist.x > eps || dist.y > eps || dist.z > eps)); // greaterThan(dist, vec3(eps)) != bvec3(false, false, false));

	/*	if (i >= 400.0)
			return vec3(1.0, 1.0, 1.0);*/

		mat3 e1 = mat3(eps, 0.0, 0.0,
				eps, 0.0, 0.0,
				eps, 0.0, 0.0);
		mat3 e2 = mat3(0.0, eps, 0.0,
				0.0, eps, 0.0,
				0.0, eps, 0.0);
		mat3 e3 = mat3(0.0, 0.0, eps,
				0.0, 0.0, eps,
				0.0, 0.0, eps);
		mat3 N = transpose(mat3(
		                  	  eval(m_origin + e1) - eval(m_origin - e1),
		                  	  eval(m_origin + e2) - eval(m_origin - e2),
		                  	  eval(m_origin + e3) - eval(m_origin - e3)
		                 	 ));
		N[0] = normalize(N[0]);
		N[1] = normalize(N[1]);
		N[2] = normalize(N[2]);
		vec3 color = 0;
		if ((tdist.x < tdist.y) && (tdist.x < tdist.z)) {
			color = phong(dir, m_origin[0], N[0], vec3(0.0, 4.0, 4.0), vec3(1.0, 0.5, 0.2), vec3(1.0, 0.7, 0.0), 50.0)
		       	       + phong(dir, m_origin[0], N[0], vec3(-4.0, -4.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), 50.0);
			dir = reflect(dir, N[0]);
		}
		else if ((tdist.y < tdist.z)) {
			color = phong(dir, m_origin[1], N[1], vec3(0.0, 4.0, 4.0), vec3(1.0, 0.7, 0.2), vec3(0.0, 0.0, 1.0), 50.0)
		       	       + phong(dir, m_origin[1], N[1], vec3(-4.0, -4.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), 50.0);
			dir = reflect(dir, N[1]);
		}
		else {
			color = phong(dir, m_origin[2], N[2], vec3(0.0, 4.0, 4.0), vec3(0.2, 0.1, 0.0), vec3(0.0, 0.0, 0.7), 100.0)
		       	       + phong(dir, m_origin[2], N[2], vec3(-4.0, -4.0, 0.0), vec3(0.2, 0.1, 0.5), vec3(0.0, 0.0, 0.7), 100.0);
			dir = reflect(dir, N[2]);
		}
	/*	if ((tdist.x < tdist.y) && (tdist.x < tdist.z)) {
			color = 0;
			color = phong(dir, m_origin[0], N[0], vec3(0.0, 4.0, 4.0), vec3(0.2, 0.4, 0.2), vec3(0.4, 0.0, 0.0), 50.0)
		       	       + phong(dir, m_origin[0], N[0], vec3(-4.0, -4.0, 0.0), vec3(0.0, 0.5, 0.0), vec3(0.0, 0.0, 0.4), 50.0);
			dir = reflect(dir, N[0]);
		}
		else if ((tdist.y < tdist.z)) {
			color = 0;
			color = phong(dir, m_origin[1], N[1], vec3(0.0, 4.0, 4.0), vec3(0.2, 0.2, 0.0), vec3(1.0, 1.0, 1.0), 50.0)
		       	       + phong(dir, m_origin[1], N[1], vec3(-4.0, -4.0, 0.0), vec3(0.0, 0.0, 0.2), vec3(1.0, 1.0, 1.0), 50.0);
			dir = reflect(dir, N[1]);
		}
		else {
			color = 0;
			color = phong(dir, m_origin[2], N[2], vec3(0.0, 4.0, 4.0), vec3(0.4, 0.4, 0.1), vec3(0.4, 0.1, 0.3), 50.0)
		       	       + phong(dir, m_origin[2], N[2], vec3(-4.0, -4.0, 0.0), vec3(0.4, 0.1, 0.1), vec3(0.1, 0.4, 0.1), 50.0);
			dir = reflect(dir, N[2]);
		}*/
		return color;
	}

	void glslmain( void )
	{
		vec2 p = vec2(gl_FragCoord.x + (gl_TexCoord[0].y - gl_TexCoord[0].x)/2.0, gl_FragCoord.y) / gl_TexCoord[0].yy * 2.0 - 1.0;
		mat3 m = mat3(1.0);
		vec3 tr = vec3(0.0);
		time = gl_TexCoord[0].z;

		m *= rm(time*0.1, vec3(1.0, 1.0, 0.0));

		vec3 dir = normalize(vec3(p, -1.0) * m);
		vec3 origin = vec3(0.0, 0.0, 3.5 - (cos(time*0.43)*0.75+cos(time*1.04)*0.25+1.0)*params[1].w) * m + tr;

		vec3 c1 = cast(dir, origin);
		vec3 c2 = cast(dir, origin);
		vec3 c = c1 * 0.75 + c2 * 0.25;
	/*	float exposure = 3.0, maxbright = 1.0;
		c = c * exposure * (exposure / (maxbright + 1.0)) / (exposure + 1.0);*/
		c *= 1.125;
		gl_FragColor = vec4(pow(c, vec3(1.0 / 1.0)), 1.0);
	}

	//#include "main.glsl.int"

	int main()
	{
		std::ofstream out("out.raw");
		vec3 v(0.0f, 1.0f, 0.0f), v2 = v + v;
		zip(v, v2, plus<float>);
		zip(v, v2, plus_assign<float>);
		map(v, 1.0f, plus<float>);
		map(v, 1.0f, plus_assign<float>);
		v2 = v2 + 1.0;
		v2 = 1.0f + v2;
		v2 += v;
		v2 += (v += v2).xyz;
		v2.xy += 1.0f;
		printf("%f %f %f\n", v2.x, v2.y, v2.z);
		vec3 v3(1.0f);
		printf("%f %f %f\n", v3.x, v3.y, v3.z);
		float l1 = length(v2);
		float l2 = length(v2.xy);
		printf("%f %f\n", l1, l2);
		printf("%f\n", cross(v, v2).z);
		mat3 m = outerProduct(v, v);
		mat3 m2 = m + m;
		mat3 m3 = m * m2;
		mat<float, 4, 2> m4;
		mat<float, 2, 3> m5;
		mat<float, 4, 3> m6 = m4 * m5;
		mat3 m7 = rm(pi/4, vec3(0.0f, 1.0f, 0.0f));
		vec3 v4 = vec3(1.0f, 0.0f, 0.0f) * m7;
		printf("%f %f %f\n", v4.x, v4.y, v4.z);
		printf("\n%f %f %f\n%f %f %f\n%f %f %f\n", m7[0][0], m7[1][0], m7[2][0], m7[0][1], m7[1][1], m7[2][1], m7[0][2], m7[1][2], m7[2][2]);
		vec4 v5 = vec4(vec2(vec2(1.0f, 2.0f)).xy, vec2(3.0f, 4.0f));
		printf("%f %f %f %f\n", v5.x, v5.y, v5.z, v5.w);

		gl_TexCoord[0] = vec4(720, 480, 0.0f, 0.0f);
		params[0] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		params[1] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		for (int y = 479; y >= 0; --y) {
			for (int x = 0; x < 720; ++x) {
				gl_FragCoord = vec4(x, y, 0.0f, 0.0f);
				glslmain();
	//			printf("%f %f %f %f\n", gl_FragColor.r, gl_FragColor.g, gl_FragColor.b, gl_FragColor.a);
				out << (char) clamp(gl_FragColor.r * 255, 0, 255) << (char) clamp(gl_FragColor.g * 255, 0, 255) << (char) clamp(gl_FragColor.b * 255, 0, 255);
			}
		}
		return 0;
	}

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
