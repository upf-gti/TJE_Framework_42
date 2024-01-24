varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

uniform vec4 u_background_color;
uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;
uniform vec3 u_light_color;
uniform vec3 u_light_position;

uniform float u_tiling;

// materials
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

uniform sampler2D u_texture;
uniform sampler2D u_normals_texture;
uniform vec2 u_maps;

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv){
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );

	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	return mat3( T * invmax, B * invmax, N );
}

vec3 perturbNormal( vec3 N, vec3 V, vec2 texcoord, vec3 normal_pixel ){

	// assume N, the interpolated vertex normal and
	// V, the view vector (eye to vertex)
	normal_pixel = normal_pixel * 255./127. - 128./127.;
	mat3 TBN = cotangent_frame(N, V, texcoord);
	return normalize(TBN * normal_pixel);
}

vec4 applyLight()
{
	vec4 Kd = vec4(u_Kd, 1.0);

	if(u_maps.x > 0.0) {
		Kd = texture2D( u_texture, v_uv * u_tiling );
	}

	vec3 L = normalize(u_light_position  - v_world_position);
	vec3 V = normalize(u_camera_position - v_world_position);

	vec3 N = normalize(v_normal);
	if(u_maps.y > 0.0) {
		vec3 normals_texture = texture2D( u_normals_texture, v_uv * u_tiling ).xyz;
		N = perturbNormal( normalize(v_normal), -V, v_uv * u_tiling, normals_texture );
	}

	//ambient
	vec3 light = u_Ka * Kd.xyz;
	//direct
	float NdotL = max(0.0,dot(L,N));

	light += Kd.xyz * u_light_color * NdotL;

	vec3 R = reflect(-L, N);	
	float RdotV = pow(max(0.0,dot(R,V)), 20.0);

	light += u_Ks * u_light_color * RdotV;

	return vec4(light, Kd.a);
}

void main() {
	
	vec4 final_color = applyLight();

	if (final_color.a <= 0.3) discard;

	float dist = length( u_camera_position - v_world_position);
	
	float fogMaxDist = 10.0;
	float fogMinDist = 4.0;
	float fog_factor = pow( clamp( 1.0 - (fogMaxDist / dist), 0.0, 1.0), fogMinDist );

	final_color.rgb = mix( final_color.rgb, vec3(0.6, 0.6, 0.6), fog_factor );

	gl_FragColor = final_color;
}
