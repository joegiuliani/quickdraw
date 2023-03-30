#version 450 compatibility

// Dynamic attributes
layout(location = 0) in vec2 pos;
layout(location = 1) in float corner_mask;
layout(location = 2) in vec4 fill_color;
layout(location = 3) in vec4 outline_color;

// Static attributes
layout(location = 4) in vec2 uv;
layout(location = 5) in unsigned int quad_index;

// Per quad attributes
layout(std430, binding = 0) buffer Quads
{
	float quad_array[];
};

uniform vec2 transform;

out vec2 frag_pos;
out float frag_corner_mask;
out vec4 frag_fill_color;
out vec4 frag_outline_color;

out vec2 frag_uv;
flat out unsigned int frag_quad_index;

void main()
{
	frag_pos					= pos;
	frag_corner_mask			= corner_mask;
	frag_fill_color				= fill_color;
	frag_outline_color			= outline_color;	
	frag_uv						= uv;
	frag_quad_index             = quad_index;
	
	// vert_pos is in pixel space with the origin at the top left corner of the context
	// y coordinates increase down the screen, ie flipped
	
	vec2 ndc_pos = (pos*transform-0.5)*2;
	gl_Position = vec4(ndc_pos.x, -ndc_pos.y,0.5,1);
}