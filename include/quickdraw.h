#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#include "freetype\config/ftheader.h"
#include "freetype\freetype.h"
#include "glad\glad.h"
#include "GLFW\glfw3native.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include "glm\geometric.hpp"
#include "glm\vec2.hpp"
#include "glm\vec4.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <functional>

// FORWARD DECLARATIONS
namespace quickdraw
{
constexpr int KEY_UNKNOWN = GLFW_KEY_UNKNOWN;
constexpr int KEY_SPACE = GLFW_KEY_SPACE;
constexpr int KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE;
constexpr int KEY_COMMA = GLFW_KEY_COMMA;
constexpr int KEY_MINUS = GLFW_KEY_MINUS;
constexpr int KEY_PERIOD = GLFW_KEY_PERIOD;
constexpr int KEY_SLASH = GLFW_KEY_SLASH;
constexpr int KEY_0 = GLFW_KEY_0;
constexpr int KEY_1 = GLFW_KEY_1;
constexpr int KEY_2 = GLFW_KEY_2;
constexpr int KEY_3 = GLFW_KEY_3;
constexpr int KEY_4 = GLFW_KEY_4;
constexpr int KEY_5 = GLFW_KEY_5;
constexpr int KEY_6 = GLFW_KEY_6;
constexpr int KEY_7 = GLFW_KEY_7;
constexpr int KEY_8 = GLFW_KEY_8;
constexpr int KEY_9 = GLFW_KEY_9;
constexpr int KEY_SEMICOLON = GLFW_KEY_SEMICOLON;
constexpr int KEY_EQUAL = GLFW_KEY_EQUAL;
constexpr int KEY_A = GLFW_KEY_A;
constexpr int KEY_B = GLFW_KEY_B;
constexpr int KEY_C = GLFW_KEY_C;
constexpr int KEY_D = GLFW_KEY_D;
constexpr int KEY_E = GLFW_KEY_E;
constexpr int KEY_F = GLFW_KEY_F;
constexpr int KEY_G = GLFW_KEY_G;
constexpr int KEY_H = GLFW_KEY_H;
constexpr int KEY_I = GLFW_KEY_I;
constexpr int KEY_J = GLFW_KEY_J;
constexpr int KEY_K = GLFW_KEY_K;
constexpr int KEY_L = GLFW_KEY_L;
constexpr int KEY_M = GLFW_KEY_M;
constexpr int KEY_N = GLFW_KEY_N;
constexpr int KEY_O = GLFW_KEY_O;
constexpr int KEY_P = GLFW_KEY_P;
constexpr int KEY_Q = GLFW_KEY_Q;
constexpr int KEY_R = GLFW_KEY_R;
constexpr int KEY_S = GLFW_KEY_S;
constexpr int KEY_T = GLFW_KEY_T;
constexpr int KEY_U = GLFW_KEY_U;
constexpr int KEY_V = GLFW_KEY_V;
constexpr int KEY_W = GLFW_KEY_W;
constexpr int KEY_X = GLFW_KEY_X;
constexpr int KEY_Y = GLFW_KEY_Y;
constexpr int KEY_Z = GLFW_KEY_Z;
constexpr int KEY_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
constexpr int KEY_BACKSLASH = GLFW_KEY_BACKSLASH;
constexpr int KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
constexpr int KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
constexpr int KEY_WORLD_1 = GLFW_KEY_WORLD_1;
constexpr int KEY_WORLD_2 = GLFW_KEY_WORLD_2;
constexpr int KEY_ESCAPE = GLFW_KEY_ESCAPE;
constexpr int KEY_ENTER = GLFW_KEY_ENTER;
constexpr int KEY_TAB = GLFW_KEY_TAB;
constexpr int KEY_BACKSPACE = GLFW_KEY_BACKSPACE;
constexpr int KEY_INSERT = GLFW_KEY_INSERT;
constexpr int KEY_DELETE = GLFW_KEY_DELETE;
constexpr int KEY_RIGHT = GLFW_KEY_RIGHT;
constexpr int KEY_LEFT = GLFW_KEY_LEFT;
constexpr int KEY_DOWN = GLFW_KEY_DOWN;
constexpr int KEY_UP = GLFW_KEY_UP;
constexpr int KEY_PAGE_UP = GLFW_KEY_PAGE_UP;
constexpr int KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
constexpr int KEY_HOME = GLFW_KEY_HOME;
constexpr int KEY_END = GLFW_KEY_END;
constexpr int KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
constexpr int KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
constexpr int KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK;
constexpr int KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
constexpr int KEY_PAUSE = GLFW_KEY_PAUSE;
constexpr int KEY_F1 = GLFW_KEY_F1;
constexpr int KEY_F2 = GLFW_KEY_F2;
constexpr int KEY_F3 = GLFW_KEY_F3;
constexpr int KEY_F4 = GLFW_KEY_F4;
constexpr int KEY_F5 = GLFW_KEY_F5;
constexpr int KEY_F6 = GLFW_KEY_F6;
constexpr int KEY_F7 = GLFW_KEY_F7;
constexpr int KEY_F8 = GLFW_KEY_F8;
constexpr int KEY_F9 = GLFW_KEY_F9;
constexpr int KEY_F10 = GLFW_KEY_F10;
constexpr int KEY_F11 = GLFW_KEY_F11;
constexpr int KEY_F12 = GLFW_KEY_F12;
constexpr int KEY_F13 = GLFW_KEY_F13;
constexpr int KEY_F14 = GLFW_KEY_F14;
constexpr int KEY_F15 = GLFW_KEY_F15;
constexpr int KEY_F16 = GLFW_KEY_F16;
constexpr int KEY_F17 = GLFW_KEY_F17;
constexpr int KEY_F18 = GLFW_KEY_F18;
constexpr int KEY_F19 = GLFW_KEY_F19;
constexpr int KEY_F20 = GLFW_KEY_F20;
constexpr int KEY_F21 = GLFW_KEY_F21;
constexpr int KEY_F22 = GLFW_KEY_F22;
constexpr int KEY_F23 = GLFW_KEY_F23;
constexpr int KEY_F24 = GLFW_KEY_F24;
constexpr int KEY_F25 = GLFW_KEY_F25;
constexpr int KEY_KP_0 = GLFW_KEY_KP_0;
constexpr int KEY_KP_1 = GLFW_KEY_KP_1;
constexpr int KEY_KP_2 = GLFW_KEY_KP_2;
constexpr int KEY_KP_3 = GLFW_KEY_KP_3;
constexpr int KEY_KP_4 = GLFW_KEY_KP_4;
constexpr int KEY_KP_5 = GLFW_KEY_KP_5;
constexpr int KEY_KP_6 = GLFW_KEY_KP_6;
constexpr int KEY_KP_7 = GLFW_KEY_KP_7;
constexpr int KEY_KP_8 = GLFW_KEY_KP_8;
constexpr int KEY_KP_9 = GLFW_KEY_KP_9;
constexpr int KEY_KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
constexpr int KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
constexpr int KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
constexpr int KEY_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
constexpr int KEY_KP_ADD = GLFW_KEY_KP_ADD;
constexpr int KEY_KP_ENTER = GLFW_KEY_KP_ENTER;
constexpr int KEY_KP_EQUAL = GLFW_KEY_KP_EQUAL;
constexpr int KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
constexpr int KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
constexpr int KEY_LEFT_ALT = GLFW_KEY_LEFT_ALT;
constexpr int KEY_LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
constexpr int KEY_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
constexpr int KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
constexpr int KEY_RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
constexpr int KEY_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;
constexpr int KEY_MENU = GLFW_KEY_MENU;
constexpr int KEY_LAST = GLFW_KEY_LAST;
constexpr int KEY_MOD_SHIFT = GLFW_MOD_SHIFT;
constexpr int KEY_MOD_CONTROL = GLFW_MOD_CONTROL;
constexpr int KEY_MOD_ALT = GLFW_MOD_ALT;
constexpr int MOD_SUPER = GLFW_MOD_SUPER;
constexpr int MOD_CAPS_LOCK = GLFW_MOD_CAPS_LOCK;
constexpr int MOD_NUM_LOCK = GLFW_MOD_NUM_LOCK;
constexpr int NUM_PIXEL_CHANNELS = 4;
constexpr int NUM_CURSORS = 6;
constexpr int NUM_MOUSE_BUTTONS = 3;

enum Axis
{
    HORIZONTAL = 0,
    VERTICAL = 1
};
enum ButtonState
{
    UP = GLFW_RELEASE,
    DOWN = GLFW_PRESS
};
enum ShapeLoc
{
    UPPER_START = 0,
    UPPER_END = 1,
    LOWER_END = 2,
    LOWER_START = 3
};
enum Cursor
{
    ARROW = GLFW_ARROW_CURSOR,
    TEXT = GLFW_IBEAM_CURSOR,
    CROSS = GLFW_CROSSHAIR_CURSOR,
    HAND = GLFW_HAND_CURSOR,
    HRESIZE = GLFW_HRESIZE_CURSOR,
    VRESIZE = GLFW_VRESIZE_CURSOR,
};
enum MouseButton
{
    LEFT = GLFW_MOUSE_BUTTON_LEFT,
    RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
};

// - Channel type: float
// - Range: [0,1]
using RGBA = glm::vec<NUM_PIXEL_CHANNELS, float, glm::packed_highp>;
// - 2 float components x and y
// - Vec2(0,0) is the top-left pixel of the window
using Vec2 = glm::vec<2, float, glm::packed_highp>;
using TextureHandle = void*;
using FontHandle = void*;
struct MouseSnapshot
{
    bool is_down[NUM_MOUSE_BUTTONS] = { false };
    bool is_pressed[NUM_MOUSE_BUTTONS] = { false };
    bool is_released[NUM_MOUSE_BUTTONS] = { false };
    Vec2 pos;
    Vec2 delta;
    int  scroll = 0;
    int  curr_key_mods = 0;
    bool is_moving = false;
    bool any_down();
    bool any_pressed();
    bool any_released();
};
/*
    Snapshots are used to send Mouse and Keyboard data to observers.
    The point of sending Mouse and Keyboard states through callback functions
    instead of providing a randomly accessable getter is to discourage the use
    of mouse and keyboard states during the drawing stage when those states 
    are inaccurate.

    The client will probably still end up creating saved keyboard and mouse
    states, but they will then be clearly aware of the limitations.
*/
struct KeyboardSnapshot
{
    std::set<int> down_keys;
    std::set<int> repeated_keys;
    int pressed_key = KEY_UNKNOWN;
    int released_key = KEY_UNKNOWN;
    int repeated_key = KEY_UNKNOWN;
    unsigned char typed_char = 0;
    int curr_key_mods = 0;
};
struct WindowSnapshot
{
    Vec2 viewport_size = Vec2(0);
};
class Observer
{
public:
    void set_enabled(bool flag);
    bool enabled();
private:
    bool enabled_ = true;
};
class WindowObserver : public Observer
{
public:
    virtual void on_window_terminate(const WindowSnapshot&) = 0;
    virtual void on_window_resize(const WindowSnapshot&) = 0;
};
class MouseObserver : public Observer
{
public:
    virtual void on_mouse_move(const MouseSnapshot&) = 0;
    virtual void on_mouse_press(const MouseSnapshot&) = 0;
    virtual void on_mouse_release(const MouseSnapshot&) = 0;
    virtual void on_mouse_scroll(const MouseSnapshot&) = 0;
};
class KeyboardObserver : public Observer
{
public:
    virtual void on_key_repeat(const KeyboardSnapshot& keyboard) = 0;
    virtual void on_key_press(const KeyboardSnapshot& keyboard) = 0;
    virtual void on_key_release(const KeyboardSnapshot& keyboard) = 0;
    virtual void on_char_type(const KeyboardSnapshot& keyboard) = 0;
};
// - Returns a non-zero value if there was an issue during initialization.
//   width and height must be greater than 0
bool Init(const std::string& name, const Vec2& size);
bool AddWindowObserver(WindowObserver* ob);
bool RemoveWindowObserver(WindowObserver* ob);
bool AddMouseObserver(MouseObserver* ob);
bool RemoveMouseObserver(MouseObserver* ob);
bool AddKeyboardObserver(KeyboardObserver* ob);
bool RemoveKeyboardObserver(KeyboardObserver* ob);
// Returns nullptr before quickdraw::window::Init() is called
GLFWwindow* GetGLFWWindowHandle();
void SetWindowIcon(std::filesystem::path file);
// - Returns the time in seconds
double Time();
// - Returns the duration of time since the start of the last frame in seconds
double DeltaTime();
// - Returns the number of frames since the start of the program so that
//   0 is returned during the first frame.
// - A negative number may be returned that the previous frame number is
// always less than the current
int FrameNumber();
// - Returns the size of the drawing context
Vec2 ViewportSize();
void SetBackgroundColor(const RGBA& color);
// - Updates frame information and polls input events
void NewFrame();
void DrawFrame();
bool ShouldClose();
void Terminate();
void DrawRect(const Vec2& pos, const Vec2& size);
// - Set scale with SetTextScale
// - Set spacing with SetTextSpacing
void DrawText(const Vec2& pos, const std::string& Text);
// - Works best with high resolution curves.
// - points.size() cannot be less than 2
void DrawPath(const std::vector<Vec2>& points, float thickness, const Vec2& offset = Vec2(0));
// - When the image is drawn its pixels are multiplied by the current fill color.
// - For example, if fill = RGBA(0,0,0,0) the image is invisible,
//   if fill = RGBA(1,0,1,0.5) the image is half transparent and only the
//   red and blue channels are visible.
void DrawTexture(TextureHandle handle, const Vec2& pos, const Vec2& size);
// - Filetype must be supoprted by stb_image
std::pair<TextureHandle, Vec2> LoadTexture(std::filesystem::path file);
void EnableScissor(const Vec2& pos, const Vec2& size);
// - Disables the scissor test
void DisableScissor();
void SetFillColor(const RGBA& color, ShapeLoc loc);
void SetFillColor(const RGBA& color);
void SetOutlineColor(const RGBA& color, ShapeLoc loc);
void SetOutlineColor(const RGBA& color);
void SetRectRoundedMask(bool mask, ShapeLoc loc);
void SetRectRoundedMask(bool mask);
void SetRectRoundedSize(float size);
// Sets the thickness of the outline of the rectangle to be drawn
void SetOutlineThickness(float thickness);
// - Sets the scale of Text. Make sure to set the desired text scale before
//   calling TextSize(...) or MaxTextHeight()
void SetTextScale(float scale);
// - Returns the size of the bounding box of str if it were drawn on the screen.
// - Make sure to set the desired text scale before calling this method.
Vec2 TextSize(const std::string& str);
// - Make sure to set the desired text scale before calling this method.
float MaxTextHeight();
void SetCursor(Cursor cursor);
// - Shows/hides the cursor when inside the window
void SetCursorEnabled(bool flag);
// - Returns nullptr if font could not be loaded
// - Filetype must be supported by FreeType
FontHandle LoadFont(int resolution, std::filesystem::path path);
// - Must be an underlying pointer returned by LoadFont
// - Passing nullptr sets the default font to active
void SetActiveFont(FontHandle font);
namespace detail
{
enum QuadMode
{
    RECT_MODE = 0,
    TEXTURE_MODE = 1,
    PATH_MODE = 2
};
template<typename T>
struct BinaryStateSaver;
struct DynamicVertexAttribs;
struct StaticVertexAttribs;
struct Glyph;
struct Texture;
class Font;
void SetQuadMode(QuadMode mode);
void SetRectPosAndSize(const Vec2& pos, const Vec2& size);
void SetPathThickness(float thickness);
void DrawQuad();
Vec2 Normal(const Vec2& slope);
std::pair<TextureHandle, Vec2> LoadTexture(int width, int height, std::vector<GLubyte> rgba_bitmap);
void PackTextures();
void GLFWErrorCallback(int error, const char* description);
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorCallback(GLFWwindow* window, double x, double y);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessMouseEvents();
bool InitOpenGL();
void InitMouse();
void InitKeyboard();
void InitCursors();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CharCallback(GLFWwindow* window, unsigned int codepoint);
MouseSnapshot CopyMouseState();
KeyboardSnapshot CopyKeyboardState();
WindowSnapshot CopyWindowState();
} // namespace detail

#define QUICKDRAW_NOTIFY_OBSERVERS(arg_observer_type, arg_observers, arg_member_function) \
{\
std::set<arg_observer_type*> observers_this_notification = arg_observers; \
for (arg_observer_type* ob : observers_this_notification)\
{\
    ob->arg_member_function;\
}\
}
#define QUICKDRAW_ANY_MOUSE_BUTTON_TRUE(bool_array) bool_array[LEFT] || bool_array[MIDDLE] || bool_array[RIGHT]
// DEFINITIONS

namespace detail
{
std::istringstream FRAGMENT_SHADER_CODE(
R"~(

#version 450 compatibility
layout(std430, binding = 0) buffer Quads
{
float quad_array[];
};

uniform sampler2D texture_atlas;

in vec2 frag_pos;
in float frag_corner_mask;
in vec4 frag_fill_color;
in vec4 frag_outline_color;
in vec2 frag_uv;
flat in unsigned int frag_quad_index;

out vec4 color_out;

#define MODE_RECT 0
#define MODE_TEXTURE 1
#define MODE_PATH 2

bool in_bounds(float x, float mini, float maxi)
{
return x >= mini && x <= maxi;
}

float map(float value, float min1, float max1, float min2, float max2)
{
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

if (quad_mode() == MODE_TEXTURE)
{
	color *= texture(texture_atlas, frag_uv);
}
else  if (quad_mode() == MODE_PATH)
{
	// Pixel is in the outline
	if (path_thickness() * (1 - abs((frag_uv.y - 0.5) * 2)) < quad_outline_thickness())
	{
		color = frag_outline_color;
	}
}
else if (quad_mode() == MODE_RECT)
{
	vec2 local_frag_pos = frag_pos - rect_pos();
	vec2 local_vert_pos = round(frag_uv) * rect_size(); // Closest vertex to fragment

	if (round(frag_corner_mask) == 0)
	{
		// Pixel is in the outline
		vec2 frag_to_vert_dist = abs(local_vert_pos - local_frag_pos);
		if (frag_to_vert_dist.x < quad_outline_thickness() || frag_to_vert_dist.y < quad_outline_thickness())
		{
			color = frag_outline_color;
		}
	}
	else
	{
		if (in_bounds(local_frag_pos.x, rect_corner_size(), rect_size().x - rect_corner_size()) || in_bounds(local_frag_pos.y, rect_corner_size(), rect_size().y - rect_corner_size()))
		{
			// Pixel is in the outline
			vec2 frag_to_vert_dist = abs(local_vert_pos - local_frag_pos);
			if (frag_to_vert_dist.x < quad_outline_thickness() || frag_to_vert_dist.y < quad_outline_thickness())
			{
				color = frag_outline_color;
			}
		}

		else
		{
			vec2 origin = local_vert_pos - rect_corner_size() * (round(frag_uv) * 2 - 1);
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
}

color_out = color;
}
)~");


    std::istringstream VERTEX_SHADER_CODE(
        R"~(
#version 450 compatibility

// Dynamic attributes
layout(location = 0) in vec2 pos;
layout(location = 1) in float corner_mask;
layout(location = 2) in vec4 fill_color;
layout(location = 3) in vec4 outline_color;
layout(location = 4) in vec2 uv;

// Static attributes
layout(location = 5) in unsigned int quad_index;

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
gl_Position = vec4(ndc_pos.x, -ndc_pos.y,1,1);
}
)~");
constexpr size_t ATTRIBS_PER_QUAD = 7;
constexpr size_t VERTS_PER_QUAD = 4;
constexpr size_t ELEMS_PER_QUAD = 6;
struct Glyph
{
    TextureHandle texture = nullptr;
    Vec2 size = Vec2(0);
    Vec2 bearing = Vec2(0);
    float advance = 0;
};
struct Texture
{
    std::vector<GLubyte> bitmap = std::vector<GLubyte>(0);
    Vec2 uv_start = Vec2(0);
    Vec2 uv_end = Vec2(0);
    int width = 0;
    int height = 0;
};
class Font
{
public:
    static constexpr bool IsDisplayableChar(unsigned char c);
    Font(int resolution, std::filesystem::path path);
    // Returns this->end() if IsDisplayableChar(c) is false
    detail::Glyph* get(unsigned char c)
    {
        if (!IsDisplayableChar(c))
            return end();
        return begin() + (c - FIRST_VALID_CHAR);
    }
    detail::Glyph* begin()
    {
        return &glyphs_[0];
    }
    detail::Glyph* end()
    {
        return glyphs_ + NUM_VALID_CHARS;
    }
    bool ready_to_use()
    {
        return ready_to_use_;
    }
    float max_text_height()
    {
        return max_text_height_;
    }
    float space_glyph_width()
    {
        return space_glyph_width_;
    }
    float centering_offset()
    {
        return centering_offset_;
    }
private:
    static constexpr unsigned char FIRST_VALID_CHAR = 33;
    static constexpr unsigned char LAST_VALID_CHAR = 126;
    static constexpr int NUM_VALID_CHARS = 1 + LAST_VALID_CHAR - FIRST_VALID_CHAR;
    detail::Glyph glyphs_[NUM_VALID_CHARS];
    bool ready_to_use_ = false;
    float max_text_height_ = 0;
    float centering_offset_ = 0;
    float space_glyph_width_ = 0;
};
template<typename T>
class BinaryStateSaver
{

public:
    void new_state(T state)
    {
        previous_ = current_;
        current_ = state;
    }
    T previous() const
    {
        return previous_;
    }
    T current() const
    {
        return current_;
    }
private:
    T current_;
    T previous_;
};
struct DynamicVertexAttribs
{
    Vec2 pos = Vec2(0);
    Vec2 uv = Vec2(0);
    GLfloat corner_mask = 1;
    RGBA fill_color = RGBA(1, 0, 1, 1);
    RGBA outline_color = RGBA(1, 0, 1, 1);
};
struct StaticVertexAttribs
{
    GLuint quad_index;
    StaticVertexAttribs(GLuint t_quad_index) :quad_index(t_quad_index)
    {}
};
GLFWwindow* glfw_window_handle = nullptr;
GLuint texture_atlas_handle = 0;
GLuint transform_handle, shader_handle;
GLuint quad_vao, vertex_dynamic_attribs_vbo, vertex_static_attribs_vbo, vertex_ebo, quad_attribs_ssbo;
Font* active_font = nullptr;
Font* default_font = nullptr;
GLuint BASE_VERTEX_ELEMS[ELEMS_PER_QUAD]{ 2, 1, 0, 2, 0, 3 };
std::set<WindowObserver*> window_observers;
std::set<MouseObserver*> mouse_observers;
std::set<KeyboardObserver*> keyboard_observers;
std::list<Texture> atlas_textures;
std::list<Font> fonts;
GLFWcursor* cursor_ptrs[NUM_CURSORS];
std::vector<DynamicVertexAttribs> vertex_dynamic_attribs_to_draw;
std::vector<StaticVertexAttribs> vertex_static_attribs_to_draw;
std::vector<GLuint> vertex_elements_to_draw;
std::vector<float> quads_attribs_to_draw;
DynamicVertexAttribs curr_vertex_attribs[VERTS_PER_QUAD];
size_t most_quads_drawn = 0;
float curr_text_scale = 24;
float curr_quad_attribs[ATTRIBS_PER_QUAD];
RGBA background_color = RGBA(0);
Vec2 viewport_size(640, 480);
double last_time = 0;
double delta_time = 0;
int frame_number = 0;
bool scroll_update_received = false;
bool cursor_update_received = false;
bool mouse_button_update_received = false;
BinaryStateSaver<Vec2> mouse_pos_state;
BinaryStateSaver<int> mouse_button_states[NUM_MOUSE_BUTTONS];
BinaryStateSaver<int> mouse_scroll_state;
Vec2 mouse_delta;
int curr_key_pressed = KEY_UNKNOWN;
int curr_key_released = KEY_UNKNOWN;
std::set<int> curr_keys_down;
std::set<int> curr_keys_repeated;
int curr_key_repeated;
int curr_key_mods = 0;
unsigned char typed_char = 0;
bool atlas_updated = false;
bool InitOpenGL()
{
    const char* METHOD_ERROR_HEADER = "quickdraw::InitOpenGL ";

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << METHOD_ERROR_HEADER << "Failed to load OpenGL\n";
        return false;
    }

    std::filesystem::path vertex_shader_path = std::filesystem::current_path() / "resources" / "quad.vert";
    std::filesystem::path fragment_shader_path = std::filesystem::current_path() / "resources" / "quad.frag";
    std::string vert_code = VERTEX_SHADER_CODE.str(), frag_code = FRAGMENT_SHADER_CODE.str();
    const char* vertex_code_cstr = vert_code.c_str();
    const char* fragment_code_cstr = frag_code.c_str();
    GLint success_flag;
    const size_t LOG_SIZE = 4096;
    char log[LOG_SIZE];
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code_cstr, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success_flag);
    if (!success_flag)
    {
        glGetShaderInfoLog(vertex, LOG_SIZE, NULL, log);
        std::cout << METHOD_ERROR_HEADER << "Failed to compile vertex shader\n" << log << "\n";
        return false;
    }
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code_cstr, NULL);
    glCompileShader(fragment);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success_flag);
    if (!success_flag)
    {
        glGetShaderInfoLog(vertex, LOG_SIZE, NULL, log);
        std::cout << METHOD_ERROR_HEADER << "Failed to compile fragment shader\n" << log << "\n";
        return false;
    }
    shader_handle = glCreateProgram();
    glAttachShader(shader_handle, vertex);
    glAttachShader(shader_handle, fragment);
    glLinkProgram(shader_handle);
    glGetProgramiv(shader_handle, GL_LINK_STATUS, &success_flag);
    if (!success_flag)
    {
        glGetProgramInfoLog(shader_handle, LOG_SIZE, NULL, log);
        std::cout << METHOD_ERROR_HEADER << "Failed to link shader program\n" << log << '\n';
        return false;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(shader_handle);
    transform_handle = glGetUniformLocation(shader_handle, "transform");
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &vertex_dynamic_attribs_vbo);
    glGenBuffers(1, &vertex_static_attribs_vbo);
    glGenBuffers(1, &vertex_ebo);
    glGenBuffers(1, &quad_attribs_ssbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_dynamic_attribs_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_static_attribs_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_ebo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, quad_attribs_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, quad_attribs_ssbo);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_dynamic_attribs_vbo);
    GLuint vertex_pos_loc = glGetAttribLocation(shader_handle, "pos");
    glEnableVertexAttribArray(vertex_pos_loc);
    glVertexAttribPointer(vertex_pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::pos));
    GLuint vertex_corner_mask_loc = glGetAttribLocation(shader_handle, "corner_mask");
    glEnableVertexAttribArray(vertex_corner_mask_loc);
    glVertexAttribPointer(vertex_corner_mask_loc, 1, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::corner_mask));
    GLuint vertex_fill_color_loc = glGetAttribLocation(shader_handle, "fill_color");
    glEnableVertexAttribArray(vertex_fill_color_loc);
    glVertexAttribPointer(vertex_fill_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::fill_color));
    GLuint vertex_outline_color_loc = glGetAttribLocation(shader_handle, "outline_color");
    glEnableVertexAttribArray(vertex_outline_color_loc);
    glVertexAttribPointer(vertex_outline_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::outline_color));
    GLuint vertex_uv_loc = glGetAttribLocation(shader_handle, "uv");
    glEnableVertexAttribArray(vertex_uv_loc);
    glVertexAttribPointer(vertex_uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::uv));

    glBindBuffer(GL_ARRAY_BUFFER, vertex_static_attribs_vbo);
    GLuint vertex_quad_index_loc = glGetAttribLocation(shader_handle, "quad_index");
    glEnableVertexAttribArray(vertex_quad_index_loc);
    glVertexAttribIPointer(vertex_quad_index_loc, 1, GL_UNSIGNED_INT, sizeof(StaticVertexAttribs), (void*)offsetof(StaticVertexAttribs, StaticVertexAttribs::quad_index));

    glGenTextures(1, &texture_atlas_handle);

    return true;
}
void InitMouse()
{
    for (auto& st : mouse_button_states)
    {
        st.new_state(UP);
        st.new_state(UP);
    }

    for (int k = ARROW; k <= VRESIZE; k++)
    {
        cursor_ptrs[k - ARROW] = glfwCreateStandardCursor(k);

        if (cursor_ptrs[k - ARROW] == NULL)
            std::cout << "Error creating cursor " << k << "\n";
    }

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(glfw_window_handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetScrollCallback(glfw_window_handle, ScrollCallback);
    glfwSetCursorPosCallback(glfw_window_handle, CursorCallback);
    glfwSetMouseButtonCallback(glfw_window_handle, MouseButtonCallback);
}
void InitKeyboard()
{
    glfwSetKeyCallback(glfw_window_handle, KeyCallback);
    glfwSetCharCallback(glfw_window_handle, CharCallback);
}
void InitCursors()
{
    for (int k = 0; k < NUM_CURSORS; k++)
    {
        cursor_ptrs[k] = glfwCreateStandardCursor(k + Cursor::ARROW);
    }
}
void KeyCallback(GLFWwindow* window_ptr, int key, int scancode, int action, int mods)
{
    curr_key_mods = mods;

    // We don't care about modifier keys. We'll leave that to the mods parameter.
    if (key >= KEY_LEFT_SHIFT && key <= KEY_RIGHT_SUPER)
        return;

    curr_key_pressed = KEY_UNKNOWN;
    curr_key_released = KEY_UNKNOWN;
    curr_key_repeated = KEY_UNKNOWN;
    if (action == GLFW_PRESS)
    {
        curr_key_pressed = key;
        curr_keys_down.insert(key);
        KeyboardSnapshot ks = CopyKeyboardState();
        QUICKDRAW_NOTIFY_OBSERVERS(KeyboardObserver, keyboard_observers, on_key_press(ks));
    }
    else if (action == GLFW_RELEASE)
    {
        curr_key_released = key;
        curr_keys_down.erase(key);
        curr_keys_repeated.erase(key);
        KeyboardSnapshot ks = CopyKeyboardState();
        QUICKDRAW_NOTIFY_OBSERVERS(KeyboardObserver, keyboard_observers, on_key_release(ks));
    }
    else if (action == GLFW_REPEAT)
    {
        curr_keys_repeated.insert(key);
        curr_key_repeated = key;
        KeyboardSnapshot ks = CopyKeyboardState();
        QUICKDRAW_NOTIFY_OBSERVERS(KeyboardObserver, keyboard_observers, on_key_repeat(ks));
    }
}
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    typed_char = codepoint;
    KeyboardSnapshot ks = CopyKeyboardState();
    QUICKDRAW_NOTIFY_OBSERVERS(KeyboardObserver, keyboard_observers, on_char_type(ks));
}
void GLFWErrorCallback(int error, const char* description)
{
    fprintf(stderr, "quickdraw::window GLFW Error %d: %s\n", error, description);
}
void WindowSizeCallback(GLFWwindow* window_ptr, int width, int height)
{
    viewport_size = Vec2(width, height);
    glViewport(0, 0, width, height);

    glUseProgram(shader_handle);
    Vec2 transform = Vec2(1.0f / (viewport_size - 1.0f));
    glUniform2fv(transform_handle, 1, &transform[0]);

    QUICKDRAW_NOTIFY_OBSERVERS(WindowObserver, window_observers, on_window_resize(CopyWindowState()));
}
void MouseButtonCallback(GLFWwindow* window_ptr, int button, int action, int mods)
{
    mouse_button_states[button].new_state(action);
    curr_key_mods = mods;

    MouseSnapshot mouse_snapshot = CopyMouseState();
    if (mouse_button_states[MouseButton(button)].current() != mouse_button_states[MouseButton(button)].previous())
    {
        if (mouse_button_states[MouseButton(button)].current() == DOWN)
        {
            QUICKDRAW_NOTIFY_OBSERVERS(MouseObserver, mouse_observers, on_mouse_press(mouse_snapshot));
        }
        else
        {
            QUICKDRAW_NOTIFY_OBSERVERS(MouseObserver, mouse_observers, on_mouse_release(mouse_snapshot));
        }
    }

    mouse_button_update_received = true;
}
void CursorCallback(GLFWwindow* window_ptr, double x, double y)
{
    double mx, my;
    glfwGetCursorPos(window_ptr, &mx, &my);
    mouse_pos_state.new_state(Vec2(mx, my));
    mouse_delta = mouse_pos_state.current() - mouse_pos_state.previous();
    MouseSnapshot ms = CopyMouseState();
    QUICKDRAW_NOTIFY_OBSERVERS(MouseObserver, mouse_observers, on_mouse_move(ms));
    cursor_update_received = true;
}
void ScrollCallback(GLFWwindow* window_ptr, double xoffset, double yoffset)
{
    scroll_update_received = true;
    mouse_scroll_state.new_state((int)yoffset);
    MouseSnapshot ms = CopyMouseState();
    QUICKDRAW_NOTIFY_OBSERVERS(MouseObserver, mouse_observers, on_mouse_scroll(ms));
}
void ProcessMouseEvents()
{
    // Make sure to set scroll state to 0 if we aren't scrolling
    if (!scroll_update_received && mouse_scroll_state.current() != 0)
    {
        ScrollCallback(glfw_window_handle, 0, 0);
    }

    // Make sure to set delta to 0 if mouse hasn't moved.
    if (!cursor_update_received && mouse_delta != Vec2(0))
    {
        mouse_delta = Vec2(0);
    }

    if (!mouse_button_update_received)
    {
        for (auto& bs : mouse_button_states)
        {
            if (bs.current() != bs.previous())
                bs.new_state(bs.current());
        }
    }
    scroll_update_received = false;
    cursor_update_received = false;
    mouse_button_update_received = false;
}
MouseSnapshot CopyMouseState()
{
    MouseSnapshot mouse;

    for (int b = 0; b < NUM_MOUSE_BUTTONS; b++)
    {
        mouse.is_down[b] = mouse_button_states[b].current() == DOWN;
    }
    for (int b = 0; b < NUM_MOUSE_BUTTONS; b++)
    {
        mouse.is_pressed[b] = mouse_button_states[b].current() == DOWN && mouse_button_states[b].previous() == UP;
    }
    for (int b = 0; b < NUM_MOUSE_BUTTONS; b++)
    {
        mouse.is_down[b] = mouse_button_states[b].current() == UP && mouse_button_states[b].previous() == DOWN;
    }
    mouse.is_moving = mouse_delta != Vec2(0);
    mouse.delta = mouse_delta;
    mouse.pos = mouse_pos_state.current();
    mouse.scroll = mouse_scroll_state.current();
    mouse.curr_key_mods = curr_key_mods;

    return mouse;
}
KeyboardSnapshot CopyKeyboardState()
{
    KeyboardSnapshot keyboard;
    keyboard.curr_key_mods = curr_key_mods;
    keyboard.typed_char = typed_char;
    keyboard.down_keys = curr_keys_down;
    keyboard.repeated_keys = curr_keys_repeated;
    keyboard.pressed_key = curr_key_pressed;
    keyboard.released_key = curr_key_released;
    keyboard.repeated_key = curr_key_repeated;
    return keyboard;
}
WindowSnapshot CopyWindowState()
{
    WindowSnapshot window;
    window.viewport_size = viewport_size;
    return window;
};
void SetQuadMode(QuadMode mode)
{
    curr_quad_attribs[4] = mode;

    if (mode == RECT_MODE || mode == PATH_MODE)
    {
        curr_vertex_attribs[UPPER_START].uv = Vec2(0, 0);
        curr_vertex_attribs[UPPER_END].uv = Vec2(1, 0);
        curr_vertex_attribs[LOWER_END].uv = Vec2(1, 1);
        curr_vertex_attribs[LOWER_START].uv = Vec2(0, 1);
    }
}
void SetRectPosAndSize(const Vec2& pos, const Vec2& size)
{
    curr_quad_attribs[0] = pos.x;
    curr_quad_attribs[1] = pos.y;
    curr_quad_attribs[2] = size.x;
    curr_quad_attribs[3] = size.y;

    curr_vertex_attribs[UPPER_START].pos = pos + size * Vec2(0, 0);
    curr_vertex_attribs[UPPER_END].pos = pos + size * Vec2(1, 0);
    curr_vertex_attribs[LOWER_END].pos = pos + size * Vec2(1, 1);
    curr_vertex_attribs[LOWER_START].pos = pos + size * Vec2(0, 1);
}
void SetPathThickness(float thickness)
{
    curr_quad_attribs[2] = thickness;
}
void DrawQuad()
{
    vertex_dynamic_attribs_to_draw.emplace_back(curr_vertex_attribs[UPPER_START]);
    vertex_dynamic_attribs_to_draw.emplace_back(curr_vertex_attribs[UPPER_END]);
    vertex_dynamic_attribs_to_draw.emplace_back(curr_vertex_attribs[LOWER_END]);
    vertex_dynamic_attribs_to_draw.emplace_back(curr_vertex_attribs[LOWER_START]);

    for (float& attrib : curr_quad_attribs)
        quads_attribs_to_draw.emplace_back(attrib);
}
Vec2 Normal(const Vec2& slope)
{
    return Vec2(slope.y, -slope.x) / glm::length(slope);
}
std::pair<TextureHandle, Vec2> LoadTexture(int width, int height, std::vector<GLubyte> rgba_bitmap)
{
    if (width < 1 || height < 1)
    {
        std::cout << "quickdraw::LoadTexture Failed to load texture because width or height was less than 1\n";
        return std::pair(nullptr, Vec2(0));
    }
    Texture& new_texture = atlas_textures.emplace_back();
    atlas_updated = true;
    new_texture.width = width;
    new_texture.height = height;
    new_texture.bitmap = rgba_bitmap;
    return std::pair(&new_texture, Vec2(width, height));
}
void PackTextures()
{
    const int rect_spacing = 2;
    size_t sum_widths = 0;
    size_t sum_heights = 0;
    int max_height = 0;
    for (Texture& texture : atlas_textures)
    {
        sum_widths += (size_t)texture.width + rect_spacing;
        sum_heights += (size_t)texture.height + rect_spacing;
        max_height = std::max(texture.height + rect_spacing, max_height);
    }

    size_t packing_area = sum_widths * max_height;
    double aspect_ratio = double(sum_widths) / sum_heights;
    int packing_width = sqrt(packing_area * aspect_ratio);
    int packing_height = packing_width / aspect_ratio;
    stbrp_context packing_context = {};
    std::vector<stbrp_node> packing_nodes(packing_width); // Good practice according to stb_rect_pack documentation
    std::vector<stbrp_rect> packing_rects(atlas_textures.size());

    int rect_index = 0;
    for (Texture& texture : atlas_textures)
    {
        packing_rects[rect_index].w = texture.width + rect_spacing;
        packing_rects[rect_index].h = texture.height + rect_spacing;
        ++rect_index;
    }
    stbrp_init_target(&packing_context, packing_width, packing_height, packing_nodes.data(), packing_nodes.size());
    stbrp_setup_allow_out_of_mem(&packing_context, 1);
    stbrp_pack_rects(&packing_context, packing_rects.data(), packing_rects.size());

    int min_atlas_width = 0;
    int min_atlas_height = 0;
    for (auto& rect : packing_rects)
    {
        min_atlas_width = std::max(rect.x + rect.w, min_atlas_width);
        min_atlas_height = std::max(rect.y + rect.h, min_atlas_height);
    }

    glBindTexture(GL_TEXTURE_2D, texture_atlas_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, min_atlas_width, min_atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    rect_index = 0;
    for (Texture& texture : atlas_textures)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, packing_rects[rect_index].x + rect_spacing, packing_rects[rect_index].y + rect_spacing, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.bitmap.data());

        // Set UV coordinates for textures
        texture.uv_start = Vec2((packing_rects[rect_index].x + rect_spacing) / (double)min_atlas_width, (packing_rects[rect_index].y + rect_spacing) / (double)min_atlas_height);
        texture.uv_end = Vec2((packing_rects[rect_index].x + rect_spacing + texture.width) / (double)min_atlas_width, (packing_rects[rect_index].y + rect_spacing + texture.height) / (double)min_atlas_height);

        ++rect_index;
    }
}
constexpr bool Font::IsDisplayableChar(unsigned char c)
{
    using namespace detail;
    return c >= FIRST_VALID_CHAR && c <= LAST_VALID_CHAR;
}
Font::Font(int resolution, std::filesystem::path path)
{
    using namespace detail;
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "quickdraw::FontAtlas::FontAtlas Failed to initalize FreeType\n";
        return;
    }
    std::string path_str = path.generic_string();
    FT_Face face;
    if (FT_New_Face(ft, path_str.c_str(), 0, &face))
    {
        std::cout << "quickdraw::Font::Font Failed to load " << path << '\n';
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, resolution);

    std::vector<GLubyte> glyph_bitmaps[NUM_VALID_CHARS];
    int min_atlas_area = 0;
    // width / rows
    float max_dist_above_baseline = 0;
    float max_dist_below_baseline = 0;
    // Load displayable characters
    float ADVANCE_FAC = 1.0f / float(1 << 6);
    for (int k = 0; k < NUM_VALID_CHARS; k++)
    {
        Glyph& glyph = glyphs_[k];
        if (FT_Load_Char(face, k + FIRST_VALID_CHAR, FT_LOAD_RENDER))
        {
            std::cout << "quickdraw::FontAtlas::FontAtlas Failed to load \'" << (char)(k + FIRST_VALID_CHAR) << "\'\n";
            return;
        }
        // Note we set up the glyph uv's later.
        glyph.size = Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        glyph.bearing = Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        glyph.advance = face->glyph->advance.x * ADVANCE_FAC; // If text quads are positioned incorrectly its probably this
        min_atlas_area += glyph.size.x * glyph.size.y;
        space_glyph_width_ += glyph.size.x; // will be average of displayable character widths
        centering_offset_ += -glyph.bearing.y + glyph.size.y * 0.5f;
        max_dist_above_baseline = std::max(max_dist_above_baseline, glyph.bearing.y);
        max_dist_below_baseline = std::max(max_dist_below_baseline, glyph.size.y - glyph.bearing.y);

        std::vector<GLubyte> bitmap(glyph.size.x * NUM_PIXEL_CHANNELS * glyph.size.y);
        for (int x = 0; x < glyph.size.x; x++)
        {
            for (int y = 0; y < glyph.size.y; y++)
            {
                bitmap[(x + y * glyph.size.x) * NUM_PIXEL_CHANNELS + 0] = 255;
                bitmap[(x + y * glyph.size.x) * NUM_PIXEL_CHANNELS + 1] = 255;
                bitmap[(x + y * glyph.size.x) * NUM_PIXEL_CHANNELS + 2] = 255;
                bitmap[(x + y * glyph.size.x) * NUM_PIXEL_CHANNELS + 3] = face->glyph->bitmap.buffer[x + y * (int)glyph.size.x];
            }
        }

        glyph.texture = LoadTexture(glyph.size.x, glyph.size.y, bitmap).first;
        if (glyph.texture == nullptr)
        {
            std::cout << "quickdraw::FontAtlas::FontAtlas Failed to load \'" << path << "\'\n";
            return;
        }
    }

    // Set fields
    space_glyph_width_ /= NUM_VALID_CHARS; // is the average of displayable character widths
    centering_offset_ /= NUM_VALID_CHARS;
    max_text_height_ = max_dist_above_baseline + max_dist_below_baseline;

    // Apply import scale
    float font_import_scale = 1.0f / resolution;
    for (Glyph& g : glyphs_)
    {
        g.size *= font_import_scale;
        g.bearing *= font_import_scale;
        g.advance *= font_import_scale;
    }
    space_glyph_width_ *= font_import_scale;
    max_text_height_ *= font_import_scale;

    // Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    ready_to_use_ = true;
}
} // namespace detail
bool Init(const std::string& name, const Vec2& size)
{
    using namespace detail;
    if ((int)size.x <= 0 || (int)size.y <= 0)
    {
        std::cout << "quickdraw::Init Failed to initialize because width or height was less than 1\n";
        return false;
    }

    viewport_size = size;

    glfwSetErrorCallback(GLFWErrorCallback);
    if (!glfwInit())
    {
        std::cout << "Could not initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_DECORATED, true);

    glfw_window_handle = glfwCreateWindow((int)viewport_size.x, (int)viewport_size.y, name.c_str(), NULL, NULL);
    if (glfw_window_handle == NULL)
    {
        std::cout << "quickdraw::window Failed to create GLFW window\n";
        return false;
    }
    glfwMakeContextCurrent(glfw_window_handle);
    glfwSetWindowSizeCallback(glfw_window_handle, WindowSizeCallback);
    InitCursors();
    InitMouse();
    InitKeyboard();

    InitOpenGL();

    default_font = (Font*)LoadFont(48, "C:\\Windows\\Fonts\\segoeuil.ttf");
    SetActiveFont(default_font);

    WindowSizeCallback(glfw_window_handle, (int)viewport_size.x, (int)viewport_size.y);
    last_time = glfwGetTime();

    return true;
}
void NewFrame()
{
    using namespace detail;
    frame_number++;
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    glfwPollEvents();
    ProcessMouseEvents();

    if (atlas_updated)
    {
        PackTextures();
        atlas_updated = false;
    }
}
void DrawFrame()
{
    using namespace detail;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_atlas_handle);
    glBindVertexArray(quad_vao);
    glUseProgram(shader_handle);
    const size_t num_verts_this_frame = vertex_dynamic_attribs_to_draw.size();
    const size_t num_quads_this_frame = num_verts_this_frame / VERTS_PER_QUAD;
    const bool more_vertex_memory_needed = num_quads_this_frame > most_quads_drawn;
    most_quads_drawn = std::max(num_quads_this_frame, most_quads_drawn);

    // Update static vertex attributes and element buffer
    if (more_vertex_memory_needed)
    {
        GLuint quad_index = vertex_static_attribs_to_draw.size() / VERTS_PER_QUAD;
        while (vertex_elements_to_draw.size() / ELEMS_PER_QUAD < num_quads_this_frame)
        {
            for (int k = 0; k < 6; k++)
            {
                vertex_elements_to_draw.push_back(BASE_VERTEX_ELEMS[k] + quad_index * VERTS_PER_QUAD);
            }

            vertex_static_attribs_to_draw.emplace_back(quad_index * ATTRIBS_PER_QUAD);
            vertex_static_attribs_to_draw.emplace_back(quad_index * ATTRIBS_PER_QUAD);
            vertex_static_attribs_to_draw.emplace_back(quad_index * ATTRIBS_PER_QUAD);
            vertex_static_attribs_to_draw.emplace_back(quad_index * ATTRIBS_PER_QUAD);

            quad_index++;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_elements_to_draw.size(), vertex_elements_to_draw.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_static_attribs_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(StaticVertexAttribs) * vertex_static_attribs_to_draw.size(), vertex_static_attribs_to_draw.data(), GL_STATIC_DRAW);
    }
    // Update dynamic vertex attributes
    if (more_vertex_memory_needed)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_dynamic_attribs_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DynamicVertexAttribs) * vertex_dynamic_attribs_to_draw.size(), vertex_dynamic_attribs_to_draw.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, quad_attribs_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * quads_attribs_to_draw.size(), quads_attribs_to_draw.data(), GL_DYNAMIC_COPY);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_dynamic_attribs_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DynamicVertexAttribs) * vertex_dynamic_attribs_to_draw.size(), vertex_dynamic_attribs_to_draw.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, quad_attribs_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * quads_attribs_to_draw.size(), quads_attribs_to_draw.data());
    }

    glDrawElements(GL_TRIANGLES, num_quads_this_frame * ELEMS_PER_QUAD, GL_UNSIGNED_INT, 0);

    quads_attribs_to_draw.clear();
    vertex_dynamic_attribs_to_draw.clear();
    glfwSwapBuffers(glfw_window_handle);
    glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
    glClearDepth(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
bool ShouldClose()
{
    using namespace detail;
    return glfwWindowShouldClose(glfw_window_handle);
}
void Terminate()
{
    using namespace detail;
    glfwTerminate();
    QUICKDRAW_NOTIFY_OBSERVERS(WindowObserver, window_observers, on_window_terminate(CopyWindowState()));
}
void SetBackgroundColor(const RGBA& color)
{
    using namespace detail;
    background_color = color;
}
void SetWindowIcon(std::filesystem::path file)
{
    using namespace detail;
    std::string file_str = file.generic_string();
    GLFWimage image = {};
    image.pixels = stbi_load(file_str.c_str(), &image.width, &image.height, nullptr, 4);
    glfwSetWindowIcon(glfw_window_handle, 1, &image);
    stbi_image_free(image.pixels);
}
GLFWwindow* GetGLFWWindowHandle()
{
    using namespace detail;
    if (glfw_window_handle == nullptr)
    {
        std::cout << "quickdraw::GetGLFWWindowHandle Failed to return handle because quickdraw::Init() has not been called\n";
    }
    return glfw_window_handle;
}
double Time()
{
    using namespace detail;
    return glfwGetTime();
}
double DeltaTime()
{
    using namespace detail;
    return delta_time;
}
Vec2 ViewportSize()
{
    using namespace detail;
    return viewport_size;
}
int FrameNumber()
{
    using namespace detail;
    return frame_number;
}
std::pair<TextureHandle, Vec2> LoadTexture(std::filesystem::path file)
{
    using namespace detail;
    int width, height;
    auto file_str = file.string();
    GLubyte* stbi_pixel_buffer = stbi_load(file_str.c_str(), &width, &height, nullptr, NUM_PIXEL_CHANNELS);
    if (stbi_pixel_buffer == NULL)
    {
        return std::pair(nullptr, Vec2(0));
    }
    std::vector<GLubyte> my_pixel_buffer((size_t)width * height * NUM_PIXEL_CHANNELS);
    for (int k = 0; k < my_pixel_buffer.size(); k++)
    {
        my_pixel_buffer[k] = stbi_pixel_buffer[k];
    }
    stbi_image_free(stbi_pixel_buffer);
    return LoadTexture(width, height, my_pixel_buffer);
}
void UnloadTexture(TextureHandle texture_to_remove)
{
    using namespace detail;
    atlas_textures.remove_if([&](const Texture& texture)->bool { return &texture == texture_to_remove; });
    atlas_updated = true;
}
void DrawRect(const Vec2& pos, const Vec2& size)
{
    using namespace detail;
    SetQuadMode(RECT_MODE);
    SetRectPosAndSize(pos, size);
    DrawQuad();
}
void DrawText(const Vec2& pos, const std::string& text)
{
    using namespace detail;
    DynamicVertexAttribs saved_vertex_attribs[4];
    for (int k = 0; k < 4; k++)
    {
        saved_vertex_attribs[k] = curr_vertex_attribs[k];
    }
    float text_width = TextSize(text).x;
    float x_cursor = 0;
    for (char c : text)
    {
        if (!Font::IsDisplayableChar(c))
        {
            if (c == ' ')
            {
                x_cursor += active_font->space_glyph_width();
                continue;
            }

            std::cout << "quickdraw::DrawText Failed to draw text because 'text' argument has a non-displayable character\n";
            return;
        }
        Glyph& glyph = *active_font->get(c);
        Vec2 glyph_draw_pos = pos + curr_text_scale * Vec2(x_cursor + glyph.bearing.x, -glyph.bearing.y + active_font->centering_offset());
        Vec2 glyph_draw_size = curr_text_scale * glyph.size;
        curr_vertex_attribs[UPPER_START].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, (glyph_draw_pos.x - pos.x) / text_width);
        curr_vertex_attribs[UPPER_END].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, (glyph_draw_pos.x + glyph_draw_size.x - pos.x) / text_width);
        curr_vertex_attribs[LOWER_START].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, (glyph_draw_pos.x - pos.x) / text_width);
        curr_vertex_attribs[LOWER_END].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, (glyph_draw_pos.x + glyph_draw_size.x - pos.x) / text_width);
        DrawTexture(glyph.texture, glyph_draw_pos, glyph_draw_size);

        // Advance the horizontal cursor to the drawing position of the next
        // character
        x_cursor += glyph.advance;
    }

    for (int k = 0; k < 4; k++)
    {
        curr_vertex_attribs[k] = saved_vertex_attribs[k];
    }
}
void DrawTexture(TextureHandle handle, const Vec2& pos, const Vec2& size)
{
    using namespace detail;
    SetQuadMode(TEXTURE_MODE);

    curr_vertex_attribs[UPPER_START].pos = pos;
    curr_vertex_attribs[UPPER_END].pos = Vec2(pos.x + size.x, pos.y);
    curr_vertex_attribs[LOWER_END].pos = pos + size;
    curr_vertex_attribs[LOWER_START].pos = Vec2(pos.x, pos.y + size.y);

    Texture& texture = *(Texture*)handle;
    curr_vertex_attribs[UPPER_START].uv = texture.uv_start;
    curr_vertex_attribs[UPPER_END].uv = Vec2(texture.uv_end.x, texture.uv_start.y);
    curr_vertex_attribs[LOWER_END].uv = texture.uv_end;
    curr_vertex_attribs[LOWER_START].uv = Vec2(texture.uv_start.x, texture.uv_end.y);

    DrawQuad();
}
void DrawPath(const std::vector<Vec2>& points, float thickness, const Vec2& offset)
{
    using namespace detail;
    if (points.size() < 2)
    {
        std::cout << "quickdraw::window Cannot draw a path with less than 2 points\n";
        return;
    }

    // This method draws quads as line segments along a path

    curr_quad_attribs[2] = thickness;
    SetQuadMode(PATH_MODE);
    DynamicVertexAttribs saved_vertex_attribs[4];
    for (int k = 0; k < 4; k++)
    {
        saved_vertex_attribs[k] = curr_vertex_attribs[k];
    }

    std::vector<float> length_at_point;
    float path_length = 0;

    // Get the length of the curve so far at each point
    // so that we can properly interpolate the fill/outline colors.
    length_at_point.reserve(points.size());
    length_at_point.push_back(0);
    for (size_t k = 0; k < points.size() - 1; k++)
    {
        float segment_length = glm::distance(points[k], points[k + 1]);
        path_length += segment_length;
        length_at_point.push_back(path_length);
    }


    // Calculate the normal of the previous, current, and next line
    Vec2 line_normal = Normal(points[1] - points[0]);
    Vec2 next_normal = line_normal;

    // If there are more than 2 points, use the next line's normal
    if (points.size() > 2)
        next_normal = Normal(points[2] - points[1]);

    // Precomputed value that when multiplied by the end ray gives the average
    // between the current normal and the next normal and sets the length of end
    // ray to the thickness of the line
    const float end_edge_slope_coeff = 0.5f * thickness;

    // The slopes of the edges connecting this quad to the previous and next quad
    Vec2 start_edge_slope = -line_normal * thickness;

    // Average the line's normal with the next line's normal to get the slope of
    // the edge connecting the line with the next line
    Vec2 end_edge_slope = -(line_normal + next_normal) * end_edge_slope_coeff;

    // Set edge data
    curr_vertex_attribs[UPPER_START].pos = start_edge_slope + points[0] + offset;
    curr_vertex_attribs[UPPER_START].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, length_at_point[0] / path_length);
    curr_vertex_attribs[UPPER_START].outline_color = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[0] / path_length);

    curr_vertex_attribs[UPPER_END].pos = end_edge_slope + points[1] + offset;
    curr_vertex_attribs[UPPER_END].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, length_at_point[1] / path_length);
    curr_vertex_attribs[UPPER_END].outline_color = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[1] / path_length);

    curr_vertex_attribs[LOWER_END].pos = -end_edge_slope + points[1] + offset;
    curr_vertex_attribs[LOWER_END].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, length_at_point[1] / path_length);
    curr_vertex_attribs[LOWER_END].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[1] / path_length);

    curr_vertex_attribs[LOWER_START].pos = -start_edge_slope + points[0] + offset;
    curr_vertex_attribs[LOWER_START].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, length_at_point[0] / path_length);
    curr_vertex_attribs[LOWER_START].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[0] / path_length);

    // draw the quad
    DrawQuad();

    if (points.size() > 2)
    {
        // Draws each line except the first and last one
        size_t k = 1;
        while (k < points.size() - 2)
        {
            // We get the start edge data from the previous end edge
            curr_vertex_attribs[UPPER_START] = curr_vertex_attribs[UPPER_END];
            curr_vertex_attribs[LOWER_START] = curr_vertex_attribs[LOWER_END];

            // Use the next normal from the last line as the normal for the current
            // line
            line_normal = next_normal;

            // Set the next normal to be the normal of the line after the current line
            next_normal = Normal(points[k + 2] - points[k + 1]);

            // Find the slope of the end edge
            end_edge_slope = -(line_normal + next_normal) * end_edge_slope_coeff;

            // Set the end edge data
            curr_vertex_attribs[UPPER_END].pos = end_edge_slope + points[k + 1] + offset;
            curr_vertex_attribs[UPPER_END].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, length_at_point[k + 1] / path_length);
            curr_vertex_attribs[UPPER_END].outline_color = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[k + 1] / path_length);

            curr_vertex_attribs[LOWER_END].pos = -end_edge_slope + points[k + 1] + offset;
            curr_vertex_attribs[LOWER_END].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, length_at_point[k + 1] / path_length);
            curr_vertex_attribs[LOWER_END].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[k + 1] / path_length);

            DrawQuad();
            k++;
        }

        // draw the last line
        end_edge_slope = -next_normal * thickness;
        curr_vertex_attribs[UPPER_START] = curr_vertex_attribs[UPPER_END];
        curr_vertex_attribs[LOWER_START] = curr_vertex_attribs[LOWER_END];
        curr_vertex_attribs[UPPER_END].pos = end_edge_slope + points[k + 1] + offset;
        curr_vertex_attribs[UPPER_END].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, length_at_point[k + 1] / path_length);
        curr_vertex_attribs[UPPER_END].outline_color = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[k + 1] / path_length);
        curr_vertex_attribs[LOWER_END].pos = -end_edge_slope + points[k + 1] + offset;
        curr_vertex_attribs[LOWER_END].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, length_at_point[k + 1] / path_length);
        curr_vertex_attribs[LOWER_END].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[k + 1] / path_length);

        DrawQuad();
    }
    for (int k = 0; k < 4; k++)
    {
        curr_vertex_attribs[k] = saved_vertex_attribs[k];
    }
}
void SetFillColor(const RGBA& color, ShapeLoc loc)
{
    using namespace detail;
    curr_vertex_attribs[loc].fill_color = color;
}
void SetFillColor(const RGBA& color)
{
    using namespace detail;
    SetFillColor(color, UPPER_START);
    SetFillColor(color, UPPER_END);
    SetFillColor(color, LOWER_END);
    SetFillColor(color, LOWER_START);
}
void SetOutlineColor(const RGBA& color, ShapeLoc loc)
{
    using namespace detail;
    curr_vertex_attribs[loc].outline_color = color;
}
void SetOutlineColor(const RGBA& color)
{
    using namespace detail;
    SetOutlineColor(color, UPPER_START);
    SetOutlineColor(color, UPPER_END);
    SetOutlineColor(color, LOWER_END);
    SetOutlineColor(color, LOWER_START);
}
void SetRectRoundedMask(bool mask, ShapeLoc loc)
{
    using namespace detail;
    curr_vertex_attribs[loc].corner_mask = mask;
}
void SetRectRoundedMask(bool mask)
{
    using namespace detail;
    SetRectRoundedMask(mask, UPPER_START);
    SetRectRoundedMask(mask, UPPER_END);
    SetRectRoundedMask(mask, LOWER_END);
    SetRectRoundedMask(mask, LOWER_START);
}
void SetOutlineThickness(float thickness)
{
    using namespace detail;
    curr_quad_attribs[5] = thickness;
}
void SetRectRoundedSize(float size)
{
    using namespace detail;
    curr_quad_attribs[6] = size;
}
Vec2 TextSize(const std::string& str)
{
    using namespace detail;
    if (str.length() == 0)
        return Vec2(0);

    float width = 0;
    for (const char& c : str)
    {
        if (c == ' ')
        {
            width += active_font->space_glyph_width();
        }
        else if (!Font::IsDisplayableChar(c))
        {
            std::cout << "quickdraw::TextSize Failed to get text width because 'text' argument has a character with no width\n";
            return Vec2(0);
        }
        else
        {
            width += active_font->get(c)->advance;
        }
    }
    return curr_text_scale * Vec2(width, active_font->max_text_height());
}
float MaxTextHeight()
{
    using namespace detail;
    return active_font->max_text_height() * curr_text_scale;
}
void SetTextScale(float s)
{
    using namespace detail;
    curr_text_scale = s;
}
void EnableScissor(const Vec2& pos, const Vec2& size)
{
    using namespace detail;
    glEnable(GL_SCISSOR_TEST);
    glScissor((GLint)pos.x, (GLint)(viewport_size.y - pos.y - size.y), (GLint)size.x, (GLint)size.y);
}
void DisableScissor()
{
    using namespace detail;
    glDisable(GL_SCISSOR_TEST);
}
bool AddWindowObserver(WindowObserver* ob)
{
    using namespace detail;
    return window_observers.insert(ob).second;
}
bool RemoveWindowObserver(WindowObserver* ob)
{
    using namespace detail;
    return window_observers.erase(ob);
}
bool AddMouseObserver(MouseObserver* ob)
{
    using namespace detail;
    return mouse_observers.insert(ob).second;
}
bool RemoveMouseObserver(MouseObserver* ob)
{
    using namespace detail;
    return mouse_observers.erase(ob);
}
bool AddKeyboardObserver(KeyboardObserver* ob)
{
    using namespace detail;
    return keyboard_observers.insert(ob).second;
}
bool RemoveKeyboardObserver(KeyboardObserver* ob)
{
    using namespace detail;
    return keyboard_observers.erase(ob);
}
void SetCursor(Cursor cursor)
{
    using namespace detail;
    glfwSetCursor(glfw_window_handle, cursor_ptrs[cursor - ARROW]);
}
void SetCursorEnabled(bool flag)
{
    using namespace detail;
    glfwSetInputMode(glfw_window_handle, GLFW_CURSOR, flag ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}
void SetActiveFont(FontHandle font)
{
    using namespace detail;
    if (font == nullptr)
        active_font = default_font;
    else
        active_font = (Font*)font;
}
FontHandle LoadFont(int resolution, std::filesystem::path path)
{
    using namespace detail;
    Font& new_font = fonts.emplace_back(resolution, path);
    if (!new_font.ready_to_use())
    {
        return nullptr;
    }
    return &new_font;
}
}

#ifndef QUICKDRAW_DISBALE_VEC2_INEQUALITY_OPERATORS
bool operator<(const quickdraw::Vec2& lhs, const quickdraw::Vec2& rhs)
{
    return lhs.x < rhs.x && lhs.y < rhs.y;
}
bool operator>(const quickdraw::Vec2& lhs, const quickdraw::Vec2& rhs)
{
    return lhs.x > rhs.x && lhs.y > rhs.y;
}
bool operator<=(const quickdraw::Vec2& lhs, const quickdraw::Vec2& rhs)
{
    return lhs.x <= rhs.x && lhs.y <= rhs.y;
}
bool operator>=(const quickdraw::Vec2& lhs, const quickdraw::Vec2& rhs)
{
    return lhs.x >= rhs.x && lhs.y >= rhs.y;
}
#endif