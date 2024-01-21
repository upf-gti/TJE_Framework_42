
varying vec3 v_position;
varying vec3 v_world_position;
varying vec2 v_uv;
varying vec4 v_color;

uniform sampler2D u_texture;

void main()
{
	vec2 uv = v_uv;
	vec4 color = v_color * texture2D( u_texture, uv );
	gl_FragColor = color;
}
