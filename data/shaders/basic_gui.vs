attribute vec3 a_vertex;
attribute vec2 a_uv;

varying vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_viewprojection;

void main()
{	
	//store the texture coordinates
	v_uv = a_uv;

	vec3 world_position = (u_model * vec4( a_vertex, 1.0) ).xyz;

	//calcule the position of the vertex using the matrices
	gl_Position = u_viewprojection * vec4( world_position, 1.0 );
}