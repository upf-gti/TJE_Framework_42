
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform sampler2D u_texture;

void main()
{
	vec2 uv = v_uv;

	float pixels = 200.0;

	uv.x = floor(uv.x * pixels) / pixels;
    uv.y = floor(uv.y * pixels) / pixels;

	vec3 screen_pixel = texture2D( u_texture, v_uv ).rgb;

	gl_FragColor = vec4(screen_pixel, 1.0);
}
