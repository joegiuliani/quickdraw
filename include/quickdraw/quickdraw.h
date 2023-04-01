#ifndef QUICKDRAW_QUICKDRAW_H_
#define QUICKDRAW_QUICKDRAW_H_

#include "glad/glad.h"
#include "GLFW/glfw3native.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <set>
#include <string>
#include <vector>
#include <filesystem>

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
constexpr int MOD_SHIFT = GLFW_MOD_SHIFT;
constexpr int MOD_CONTROL = GLFW_MOD_CONTROL;
constexpr int MOD_ALT = GLFW_MOD_ALT;
constexpr int MOD_SUPER = GLFW_MOD_SUPER;
constexpr int MOD_CAPS_LOCK = GLFW_MOD_CAPS_LOCK;
constexpr int MOD_NUM_LOCK = GLFW_MOD_NUM_LOCK;

using RGBA = glm::vec<4,float,glm::packed_highp>;

// signed (x,y) coordinates in pixels.
using Vec2 = glm::vec<2, float, glm::packed_highp>;

class AbstractObserver
{
public:
    void set_enabled(bool flag);
    bool enabled();
private:
    bool enabled_ = true;
};

enum Axis
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

namespace window
{
    class WindowTerminationObserver : public AbstractObserver
    {
    public:
        virtual void on_window_termination() = 0;
    };

    bool AddWindowTerminationObserver(WindowTerminationObserver* ob);
    bool RemoveWindowTerminationObserver(WindowTerminationObserver* ob);

    class WindowResizeObserver : public AbstractObserver
    {

    public:
        virtual void on_window_resize() = 0;
    };

    bool AddWindowResizeObserver(WindowResizeObserver* ob);
    bool RemoveWindowResizeObserver(WindowResizeObserver* ob);

using TextureHandle = GLuint;

enum ButtonState
{
    UP = GLFW_RELEASE,
    DOWN = GLFW_PRESS
};

// Returns a non-zero value if there was an issue during initialization.
bool Init(const char* name, unsigned int width, unsigned int height);

// - If unable to load the texture, returns an invalid handle defined by TextureHandleIsValid()
// - File type can be anything supported by stbimage
// - output_dimensions can be nullptr
TextureHandle LoadTexture(std::filesystem::path file, Vec2* output_dimensions);
void UnloadTexture(TextureHandle* handle);

// Returns true if the value of the handle is valid for functions in this library
bool TextureHandleIsValid(TextureHandle handle);

// TODO add scissor functionality for font shadows

void SetWindowIcon(TextureHandle image);

// Updates frame information and polls input events
void NewFrame();
void DrawFrame();
bool ShouldClose();
void Terminate();

void DrawRect(const Vec2& pos, const Vec2& size);

// Draws Text with a specified position
void DrawText(const Vec2& pos, const std::string &Text);

// - Works best with high resolution curves.
// - points.size() cannot be less than 2
void DrawPath(const std::vector<Vec2>& points, float thickness, const Vec2& offset = Vec2(0));

/*
void DrawTexture(TextureHandle handle, const Vec2& pos, const Vec2& size);
*/
void StartScissor(const Vec2& pos, const Vec2& size);

// Disables the scissor test
void StopScissor();

// Returns the time in seconds
double Time();

// Returns the duration of time since the start of the last frame in seconds
double DeltaTime();

// Returns the number of frames since the start of the program so that
// 0 is returned during the first frame.
//
// A negative number may be returned that the previous frame number is
// always less than the current
int FrameNumber();

// Returns the size of the drawing context
Vec2 ViewportSize();

namespace mouse
{
class Observer : public AbstractObserver
{

public:
    virtual void on_mouse_move() = 0;
    virtual void on_mouse_press() = 0;
    virtual void on_mouse_release() = 0;
    virtual void on_mouse_scroll() = 0;
};

bool AddObserver(Observer* ob);
bool RemoveObserver(Observer* ob);

enum Cursor
{
    ARROW = GLFW_ARROW_CURSOR,
    TEXT = GLFW_IBEAM_CURSOR,
    CROSS = GLFW_CROSSHAIR_CURSOR,
    HAND = GLFW_HAND_CURSOR,
    HRESIZE = GLFW_HRESIZE_CURSOR,
    VRESIZE = GLFW_VRESIZE_CURSOR
};

void SetCursor(Cursor cursor);

// Shows/hides the cursor when inside the window
void SetCursorEnabled(bool flag);

enum Button
{
    LEFT = GLFW_MOUSE_BUTTON_LEFT,
    RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE
};

// Returns whether any of the mouse buttons are down
bool IsDown();

// Returns whether any of the mouse buttons are pressed
bool IsPressed();

// Returns whether any of the mouse buttons are released
bool IsReleased();

bool IsDown(Button b);

bool IsPressed(Button b);

bool IsReleased(Button b);

bool IsMoving();

// Returns the difference between the current position and the
// last polled position
Vec2 Delta();

// Returns the last polled distance from the top left of the viewport
Vec2 Pos();

// Returns -1, 0, or 1
int ScrollDir();

} // namespace mouse

namespace shader
{
    enum VertexIndex
    {
        TOP_LEFT = 0,
        TOP_RIGHT = 1,
        BOTTOM_RIGHT = 2,
        BOTTOM_LEFT = 3
    };

    void SetFillColor(const RGBA& color, VertexIndex index);
    void SetFillColor(const RGBA& color);
    void SetOutlineColor(const RGBA& color, VertexIndex index);
    void SetOutlineColor(const RGBA& color);
    void SetRectCornerMask(bool mask, VertexIndex index);
    void SetRectCornerMask(bool mask);
    void SetRectCornerSize(float size);

    // Sets the thickness of the outline of the rectangle to be drawn
    void SetOutlineThickness(float thickness);

    /*
    // Sets the scale of Text. Make sure to set the desired Text scale before
    // calling TextSize(...)
    void SetTextScale(float s);

    // Returns the size of the bounding box of str if it were drawn on the screen.
    // Make sure to set the desired Text scale before calling this method.
    Vec2 TextSize(const std::string& str);
    */
} // namespace shader

namespace keyboard
{

class Observer : public AbstractObserver
{

public:
    virtual void on_keyboard_press() = 0;
};

bool AddObserver(Observer* ob);
bool RemoveObserver(Observer* ob);

int KeyModifiers();
const std::set<int> &DownKeys();
} // namespace keyboard

} // namespace window
} // namespace quickdraw

// QUICKDRAW_QUICKDRAW_H_
#endif
