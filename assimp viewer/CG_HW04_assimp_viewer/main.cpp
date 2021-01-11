#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>
#include <cmath>
#include <chrono>

/* assimp include files. These three are usually needed. */
// #include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/cimport.h>
#include <assimp/scene.h>        
#include <assimp/postprocess.h>

#include "projection.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

namespace kmuvcl 
{
  struct Face
  {    
    GLuint index_buffer = 0;
    GLuint num_indices = 0;
  };

  struct Mesh
  {
    GLuint  position_buffer;
    GLuint  texcoord_buffer;
    GLuint  normal_buffer;
    bool    has_texture;  
    std::vector<Face> faces;
    unsigned int material_index;    
  };  
}

const float pi = 3.14159265358979323846;

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값

GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_normal;     // attribute 변수 a_normal 위치
GLint   loc_a_texcoord;   // attribute 변수 a_texcoord 위치

GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치
GLint   loc_u_M;          // uniform 변수 u_M 위치

GLint   loc_u_view_position_wc;       // uniform 변수 u_view_position_wc 위치
GLint   loc_u_light_position_wc;      // uniform 변수 u_light_postion_wc 위치

GLint   loc_u_light_ambient;
GLint   loc_u_light_diffuse;          // uniform 변수 u_light_diffuse 위치
GLint   loc_u_light_specular;         // uniform 변수 u_light_specular 위치

GLint   loc_u_material_ambient;
GLint   loc_u_material_specular;      // uniform 변수 u_material_specular 위치
GLint   loc_u_material_shininess;     // uniform 변수 u_material_shininess 위치

GLint   loc_u_diffuse_texture;

std::vector<kmuvcl::Mesh> meshes;

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
aiMatrix4x4 mat_model, mat_view, mat_proj;
aiMatrix4x4 mat_PVM;

float g_angle = 0.0;
bool  g_is_animation = false;
std::chrono::time_point<std::chrono::system_clock> prev, curr;

enum proj_mode {kortho, kperspective};
proj_mode mode_ = kortho;

proj_mode mode();
void set_mode(proj_mode _mode);

void set_transform();
////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// /// 렌더링 관련 변수 및 함수
// ////////////////////////////////////////////////////////////////////////////////
const aiScene* scene;

std::string basepath;

std::map<std::string, GLuint> texture_map;

aiVector3D view_position_wc;
aiVector3D light_position_wc = aiVector3D(1.0f, 10.0f, 10.0f);

aiColor4D light_ambient      = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
aiColor4D light_diffuse      = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
aiColor4D light_specular     = aiColor4D(0.5f, 0.5f, 0.5f, 1.0f);

aiColor4D material_ambient   = aiColor4D(0.0f, 0.0f, 0.0f, 1.0f);
aiColor4D material_specular  = aiColor4D(0.5f, 0.5f, 0.5f, 1.0f);
float     material_shininess = 60.0f;

bool load_asset(const std::string& filename);
void print_scene_info(const aiScene* scene);
void print_mesh_info(const aiMesh* mesh);

void init();
void init_texture_object();
void init_buffer_objects();     

void draw_scene();
void draw_node_recursive(const aiNode* node, const aiMatrix4x4t<float>& mat_model);
void draw_mesh(const aiMesh* mesh, const aiMatrix4x4t<float>& mat_model);         

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void init()
{
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  glFrontFace(GL_CCW); 

  camera.mPosition = aiVector3D(0.0f, 0.5f, 1.0f);

  camera.mClipPlaneNear = 0.1f;
  camera.mClipPlaneFar = 100.0f; 
  camera.mHorizontalFOV = pi/2.0f; // 90 degree
  camera.mAspect = 1.0f;
}

// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
     std::istreambuf_iterator<char>());

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar**)&shader_src, NULL);
  glCompileShader(shader);

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");  
  loc_u_M   = glGetUniformLocation(program, "u_M");

  
  loc_u_view_position_wc = glGetUniformLocation(program, "u_view_position_wc");
  loc_u_light_position_wc = glGetUniformLocation(program, "u_light_position_wc");
  
  loc_u_light_ambient     = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse     = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular    = glGetUniformLocation(program, "u_light_specular");

  loc_u_material_ambient   = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_specular  = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");

  loc_u_diffuse_texture    = glGetUniformLocation(program, "u_diffuse_texture");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_normal   = glGetAttribLocation(program, "a_normal");
  loc_a_texcoord = glGetAttribLocation(program, "a_texcoord");

}

bool load_asset(const std::string& filename)
{
  std::cout << "load asset: " << filename << std::endl;
  
  size_t pos = filename.rfind("/");
  basepath = filename.substr(0, pos + 1);

  // Assimp::Importer importer;
  // scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
  scene = aiImportFile(filename.c_str(), 
                       aiProcessPreset_TargetRealtime_MaxQuality);
  if (scene != NULL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void print_mesh_info(const aiMesh* mesh)
{
  std::cout << "print mesh " << mesh->mName.data <<  std::endl;
  std::cout << "num vertices " << mesh->mNumVertices << std::endl;

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    aiVector3D vertex = mesh->mVertices[i];
    // std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
    
    if (mesh->mNormals != NULL)
    {
      aiVector3D normal = mesh->mNormals[i];
      // std::cout << "  normal  ("  << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
    }

    if (mesh->mTextureCoords[0] != NULL)
    {
      aiVector3D texcoord = mesh->mTextureCoords[0][i];
      // std::cout << "  texcoord  (" << texcoord.x << ", " <<  texcoord.y << ", " << texcoord.z << ")" << std::endl;
    }
  }

  std::cout << "num faces " << mesh->mNumFaces << std::endl;
  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    const aiFace& face = mesh->mFaces[i];

    if (face.mNumIndices == 1)
    {
      // std::cout << "  point" << std::endl;
    }
    else if (face.mNumIndices == 2)
    {
      // std::cout << "  line" << std::endl;
    }
    else if (face.mNumIndices == 3)
    {
      // std::cout << "  triangle" << std::endl;
    }
    else
    {
      // std::cout << " polygon" << std::endl;
    }

    for (int j = 0; j < face.mNumIndices; ++j)
    {
      // std::cout << "    index " << face.mIndices[j] << std::endl;
    }
  }
}

void print_material_info(const aiMaterial* material)
{
  std::cout << "print material info" << std::endl;
  
  for (int i = 0; i < scene->mNumMaterials; i++) 
  {
    aiMaterial* currentMaterial = scene->mMaterials[i];
    aiString materialName;

    // material name
    currentMaterial->Get(AI_MATKEY_NAME, materialName);
    std::cout << "material name: " << materialName.C_Str() << std::endl;

    // diffuse texture of material
    unsigned int diffuseTextureCount = currentMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		std::cout << "Diffuse texture count " << diffuseTextureCount << std::endl;
    for (unsigned int j = 0; j < diffuseTextureCount; j++) 
    {
      aiString textureFilePath;
      if (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 
          j, &textureFilePath)) 
      {
        std::cout << "Diffuse Texture file: " << textureFilePath.data << std::endl;
      }
      else 
      {
        std::cout << "no exist diffuseTexture." << std::endl;
      }
    }
  }
}

void print_scene_info(const aiScene* scene)
{
  std::cout << "print scene info" << std::endl;
  std::cout << "num meshes in the scene " << scene->mNumMeshes << std::endl;
  for (int i = 0; i < scene->mNumMeshes; ++i)
  {
    const aiMesh* mesh = scene->mMeshes[i];
    print_mesh_info(mesh);
  }

  std::cout << "num materials in the scene " << scene->mNumMaterials << std::endl;
  for (int i = 0; i < scene->mNumMaterials; ++i)
  {
    const aiMaterial* material = scene->mMaterials[i];
    print_material_info(material);
  }
}

void init_buffer_objects()
{
  // TODO: Fill this function to initiazlize buffer objects
    
}

void init_texture_objects()
{   
  // TODO: Fill this function to initiazlize texture objects
}

void print_matrix(const std::string& log, const aiMatrix4x4& m)
{
  // debugging current matrix
  std::cout << log << std::endl;
  std::cout << m.a1 << ", " << m.a2 << ", " << m.a3 << ", " << m.a4 << std::endl;
  std::cout << m.b1 << ", " << m.b2 << ", " << m.b3 << ", " << m.b4 << std::endl;
  std::cout << m.c1 << ", " << m.c2 << ", " << m.c3 << ", " << m.c4 << std::endl;
  std::cout << m.d1 << ", " << m.d2 << ", " << m.d3 << ", " << m.d4 << std::endl;
}

void set_transform()
{ 
  camera.GetCameraMatrix(mat_view);

  if (mode() == kortho)
    kmuvcl::ortho(-1, 1, -1, 1, mat_proj);
  else if (mode() == kperspective)
    kmuvcl::perspective(mat_proj);
  
  aiMatrix4x4::RotationY(g_angle*pi/180.0, mat_model);
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  // light translate
  if (key == GLFW_KEY_A  && action == GLFW_PRESS)
    light_position_wc[0] -= 1.0f;
  else if (key == GLFW_KEY_D  && action == GLFW_PRESS)
    light_position_wc[0] += 1.0f;
  else if (key == GLFW_KEY_W  && action == GLFW_PRESS)
    light_position_wc[1] -= 1.0f;
  else if (key == GLFW_KEY_S  && action == GLFW_PRESS)
    light_position_wc[1] += 1.0f;
  else if (key == GLFW_KEY_Z  && action == GLFW_PRESS)
    light_position_wc[2] -= 1.0f;
  else if (key == GLFW_KEY_C  && action == GLFW_PRESS)
    light_position_wc[2] += 1.0f;

  else if (key == GLFW_KEY_LEFT  && action == GLFW_PRESS)
    camera.mPosition[0] -= 0.1f;
  else if (key == GLFW_KEY_RIGHT  && action == GLFW_PRESS)
    camera.mPosition[0] += 0.1f;
  else if (key == GLFW_KEY_N  && action == GLFW_PRESS)
    camera.mPosition[1] -= 0.1f;
  else if (key == GLFW_KEY_M  && action == GLFW_PRESS)
    camera.mPosition[1] += 0.1f;
  else if (key == GLFW_KEY_UP  && action == GLFW_PRESS)
    camera.mPosition[2] -= 0.1f;
  else if (key == GLFW_KEY_DOWN  && action == GLFW_PRESS)
    camera.mPosition[2] += 0.1f;
  else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
  {
    set_mode(mode() == kortho ? kperspective : kortho);    
  }

  else if (key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    g_is_animation = !g_is_animation;
    std::cout << (g_is_animation ? "animation" : "no animation") << std::endl;

  }
  // {
  //   const aiVector3D& v = light_position_wc;
  //   std::cout << "light position: " <<  v[0] << ", " << v[1] << ", " << v[2] << std::endl;
  // }
  
  // {
  //   const aiVector3D& v = camera.mPosition;
  //   std::cout << "camera position: " <<  v[0] << ", " << v[1] << ", " << v[2] << std::endl;
  // }
}

proj_mode mode()
{
  return mode_;
}

void set_mode(proj_mode _mode)
{
  mode_ = _mode;
}

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  camera.mAspect = (float)width / (float)height;
}

void draw_scene()
{
  const aiNode* node = scene->mRootNode;
  draw_node_recursive(node, mat_model);
  
}

void draw_node_recursive(const aiNode* node, const aiMatrix4x4& mat_parent)
{
  aiMatrix4x4 mat_curr = mat_parent*node->mTransformation; 

  // draw node
  for (int i = 0; i < node->mNumMeshes; ++i)
  {
    draw_mesh(scene->mMeshes[node->mMeshes[i]], mat_curr);
  }
  
  // draw all node
  for (int i = 0; i < node->mNumChildren; ++i)
  {
    draw_node_recursive(node->mChildren[i], mat_curr);
  }
}

void draw_mesh(const aiMesh* mesh, const aiMatrix4x4& mat_model)
{
  // TODO: Fill this function to render the scene

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program); 
    
  aiMatrix4x4 mat_PVM = mat_proj*mat_view*mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, (float*)&mat_PVM.Transpose());

  aiMatrix4x4 m = mat_model;
  glUniformMatrix4fv(loc_u_M, 1, GL_FALSE, (float*)&m.Transpose());
  
  glUniform3fv(loc_u_view_position_wc, 1, (float*)&camera.mPosition);   // view position


  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "neeed model filepath!" << std::endl;
    std::cerr << "usage: ./viewer [model_filepath]" << std::endl;
    return -1;
  }
  
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Assimp Viewer", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // Make the current OpenGL context as one in the window
  glfwMakeContextCurrent(window);

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
  {
    std::cout << "GLEW Init Error!" << std::endl;
  }

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;
  
  init();
  init_shader_program();

  if (!load_asset(argv[1]))
  {
    std::cout << "Failed to load a asset file" << std::endl;
    return -1;
  }

  print_scene_info(scene);

  init_buffer_objects();
  init_texture_objects();
  
  glfwSetKeyCallback(window, key_callback);
  
  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  prev = curr = std::chrono::system_clock::now();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_transform();
    draw_scene();

    curr = std::chrono::system_clock::now();
    std::chrono::duration<float> elaped_seconds = (curr - prev);
    prev = curr;

    if (g_is_animation)
    {
      g_angle += 30.0f * elaped_seconds.count();
      if (g_angle > 360.0f)
        g_angle = 0.0f;
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
