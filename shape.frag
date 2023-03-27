#version 450 compatibility

in vec2 frag_pos;
in vec2 frag_uv;
out vec4 color_out;

/*
	
Eventually these are gonna be turned into vertex data
and batch drawing will be implemented

*/

uniform vec4 fill_start;
uniform vec4 fill_end;
uniform int fill_direction;

uniform vec4 outline_start;
uniform vec4 outline_end;
uniform int outline_direction;
uniform float outline_thickness;

uniform int mode;

#define MODE_RECT 0
uniform vec2 pos;
uniform vec2 size;
uniform float corner_size;
uniform float header_depth;
uniform vec4 header_color;

#define MODE_IMAGE 1
uniform sampler2D image;

#define MODE_FONT 2

#define MODE_PATH 3
uniform float path_thickness;
uniform float path_length;
uniform float curr_path_progress;
uniform float next_path_progress;

bool in_bounds(float x, float mini, float maxi)
{
	return x >= mini && x <= maxi;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
	vec4 color = mix(fill_start, fill_end, frag_uv[fill_direction]);

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
	else if (mode == MODE_PATH)
	{
		// Pixel is in the outline
		if (path_thickness * (1 - abs((frag_uv.y-0.5)*2)) < outline_thickness)
		{
			color = mix(outline_start, outline_end, frag_uv[outline_direction]);
		}
		else if (fill_direction == 0) // Horizontal, along the path
		{
			color = mix(fill_start, fill_end, frag_uv[0]*(next_path_progress-curr_path_progress)+curr_path_progress);
		}
	}
	else if (mode == MODE_RECT)
	{
		vec2 local_pos = (frag_pos-pos);
		vec2 vert = round(frag_uv)*size; // Closest corner to fragment

		if (local_pos.y < header_depth)
		{
			color = header_color;
		}

		// Drawing rounded corners
		if (corner_size > 0 && !(in_bounds(local_pos.x, corner_size, size.x-corner_size) || in_bounds(local_pos.y, corner_size, size.y-corner_size)))
		{
			vec2 origin = vert - corner_size * (round(frag_uv)*2-1);
			float dist = distance(local_pos, origin);

			// Pixel is outside the rounded rect
			if (dist > corner_size)
			{
				discard;
			}

			// Pixel is in the outline
			else if (dist > corner_size - outline_thickness)
			{
				color = mix(outline_start, outline_end, frag_uv[outline_direction]);
			}
		}
		else
		{
			// Pixel is in the outline
			if (distance(local_pos.y, vert.y) < outline_thickness || distance(local_pos.x, vert.x) < outline_thickness)
			{
				color = mix(outline_start, outline_end, frag_uv[outline_direction]);
			}
		}
	}
	
	color_out = color;
}