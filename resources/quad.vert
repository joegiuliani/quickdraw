#version 450 compatibility

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec2 vertex_uv;
layout(location = 2) in unsigned int vertex_corner_mask;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec4 vertex_outline_color;
layout(location = 5) in unsigned int quad_mode;
layout(location = 6) in vec2 quad_pos;
layout(location = 7) in vec2 quad_size;
layout(location = 8) in float quad_outline_thickness;
layout(location = 9) in unsigned int quad_texture_id;
layout(location = 10) in float quad_corner_size;

uniform vec2 transform;

out vec2 frag_vertex_pos;
out vec2 frag_vertex_uv;
out unsigned int frag_vertex_corner_mask;
out vec4 frag_vertex_color;
out vec4 frag_vertex_outline_color;
flat out unsigned int frag_quad_mode;
flat out vec2 frag_quad_pos;
flat out vec2 frag_quad_size;
flat out float frag_quad_outline_thickness;
flat out unsigned int frag_quad_texture_id;
flat out float frag_quad_corner_size;

void main()
{
	frag_vertex_pos = vertex_pos;
	frag_vertex_uv = vertex_uv;
	frag_vertex_corner_mask = vertex_corner_mask;
	frag_vertex_color = vertex_color;
	frag_vertex_outline_color = vertex_outline_color;
	frag_quad_mode = quad_mode;
	frag_quad_pos = quad_pos;
    frag_quad_size = quad_size;
	frag_quad_outline_thickness = quad_outline_thickness;
	frag_quad_texture_id = quad_texture_id;
	frag_quad_corner_size = quad_corner_size;

	// vert_pos is in pixel space with the origin at the top left corner of the context
	// y coordinates increase down the screen, ie flipped
	
	vec2 ndc_pos = (vertex_pos*transform-0.5)*2;
	gl_Position = vec4(ndc_pos.x, -ndc_pos.y,0.5,1);
}