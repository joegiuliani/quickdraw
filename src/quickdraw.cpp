#include "../include/quickdraw/quickdraw.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "../include/stb_rect_pack.h"
#include "../include/ft2build.h"
#include FT_FREETYPE_H
#include "../include/glm/geometric.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <array>
#include <stack>

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
GLuint texture_atlas_handle = 0;
std::list<Texture> atlas_textures;
bool atlas_updated = false;

struct Glyph
{
    Texture* texture = nullptr;
    Vec2 size = Vec2(0);
    Vec2 bearing = Vec2(0);
    float advance = 0;
};

class FontAtlas
{
public:

    static constexpr bool IsDisplayableChar(unsigned char c);
   
    FontAtlas(int resolution, std::filesystem::path path);
    // Returns this->end() if IsDisplayableChar(c) is false
    Glyph* get(unsigned char c);
    Glyph* begin();
    Glyph* end();
    bool ready_to_use();
    float font_offset();
    float font_height();
    float space_character_width();
private:
    static constexpr unsigned char FIRST_VALID_CHAR = 33;
    static constexpr unsigned char LAST_VALID_CHAR = 126;
    static constexpr int NUM_VALID_CHARS = 1 + LAST_VALID_CHAR - FIRST_VALID_CHAR;
    Glyph glyphs_[NUM_VALID_CHARS];
    bool ready_to_use_ = false;
    float horizontal_spacing_ = 3;
    float font_offset_ = 0;
    float font_height_ = 0;
    float space_character_width_ = 0;
};

RGBA background_color = RGBA(0);

FontAtlas* curr_font_atlas = nullptr;

GLuint TRANSFORM_LOC, SHADER_HANDLE;
GLuint quad_vao, vertex_dynamic_attribs_vbo, vertex_static_attribs_vbo, vertex_ebo, quad_attribs_ssbo;
GLuint BASE_VERTEX_ELEMS[6]{ 2, 1, 0, 2, 0, 3 };

Vec2 VERTEX_UVS[4] =
{
    Vec2(0,0),
    Vec2(1,0),
    Vec2(1,1),
    Vec2(0,1)
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
    {
    }
};
constexpr size_t ATTRIBS_PER_QUAD = 7;
constexpr size_t VERTS_PER_QUAD = 4;
constexpr size_t ELEMS_PER_QUAD = 6;
std::vector<DynamicVertexAttribs> vertex_dynamic_attribs_to_draw;
std::vector<StaticVertexAttribs> vertex_static_attribs_to_draw;
std::vector<GLuint> vertex_elements_to_draw;
std::vector<float> quads_attribs_to_draw;
  
GLFWwindow* glfw_window_handle = nullptr;
Vec2 viewport_dim(640, 480);
double last_time = 0;
double delta_time = 0;
unsigned int frame_number = -1;
bool scroll_update_received = false;
bool cursor_update_received = false;
bool mouse_button_update_received = false;
size_t max_number_of_quads;

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
float curr_text_scale = 24;
constexpr GLuint RECT_MODE = 0,
                 TEXTURE_MODE = 1,
                 PATH_MODE = 2;

DynamicVertexAttribs curr_vertex_attribs[VERTS_PER_QUAD];
float curr_quad_attribs[ATTRIBS_PER_QUAD];

bool Init(const std::filesystem::path& vert_path, const std::filesystem::path& frag_path);
void Activate();
/**
* Must be one of RECT_MODE, TEXTURE_MODE, PATH_MODE
*/
void SetQuadMode(GLuint mode);
void SetRectPosAndSize(const Vec2& pos, const Vec2& size);
void SetPathThickness(float thickness);
void DrawQuad();
Vec2 Normal(const Vec2& slope);
std::pair<Texture*, Vec2> LoadTexture(int width, int height, std::vector<GLubyte> rgba_bitmap);
void PackTextures();
void glfwErrorCallback(int error, const char* description);
void WindowSizeCallback(GLFWwindow* window_ptr, int width, int height);
} // namespace
struct Texture
{
    std::vector<GLubyte> bitmap = std::vector<GLubyte>(0);
    Vec2 uv_start = Vec2(0);
    Vec2 uv_end = Vec2(0);
    int width = 0;
    int height = 0;
};
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

void ButtonCallback(GLFWwindow* window_ptr, int button, int action, int mods);
void CursorCallback(GLFWwindow* window_ptr, double x, double y);
void ScrollCallback(GLFWwindow* window_ptr, double xoffset, double yoffset);
void ProcessEvents();
void Init();
} // namespace
} // namespace mouse
namespace keyboard
{
namespace
{
    StateTracker<std::set<int>> down_keys;
    int key_mods = 0;
    std::set<Observer*> observers;
    void KeyCallback(GLFWwindow* window_ptr, int key, int scancode, int action, int mods);
    void Init();
} // namespace
} // namespace keyboard
}

namespace quickdraw
{
namespace
{
constexpr bool FontAtlas::IsDisplayableChar(unsigned char c)
{
    return c >= FIRST_VALID_CHAR && c <= LAST_VALID_CHAR;
}
FontAtlas::FontAtlas(int resolution, std::filesystem::path path)
{
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
        std::cout << "quickdraw::FontAtlas::FontAtlas Failed to load " << path << '\n';
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, resolution);

    std::vector<GLubyte> glyph_bitmaps[NUM_VALID_CHARS];
    int min_atlas_area = 0;
    // width / rows
    float avg_glyph_aspect_ratio = 0;

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
        min_atlas_area += face->glyph->bitmap.width * face->glyph->bitmap.rows;
        avg_glyph_aspect_ratio += float(face->glyph->bitmap.width) / face->glyph->bitmap.rows;
        space_character_width_ += face->glyph->bitmap.width; // is the average of displayable character widths

        // Note we set up the glyph uv's later.
        glyph.size = Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        glyph.bearing = Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        glyph.advance = face->glyph->advance.x * ADVANCE_FAC + horizontal_spacing_; // If text quads are positioned incorrectly its probably this

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
    space_character_width_ /= NUM_VALID_CHARS; // is the average of displayable character widths

    float font_import_scale = 1.0f / resolution;
    for (Glyph& g : glyphs_)
    {
        g.size *= font_import_scale;
        g.bearing *= font_import_scale;
        g.advance *= font_import_scale;
    }
    space_character_width_ *= font_import_scale;

    float max_height = 0;
    for (const auto& g : glyphs_)
    {
        if (g.size.y > max_height)
        {
            max_height = g.bearing.y;
        }
    }
    font_height_ = max_height;
    font_offset_ = max_height * 0.5f;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    ready_to_use_ = true;
}
// Returns this->end() if IsDisplayableChar(c) is false
Glyph* FontAtlas::get(unsigned char c)
{
    if (!IsDisplayableChar(c))
        return end();
    return begin() + (c - FIRST_VALID_CHAR);
}
Glyph* FontAtlas::begin()
{
    return &glyphs_[0];
}
Glyph* FontAtlas::end()
{
    return glyphs_ + NUM_VALID_CHARS;
}
bool FontAtlas::ready_to_use()
{
    return ready_to_use_;
}
float FontAtlas::font_offset()
{
    return font_offset_;
}
float FontAtlas::font_height()
{
    return font_height_;
}
float FontAtlas::space_character_width()
{
    return space_character_width_;
}
bool InitOpenGL()
{
    const char* METHOD_ERROR_HEADER = "quickdraw::InitOpenGL ";

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "quickdraw::InitOpenGL Failed to load OpenGL\n";
        return false;
    }

    std::filesystem::path vertex_shader_path = std::filesystem::current_path() / "resources" / "quad.vert";
    std::filesystem::path fragment_shader_path = std::filesystem::current_path() / "resources" / "quad.frag";

    // Load shader program
    std::string vert_code, frag_code;
    std::ifstream vert_file, frag_file;

    vert_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    frag_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vert_file.open(vertex_shader_path);
        frag_file.open(fragment_shader_path);
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
        std::cout << METHOD_ERROR_HEADER << "Failed to read shader file\n"
            << e.what() << "\n";
        return false;
    }

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
    SHADER_HANDLE = glCreateProgram();
    glAttachShader(SHADER_HANDLE, vertex);
    glAttachShader(SHADER_HANDLE, fragment);
    glLinkProgram(SHADER_HANDLE);
    glGetProgramiv(SHADER_HANDLE, GL_LINK_STATUS, &success_flag);
    if (!success_flag)
    {
        glGetProgramInfoLog(SHADER_HANDLE, LOG_SIZE, NULL, log);
        std::cout << METHOD_ERROR_HEADER << "Failed to link shader program\n" << log << '\n';
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(SHADER_HANDLE);

    TRANSFORM_LOC = glGetUniformLocation(SHADER_HANDLE, "transform");

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

    GLuint vertex_pos_loc = glGetAttribLocation(SHADER_HANDLE, "pos");
    glEnableVertexAttribArray(vertex_pos_loc);
    glVertexAttribPointer(vertex_pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::pos));

    GLuint vertex_corner_mask_loc = glGetAttribLocation(SHADER_HANDLE, "corner_mask");
    glEnableVertexAttribArray(vertex_corner_mask_loc);
    glVertexAttribPointer(vertex_corner_mask_loc, 1, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::corner_mask));

    GLuint vertex_fill_color_loc = glGetAttribLocation(SHADER_HANDLE, "fill_color");
    glEnableVertexAttribArray(vertex_fill_color_loc);
    glVertexAttribPointer(vertex_fill_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::fill_color));

    GLuint vertex_outline_color_loc = glGetAttribLocation(SHADER_HANDLE, "outline_color");
    glEnableVertexAttribArray(vertex_outline_color_loc);
    glVertexAttribPointer(vertex_outline_color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::outline_color));

    GLuint vertex_uv_loc = glGetAttribLocation(SHADER_HANDLE, "uv");
    glEnableVertexAttribArray(vertex_uv_loc);
    glVertexAttribPointer(vertex_uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicVertexAttribs), (void*)offsetof(DynamicVertexAttribs, DynamicVertexAttribs::uv));


    // Configure static vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, vertex_static_attribs_vbo);

    GLuint vertex_quad_index_loc = glGetAttribLocation(SHADER_HANDLE, "quad_index");
    glEnableVertexAttribArray(vertex_quad_index_loc);
    glVertexAttribIPointer(vertex_quad_index_loc, 1, GL_UNSIGNED_INT, sizeof(StaticVertexAttribs), (void*)offsetof(StaticVertexAttribs, StaticVertexAttribs::quad_index));

    return true;
}
/**
* Must be one of RECT_MODE, TEXTURE_MODE, PATH_MODE
*/
void SetQuadMode(GLuint mode)
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
float GetTextWidth(const std::string& text)
{
    float ret = 0;
    for (const char& c : text)
    {
        if (!FontAtlas::IsDisplayableChar(c))
        {
            if (c == ' ')
            {
                ret += curr_font_atlas->space_character_width();
                continue;
            }

            std::cout << "quickdraw::GetTextWidth Failed to get text width because 'text' argument has a character with no width\n";
            return 0;
        }
        ret += curr_font_atlas->get(c)->advance;
    }
    return ret * curr_text_scale;
}
float GetTextHeight()
{
    return curr_text_scale * curr_font_atlas->font_height();
}
Vec2 Normal(const Vec2& slope)
{
    return Vec2(slope.y, -slope.x) / glm::length(slope);
}
std::pair<Texture*, Vec2> LoadTexture(int width, int height, std::vector<GLubyte> rgba_bitmap)
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

    size_t sum_widths = 0;
    size_t sum_heights = 0;
    int max_height = 0;
    for (Texture& texture : atlas_textures)
    {
        sum_widths += texture.width;
        sum_heights += texture.height;
        max_height = std::max(texture.height, max_height);
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
        packing_rects[rect_index].w = texture.width;
        packing_rects[rect_index].h = texture.height;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, min_atlas_width, min_atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    rect_index = 0;
    for (Texture& texture : atlas_textures)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, packing_rects[rect_index].x, packing_rects[rect_index].y, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.bitmap.data());

        // Set UV coordinates for textures
        texture.uv_start = Vec2(packing_rects[rect_index].x / (double)min_atlas_width, packing_rects[rect_index].y / (double)min_atlas_height);
        texture.uv_end = Vec2((packing_rects[rect_index].x + texture.width) / (double)min_atlas_width, (packing_rects[rect_index].y + texture.height) / (double)min_atlas_height);

        ++rect_index;
    }
}
void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "quickdraw::window GLFW Error %d: %s\n", error, description);
}
void WindowSizeCallback(GLFWwindow* window_ptr, int width, int height)
{
    viewport_dim = Vec2(width, height);
    glViewport(0, 0, width, height);

    glUseProgram(SHADER_HANDLE);
    Vec2 transform = Vec2(1.0f / (viewport_dim - 1.0f));
    glUniform2fv(TRANSFORM_LOC, 1, &transform[0]);

    QUICKDRAW_NOTIFY_OBSERVERS(WindowResizeObserver, window_resize_observers, on_window_resize);
}
}
bool Init(const std::string& name, const Vec2& size)
{
    if ((int)size.x <= 0 || (int)size.y <= 0)
    {
        std::cout << "quickdraw::Init Failed to initialize because width or height was less than 1\n";
        return false;
    }

    viewport_dim = size;

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

    glfw_window_handle = glfwCreateWindow((int)viewport_dim.x, (int)viewport_dim.y, name.c_str(), NULL, NULL);
    if (glfw_window_handle == NULL)
    {
        std::cout << "quickdraw::window Failed to create GLFW window\n";
        return false;
    }
    glfwMakeContextCurrent(glfw_window_handle);
    glfwSetWindowSizeCallback(glfw_window_handle, WindowSizeCallback);
    mouse::Init();
    keyboard::Init();

    InitOpenGL();

    glGenTextures(1, &texture_atlas_handle);
    curr_font_atlas = new FontAtlas(64, "C:\\Windows\\Fonts\\segoeuil.ttf");
    if (!curr_font_atlas->ready_to_use())
    {
        return false;
    }

    WindowSizeCallback(glfw_window_handle, (int)viewport_dim.x, (int)viewport_dim.y);
    last_time = glfwGetTime();

    return true;
}
std::pair<Texture*, Vec2> LoadTexture(std::filesystem::path file)
{
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
void UnloadTexture(Texture* texture_to_remove)
{
    atlas_textures.remove_if([&](const Texture& texture)->bool { return &texture == texture_to_remove; });
    atlas_updated = true;
}
GLFWwindow* GetGLFWWindowHandle()
{
    if (glfw_window_handle == nullptr)
    {
        std::cout << "quickdraw::GetGLFWWindowHandle Failed to return handle because quickdraw::Init() has not been called\n";
    }
    return glfw_window_handle;
}
void NewFrame()
{
    frame_number++;
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    glfwPollEvents();
    mouse::ProcessEvents();

    if (atlas_updated)
    {
        PackTextures();
        atlas_updated = false;
    }
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
    glBindTexture(GL_TEXTURE_2D, texture_atlas_handle);
    glBindVertexArray(quad_vao);
    glUseProgram(SHADER_HANDLE);

    const size_t num_verts_this_frame = vertex_dynamic_attribs_to_draw.size();
    const size_t num_quads_this_frame = num_verts_this_frame / VERTS_PER_QUAD;
    const bool more_vertex_memory_needed = num_quads_this_frame > max_number_of_quads;
    max_number_of_quads = std::max(num_quads_this_frame, max_number_of_quads);

    // Update static vertex attributes
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
    glClear(GL_COLOR_BUFFER_BIT);
}
bool ShouldClose()
{
    return glfwWindowShouldClose(glfw_window_handle);
}
void Terminate()
{
    delete curr_font_atlas;
    glfwTerminate();
}
void SetBackgroundColor(const RGBA& color)
{
    background_color = color;
}
void SetWindowIcon(std::filesystem::path file)
{
    std::string file_str = file.generic_string();
    GLFWimage image = {};
    image.pixels = stbi_load(file_str.c_str(), &image.width, &image.height, nullptr, 4);
    glfwSetWindowIcon(glfw_window_handle, 1, &image);
    stbi_image_free(image.pixels);
}
void DrawRect(const Vec2& pos, const Vec2& size)
{
    SetQuadMode(RECT_MODE);
    SetRectPosAndSize(pos, size);
    DrawQuad();
}
void DrawText(const Vec2& pos, const std::string &text)
{
    DynamicVertexAttribs saved_vertex_attribs[4];
    for (int k = 0; k < 4; k++)
    {
        saved_vertex_attribs[k] = curr_vertex_attribs[k];
    }
    float text_width = GetTextWidth(text);
    float curs = 0;
    for (char c : text)
    {
        if (!FontAtlas::IsDisplayableChar(c))
        {
            if (c == ' ')
            {
                curs += curr_font_atlas->space_character_width() * curr_text_scale;
                continue;
            }

            std::cout << "quickdraw::DrawText Failed to draw text because 'text' argument has a non-displayable character\n";
            return;
        }
        Glyph& glyph = *curr_font_atlas->get(c);
        float offset = (curr_font_atlas->font_height() - glyph.bearing.y - curr_font_atlas->font_offset()) * curr_text_scale;

        Vec2 draw_pos = Vec2(pos.x + glyph.bearing.x * curr_text_scale + curs, pos.y + offset);    
        Vec2 draw_size = glyph.size * curr_text_scale;
        curr_vertex_attribs[UPPER_START].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, (draw_pos.x - pos.x) / text_width);
        curr_vertex_attribs[UPPER_END].fill_color = glm::mix(saved_vertex_attribs[UPPER_START].fill_color, saved_vertex_attribs[UPPER_END].fill_color, (draw_pos.x + draw_size.x - pos.x) / text_width);
        curr_vertex_attribs[LOWER_START].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, (draw_pos.x - pos.x) / text_width);
        curr_vertex_attribs[LOWER_END].fill_color = glm::mix(saved_vertex_attribs[LOWER_START].fill_color, saved_vertex_attribs[LOWER_END].fill_color, (draw_pos.x + draw_size.x - pos.x) / text_width);
        DrawTexture(glyph.texture, draw_pos, draw_size);

        // Advance the horizontal cursor to the drawing position of the next
        // character
        curs += glyph.advance * curr_text_scale;
    }

    for (int k = 0; k < 4; k++)
    {
        curr_vertex_attribs[k] = saved_vertex_attribs[k];
    }
}
void DrawTexture(Texture* texture, const Vec2& pos, const Vec2& size)
{
    SetQuadMode(TEXTURE_MODE);
   
    curr_vertex_attribs[UPPER_START].pos = pos;
    curr_vertex_attribs[UPPER_END].pos = Vec2(pos.x + size.x, pos.y);
    curr_vertex_attribs[LOWER_END].pos = pos + size;
    curr_vertex_attribs[LOWER_START].pos = Vec2(pos.x, pos.y + size.y);

    curr_vertex_attribs[UPPER_START].uv = texture->uv_start;
    curr_vertex_attribs[UPPER_END].uv = Vec2(texture->uv_end.x, texture->uv_start.y);
    curr_vertex_attribs[LOWER_END].uv = texture->uv_end;
    curr_vertex_attribs[LOWER_START].uv = Vec2(texture->uv_start.x, texture->uv_end.y);

    DrawQuad();
}
void DrawPath(const std::vector<Vec2> &points, float thickness, const Vec2& offset)
{
    /*
    * The use of VertexIndex breaks down here. For example, if points[n].x > points[n+1].x,
    * then the UPPER_END vertex is actually to the left of the UPPER_START vertex
    */

    if (points.size() < 2)
    {
        std::cout << "quickdraw::window Cannot draw a path with less than 2 points\n";
        return;
    }

    curr_quad_attribs[2] = thickness;
    SetQuadMode(PATH_MODE);
    // ^ Sets up UVs for PATH_MODE

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
    
    // This method deals with the edges shared by the quads used
    // to draw the path. Each edge's slope is equal to the normal of
    // the path at the point the edge lies on. Each edge's length is
    // equal to thickness. Filling the edges between two points
    // gives a quad which will be drawn

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
    curr_vertex_attribs[UPPER_START].pos               = start_edge_slope + points[0] + offset;
    curr_vertex_attribs[UPPER_START].fill_color        = glm::mix(saved_vertex_attribs[UPPER_START].fill_color,    saved_vertex_attribs[UPPER_END].fill_color,    length_at_point[0] / path_length);
    curr_vertex_attribs[UPPER_START].outline_color     = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[0] / path_length);

    curr_vertex_attribs[UPPER_END].pos              = end_edge_slope + points[1] + offset;
    curr_vertex_attribs[UPPER_END].fill_color       = glm::mix(saved_vertex_attribs[UPPER_START].fill_color,    saved_vertex_attribs[UPPER_END].fill_color,    length_at_point[1] / path_length);
    curr_vertex_attribs[UPPER_END].outline_color    = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[1] / path_length);

    curr_vertex_attribs[LOWER_END].pos           = -end_edge_slope + points[1] + offset;
    curr_vertex_attribs[LOWER_END].fill_color    = glm::mix(saved_vertex_attribs[LOWER_START].fill_color,    saved_vertex_attribs[LOWER_END].fill_color,    length_at_point[1] / path_length);
    curr_vertex_attribs[LOWER_END].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[1] / path_length);

    curr_vertex_attribs[LOWER_START].pos            = -start_edge_slope + points[0] + offset;
    curr_vertex_attribs[LOWER_START].fill_color     = glm::mix(saved_vertex_attribs[LOWER_START].fill_color,    saved_vertex_attribs[LOWER_END].fill_color,    length_at_point[0] / path_length);
    curr_vertex_attribs[LOWER_START].outline_color  = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[0] / path_length);

    // draw the quad
    DrawQuad();

    if (points.size() > 2)
    {
        // Draws each line except the first and last one
        size_t k = 1;
        while (k < points.size() - 2)
        {
            // We get the start edge data from the previous end edge
            curr_vertex_attribs[UPPER_START]    = curr_vertex_attribs[UPPER_END];
            curr_vertex_attribs[LOWER_START] = curr_vertex_attribs[LOWER_END];

            // Use the next normal from the last line as the normal for the current
            // line
            line_normal = next_normal;

            // Set the next normal to be the normal of the line after the current line
            next_normal = Normal(points[k + 2] - points[k + 1]);

            // Find the slope of the end edge
            end_edge_slope = -(line_normal + next_normal) * end_edge_slope_coeff;

            // Set the end edge data
            curr_vertex_attribs[UPPER_END].pos              = end_edge_slope + points[k + 1] + offset;
            curr_vertex_attribs[UPPER_END].fill_color       = glm::mix(saved_vertex_attribs[UPPER_START].fill_color,    saved_vertex_attribs[UPPER_END].fill_color,    length_at_point[k + 1] / path_length);
            curr_vertex_attribs[UPPER_END].outline_color    = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[k + 1] / path_length);

            curr_vertex_attribs[LOWER_END].pos           = -end_edge_slope + points[k + 1] + offset;
            curr_vertex_attribs[LOWER_END].fill_color    = glm::mix(saved_vertex_attribs[LOWER_START].fill_color,    saved_vertex_attribs[LOWER_END].fill_color,    length_at_point[k + 1] / path_length);
            curr_vertex_attribs[LOWER_END].outline_color = glm::mix(saved_vertex_attribs[LOWER_START].outline_color, saved_vertex_attribs[LOWER_END].outline_color, length_at_point[k + 1] / path_length);
            
            DrawQuad();
            k++;
        }

        // draw the last line
        end_edge_slope = -next_normal * thickness;
        curr_vertex_attribs[UPPER_START]                   = curr_vertex_attribs[UPPER_END];
        curr_vertex_attribs[LOWER_START]                = curr_vertex_attribs[LOWER_END];
        curr_vertex_attribs[UPPER_END].pos              = end_edge_slope  + points[k + 1] + offset;
        curr_vertex_attribs[UPPER_END].fill_color       = glm::mix(saved_vertex_attribs[UPPER_START].fill_color,    saved_vertex_attribs[UPPER_END].fill_color,    length_at_point[k + 1] / path_length);
        curr_vertex_attribs[UPPER_END].outline_color    = glm::mix(saved_vertex_attribs[UPPER_START].outline_color, saved_vertex_attribs[UPPER_END].outline_color, length_at_point[k + 1] / path_length);
        curr_vertex_attribs[LOWER_END].pos           = -end_edge_slope + points[k + 1] + offset;
        curr_vertex_attribs[LOWER_END].fill_color    = glm::mix(saved_vertex_attribs[LOWER_START].fill_color,    saved_vertex_attribs[LOWER_END].fill_color,    length_at_point[k + 1] / path_length);
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
    curr_vertex_attribs[loc].fill_color = color;
}
void SetFillColor(const RGBA& color)
{
    SetFillColor(color, UPPER_START);
    SetFillColor(color, UPPER_END);
    SetFillColor(color, LOWER_END);
    SetFillColor(color, LOWER_START);
}
void SetOutlineColor(const RGBA& color, ShapeLoc loc)
{
    curr_vertex_attribs[loc].outline_color = color;
}
void SetOutlineColor(const RGBA& color)
{
    SetOutlineColor(color, UPPER_START);
    SetOutlineColor(color, UPPER_END);
    SetOutlineColor(color, LOWER_END);
    SetOutlineColor(color, LOWER_START);
}
void SetRectRoundedMask(bool mask, ShapeLoc loc)
{
    curr_vertex_attribs[loc].corner_mask = mask;
}
void SetRectRoundedMask(bool mask)
{
    SetRectRoundedMask(mask, UPPER_START);
    SetRectRoundedMask(mask, UPPER_END);
    SetRectRoundedMask(mask, LOWER_END);
    SetRectRoundedMask(mask, LOWER_START);
}
void SetOutlineThickness(float thickness)
{
    curr_quad_attribs[5] = thickness;
}
void SetRectCornerSize(float size)
{
    curr_quad_attribs[6] = size;
}
Vec2 TextSize(const std::string& str)
{
    return Vec2(GetTextWidth(str), GetTextHeight());
}
void SetTextScale(float s)
{
    curr_text_scale = s;
}
void EnableScissor(const Vec2& pos, const Vec2& size)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor((GLint)pos.x, (GLint)(viewport_dim.y - pos.y - size.y), (GLint)size.x, (GLint)size.y);
}
void DisableScissor()
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
void AbstractObserver::set_enabled(bool flag)
{
    enabled_ = flag;
}
bool AbstractObserver::enabled()
{
    return enabled_;
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

namespace mouse
{
    namespace
    {
        void ButtonCallback(GLFWwindow* window_ptr, int button, int action, int mods)
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
        void CursorCallback(GLFWwindow* window_ptr, double x, double y)
        {
            double mx, my;
            glfwGetCursorPos(window_ptr, &mx, &my);
            pos_state.new_state(Vec2(mx, my));
            delta_state = pos_state.current - pos_state.previous;
            QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_move);
            cursor_update_received = true;
        }
        void ScrollCallback(GLFWwindow* window_ptr, double xoffset, double yoffset)
        {
            scroll_update_received = true;
            scroll_state.new_state((int)yoffset);
            QUICKDRAW_NOTIFY_OBSERVERS(Observer, observers, on_mouse_scroll);
        }
        void ProcessEvents()
        {
            // Make sure to set scroll state to 0 if we aren't scrolling
            if (!scroll_update_received && scroll_state.current != 0)
            {
                ScrollCallback(glfw_window_handle, 0, 0);
            }

            // Make sure to set delta to 0 if mouse hasn't moved.
            if (!cursor_update_received && delta_state != Vec2(0))
            {
                delta_state = Vec2(0);
            }

            if (!mouse_button_update_received)
            {
                for (auto& bs : button_states)
                {
                    if (bs.current != bs.previous)
                        bs.new_state(bs.current);
                }
            }
            scroll_update_received = false;
            cursor_update_received = false;
            mouse_button_update_received = false;
        }
        void Init()
        {
            for (auto& st : button_states)
            {
                st.current = st.previous = UP;
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
            glfwSetMouseButtonCallback(glfw_window_handle, ButtonCallback);
        }
    }

    bool IsDown()
    {
        for (auto& s : button_states)
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
        for (auto& s : button_states)
        {
            if (s.current == DOWN && s.previous == UP)
                return true;
        }
        return false;
    }
    bool IsReleased()
    {
        for (auto& s : button_states)
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
        glfwSetCursor(glfw_window_handle, cursor_ptrs[cursor - ARROW]);
    }
    void SetCursorEnabled(bool flag)
    {
        glfwSetInputMode(glfw_window_handle, GLFW_CURSOR, flag ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
} // namespace mouse
namespace keyboard
{
    namespace
    {
        void KeyCallback(GLFWwindow* window_ptr, int key, int scancode, int action, int mods)
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
        void Init()
        {
            glfwSetKeyCallback(glfw_window_handle, KeyCallback);
        }
    }
    const std::set<int>& DownKeys()
    {
        return down_keys.current;
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
}
}


