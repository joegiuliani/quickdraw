#version 450 compatibility

// Per quad attributes
layout(std430, binding = 0) buffer Quads
{
	float quad_array[];
};

in vec2 frag_pos;
in float frag_corner_mask;
in vec4 frag_fill_color;
in vec4 frag_outline_color;
in vec2 frag_uv;
flat in unsigned int frag_quad_index;

out vec4 color_out;

#define MODE_RECT 0

/*
#define MODE_IMAGE 1
uniform sampler2D image;

#define MODE_FONT 2
*/

#define MODE_PATH 3

bool in_bounds(float x, float mini, float maxi)
{
	return x >= mini && x <= maxi;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec2 rect_pos()
{
	return vec2(quad_array[frag_quad_index + 0], quad_array[frag_quad_index + 1]);
}
vec2 rect_size()
{
	return vec2(quad_array[frag_quad_index + 2], quad_array[frag_quad_index + 3]);
}
float path_thickness()
{
	return quad_array[frag_quad_index + 2];
}
float quad_mode()
{
	return quad_array[frag_quad_index + 4];
}
float quad_outline_thickness()
{
	return quad_array[frag_quad_index + 5];
}
float rect_corner_size()
{
	return quad_array[frag_quad_index + 6];
}

void main()
{
	vec4 color = frag_fill_color;
	
	/*
	if (mode == MODE_IMAGE)
	{
		color *= texture(image, frag_uv);
	}
	else if (mode == MODE_FONT)
	{
		float glyph_alpha = texture(image, frag_uv).r;
		color.w *= glyph_alpha;
		color.xyz = mix(color.xyz, vec3(frag_uv.x > 0.5,0,frag_uv.x < 0.5), 1.0-glyph_alpha*0.95);
	}
	else */ if (quad_mode() == MODE_PATH)
	{
		// Pixel is in the outline
		if (path_thickness() * (1 - abs((frag_uv.y-0.5)*2)) < quad_outline_thickness())
		{
			color = frag_outline_color;
		}
	}
	else if (quad_mode() == MODE_RECT)
	{
		vec2 local_frag_pos = frag_pos-rect_pos();
		vec2 local_vert_pos = round(frag_uv)*rect_size(); // Closest vertex to fragment

		if (in_bounds(local_frag_pos.x, rect_corner_size(), rect_size().x-rect_corner_size()) || in_bounds(local_frag_pos.y, rect_corner_size(), rect_size().y-rect_corner_size()))
		{
			// Pixel is in the outline
			vec2 frag_to_vert_dist = abs(local_vert_pos - local_frag_pos);
			if (frag_to_vert_dist.x < quad_outline_thickness() || frag_to_vert_dist.y < quad_outline_thickness())
			{
				color = frag_outline_color;
			}
		}

		else if (round(frag_corner_mask) != 0)
		{
			vec2 origin = local_vert_pos - rect_corner_size() * (round(frag_uv)*2-1);
			float dist = distance(local_frag_pos, origin);

			// Pixel is outside the rounded rect
			if (dist > rect_corner_size())
			{
				discard;
			}

			// Pixel is in the outline
			else if (dist > rect_corner_size() - quad_outline_thickness())
			{
				color = frag_outline_color;
			}
		}
	}

	color_out = color;
}