#version 130

uniform vec2 u_resolution;
uniform vec2 u_mouse_pos;
uniform vec3 u_camera_pos;
uniform float u_time;
uniform sampler2D u_sample;
uniform float u_sample_part;
uniform vec2 u_seed1;
uniform vec2 u_seed2;
uniform int u_max_ref;
uniform int u_rays_count;

const float MAX_DIST = 99999.0;
vec3 light = normalize(vec3(-0.5, 0.75, -1.0));

uvec4 R_STATE;

// https://indico.cern.ch/event/130322/contributions/119658/attachments/91996/131518/ALICE_cern_presentation.pdf
// GPU haven`t random, found random on each pixels

uint TausStep(uint z, int S1, int S2, int S3, uint M)
{
	uint b = (((z << S1) ^ z) >> S2);
	return (((z & M) << S3) ^ b);	
}

uint LCGStep(uint z, uint A, uint C)
{
	return (A * z + C);	
}

vec2 hash22(vec2 p)
{
	p += u_seed1.x;
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

float random()
{
	R_STATE.x = TausStep(R_STATE.x, 13, 19, 12, uint(4294967294));
	R_STATE.y = TausStep(R_STATE.y, 2, 25, 4, uint(4294967288));
	R_STATE.z = TausStep(R_STATE.z, 3, 11, 17, uint(4294967280));
	R_STATE.w = LCGStep(R_STATE.w, uint(1664525), uint(1013904223));
	return 2.3283064365387e-10 * float((R_STATE.x ^ R_STATE.y ^ R_STATE.z ^ R_STATE.w));
}

// https://indico.cern.ch/event/130322/contributions/119658/attachments/91996/131518/ALICE_cern_presentation.pdf

vec3 randomOnSphere() {
	vec3 rand = vec3(random(), random(), random());
	float theta = rand.x * 2.0 * 3.14159265;
	float v = rand.y;
	float phi = acos(2.0 * v - 1.0);
	float r = pow(rand.z, 1.0 / 3.0);
	float x = r * sin(phi) * cos(theta);
	float y = r * sin(phi) * sin(theta);
	float z = r * cos(phi);
	return vec3(x, y, z); // random point on sphere
}

mat2 rot(float a) {
	float s = sin(a);
	float c = cos(a);
	return mat2(c, -s, s, c);
}

vec2 sphIntersect(in vec3 ro, in vec3 rd, float ra) {
	float b = dot(ro, rd);
	float c = dot(ro, ro) - ra * ra;
	float h = b * b - c;
	if(h < 0.0) return vec2(-1.0); // no intersiction
	h = sqrt(h);
	return vec2(-b - h, -b + h); // intersection pos
}

vec2 boxIntersection(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN)  {
	vec3 m = 1.0 / rd;
	vec3 n = m * ro;
	vec3 k = abs(m) * rad;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2(-1.0); // no intersection
	oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	return vec2(tN, tF); // intersection pos
}

float plaIntersect(in vec3 ro, in vec3 rd, in vec4 p) {
	return -(dot(ro, p.xyz) + p.w) / dot(rd, p.xyz);
}

vec3 getSky(vec3 rd) {
	vec3 col = vec3(0.3, 0.6, 1.0);
	vec3 sun = vec3(0.95, 0.9, 1.0);
	sun *= max(0.0, pow(dot(rd, light), 256.0));
	col *= max(0.0, dot(light, vec3(0.0, 0.0, -1.0)));
	return clamp(sun + col * 0.01, 0.0, 1.0);
}

vec4 castRay(inout vec3 ro, inout vec3 rd) {
	vec4 color;
	vec2 min_intersection = vec2(MAX_DIST);
	vec2 intersection;
	vec3 normal;
	
	// Add spheres
	mat2x4 spheres[6]; 
	spheres[0][0] = vec4(-1.0, 0.0, -0.01, 1.0);
	spheres[0][1] = vec4(1.0, 1.0, 1.0, -0.5);
	spheres[1][0] = vec4(0.0, 3.0, -0.01, 1.0);
	spheres[1][1] = vec4(1.0, 1.0, 1.0, 0.5);
	spheres[2][0] = vec4(1.0, -2.0, -0.01, 1.0);
	spheres[2][1] = vec4(1.0, 0.0, 0.5, 1.0);
	spheres[3][0] = vec4(3.5, -1.0, 0.5, 0.5);
	spheres[3][1] = vec4(1.0, 1.0, 1.0, -2.0);
	spheres[4][0] = vec4(-3.5, -1.0, 0.0, 0.5);
	spheres[4][1] = vec4(0.5, 1.0, 0.5, -2.0);
	spheres[5][0] = vec4(-5.5, -0.5, -0.01, 1.0);
	spheres[5][1] = vec4(0.5, 0.5, 0.5, 0.0);

	// Check intersection ray with spheres and apply color
	for(int i = 0; i < spheres.length(); i++) {
		intersection = sphIntersect(ro - spheres[i][0].xyz, rd, spheres[i][0].w);
		if(intersection.x > 0.0 && intersection.x < min_intersection.x) {
			min_intersection = intersection;
			vec3 intersection_pos = ro + rd * intersection.x;
			normal = normalize(intersection_pos - spheres[i][0].xyz);
			color = spheres[i][1];
		}
	}

	// Check intersection ray with box and apply color
	vec3 box_normal;
	vec3 box_pos = vec3(3.0, 1.0, -0.001);
	intersection = boxIntersection(ro - box_pos, rd, vec3(1.0), box_normal);
	if(intersection.x > 0.0 && intersection.x < min_intersection.x) {
		min_intersection = intersection;
		normal = box_normal;
		color = vec4(0.9, 0.2, 0.2, -0.5);
	}

	// Check intersection ray with plane(ground) and apply color
	vec3 planeNormal = vec3(0.0, 0.0, -1.0);
	intersection = vec2(plaIntersect(ro, rd, vec4(planeNormal, 1.0)));
	if(intersection.x > 0.0 && intersection.x < min_intersection.x) {
		min_intersection = intersection;
		normal = planeNormal;
		color = vec4(0.5, 0.25, 0.1, 0.01); // ground color
	}

	if(min_intersection.x == MAX_DIST) return vec4(getSky(rd), -2.0); // ray sky color

	if(color.a == -2.0) return color; // shadow

	vec3 reflected = reflect(rd, normal); // count light reflect 

	// shadow
	if(color.a < 0.0) {
		float fresnel = 1.0 - abs(dot(-rd, normal));
		if(random() - 0.1 < fresnel * fresnel) { // random fresnel
			rd = reflected; // new direction origin
			return color;
		}
		ro += rd * (min_intersection.y + 0.001); // new ray origin
		rd = refract(rd, normal, 1.0 / (1.0 - color.a)); // new ray direction
		return color;
	}

	// Matte sphere
	vec3 itPos = ro + rd * intersection.x;
	vec3 r = randomOnSphere();
	vec3 diffuse = normalize(r * dot(r, normal)); 
	ro += rd * (min_intersection.x - 0.001); // new ray origin
	rd = mix(diffuse, reflected, color.a);  // new ray direction

	return color;
}

vec3 traceRay(vec3 ro, vec3 rd) {
	// Trace re-rays(reflections) and mix color
	vec3 col = vec3(1.0);
	for(int i = 0; i < u_max_ref; i++)
	{
		vec4 refCol = castRay(ro, rd); // cast ray, recalculate ro, rd
		col *= refCol.rgb;
		if(refCol.a == -2.0) return col; // if it`s shadow return
	}
	return vec3(0.0);
}

void main() {
	vec2 uv = (gl_TexCoord[0].xy - 0.5) * u_resolution / u_resolution.y;

	// Random
	vec2 uvRes = hash22(uv + 1.0) * u_resolution + u_resolution;
	R_STATE.x = uint(u_seed1.x + uvRes.x);
	R_STATE.y = uint(u_seed1.y + uvRes.x);
	R_STATE.z = uint(u_seed2.x + uvRes.y);
	R_STATE.w = uint(u_seed2.y + uvRes.y);

	// Ray: pos, direction
	vec3 rayOrigin = u_camera_pos;
	vec3 rayDirection = normalize(vec3(1.0, uv));
	rayDirection.zx *= rot(-u_mouse_pos.y);
	rayDirection.xy *= rot(u_mouse_pos.x);

	// Trace ray and mix color
	vec3 color = vec3(0.0);
	for(int i = 0; i < u_rays_count; i++) {
		color += traceRay(rayOrigin, rayDirection);
	}

	// 
	color /= u_rays_count;
	float white = 20.0;
	color *= white * 16.0;
	color = (color * (1.0 + color / white / white)) / (1.0 + color);
	vec3 sampleColor = texture(u_sample, gl_TexCoord[0].xy).rgb;
	color = mix(sampleColor, color, u_sample_part);

	gl_FragColor = vec4(color, 1.0);
}
