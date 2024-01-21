
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform sampler2D u_texture;

void main()
{
	vec3 screen_pixel = texture2D( u_texture, v_uv ).rgb;

	// screen_pixel *= 1.0 - length(v_uv - vec2(0.5));

	gl_FragColor = vec4(screen_pixel, 1.0);
}
