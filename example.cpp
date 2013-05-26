#include <fstream>

#include "glsl++.h"

namespace glsl {

struct shader {

	float eps = 0.0001;
	float pi = 3.1415926;
	float pi2 = pi/2.0;
	float pi4 = pi/4.0;
	float time;
	uniform vec3 origin;
	uniform vec3 up;
	uniform vec3 dir;

	mat3 rm(float angle, vec3 n)
	{
		float c = cos(angle), s = sin(angle);
		n = normalize(n);
		return mat3(c,n.z*s,-n.y*s,
	            	    -n.z*s,c,n.x*s,
	            	    n.y*s,-n.x*s,c) + outerProduct(n * (1.0-c), n);
	}




	float hash( float n ) { return fract(sin(n)*43758.5453); }

	float mix2(float x, float y, float a)
	{
		float z = (a - 0.5) * 2.0;
		return mix(x, y, pow(abs(z), 1.5) * sign(z) * 0.5 + 0.5);
	}

	float noise( vec3 x )
	{
		vec3 p = floor(x);
		vec3 f = fract(x);
    		f = f*f*(3.0-2.0*f);
    		float n = p.x + p.y*57.0 + p.z * 43.0;
    		float res1 = mix2(mix2(hash(n+0.0), hash(n+1.0),f.x), mix2(hash(n+57.0), hash(n+57.0+1.0),f.x),f.y);
    		float res2 = mix2(mix2(hash(n+43.0), hash(n+43.0+1.0),f.x), mix2(hash(n+43.0+57.0), hash(n+43.0+57.0+1.0),f.x),f.y);
		float res = mix2(res1, res2, f.z);
    		return res;
	}

	float cloud(vec3 p) {
		float f = pow(0.5*(noise(p*1.0)+0.5*(noise(p*2.0)+0.5*(noise(p*4.0)+0.5*noise(p*8.0)))), 2.0);
		return f;
	}












	float DIST_MULTIPLIER = 0.035;
	float MAX_DIST = 100.0;




	float
  	  min_dist = 0.0001,           
  	  ao_eps = 0.0005,             
  	  ao_strength = 0.12,        
  	  glow_strength = 0.75,      
  	  dist_to_color = 0.1;      

	int 
  	  max_steps = 1800;          


	vec3 
  	  surfaceColor1 = vec3(0.95, 0.64, 0.1),
  	  surfaceColor2 = vec3(0.89, 0.95, 0.75),
  	  surfaceColor3 = vec3(0.55, 0.06, 0.03),
  	  specularColor = vec3(1.0, 0.8, 0.4),
  	  glowColor = vec3(0.03, 0.4, 0.4)/1.5,
  	  aoColor = vec3(0, 0, 0),
  	  fogColor = vec3(0.1);



	void sphereFold(inout vec3& z, inout float& dz, float fixedRadius2) {
		float r2 = dot(z,z) - 0.04;
 	 if (r2<fixedRadius2) {
			float temp =(fixedRadius2/r2);
			z*=temp;
			dz*=temp;
		}
	}


	void boxFold(inout vec3& z, inout float& dz, vec3 foldingLimit) {
		z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
	}





	void powN2(inout vec3& z, float zr0, inout float& dr, float Power2, float ZMUL) {
		z.x = abs(z.x);
		float zo0 = asin( z.z/zr0 );
		float zi0 = atan( z.y,z.x );
		float zr = pow( zr0, Power2-1.0 );
		float zo = zo0 * Power2;
		float zi = zi0 * Power2;
		dr = zr*dr*Power2*abs(length(vec3(1.0,1.0,ZMUL)/sqrt(3.0))) + 1.0;
		zr *= zr0;
		z  = zr*vec3( cos(zo)*cos(zi), cos(zo)*sin(zi), ZMUL*sin(zo) );
	}






	float DE(vec3 z, out vec3& col)
	{
		int n = 0;
		float dr = 1.0;
		float r = length(z);
		vec4 trap = vec4(1.0f);
		while (r<7.5 && n < 4) {
			float sc = clamp(pow(z.z, 3.0), 0.75, 50.0);
			boxFold(z,dr,vec3(0.5, 0.5, 1.282500));
			sphereFold(z,dr,0.5);
			r=length(z);
			powN2(z,r,dr,2.437598,9.072040);
			dr*=abs(sc);
			z = sc*z; 
			r = length(z);
			
		 	 trap = min(trap, (vec4(abs(4.0*z),dot(z,z))));
			n++;
		}
		
		
		
		vec2 c = clamp(vec2( 0.05*log(dot(z,z))-0.25, trap.z ), 0.0, 1.0);

		col = mix(mix(surfaceColor1, surfaceColor2, trap.z*trap.w), surfaceColor3, c.x);

		return (r * log(r) / abs(dr)) * DIST_MULTIPLIER;
	}

	float eval(vec3 pos, out vec3& col) {
		return DE(pos, col);
	}



	float normal_eps = 0.000005;



	vec3 normal(vec3 pos, float d_pos) {
  	  vec4 Eps = vec4(0, normal_eps, 2.0*normal_eps, 3.0*normal_eps);
  	  vec3 col;
  	  return normalize(vec3(
  	  




  	  
    	    -eval(pos-Eps.yxx,col)+eval(pos+Eps.yxx,col),
    	    -eval(pos-Eps.xyx,col)+eval(pos+Eps.xyx,col),
    	    -eval(pos-Eps.xxy,col)+eval(pos+Eps.xxy,col)

  	  




  	  



  	  ));
	}




	vec3 blinn_phong(vec3 normal, vec3 view, vec3 light, vec3 diffuseColor) {
  	  vec3 halfLV = normalize(light + view);
  	  float spe = pow(max( dot(normal, halfLV), 0.0 ), 32.0);
  	  float dif = dot(normal, light) * 0.5 + 0.75;
  	  return dif*diffuseColor + spe*specularColor;
	}



	float ambient_occlusion(vec3 p, vec3 n) {
  	  float ao = 1.0, w = ao_strength/ao_eps;
  	  float dist = 2.0 * ao_eps;
  	  vec3 col;

  	  for (int i=0; i<5; i++) {
    	    float D = eval(p + n*dist,col);
    	    ao -= (dist-D) * w;
    	    w *= 0.5;
    	    dist = dist*2.0 - ao_eps;  
  	  }
  	  return clamp(ao, 0.0, 1.0);
	}

	float sphere(vec3 p) {
   	   return length(p)-2.5;
	}

	float f(vec3 p) {
   	   return sphere(p) + cloud(p*5.+time) * 2.0+1.0;
	}

	vec3 cast(inout vec3& dir, inout vec3& eye) {
  	  
  	  vec3 eye_in = eye;



  	  vec3 p = eye_in, dp = normalize(dir);
  	  vec3 color;

  	  float totalD = 0.0, D = 3.4e38, extraD = 0.0, lastD;

  	  
  	  int steps;
  	  for (steps=0; steps<max_steps; steps++) {
    	    lastD = D;
    	    D = eval(p + totalD * dp, color);

    	    
    	    if (extraD > 0.0 && D < extraD) {
      	      totalD -= extraD;
      	      extraD = 0.0;
      	      D = 3.4e38;
      	      steps--;
      	      continue;
    	    }

    	    if (D < min_dist || D > MAX_DIST) break;

    	    totalD += D;

    	    
    	    totalD += extraD = 0.096 * D*(D+extraD)/lastD;

  	  }

  	  vec3 p2 = p;
   	   p2.z -= 12.;

   	   
   	   
   	   float ld, td= 0.;
   	   float w;
   	   
   	   
   	   
   	   
   	   float l, lacc = 0.;


   	   
   	   for (float i=0.; (i<1.); i+=1./64.) {
      	      if(td > .95)
           	   break;
      	      
      	      l = f(p2) * 0.6;

      	      
      	      if (l < .01) {
        	
        	ld = 0.01 - l;
        	w = (1. - td) * ld;   
        	
	    	    
        	
        	td += w;
      	      }
      	      
      	      
      	      l = max(l, 0.03);
	  	  lacc += l;
	  	  
	  	  if (lacc > totalD)
        	break;      
      	      
      	      
      	      p2 += l*dir;
   	   }

  	  vec3 edir = normalize(dp + p * 0.005);
      	      
  	  p += totalD * dp;

  	  

  	  vec3 backgroundColor = cloud(edir*10.0) * vec3(0.65, 0.45, 1.0) * 0.6;

  	  backgroundColor += clamp(cloud(edir*150.0)-0.5, 0.0, 1.0)*10.0;

  	  vec3 col = backgroundColor;

  	  
  	  if (D < min_dist) {
    	    vec3 n = normal(p, D);
    	    col = color;
    	    col = blinn_phong(n, -dp, normalize(eye_in+vec3(0,1,0)+dp), col);
    	    col = mix(aoColor, col, ambient_occlusion(p, n));
  	  }

  	  col = mix(col, td*1.5*surfaceColor3 + ld*3*surfaceColor1, clamp(td*2, 0.0, 1.0));

  	  
  	  col = mix(col, glowColor, float(steps)/float(max_steps) * glow_strength);
  	  

  	  return col;
	}

	void glslmain( void )
	{
		vec2 p = vec2(gl_FragCoord.x + (gl_TexCoord[0].y - gl_TexCoord[0].x)/2.0, gl_FragCoord.y) / gl_TexCoord[0].yy * 2.0 - 1.0;
		vec2 p2 = gl_FragCoord.xy / gl_TexCoord[0].xy * 2.0 - 1.0;
		p /= 1.2;
		mat3 m = mat3(1.0);
		vec3 tr = vec3(0.0);
		time = gl_TexCoord[0].z;











		vec3 dir = normalize(vec3(1.0, 0.0, 1.5));
		vec3 up = vec3(0.0, 1.0, 0.0);
		vec3 origin = vec3(-5.0, 0.0, -5.0);

/*		vec3 origin = pow(vec3(noise(vec3(time*0.09, 0., 0.)), noise(vec3(0., time*0.13, 0.)), noise(vec3(0., 0., time*0.11)))-0.5, 2.0)*15.0 - vec3(0.0, 0.0, 1.0);
		vec3 dir = normalize(vec3(noise(vec3(time*0.17, 0., 0.)), noise(vec3(0., time*0.19, 0.)), noise(vec3(0., 0., time*0.21)))-0.5-origin*0.1);
		vec3 up = -normalize(cross(normalize(cross(up, dir)), dir));*/

		vec3 sorigin = origin; 
		vec3 sdir = normalize(p.x * cross(up, dir) + p.y * up + 1 * dir);


		vec3 c1 = cast(sdir, sorigin);

		vec3 c = c1;
		c *= clamp(smoothstep(1.5, 0.5, length(p2)), 0.0, 1.0);

		c *= 1.125;
		gl_FragColor = vec4(pow(c, vec3(1.0 / 1.0)), 1.0);
	}

	vec4 gl_TexCoord[8];
	vec4 gl_FragCoord;
	vec4 gl_FragColor;

};

	int main()
	{
		std::ofstream outfile("out.raw");
		int xres = 720, yres = 480;
		unsigned char *data = (unsigned char*) malloc(yres*xres*3);

		params[0] = vec4(0.8f, 0.8f, 1.0f, 1.0f);
		params[1] = vec4(0.2f, 0.4f, 0.01f, 3.0f);
		#pragma omp parallel for schedule(dynamic, 4)
		for (int y = 0; y < yres; ++y) {
			for (int x = 0; x < xres; ++x) {
				shader sh;
				sh.gl_TexCoord[0] = vec4(xres, yres, 20.0f, 0.0f);
				sh.gl_FragCoord = vec4(x, y, 0.0f, 0.0f);
				sh.glslmain();
				data[((yres-y-1)*xres+x)*3] = clamp(sh.gl_FragColor.r * 255, 0, 255);
				data[((yres-y-1)*xres+x)*3+1] = clamp(sh.gl_FragColor.g * 255, 0, 255);
				data[((yres-y-1)*xres+x)*3+2] = clamp(sh.gl_FragColor.b * 255, 0, 255);
			}
		}
		// There's probably a better way to do this, something akin to fwrite(), in C++
		for (long long i = 0; i < yres*xres*3; ++i) {
			outfile << data[i];
		}
		free(data);
		return 0;
	}
}

int main()
{
	return glsl::main();
}
