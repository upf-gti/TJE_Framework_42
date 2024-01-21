varying vec3 v_world_position;
varying vec3 v_normal;
varying vec4 v_color;

uniform vec4 u_color;

void main()
{
	vec3 river_color = vec3(0.1, 0.1, 0.76);
	vec3 grass_color = vec3(0.2, 0.8, 0.1);
	vec3 mountain_color = vec3(0.15, 0.1, 0.0);

	vec3 final_color = river_color;

	if(v_world_position.y > 10)
		final_color = grass_color;
	if(v_world_position.y > 80)
		final_color = mountain_color;
	if(v_world_position.y > 180)
		final_color = vec3(1);

	gl_FragColor = vec4(final_color, 1.0);
}
