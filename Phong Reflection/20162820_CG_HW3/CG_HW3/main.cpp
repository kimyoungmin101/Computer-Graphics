#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>
#include <chrono>

/* assimp include files. These three are usually needed. */
// #include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/cimport.h>
#include <assimp/scene.h>        
#include <assimp/postprocess.h>

#include "Camera.h"
#include "../common/vec.hpp"
#include "../common/transform.hpp"

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
     GLuint  color_buffer;
     bool is_color;

     GLuint  normal_buffer;

     std::vector<Face> faces;    
   };
 }

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값


GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
GLint   loc_a_normal;     // attribute 변수 a_normal 위치

GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치
GLint   loc_u_M;          // uniform 변수 u_M 위치

GLint   loc_u_view_position_wc;       // uniform 변수 u_view_position_wc 위치
GLint   loc_u_light_position_wc;      // uniform 변수 u_light_postion_wc 위치

GLint   loc_u_light_ambient;          // uniform 변수 u_light_ambient 위치
GLint   loc_u_light_diffuse;          // uniform 변수 u_light_diffuse 위치
GLint   loc_u_light_specular;         // uniform 변수 u_light_specular 위치

GLint   loc_u_material_ambient;       // uniform 변수 u_material_ambient 위치
GLint   loc_u_material_diffuse;       // uniform 변수 u_material_diffuse 위치
GLint   loc_u_material_specular;      // uniform 변수 u_material_specular 위치
GLint   loc_u_material_shininess;     // uniform 변수 u_material_shininess 위치



std::vector<kmuvcl::Mesh> meshes;



GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

float   g_angle = 0.0;
bool    g_is_animation = false;
float   g_translatelight_x = 3.0, g_translatelight_y = 5.0, g_translatelight_z = 10.0;

std::chrono::time_point<std::chrono::system_clock> prev, curr;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// /// 렌더링 관련 변수 및 함수
// ////////////////////////////////////////////////////////////////////////////////
const aiScene* scene;

kmuvcl::math::vec3f view_position_wc;

kmuvcl::math::vec3f light_position_wc;

kmuvcl::math::vec4f light_ambient     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_diffuse     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_specular    = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

kmuvcl::math::vec4f material_ambient   = kmuvcl::math::vec4f(0.0f, 0.0f, 0.0f, 1.0f);
kmuvcl::math::vec4f material_diffuse   = kmuvcl::math::vec4f(0.0f, 1.0f, 0.0f, 1.0f);
kmuvcl::math::vec4f material_specular  = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
float               material_shininess = 60.0f;

bool load_asset(const std::string& filename);
void print_scene_info(const aiScene* scene);
void print_mesh_info(const aiMesh* mesh);

void init_buffer_objects();     
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 및 뷰포트 관련 변수
////////////////////////////////////////////////////////////////////////////////
Camera  camera;
float   g_aspect = 1.0f;

void init()
{
  glEnable(GL_DEPTH_TEST);

  camera.set_near(0.1f);
  camera.set_far(100.0f);
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
  loc_u_material_diffuse   = glGetUniformLocation(program, "u_material_diffuse");
  loc_u_material_specular  = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");


  loc_a_color    = glGetAttribLocation(program, "a_color");
  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_normal   = glGetAttribLocation(program, "a_normal");

}

bool load_asset(const std::string& filename)
{
  // Assimp::Importer importer;
  // scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
  scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
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
  std::cout << "print mesh info" << std::endl;

  std::cout << "num vertices " << mesh->mNumVertices << std::endl;
  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    aiVector3D vertex = mesh->mVertices[i];
    std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
    
    if (mesh->mNormals != NULL)
    {
      aiVector3D normal = mesh->mNormals[i];
      std::cout << "  normal  ("  << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
    }
  }

  std::cout << "num faces " << mesh->mNumFaces << std::endl;
  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    const aiFace& face = mesh->mFaces[i];

    if (face.mNumIndices == 1)
    {
      std::cout << "  point" << std::endl;
    }
    else if (face.mNumIndices == 2)
    {
      std::cout << "  line" << std::endl;
    }
    else if (face.mNumIndices == 3)
    {
      std::cout << "  triangle" << std::endl;
    }
    else
    {
      std::cout << " polygon" << std::endl;
    }

    for (int j = 0; j < face.mNumIndices; ++j)
    {
      std::cout << "    index " << face.mIndices[j] << std::endl;
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
}


void init_buffer_objects()
{
   for (int i = 0; i < scene->mNumMeshes; ++i)
  {
    const aiMesh* mesh = scene->mMeshes[i];

    kmuvcl::Mesh mesh_object;

    glGenBuffers(1, &mesh_object.position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object.position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->mVertices[0]) * mesh->mNumVertices, &mesh->mVertices[0].x, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh_object.normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object.normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->mNormals[0]) * mesh->mNumVertices, &mesh->mNormals[0].x, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh_object.color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object.color_buffer);
    if (mesh->mColors[0] != NULL)
    {
      glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->mColors[0][0]) * mesh->mNumVertices, &mesh->mColors[0][0].r, GL_STATIC_DRAW);
      mesh_object.is_color = true;
    }    
    
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
      const aiFace& face = mesh->mFaces[i];
      
      kmuvcl::Face face_object;
      face_object.num_indices = face.mNumIndices;

      glGenBuffers(1, &face_object.index_buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_object.index_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face.mIndices[0]) * face.mNumIndices, &face.mIndices[0], GL_STATIC_DRAW);
      
      mesh_object.faces.push_back(face_object);
    }

    meshes.push_back(mesh_object);
  }  
}

void set_transform()
{
  kmuvcl::math::vec3f eye     = camera.position();
  kmuvcl::math::vec3f up      = camera.up_direction();
  kmuvcl::math::vec3f center  = eye + camera.front_direction();

  mat_view = kmuvcl::math::lookAt(eye[0], eye[1], eye[2],
                                  center[0], center[1], center[2],
                                  up[0], up[1], up[2]);

  float n = camera.near();
  float f = camera.far();

  if (camera.mode() == Camera::kOrtho)
  {
    float l = camera.left();
    float r = camera.right();
    float b = camera.bottom();
    float t = camera.top();

    mat_proj = kmuvcl::math::ortho(l, r, b, t, n, f);
  }
  else if (camera.mode() == Camera::kPerspective)
  {
    mat_proj = kmuvcl::math::perspective(camera.fovy(), g_aspect, n, f);
  }

  // set object transformation

  mat_model = kmuvcl::math::rotate(g_angle*0.7f, 0.0f, 0.0f, 1.0f);
  mat_model = kmuvcl::math::rotate(g_angle*1.0f, 0.0f, 1.0f, 0.0f)*mat_model;
  mat_model = kmuvcl::math::rotate(g_angle*0.5f, 1.0f, 0.0f, 0.0f)*mat_model;
  mat_model = kmuvcl::math::translate(0.0f, 0.0f, -4.0f)*mat_model;
  
}
/*
’Q’ 키를 누르면 camera의 mode가 바뀌어야 한다.
예) ’Orthogonal mode’ ↔ ’perspective mode’
’L’ 키를 누르면 light가 -x축 방향으로 이동이 되어야한다.
’R’ 키를 누르면 light가 +x축 방향으로 이동이 되어야한다.
’D’ 키를 누르면 light가 -y축 방향으로 이동이 되어야한다.
’U’ 키를 누르면 light가 +y축 방향으로 이동이 되어야한다.
’B’ 키를 누르면 light가 -z축 방향으로 이동이 되어야한다.
’F’ 키를 누르면 light가 +z축 방향으로 이동이 되어야한다.
’P’ 키를 누르면 model이 회전이 되도록한다.

*/
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{

  // TODO: Fill this function to keyboard setting
  if (key == GLFW_KEY_P && action == GLFW_PRESS){
    g_angle += 10.0f;
}

 if (key == GLFW_KEY_L && action == GLFW_PRESS){
    g_translatelight_x -= 10;
}

if (key == GLFW_KEY_R && action == GLFW_PRESS){
    g_translatelight_x += 10;
}
if (key == GLFW_KEY_D && action == GLFW_PRESS){
    g_translatelight_y -= 10;
}
if (key == GLFW_KEY_U && action == GLFW_PRESS){
    g_translatelight_y += 10;
}
if (key == GLFW_KEY_F && action == GLFW_PRESS){
    g_translatelight_z += 10;
}

if (key == GLFW_KEY_B && action == GLFW_PRESS){
    g_translatelight_z -= 10;
}

  if (key == GLFW_KEY_H && action == GLFW_PRESS)
  {
    g_is_animation = !g_is_animation;
    std::cout << (g_is_animation ? "animation" : "no animation") << std::endl;
  }

    if(key == GLFW_KEY_Q && action == GLFW_PRESS)
{
        camera.set_mode(camera.mode() == Camera::kOrtho ? Camera::kPerspective : Camera::kOrtho);
}

}
void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  g_aspect = (float)width / (float)height;
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);
  glUniformMatrix4fv(loc_u_M, 1, GL_FALSE, mat_model);

  glUniform3fv(loc_u_view_position_wc, 1, camera.position());


  glUniform3f(loc_u_light_position_wc, g_translatelight_x, g_translatelight_y, g_translatelight_z);
  glUniform4f(loc_u_light_ambient, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(loc_u_light_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(loc_u_light_specular, 1.0f, 1.0f, 1.0f, 1.0f);

  glUniform4f(loc_u_material_ambient, 0.0f, 0.0f, 0.0f, 1.0f);
  glUniform4f(loc_u_material_diffuse, 1.0f, 0.0f, 0.0f, 1.0f);
  glUniform4f(loc_u_material_specular, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform1f(loc_u_material_shininess, 100.0f);



  for (int i = 0; i < meshes.size(); ++i)
  {
    const kmuvcl::Mesh& mesh = meshes[i];

    glBindBuffer(GL_ARRAY_BUFFER, mesh.position_buffer);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.normal_buffer);
    glEnableVertexAttribArray(loc_a_normal);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    if (mesh.is_color)
    {
      glBindBuffer(GL_ARRAY_BUFFER, mesh.color_buffer);
      glEnableVertexAttribArray(loc_a_color);
      glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }
    
    for (int j = 0; j < mesh.faces.size(); ++j)
    {
      const kmuvcl::Face& face = mesh.faces[j];
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.index_buffer);
      glDrawElements(GL_TRIANGLES, face.num_indices, GL_UNSIGNED_INT, (void*)0);
    }    

    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_normal);
    if (mesh.is_color)
    {
      glDisableVertexAttribArray(loc_a_color);
    }
  }

  glUseProgram(0);
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "neeed model filepath!" << std::endl;
    std::cerr << "usage: Phongassimp [model_filepath]" << std::endl;
    return -1;
  }

  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Phong reflection Assimp", NULL, NULL);
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

  glfwSetKeyCallback(window, key_callback);

  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_transform();
    render_object();

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
