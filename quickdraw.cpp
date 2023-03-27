#include "quickdraw.h"
#include "include/ft2build.h"
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/glm/geometric.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define QUICKDRAW_NOTIFY_OBSERVERS(arg_observer_type, arg_observers, arg_member_function) \
{\
std::set<arg_observer_type*> observers_this_notification = arg_observers; \
for (arg_observer_type* ob : observers_this_notification)\
{\
    ob->arg_member_function();\
}\
}

// TODO
// Implement batch rendering
// Control which corners are rounded

namespace quickdraw
{

namespace
{
Vec2 Normal(const Vec2 &slope)
{
    return Vec2(slope.y, -slope.x) / glm::length(slope);
}

bool IsValidSize(const Vec2 &size)
{
    return size.x >= 0 && size.y >= 0;
}

bool IsValidTextureSize(const Vec2 &size)
{
    return IsValidSize(size) && size.x >= 1 && size.y >= 1;
}

constexpr window::TextureHandle INVALID_TEXTURE_HANDLE = 0;
} // namespace

void AbstractObserver::set_enabled(bool flag)
{
    enabled_ = flag;
}
bool AbstractObserver::enabled()
{
    return enabled_;
}

namespace window
{

std::set<FrameObserver*> frame_observers;
std::set<WindowResizeObserver*> window_resize_observers;
std::set<WindowTerminationObserver*> window_termination_observers;

template <typename T> struct StateTracker
{
    T previous;
    T current;

    void new_state(const T &state)
    {
        previous = current;
        current = state;
    }
};

TextureHandle LoadTexture(const char *input_path, Vec2 *output_dimensions)
{
    TextureHandle return_handle;
    GLsizei width, height;
    unsigned char *char_buffer = stbi_load(input_path, &width, &height, nullptr, 4);
    if (char_buffer == NULL)
    {
        return INVALID_TEXTURE_HANDLE;
    }

    if (output_dimensions != nullptr)
        *output_dimensions = Vec2(width, height);

    glGenTextures(1, &return_handle);
    glBindTexture(GL_TEXTURE_2D, return_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, char_buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(char_buffer);

    return return_handle;
}
void UnloadTexture(TextureHandle *handle)
{
    glDeleteTextures(1, handle);
}
bool TextureHandleIsValid(TextureHandle handle)
{
    return handle != 0;
}
struct Glyph
{
    unsigned int texture_handle;
    Vec2 size;
    Vec2 bearing;
    float advance;
};
Glyph glyphs[128];

struct gradient
{
    Axis axis;
    Color start, end;
};

GLuint quad_vao, quad_vbo, quad_ebo;
const int NUM_QUAD_VERTS = 4;

constexpr float GLOBAL_FONT_SCALE = 1.0f;
constexpr float advance_fac = 1.0f / float(1 << 6);
float font_resolution = 128;
float font_import_scale = 1.0f / font_resolution;
float font_offset = 0;
float font_height = 0;
float font_line_distance = 1;
float text_scale = 24;
GLFWwindow *window_ptr = nullptr;
Vec2 viewport_dim(640, 480);
double last_time = 0;
double delta_time = 0;
unsigned int frame_number = -1;

bool scroll_update_received = false;
bool cursor_update_received = false;
bool mouse_button_update_received = false;

void glfwErrorCallback(int error, const char *description);
float GetTextWidth(const std::string &Text);
float GetTextHeight();
void DrawQuad(const Vec2 &top_left, const Vec2 &top_right, const Vec2 &bottom_right, const Vec2 &bottom_left);
int LoadFont(const char *path);
void InitShape();

namespace mouse
{
StateTracker<Vec2> pos_state;
Vec2 delta_state;
constexpr int NUM_BUTTONS = 3;
StateTracker<int> button_states[NUM_BUTTONS];
StateTracker<int> scroll_state;
GLFWcursor *cursor_ptrs[6];
int key_mods = 0;

std::set<Observer*> observers;

void Init()
{
    for (auto &st : button_states)
    {
        st.current = st.previous = UP;
    }

    for (int k = ARROW; k <= VRESIZE; k++)
    {
        cursor_ptrs[k - ARROW] = glfwCreateStandardCursor(k);

        if (cursor_ptrs[k - ARROW] == NULL)
            std::cout << "Error creating cursor " << k << "\n";
    }
}

bool IsDown()
{
    for (auto &s : button_states)
    {
        if (s.current == DOWN)
        {
            return true;
        }
    }

    return false;
}

bool IsDown(int button)
{
    return button_states[button].current == DOWN;
}

bool IsPressed()
{
    for (auto &s : button_states)
    {
        if (s.current == DOWN && s.previous == UP)
            return true;
    }
    return false;
}

bool IsReleased()
{
    for (auto &s : button_states)
    {
        if (s.current == UP && s.previous == DOWN)
            return true;
    }
    return false;
}

bool IsDown(Button b)
{
    return button_states[b].current == DOWN;
}

bool IsPressed(Button b)
{
    return button_states[b].current == DOWN && button_states[b].previous == UP;
}

bool IsReleased(Button b)
{
    return button_states[b].current == UP && button_states[b].previous == DOWN;
}

bool IsMoving()
{
    return delta_state != Vec2(0);
}

Vec2 Delta()
{
    return delta_state;
}

Vec2 Pos()
{
    return pos_state.current;
}

int ScrollDir()
{
    return scroll_state.current;
}

void SetCursor(Cursor cursor)
{
    glfwSetCursor(window_ptr, cursor_ptrs[cursor - ARROW]);
}
void SetCursorEnabled(bool flag)
{
    glfwSetInputMode(window_ptr, GLFW_CURSOR, flag ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void ButtonCallback(GLFWwindow *window_ptr, int button, int action, int mods)
{
    button_states[button].new_state(action);
    key_mods = mods;

    if (IsPressed(Button(button)))
    {
        QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_press);

    }
    else if (IsReleased(Button(button)))
    {
        QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_release);
    }

    mouse_button_update_received = true;
}
void CursorCallback(GLFWwindow *window_ptr, double x, double y)
{
    double mx, my;
    glfwGetCursorPos(window_ptr, &mx, &my);
    mouse::pos_state.new_state(Vec2(mx, my));
    mouse::delta_state = mouse::pos_state.current - mouse::pos_state.previous;
    QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_move);
    cursor_update_received = true;
}
void ScrollCallback(GLFWwindow *window_ptr, double xoffset, double yoffset)
{
    scroll_update_received = true;
    mouse::scroll_state.new_state((int)yoffset);
    QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_scroll);

}
} // namespace mouse

namespace keyboard
{
StateTracker<std::set<int>> down_keys;
int key_mods = 0;
std::set<Observer*> observers;

const std::set<int> &DownKeys()
{
    return down_keys.current;
}

void KeyCallback(GLFWwindow *window_ptr, int key, int scancode, int action, int mods)
{
    // We don't care about modifier keys. We'll leave that to the mods parameter.
    if (key >= KEY_LEFT_SHIFT && key <= KEY_RIGHT_SUPER)
        return;

    key_mods = mods;
    std::set<int> new_keys = down_keys.current;

    if (action == GLFW_PRESS)
    {
        new_keys.insert(key);
    }

    else if (action == GLFW_RELEASE)
    {
        new_keys.erase(key);
    }
    down_keys.new_state(new_keys);

    if (down_keys.current.size() > down_keys.previous.size())
    {
        QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_keyboard_press);

    }
}

int KeyModifiers()
{
    return key_mods;
}
} // namespace keyboard

namespace shader
{
gradient curr_outline, curr_fill;
enum Mode
{
    RECT,
    kImage,
    FONT,
    PATH
};
unsigned int ID;
void Init(const std::string &vert_path, const std::string &frag_path)
{
    std::string vert_code, frag_code;
    std::ifstream vert_file, frag_file;

    vert_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    frag_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vert_file.open(vert_path);
        frag_file.open(frag_path);
        std::stringstream vert_stream, frag_stream;
        vert_stream << vert_file.rdbuf();
        frag_stream << frag_file.rdbuf();
        vert_file.close();
        frag_file.close();
        vert_code = vert_stream.str();
        frag_code = frag_stream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "shader file could not be read"
                  << "\n"
                  << e.what() << "\n";
    }

    auto check_for_errors = [&](GLuint shader_id, const std::string &shader_type) {
        GLint success;
        const size_t LOG_SIZE = 4096;
        char log[LOG_SIZE];
        if (shader_type == "PROGRAM")
        {
            glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader_id, LOG_SIZE, NULL, log);
                std::cout << "Could not link " << shader_type << "\n" << log << "\n";
            }
        }
        else
        {
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader_id, LOG_SIZE, NULL, log);
                std::cout << "Could not compile " << shader_type << "\n" << log << "\n";
            }
        }
    };

    const char *vertex_code_cstr = vert_code.c_str();
    const char *fragment_code_cstr = frag_code.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code_cstr, NULL);
    glCompileShader(vertex);
    check_for_errors(vertex, "vertex Shader");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code_cstr, NULL);
    glCompileShader(fragment);
    check_for_errors(fragment, "fragment Shader");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    check_for_errors(ID, "Shader program");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
void Activate()
{
    glUseProgram(ID);
}

void SetBool(const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void SetInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void SetFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void SetVec(const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void SetColor(const std::string &name, const Color &color)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &color[0]);
}
void SetMode(Mode m)
{
    SetInt("mode", m);
}
void SetFill(const Color &start_color, const Color &end_color)
{
    SetColor("fill_start", start_color);
    SetColor("fill_end", end_color);
    curr_fill.start = start_color;
    curr_fill.end = end_color;
}
void SetOutline(const Color &start_color, const Color &end_color)
{
    SetColor("outline_start", start_color);
    SetColor("outline_end", end_color);
    curr_outline.start = start_color;
    curr_outline.end = end_color;
}
void SetFillDirection(Axis a)
{
    if (a == Axis::HORIZONTAL)
    {
        SetInt("fill_direction", 0);
        curr_fill.axis = a;
    }

    else if (a == Axis::VERTICAL)
    {
        SetInt("fill_direction", 1);
        curr_fill.axis = a;
    }
}
void SetOutlineDirection(Axis a)
{
    if (a == Axis::HORIZONTAL)
    {
        SetInt("outline_direction", 0);
        curr_outline.axis = a;
    }
    if (a == Axis::VERTICAL)
    {
        SetInt("outline_direction", 1);
        curr_outline.axis = a;
    }
}
void SetRectCornerSize(float size)
{
    SetFloat("corner_size", size);
}
void SetOutlineThickness(float thickness)
{
    SetFloat("outline_thickness", thickness);
}
void SetHeaderDepth(float d)
{
    SetFloat("header_depth", d);
}
void SetHeaderColor(const Color &c)
{
    SetColor("header_color", c);
}
Vec2 TextSize(const std::string &str)
{
    return Vec2(GetTextWidth(str), GetTextHeight());
}
void SetTextScale(float s)
{
    text_scale = s;
}
} // namespace shader

void WindowSizeCallback(GLFWwindow *window_ptr, int width, int height)
{
    viewport_dim = Vec2(width, height);
    glViewport(0, 0, width, height);

    shader::Activate();
    shader::SetVec("transform", Vec2(1.0f / (viewport_dim - 1.0f)));

    QUICKDRAW_NOTIFY_OBSERVERS(WindowResizeObserver, window_resize_observers, on_window_resize);
}

bool Init(const char *name, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
        return -1;

    viewport_dim = Vec2(width, height);

    // Setup window
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        std::cout << "Could not initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_DECORATED, true);

    window_ptr = glfwCreateWindow((int)viewport_dim.x, (int)viewport_dim.y, name, NULL, NULL);
    if (window_ptr == NULL)
    {
        std::cout << "Could not create GLFW window\n";
        return false;
    }
    glfwMakeContextCurrent(window_ptr);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window_ptr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetWindowSizeCallback(window_ptr, WindowSizeCallback);
    glfwSetScrollCallback(window_ptr, mouse::ScrollCallback);
    glfwSetKeyCallback(window_ptr, keyboard::KeyCallback);
    glfwSetCursorPosCallback(window_ptr, mouse::CursorCallback);
    glfwSetMouseButtonCallback(window_ptr, mouse::ButtonCallback);
    mouse::Init();

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Warning: Failed to load GL from GLAD."
                  << "\n";
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glActiveTexture(GL_TEXTURE0);

    if (LoadFont("C:\\Windows\\Fonts\\segoeuil.ttf"))
    {
        return false;
    }

    // Initializes vertices and shaders
    InitShape();

    WindowSizeCallback(window_ptr, (int)viewport_dim.x, (int)viewport_dim.y);
    last_time = glfwGetTime();

    return true;
}
void Run()
{
    while (!glfwWindowShouldClose(window_ptr))
    {
        frame_number++;

        delta_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        glfwPollEvents();

        // Make sure to set scroll state to 0 if we aren't scrolling
        if (!scroll_update_received && mouse::scroll_state.current != 0)
        {
            mouse::ScrollCallback(window_ptr, 0, 0);
        }

        // Make sure to set delta to 0 is mouse hasn't moved.
        if (!cursor_update_received && mouse::delta_state != Vec2(0))
        {
            mouse::delta_state = Vec2(0);
        }

        if (!mouse_button_update_received)
        {
            for (auto &bs : mouse::button_states)
            {
                if (bs.current != bs.previous)
                    bs.new_state(bs.current);
            }
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glDisable(GL_SCISSOR_TEST);
        shader::Activate();
        // END start of frame

        QUICKDRAW_NOTIFY_OBSERVERS(FrameObserver, frame_observers, on_new_frame);

        // BEGIN end of frame
        scroll_update_received = false;
        cursor_update_received = false;
        mouse_button_update_received = false;

        glfwSwapBuffers(window_ptr);
        glClearColor(0.5, 0.5, 0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    QUICKDRAW_NOTIFY_OBSERVERS(WindowTerminationObserver, window_termination_observers, on_window_termination);

    glfwTerminate();
}

void SetWindowIcon(TextureHandle handle)
{
    if (!TextureHandleIsValid(handle))
        return;

    glBindTexture(GL_TEXTURE_2D, handle);
    int width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    std::vector<unsigned char> window_icon_data(size_t(width * height * 4));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, window_icon_data.data());
    GLFWimage window_icon{width, height, window_icon_data.data()};

    glfwSetWindowIcon(window_ptr, 1, &window_icon);
}

void DrawRect(const Vec2 &pos, const Vec2 &size)
{
    // Set the position and size
    shader::SetVec("pos", pos);
    shader::SetVec("size", size);
    shader::SetMode(shader::Mode::RECT);
    DrawQuad(pos, Vec2(pos.x + size.x, pos.y), pos + Vec2(size), Vec2(pos.x, pos.y + size.y));
}
void DrawText(const Vec2 &pos, const std::string &Text)
{
    shader::SetMode(shader::Mode::FONT);
    float curs = 0;

    for (const char &c : Text)
    {
        Glyph ch = glyphs[(size_t)c];
        float offset = (font_height - ch.bearing.y - font_offset) * text_scale;
        glBindTexture(GL_TEXTURE_2D, ch.texture_handle);

        Vec2 quad_top_left(pos.x + ch.bearing.x * text_scale + curs, pos.y + offset);
        Vec2 quad_bottom_right = quad_top_left + Vec2(ch.size) * text_scale;
        DrawQuad(quad_top_left, Vec2(quad_bottom_right.x, quad_top_left.y), quad_bottom_right,
                 Vec2(quad_top_left.x, quad_bottom_right.y));

        // Advance the horizontal cursor to the drawing position of the next
        // character
        curs += ch.advance * text_scale;
    }
}
void DrawPath(const std::vector<Vec2> &points, float thickness, const Vec2)
{
    if (points.size() < 2)
    {
        std::cout << "Can't draw a path with less than 2 points\n";
        return;
    }
    shader::SetMode(shader::Mode::PATH);
    shader::SetFloat("path_thickness", thickness);
    std::vector<float> point_dists_from_start;
    float path_length = 0;

    bool needs_length_data =
        shader::curr_fill.axis == Axis::HORIZONTAL || shader::curr_outline.axis == Axis::HORIZONTAL;

    if (needs_length_data)
    {
        point_dists_from_start.resize(points.size());
        point_dists_from_start[0] = 0;
        for (size_t k = 1; k < points.size(); k++)
        {
            float line_length = glm::distance(points[k - 1], points[k]);
            path_length += line_length;
            point_dists_from_start[k] = path_length;
        }

        shader::SetFloat("path_length", path_length);
        shader::SetFloat("curr_path_progress", 0);
        shader::SetFloat("next_path_progress", point_dists_from_start[1] / path_length);
    }
    // This method deals with the edges shared by the quads used
    // to draw the path. Each edge's slope is equal to the normal of
    // the path at the point the edge lies on. Each edge's length is
    // equal to thickness. Filling the edges between two points
    // gives a quad which will be drawn

    // Calculate the normal of the previous, current, and next line
    Vec2 line_normal = Normal(points[1] - points[0]);
    Vec2 next_normal;

    // If there are more than 2 points, use the next line's normal
    if (points.size() > 2)
        next_normal = Normal(points[2] - points[1]);

    // otherwise
    else
        next_normal = line_normal;

    // Precomputed value that when multiplied by the end ray gives the average
    // between the current normal and the next normal and sets the length of end
    // ray to the thickness of the line
    const float end_edge_slope_coeff = 0.5f * thickness;

    // The slopes of the edges connecting this quad to the previous and next quad
    Vec2 start_edge_slope = -line_normal * thickness;

    // Average the line's normal with the next line's normal to get the slope of
    // the edge connecting the line with the next line
    Vec2 end_edge_slope = -(line_normal + next_normal) * end_edge_slope_coeff;

    // Set the quads points
    Vec2 top_left = start_edge_slope + points[0];
    Vec2 top_right = end_edge_slope + points[1];
    Vec2 bottom_right = -end_edge_slope + points[1];
    Vec2 bottom_left = -start_edge_slope + points[0];

    // draw the quad
    DrawQuad(top_left, top_right, bottom_right, bottom_left);

    if (points.size() > 2)
    {
        // Draws each line except the first and last one
        size_t k = 1;
        while (k < points.size() - 2)
        {
            // Use the end edge from the last quad as the start edge for the current
            // quad
            top_left = top_right;
            bottom_left = bottom_right;

            // Use the next normal from the last line as the normal for the current
            // line
            line_normal = next_normal;

            // Set the next normal to be the normal of the line after the current line
            next_normal = Normal(points[k + 2] - points[k + 1]);

            // Find the slope of the end edge
            end_edge_slope = -(line_normal + next_normal) * end_edge_slope_coeff;

            // We already have the start edge from the last quad, so we only need to
            // set the end edge
            top_right = end_edge_slope + points[k + 1];
            bottom_right = -end_edge_slope + points[k + 1];

            if (needs_length_data)
            {
                shader::SetFloat("curr_path_progress", point_dists_from_start[k] / path_length);
                shader::SetFloat("next_path_progress", point_dists_from_start[k + 1] / path_length);
            }

            // buffer
            DrawQuad(top_left, top_right, bottom_right, bottom_left);

            k++;
        }

        // draw the last line

        top_left = top_right;
        bottom_left = bottom_right;

        end_edge_slope = -next_normal * thickness;
        top_right = end_edge_slope + points[k + 1];
        bottom_right = -end_edge_slope + points[k + 1];

        if (needs_length_data)
        {
            shader::SetFloat("curr_path_progress", point_dists_from_start[k] / path_length);
            shader::SetFloat("next_path_progress", point_dists_from_start[k + 1] / path_length);
        }

        DrawQuad(top_left, top_right, bottom_right, bottom_left);
    }
    if (shader::curr_outline.axis == Axis::HORIZONTAL)
    {
        shader::SetOutline(shader::curr_outline.start, shader::curr_outline.end);
    }
    if (shader::curr_fill.axis == Axis::HORIZONTAL)
    {
        shader::SetFill(shader::curr_fill.start, shader::curr_fill.end);
    }
}
void DrawImage(TextureHandle handle, const Vec2 &pos, const Vec2 &size)
{
    if (handle == 0)
        return;
    shader::SetMode(shader::Mode::kImage);
    glBindTexture(GL_TEXTURE_2D, handle);

    DrawQuad(pos, Vec2(pos.x + size.x, pos.y), pos + Vec2(size), Vec2(pos.x, pos.y + size.y));
}
void StartScissor(const Vec2 &pos, const Vec2 &size)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(pos.x, viewport_dim.y - pos.y - size.y, size.x, size.y);
}
void StopScissor()
{
    glDisable(GL_SCISSOR_TEST);
}
double Time()
{
    return glfwGetTime();
}
double DeltaTime()
{
    return delta_time;
}
Vec2 ViewportSize()
{
    return viewport_dim;
}
int FrameNumber()
{
    return frame_number;
}
int LoadFont(const char *path)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Failed to initalize FreeType\n";
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face))
    {
        std::cout << "Failed to load " << path << '\n';
        return -1;
    }
    else
    {
        FT_Set_Pixel_Sizes(face, 0, font_resolution);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load readable characters
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "Failed to load glyph for \'" << c << "\'\n";
                continue;
            }

            // generate texture
            unsigned int font_texture;
            glGenTextures(1, &font_texture);
            glBindTexture(GL_TEXTURE_2D, font_texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                         GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer

            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Glyph glyph = {font_texture, Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows) * font_import_scale,
                           Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top) * font_import_scale,
                           (face->glyph->advance.x * advance_fac + 3) * font_import_scale};
            glyphs[c] = glyph;
        }

        float max_height = 0;
        for (const auto &g : glyphs)
        {
            if (g.size.y > max_height)
            {
                max_height = g.bearing.y;
            }
        }

        font_height = max_height;
        font_offset = max_height * 0.5f;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return 0;
}
void InitShape()
{
    Vec2 verts[NUM_QUAD_VERTS]{Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1)};

    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glGenBuffers(1, &quad_ebo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * NUM_QUAD_VERTS, verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int shape_uv_vbo;
    glGenBuffers(1, &shape_uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, shape_uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * NUM_QUAD_VERTS, verts, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (void *)0);
    glEnableVertexAttribArray(1);

    const int NUM_SHAPE_ELEMS = 6;
    GLushort shape_elems[NUM_SHAPE_ELEMS]{2, 1, 0, 2, 0, 3};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * NUM_SHAPE_ELEMS, shape_elems, GL_STATIC_DRAW);

    shader::Init("shape.vert", "shape.frag");
    shader::Activate();
}
void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
void DrawQuad(const Vec2 &top_left, const Vec2 &top_right, const Vec2 &bottom_right, const Vec2 &bottom_left)
{
    Vec2 verts[NUM_QUAD_VERTS]{top_left, top_right, bottom_right, bottom_left};

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec2) * NUM_QUAD_VERTS, &verts[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}
float GetTextWidth(const std::string &Text)
{
    float ret = 0;
    for (const char &c : Text)
    {
        ret += glyphs[(size_t)c].advance;
    }
    return ret * text_scale;
}
float GetTextHeight()
{
    return text_scale * font_height;
}
}
} 