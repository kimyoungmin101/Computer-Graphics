#include <iostream>
#include "vec.hpp"
#include "mat.hpp"
#include "transform.hpp"

int main()
{
  kmuvcl::math::mat4x4f mat_Translate;
  kmuvcl::math::mat4x4f mat_Rotate;
  kmuvcl::math::mat4x4f mat_Scale;
  kmuvcl::math::mat4x4f mat_LookAt;
  kmuvcl::math::mat4x4f mat_Ortho;
  kmuvcl::math::mat4x4f mat_Frustum;
  kmuvcl::math::mat4x4f mat_Perspective;

  mat_Translate = kmuvcl::math::translate(1.0f, -1.0f, 2.0f);
  std::cout << "Translation matrix" << std::endl;
  std::cout << mat_Translate << std::endl;

  mat_Rotate = kmuvcl::math::rotate(90.0f, 1.0f, 2.0f, -1.0f);
  std::cout << "Rotation matrix" << std::endl;
  std::cout << mat_Rotate << std::endl;

  mat_Scale = kmuvcl::math::scale(2.0f, 1.0f, 1.5f);
  std::cout << "Scaling matrix" << std::endl;
  std::cout << mat_Scale << std::endl;

  mat_LookAt = kmuvcl::math::lookAt(0.0f, 0.0f, -5.0f,
                                    0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f);
  std::cout << "View matrix with lookAt()" << std::endl;
  std::cout << mat_LookAt << std::endl;

  mat_Ortho = kmuvcl::math::ortho(1.0f, -1.0f, 
                                  1.0f, -1.0f, 
                                  1.0f, -1.0f);
  std::cout << "Projection matrix with ortho()" << std::endl;
  std::cout << mat_Ortho << std::endl;

  mat_Frustum = kmuvcl::math::frustum(-0.1f, 0.1f, 
                                      -0.1f, 0.1f,
                                       0.1f, 1000.0f);
  std::cout << "Projection matrix with frusutm()" << std::endl;
  std::cout << mat_Frustum << std::endl;

  mat_Perspective = kmuvcl::math::perspective(60.0f, 1.0f, 0.001f, 10000.0f);
  std::cout << "Projection matrix with perspective()" << std::endl;
  std::cout << mat_Perspective << std::endl;


  return  0;
}
