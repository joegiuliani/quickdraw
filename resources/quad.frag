#version 450 compatibility

in vec2 frag_pos;
in float frag_corner_mask;
in vec4 frag_fill_color;
in vec4 frag_outline_color;

in vec2 frag_uv;
flat in unsigned int frag_quad_index;

flat in vec2 frag_quad_pos;
flat in vec2 frag_quad_size;
flat in float frag_quad_mode;
flat in float frag_quad_texture_id;
flat in float frag_quad_outline_thickness;
flat in float frag_quad_corner_size;

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
	else */ if (frag_quad_mode == MODE_PATH)
	{
		// Pixel is in the outline
		if (frag_quad_size.x * (1 - abs((frag_uv.y-0.5)*2)) < frag_quad_outline_thickness)
		{
			color = frag_outline_color;
		}
	}
	else if (frag_quad_mode == MODE_RECT)
	{
		vec2 local_pos = (frag_pos-frag_quad_pos);
		vec2 vert = round(frag_uv)*frag_quad_size; // Closest vertex to fragment

		// Drawing rounded corners
		if (round(frag_corner_mask) != 0 && !(in_bounds(local_pos.x, frag_quad_corner_size, frag_quad_size.x-frag_quad_corner_size) || in_bounds(local_pos.y, frag_quad_corner_size, frag_quad_size.y-frag_quad_corner_size)))
		{
			vec2 origin = vert - frag_quad_corner_size * (round(frag_uv)*2-1);
			float dist = distance(local_pos, origin);

			// Pixel is outside the rounded rect
			if (dist > frag_quad_corner_size)
			{
				discard;
			}

			// Pixel is in the outline
			else if (dist > frag_quad_corner_size - frag_quad_outline_thickness)
			{
				color = frag_outline_color;
			}
		}
		else
		{
			// Pixel is in the outline
			if (distance(local_pos.y, vert.y) < frag_quad_outline_thickness || distance(local_pos.x, vert.x) < frag_quad_outline_thickness)
			{
				color = frag_outline_color;
			}
		}
	}

	color_out = color;
}