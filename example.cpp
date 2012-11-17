#include <fstream>

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

	int main()
	{
		std::ofstream out("out.raw");

		gl_TexCoord[0] = vec4(720, 480, 0.0f, 0.0f);
		params[0] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		params[1] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		for (int y = 479; y >= 0; --y) {
			for (int x = 0; x < 720; ++x) {
				gl_FragCoord = vec4(x, y, 0.0f, 0.0f);
				glslmain();
				out << (char) clamp(gl_FragColor.r * 255, 0, 255) << (char) clamp(gl_FragColor.g * 255, 0, 255) << (char) clamp(gl_FragColor.b * 255, 0, 255);
			}
		}
		return 0;
	}
}

int main()
{
	return glsl::main();
}
