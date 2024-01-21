attribute vec3 a_vertex;
varying vec2 v_uv;

void main()
{
    v_uv = a_vertex.xy * 0.5 + vec2(0.5);
    gl_Position = vec4(a_vertex.xy, 0.0, 1.0);
}