#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>

/* assimp include files. These three are usually needed. */
// #include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/cimport.h>
#include <assimp/scene.h>        
#include <assimp/postprocess.h>

  struct Face
  {    
    GLuint index_buffer;
    GLuint num_indices;
  };

  struct vertex
  {
    GLfloat vertices[1000000];
  };

  struct color
  {
    GLfloat colors[1000000];
  };

  struct Index
  {
    GLuint indices[1000000];
  };

  struct Mesh
  {
    GLuint  position_buffer;
    GLuint  color_buffer;
    GLuint num_vertices;
    GLuint num_colors;

    bool    is_color;

    };
////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값

GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치

Face face1;
Mesh mesh1;
vertex vertex1;
color color1;
Index index1;

int elementCount;


GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////
// /// 렌더링 관련 변수 및 함수
// ////////////////////////////////////////////////////////////////////////////////
const aiScene* scene;

bool load_asset(const std::string& filename);
void print_scene_info(const aiScene* scene);
void print_mesh_info(const aiMesh* mesh);

void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

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

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color    = glGetAttribLocation(program, "a_color");
  
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
  mesh1.num_vertices = mesh->mNumVertices;
  mesh1.num_colors = mesh->mNumVertices;
  
  std::cout << "num vertices " << mesh->mNumVertices << std::endl;
  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    aiVector3D vertex = mesh->mVertices[i];


       vertex1.vertices[i*3] = vertex.x;  // 0, 3, 6, 9
       vertex1.vertices[1+(i*3)] = vertex.y; // 1, 4, 7, 10
       vertex1.vertices[2+(i*3)] = vertex.z; // 2, 5, 8, 11

    std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

    if (mesh->mColors[0] != NULL)
    {
    aiColor4D color = mesh->mColors[0][i];
       color1.colors[i*4] = color.r; // 0, 4, 8, 12
       color1.colors[(i*4)+1] = color.g; // 1, 5, 9, 13
       color1.colors[(i*4)+2] = color.b; // 2, 6, 10, 14
       color1.colors[(i*4)+3] = color.a; // 3, 7, 11, 15

      std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
    }
    
    /*if (mesh->mNormals != NULL)
    {
      aiVector3D normal = mesh->mNormals[i];
      std::cout << "  normal  ("  << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
    }*/
  }

  std::cout << "num faces " << mesh->mNumFaces << std::endl;
  elementCount = mesh->mNumFaces * 3;


  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    const aiFace& face = mesh->mFaces[i];
    face1.num_indices = face.mNumIndices;
    

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
      if(j % 3 == 0){
      index1.indices[i*3] = face.mIndices[j];
      }
      else if(j % 3 == 1){
      index1.indices[i*3 + 1] = face.mIndices[j];
      }
      else{
      index1.indices[i*3 + 2] = face.mIndices[j];
      }
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
  glGenBuffers(1, &mesh1.position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, mesh1.position_buffer);
  glBufferData(GL_ARRAY_BUFFER, mesh1.num_vertices * 4 * face1.num_indices, vertex1.vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &mesh1.color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, mesh1.color_buffer);
  glBufferData(GL_ARRAY_BUFFER, mesh1.num_colors * 4 * face1.num_indices, color1.colors, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &face1.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face1.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, face1.num_indices * 4, index1.indices, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
   glUseProgram(program);

   glEnableVertexAttribArray(loc_a_position);
   glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, vertex1.vertices);
   
   glEnableVertexAttribArray(loc_a_color);
   glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, color1.colors);
   
   glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, index1.indices);

   glUseProgram(0);
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "need model filepath!" << std::endl;
    std::cerr << "usage: helloassimp [model_filepath]" << std::endl;
    return -1;
  }

  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Hello Assimp", NULL, NULL);
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

  init_shader_program();

  
  if (!load_asset(argv[1]))
  {
    std::cout << load_asset(argv[1]) << std::endl;
    std::cout << "Failed to load a asset file" << std::endl;
    return -1;
  }


  print_scene_info(scene);

  // GPU의 VBO를 초기화하는 함수 호출
  init_buffer_objects();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_object();

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
