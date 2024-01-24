varying vec2 v_uv;

uniform vec4 u_color;
uniform float u_mask;

void main()
{
	if(v_uv.y > u_mask) {
		discard;
	}

	vec3 final_color = mix(vec3(1.0, 0.3, 0.3), vec3(0.3, 1.0, 0.3), v_uv.y);

	gl_FragColor = vec4(final_color * u_color.rgb, 1.0);
}