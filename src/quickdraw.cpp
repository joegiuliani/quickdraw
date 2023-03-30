#include "quickdraw.h"
#include "../include/ft2build.h"
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/glm/geometric.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <array>

#define QUICKDRAW_NOTIFY_OBSERVERS(arg_observer_type, arg_observers, arg_member_function) \
{\
std::set<arg_observer_type*> observers_this_notification = arg_observers; \
for (arg_observer_type* ob : observers_this_notification)\
{\
    ob->arg_member_function();\
}\
}

namespace quickdraw
{
namespace
{
Vec2 Normal(const Vec2& slope)
{
    return Vec2(slope.y, -slope.x) / glm::length(slope);
}
bool IsValidSize(const Vec2& size)
{
    return size.x >= 0 && size.y >= 0;
}
bool IsValidTextureSize(const Vec2& size)
{
    return IsValidSize(size) && size.x >= 1 && size.y >= 1;
}
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
namespace
{
    constexpr TextureHandle INVALID_TEXTURE_HANDLE = 0;
    struct Glyph
    {
        TextureHandle texture_handle;
        Vec2 size;
        Vec2 bearing;
        float advance;
    };
    Glyph glyphs[128];
    struct gradient
    {
        Axis axis;
        RGBA start, end;
    };

    constexpr float GLOBAL_FONT_SCALE = 1.0f;
    constexpr float advance_fac = 1.0f / float(1 << 6);
    float font_resolution = 128;
    float font_import_scale = 1.0f / font_resolution;
    float font_offset = 0;
    float font_height = 0;
    float font_line_distance = 1;
    GLFWwindow* window_ptr = nullptr;
    Vec2 viewport_dim(640, 480);
    double last_time = 0;
    double delta_time = 0;
    unsigned int frame_number = -1;

    bool scroll_update_received = false;
    bool cursor_update_received = false;
    bool mouse_button_update_received = false;

    constexpr GLuint RECT_MODE = 0;
    // constexpr GLuint TEXTURE_MODE = 1, FONT_MODE = 2, PATH_MODE = 3;
    GLuint quad_vao, vertex_dynamic_attribs_vbo, vertex_static_attribs_vbo, vertex_ebo, quad_attribs_ssbo;
    GLuint BASE_VERTEX_ELEMS[6]{ 2, 1, 0, 2, 0, 3 };

    struct DynamicVertexAttribs
    {
        Vec2 pos = Vec2(0);
        GLfloat corner_mask = 1;
        RGBA fill_color = RGBA(1, 0, 1, 1);
        RGBA outline_color = RGBA(1, 0, 1, 1);
    };
    struct StaticVertexAttribs
    {
        Vec2 uv = Vec2(0);
        GLuint quad_index = 0;
        StaticVertexAttribs(const Vec2& t_uv, GLuint t_quad_index):uv(t_uv),quad_index(t_quad_index)
        {
        }
    };
    DynamicVertexAttribs curr_vertex_attribs[4];
    std::vector<DynamicVertexAttribs> vertex_dynamic_attribs_to_draw;
    std::vector<StaticVertexAttribs> vertex_static_attribs_to_draw;
    std::vector<GLuint> vertex_elements_to_draw;
    std::vector<float> quads_attribs_to_draw;
    float curr_quad_attribs[8];

    size_t max_number_of_quads;
    
    int last_number_of_quads = 0;
    void glfwErrorCallback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }
    void DrawQuad(const Vec2* vertex_positions)
    {
        using namespace shader;

        curr_vertex_attribs[TOP_LEFT].pos = vertex_positions[TOP_LEFT];
        vertex_dynamic_attribs_to_draw.push_back(curr_vertex_attribs[TOP_LEFT]);

        curr_vertex_attribs[TOP_RIGHT].pos = vertex_positions[TOP_RIGHT];
        vertex_dynamic_attribs_to_draw.push_back(curr_vertex_attribs[TOP_RIGHT]);

        curr_vertex_attribs[BOTTOM_RIGHT].pos = vertex_positions[BOTTOM_RIGHT];
        vertex_dynamic_attribs_to_draw.push_back(curr_vertex_attribs[BOTTOM_RIGHT]);

        curr_vertex_attribs[BOTTOM_LEFT].pos = vertex_positions[BOTTOM_LEFT];
        vertex_dynamic_attribs_to_draw.push_back(curr_vertex_attribs[BOTTOM_LEFT]);

        for (float& attrib : curr_quad_attribs)
            quads_attribs_to_draw.emplace_back(attrib);
    }
    bool LoadFont(const char* path)
    {
        FT_Library ft;

        if (FT_Init_FreeType(&ft))
        {
            std::cout << "Failed to initalize FreeType\n";
            return false;
        }

        FT_Face face;
        if (FT_New_Face(ft, path, 0, &face))
        {
            std::cout << "Failed to load " << path << '\n';
            return false;
        }
        else
        {
            FT_Set_Pixel_Sizes(face, 0, font_resolution);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Load displayable characters
            for (unsigned char c = 0; c < 128; c++)
            {
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "Failed to load \'" << c << "\'\n";
                    continue;
                }

                unsigned int font_texture;
                glGenTextures(1, &font_texture);
                glBindTexture(GL_TEXTURE_2D, font_texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                    GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer

                );

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                Glyph glyph = { font_texture, Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows) * font_import_scale,
                               Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top) * font_import_scale,
                               (face->glyph->advance.x * advance_fac + 3) * font_import_scale };
                glyphs[c] = glyph;
            }

            float max_height = 0;
            for (const auto& g : glyphs)
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

        return true;
    }

    std::set<WindowResizeObserver*> window_resize_observers;
    std::set<WindowTerminationObserver*> window_termination_observers;
    template <typename T> struct StateTracker
    {
        T previous;
        T current;

        void new_state(const T& state)
        {
            previous = current;
            current = state;
        }
    };

} // namespace

TextureHandle LoadTexture(const char* input_path, Vec2* output_dimensions)
{
    TextureHandle return_handle;
    GLsizei width, height;
    unsigned char* char_buffer = stbi_load(input_path, &width, &height, nullptr, 4);
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
void UnloadTexture(TextureHandle* handle)
{
    glDeleteTextures(1, handle);
}
bool TextureHandleIsValid(TextureHandle handle)
{
    return handle != INVALID_TEXTURE_HANDLE;
}

namespace mouse
{   
namespace 
{
    StateTracker<Vec2> pos_state;
    Vec2 delta_state;
    constexpr int NUM_BUTTONS = 3;
    StateTracker<int> button_states[NUM_BUTTONS];
    StateTracker<int> scroll_state;
    GLFWcursor* cursor_ptrs[6];
    int key_mods = 0;
    std::set<Observer*> observers;
}
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
bool AddObserver(Observer* ob)
{
    return observers.insert(ob).second;
}
bool RemoveObserver(Observer* ob)
{
    return observers.erase(ob);
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
bool AddObserver(Observer* ob)
{
    return observers.insert(ob).second;
}
bool RemoveObserver(Observer* ob)
{
    return observers.erase(ob);
}
int KeyModifiers()
{
    return key_mods;
}
} // namespace keyboard
namespace shader
{
namespace
{
    float curr_text_scale = 24;
    GLuint TRANSFORM_LOC, PROGRAM_ID;

    bool Init(const std::filesystem::path& vert_path, const std::filesystem::path& frag_path)
    {
        // Load shader program
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
        catch (std::ifstream::failure& e)
        {
            std::cout << "shader file could not be read"
                << "\n"
                << e.what() << "\n";
        }

        auto check_for_errors = [&](GLuint shader_id, const std::string& shader_type) -> bool
        {
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
                    return false;
                }
            }
            else
            {
                glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader_id, LOG_SIZE, NULL, log);
                    std::cout << "Could not compile " << shader_type << "\n" << log << "\n";
                    return false;
                }
            }

            return true;
        };

        const char* vertex_code_cstr = vert_code.c_str();
        const char* fragment_code_cstr = frag_code.c_str();

        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertex_code_cstr, NULL);
        glCompileShader(vertex);
        if (!check_for_errors(vertex, "Vertex Shader"))
            return false;
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragment_code_cstr, NULL);
        glCompileShader(fragment);
        if (!check_for_errors(fragment, "Fragment Shader"))
            return false;
        PROGRAM_ID = glCreateProgram();
        glAttachShader(PROGRAM_ID, vertex);
        glAttachShader(PROGRAM_ID, fragment);
        glLinkProgram(PROGRAM_ID);
        if (!check_for_errors(PROGRAM_ID, "Shader program"))
            return false;

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glUseProgram(PROGRAM_ID);

        TRANSFORM_LOC = glGetUniformLocation(PROGRAM_ID, "transform");

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
        
        // Configure dynamic vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vertex_dynamic_attribs_vbo);

        GLuint vertex_pos_loc = glGetAttribLocation(PROGRAM_ID, "pos");
        glEnableVertexAttribArray(vertex_pos_loc);
        glVertexAttribPointer(vertex_pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::pos));

        GLuint vertex_corner_mask_loc = glGetAttribLocation(PROGRAM_ID, "corner_mask");
        glEnableVertexAttribArray(vertex_corner_mask_loc);
        glVertexAttribPointer(vertex_corner_mask_loc, 1, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::corner_mask));

        GLuint vertex_fill_color_loc = glGetAttribLocation(PROGRAM_ID, "fill_color");
        glEnableVertexAttribArray(vertex_fill_color_loc);
        glVertexAttribPointer(vertex_fill_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::fill_color));
        
        GLuint vertex_outline_color_loc = glGetAttribLocation(PROGRAM_ID, "outline_color");
        glEnableVertexAttribArray(vertex_outline_color_loc);
        glVertexAttribPointer(vertex_outline_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::outline_color));
        
        // -----------
       
        // Configure static vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vertex_static_attribs_vbo);

        GLuint vertex_uv_loc = glGetAttribLocation(PROGRAM_ID, "uv");
        glEnableVertexAttribArray(vertex_uv_loc);
        glVertexAttribPointer(vertex_uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertexAttribs), (void*)offsetof(StaticVertexAttribs, StaticVertexAttribs::uv));

        GLuint vertex_quad_index_loc = glGetAttribLocation(PROGRAM_ID, "quad_index");
        glEnableVertexAttribArray(vertex_quad_index_loc);
        glVertexAttribIPointer(vertex_quad_index_loc, 1, GL_UNSIGNED_INT, sizeof(StaticVertexAttribs), (void*)offsetof(StaticVertexAttribs, StaticVertexAttribs::quad_index));
        
        return true;
    }
    void Activate()
    {
        glUseProgram(PROGRAM_ID);
    }
}

void SetFillColor(const RGBA& color, VertexIndex index)
{
    curr_vertex_attribs[index].fill_color = color;
}
void SetFillColor(const RGBA& color)
{
    // TODO see if a for loop or memcpy are faster
    SetFillColor(color, TOP_LEFT);
    SetFillColor(color, TOP_RIGHT);
    SetFillColor(color, BOTTOM_RIGHT);
    SetFillColor(color, BOTTOM_LEFT);
}
void SetOutlineColor(const RGBA& color, VertexIndex index)
{
    curr_vertex_attribs[index].outline_color = color;
}
void SetOutlineColor(const RGBA& color)
{    
    // TODO see if a for loop or memcpy are faster
    SetOutlineColor(color, TOP_LEFT);
    SetOutlineColor(color, TOP_RIGHT);
    SetOutlineColor(color, BOTTOM_RIGHT);
    SetOutlineColor(color, BOTTOM_LEFT);
}
void SetRectCornerMask(bool mask, VertexIndex index)
{
    curr_vertex_attribs[index].corner_mask = mask;
}
void SetRectCornerMask(bool mask)
{
    // TODO see if a for loop or memcpy are faster
    SetRectCornerMask(mask, TOP_LEFT);
    SetRectCornerMask(mask, TOP_RIGHT);
    SetRectCornerMask(mask, BOTTOM_RIGHT);
    SetRectCornerMask(mask, BOTTOM_LEFT);
}
void SetRectCornerSize(float size)
{
    curr_quad_attribs[7] = size;
}
void SetOutlineThickness(float thickness)
{
    curr_quad_attribs[6] = thickness;
}
float GetTextWidth(const std::string& Text)
{
    float ret = 0;
    for (const char& c : Text)
    {
        ret += glyphs[(size_t)c].advance;
    }
    return ret * curr_text_scale;
}
float GetTextHeight()
{
    return curr_text_scale * font_height;
}

Vec2 TextSize(const std::string& str)
{
    return Vec2(GetTextWidth(str), GetTextHeight());
}
void SetTextScale(float s)
{
    curr_text_scale = s;
}
} // namespace shader

void WindowSizeCallback(GLFWwindow *window_ptr, int width, int height)
{
    viewport_dim = Vec2(width, height);
    glViewport(0, 0, width, height);

    shader::Activate();
    Vec2 transform = Vec2(1.0f / (viewport_dim - 1.0f));
    glUniform2fv(shader::TRANSFORM_LOC, 1, &transform[0]);

    QUICKDRAW_NOTIFY_OBSERVERS(WindowResizeObserver, window_resize_observers, on_window_resize);
}
bool Init(const char* name, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
        return false;

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
        return false;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glActiveTexture(GL_TEXTURE0);

    if (!LoadFont("C:\\Windows\\Fonts\\segoeuil.ttf"))
    {
        return false;
    }

    // Initializes vertices and shaders
    if (!shader::Init(std::filesystem::current_path() / "resources" / "quad.vert", std::filesystem::current_path() / "resources" / "quad.frag"))
        return false;

    WindowSizeCallback(window_ptr, (int)viewport_dim.x, (int)viewport_dim.y);
    last_time = glfwGetTime();

    return true;
}
void NewFrame()
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

    // Make sure to set delta to 0 if mouse hasn't moved.
    if (!cursor_update_received && mouse::delta_state != Vec2(0))
    {
        mouse::delta_state = Vec2(0);
    }

    if (!mouse_button_update_received)
    {
        for (auto& bs : mouse::button_states)
        {
            if (bs.current != bs.previous)
                bs.new_state(bs.current);
        }
    }
    scroll_update_received = false;
    cursor_update_received = false;
    mouse_button_update_received = false;   
}
void DrawFrame()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    shader::Activate();

    size_t num_verts_this_frame = vertex_dynamic_attribs_to_draw.size();
    size_t num_quads_this_frame = num_verts_this_frame / 4;

    bool more_vertex_memory_needed = num_quads_this_frame > max_number_of_quads;
    max_number_of_quads = std::max(num_quads_this_frame, max_number_of_quads);

    glBindVertexArray(quad_vao);    

    // Update static vertex attributes
    if (more_vertex_memory_needed)
    {
        GLuint quad_index = vertex_static_attribs_to_draw.size() / 4;
        while (vertex_elements_to_draw.size() / 6 < num_quads_this_frame)
        {
            for (int k = 0; k < 6; k++)
            {
                vertex_elements_to_draw.push_back(BASE_VERTEX_ELEMS[k] + quad_index * 4);
            }

            vertex_static_attribs_to_draw.emplace_back(StaticVertexAttribs(Vec2(0, 0), quad_index * 8));
            vertex_static_attribs_to_draw.emplace_back(StaticVertexAttribs(Vec2(1, 0), quad_index * 8));
            vertex_static_attribs_to_draw.emplace_back(StaticVertexAttribs(Vec2(1, 1), quad_index * 8));
            vertex_static_attribs_to_draw.emplace_back(StaticVertexAttribs(Vec2(0, 1), quad_index * 8));

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
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDrawElements(GL_TRIANGLES, num_quads_this_frame * 6, GL_UNSIGNED_INT, 0);

    quads_attribs_to_draw.clear();
    vertex_dynamic_attribs_to_draw.clear();

    glfwSwapBuffers(window_ptr);
    glClearColor(0.5, 0.5, 0.5, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
bool ShouldClose()
{
    return glfwWindowShouldClose(window_ptr);
}
void Terminate()
{
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
void DrawRect(const Vec2& pos, const Vec2& size)
{
    using namespace shader;

    curr_quad_attribs[0] = pos.x;
    curr_quad_attribs[1] = pos.y;
    curr_quad_attribs[2] = size.x;
    curr_quad_attribs[3] = size.y;
    curr_quad_attribs[4] = RECT_MODE;

    Vec2 vertices[4] = { pos + size * Vec2(0,0), pos + size * Vec2(1,0), pos + size * Vec2(1,1), pos + size * Vec2(0,1) };
    DrawQuad(vertices);
}
/*
void DrawText(const Vec2& pos, const std::string &Text)
{
    using namespace shader;
    curr_quad_attribs.mode = FONT_MODE;

    float curs = 0;

    for (const char &c : Text)
    {
        Glyph ch = glyphs[(size_t)c];
        float offset = (font_height - ch.bearing.y - font_offset) * text_scale;
        glBindTexture(GL_TEXTURE_2D, ch.texture_handle);

        Vec2 quad_top_left(pos.x + ch.bearing.x * text_scale + curs, pos.y + offset);
        Vec2 quad_bottom_right = quad_top_left + Vec2(ch.size) * text_scale;

        Vec2 vertices[4] = { quad_top_left, Vec2(quad_bottom_right.x, quad_top_left.y), quad_bottom_right,
                 Vec2(quad_top_left.x, quad_bottom_right.y) };
        DrawQuad(vertices);

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

void DrawTexture(TextureHandle handle, const Vec2& pos, const Vec2& size)
{
    using namespace shader;
    if (handle == 0)
        return;
    curr_quad_attribs.mode = TEXTURE_MODE;
    glBindTexture(GL_TEXTURE_2D, handle);

    Vec2 vertices[4] = { pos + size * Vec2(0,0), pos + size * Vec2(1,0), pos + size * Vec2(1,1), pos + size * Vec2(0,1) };
    DrawQuad(vertices);
}
*/
void StartScissor(const Vec2& pos, const Vec2& size)
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
bool AddWindowTerminationObserver(WindowTerminationObserver* ob)
{
    return window_termination_observers.insert(ob).second;
}
bool RemoveWindowTerminationObserver(WindowTerminationObserver* ob)
{
    return window_termination_observers.erase(ob);
}
bool AddWindowResizeObserver(WindowResizeObserver* ob)
{
    return window_resize_observers.insert(ob).second;
}
bool RemoveWindowResizeObserver(WindowResizeObserver* ob)
{
    return window_resize_observers.erase(ob);
}
} // namespace window

}