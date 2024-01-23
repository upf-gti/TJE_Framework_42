
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec3 u_camera_position;
uniform vec4 u_color;
uniform samplerCube u_texture;
uniform sampler2D u_normals_texture;
uniform float u_time;

uniform float u_tiling;

void main()
{
	vec3 normals_texture0 = texture2D( u_normals_texture, v_uv * u_tiling + u_time * 0.01 ).rgb;
	vec3 normals_texture1 = texture2D( u_normals_texture, v_uv * u_tiling - u_time * 0.01 ).rgb;

	normals_texture0 = normals_texture0 * 0.5 - 0.5;
	normals_texture1 = normals_texture1 * 0.5 - 0.5;

	vec3 normals_avg = (normals_texture0 + normals_texture1) * 0.5;

	normals_avg = mix(v_normal, normals_avg, 0.2);

	vec3 N = normalize( normals_avg );
	vec3 V = normalize( u_camera_position - v_world_position );

	vec3 R = reflect(V, N);

	gl_FragColor = u_color * textureCube( u_texture, R );
}
