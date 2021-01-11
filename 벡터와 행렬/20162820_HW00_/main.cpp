#include <iostream>
#include "vec.hpp"
#include "mat.hpp"

void dim_3x3_test();
void dim_4x4_test();
void testing_vector_to_pointer_type_cast(float* vec, unsigned int sz);
void testing_matrix_to_pointer_type_cast(float* mat, unsigned int sz);


int main()
{
  dim_3x3_test();
  dim_4x4_test();

  return  0;
}

void dim_3x3_test()
{
  kmuvcl::math::vec3f     x(3, 5, 7);
  kmuvcl::math::vec3f     y;
  kmuvcl::math::vec3f     z;

  kmuvcl::math::mat3x3f   A;
  kmuvcl::math::mat3x3f   B;

  kmuvcl::math::vec3f     row[3];
  kmuvcl::math::vec3f     col[3];


  // vector print
  std::cout << "x = " << x << std::endl;

  // vector = operator  
  y = x;
  std::cout << "y = " << y << std::endl;

  // vector += operator
  y += x;
  std::cout << "y += x" << std::endl;
  std::cout << "y => " << y << std::endl;

  // vector -=operator
  y -= x;
  std::cout << "y -= x" << std::endl;
  std::cout << "y => " << y << std::endl;

  // vector + operator
  std::cout << "x + y => " << x + y << std::endl;

  // vector + operator
  std::cout << "x - y => " << x - y << std::endl;

  // dot product test
  std::cout << "dot(x,y) => " << kmuvcl::math::dot(x, y) << std::endl;

  // cross product test
  x = kmuvcl::math::vec3f(1, 0, 0);
  y = kmuvcl::math::vec3f(0, 1, 0);
  z = kmuvcl::math::cross(x, y);
  std::cout << "reset x as [1, 0, 0]" << std::endl;
  std::cout << "reset y as [0, 1, 0]" << std::endl;
  std::cout << "z = cross(x, y)" << std::endl;
  std::cout << "z = " << z << std::endl;

  
  A(0, 0) =  1;   A(0, 1) =  2;   A(0, 2) = -1;
  A(1, 0) =  2;   A(1, 1) =  1;   A(1, 2) =  0;
  A(2, 0) =  3;   A(2, 1) = -2;   A(2, 2) =  1;

  std::cout << "A = " << std::endl;
  std::cout << A;

  std::cout << "B = A^T" << std::endl;
  B = A.transpose();
  std::cout << "B = " << std::endl;
  std::cout << B;

  A.get_ith_row(0, row[0]);
  A.get_ith_row(1, row[1]);
  A.get_ith_row(2, row[2]);

  std::cout << "A's rows: " << std::endl;
  std::cout << "A's row[0] = " << row[0] << std::endl;
  std::cout << "A's row[1] = " << row[1] << std::endl;
  std::cout << "A's row[2] = " << row[2] << std::endl;
  
  B.get_ith_column(0, col[0]);
  B.get_ith_column(1, col[1]);
  B.get_ith_column(2, col[2]);

  std::cout << "B's columns: " << std::endl;
  std::cout << "B's column[0] = " << col[0] << std::endl;
  std::cout << "B's column[1] = " << col[1] << std::endl;
  std::cout << "B's column[2] = " << col[2] << std::endl;

  std::cout << "A*x = " << A * x << std::endl;
  std::cout << "A*y = " << A * y << std::endl;
  std::cout << "A*z = " << A * z << std::endl;

  std::cout << "x*A = " << x * A << std::endl;
  std::cout << "y*A = " << y * A << std::endl;
  std::cout << "z*A = " << z * A << std::endl;

  std::cout << "pointer casting test" << std::endl;
  std::cout << "vector x to pointer casting" << std::endl;
  testing_vector_to_pointer_type_cast(x, 3);
  std::cout << "matrix A to pointer casting" << std::endl;
  testing_matrix_to_pointer_type_cast(A, 3*3);
}

void dim_4x4_test()
{
  kmuvcl::math::vec4f     x(3, 5, 7, 1);
  kmuvcl::math::vec4f     y;
  kmuvcl::math::vec4f     z;
  kmuvcl::math::vec4f     w;

  kmuvcl::math::mat4x4f   A;
  kmuvcl::math::mat4x4f   B;

  kmuvcl::math::vec4f     row[4];
  kmuvcl::math::vec4f     col[4];


  // vector print
  std::cout << "x = " << x << std::endl;

  // vector = operator  
  y = x;
  std::cout << "y = " << y << std::endl;

  // vector += operator
  y += x;
  std::cout << "y += x" << std::endl;
  std::cout << "y => " << y << std::endl;

  // vector -=operator
  y -= x;
  std::cout << "y -= x" << std::endl;
  std::cout << "y => " << y << std::endl;

  // vector + operator
  std::cout << "x + y => " << x + y << std::endl;

  // vector + operator
  std::cout << "x - y => " << x - y << std::endl;

  // dot product test
  std::cout << "dot(x,y) => " << kmuvcl::math::dot(x, y) << std::endl;

  // reseting
  x = kmuvcl::math::vec4f(1, 0, 0, 0);
  y = kmuvcl::math::vec4f(0, 1, 0, 0);
  z = kmuvcl::math::vec4f(0, 0, 1, 0);
  w = kmuvcl::math::vec4f(0, 0, 0, 1);
  std::cout << "reset x as [1, 0, 0, 0]" << std::endl;
  std::cout << "reset y as [0, 1, 0, 0]" << std::endl;
  std::cout << "reset z as [0, 0, 1, 0]" << std::endl;
  std::cout << "reset w as [0, 0, 0, 1]" << std::endl;
  
  A(0, 0) =  1;   A(0, 1) =  2;   A(0, 2) = -1;   A(0, 3) = -1;
  A(1, 0) =  2;   A(1, 1) =  1;   A(1, 2) =  0;   A(1, 3) =  2;
  A(2, 0) =  3;   A(2, 1) = -2;   A(2, 2) =  1;   A(2, 3) =  4;
  A(3, 0) =  0;   A(3, 1) =  0;   A(3, 2) =  0;   A(3, 3) =  1;


  std::cout << "A = " << std::endl;
  std::cout << A;

  std::cout << "B = A^T" << std::endl;
  B = A.transpose();
  std::cout << "B = " << std::endl;
  std::cout << B;

  A.get_ith_row(0, row[0]);
  A.get_ith_row(1, row[1]);
  A.get_ith_row(2, row[2]);
  A.get_ith_row(3, row[3]);

  std::cout << "A's rows: " << std::endl;
  std::cout << "A's row[0] = " << row[0] << std::endl;
  std::cout << "A's row[1] = " << row[1] << std::endl;
  std::cout << "A's row[2] = " << row[2] << std::endl;
  std::cout << "A's row[3] = " << row[3] << std::endl;

  B.get_ith_column(0, col[0]);
  B.get_ith_column(1, col[1]);
  B.get_ith_column(2, col[2]);
  B.get_ith_column(3, col[3]);

  std::cout << "B's columns: " << std::endl;
  std::cout << "B's column[0] = " << col[0] << std::endl;
  std::cout << "B's column[1] = " << col[1] << std::endl;
  std::cout << "B's column[2] = " << col[2] << std::endl;
  std::cout << "B's column[3] = " << col[3] << std::endl;

  std::cout << "A*x = " << A * x << std::endl;
  std::cout << "A*y = " << A * y << std::endl;
  std::cout << "A*z = " << A * z << std::endl;
  std::cout << "A*w = " << A * w << std::endl;

  std::cout << "x*A = " << x * A << std::endl;
  std::cout << "y*A = " << y * A << std::endl;
  std::cout << "z*A = " << z * A << std::endl;
  std::cout << "w*A = " << w * A << std::endl;

  std::cout << "pointer casting test" << std::endl;
  std::cout << "vector x to pointer casting" << std::endl;
  testing_vector_to_pointer_type_cast(x, 4);
  std::cout << "matrix A to pointer casting" << std::endl;
  testing_matrix_to_pointer_type_cast(A, 4 * 4);
}
void testing_vector_to_pointer_type_cast(float* vec, unsigned int sz)
{
  for (unsigned int i = 0; i < sz; ++i)
    std::cout << vec[i] << " ";
  std::cout << std::endl;
}

void testing_matrix_to_pointer_type_cast(float* mat, unsigned int sz)
{
  for (unsigned int i = 0; i < sz; ++i)
    std::cout << mat[i] << " ";
  std::cout << std::endl;
}