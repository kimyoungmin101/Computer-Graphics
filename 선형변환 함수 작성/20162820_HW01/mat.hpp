#ifndef KMUVCL_GRAPHICS_MAT_HPP
#define KMUVCL_GRAPHICS_MAT_HPP

#include <iostream>
#include <cstring>
#include <cstdarg>

namespace kmuvcl {
  namespace math {

    template <unsigned int M, unsigned int N, typename T>
    class mat
    {
    public:
      mat()
      {
        set_to_zero();
      }

      mat(const T elem)
      {
        std::fill(val, val + M*N, elem);
      }

      T& operator()(unsigned int r, unsigned int c)
      {
        return  val[(c*M)+r];
      }

      const T& operator()(unsigned int r, unsigned int c) const
      {
        // TODO: Fill up this function properly 
        // Notice: The matrix is column major
        return  val[(c*M)+r];   
      }

      // type casting operators
      operator const T* () const
      {
        return  val;
      }

      operator T* ()
      {
        return  val;
      }

      void set_to_zero()
      {
		for (int a=0; a<M*N; a++)
			val[a] = 0; 
      }

      void get_ith_column(unsigned int i, vec<M, T>& col) const
      {
		for (int b=0; b<M; b++)
			col(b) = val[(i*M)+b]; 
      }

      void set_ith_column(unsigned int i, const vec<M, T>& col)
      {
		for (int b=0; b<M; b++)
			val[(b*M)+i] = col(b);
      }

      void get_ith_row(unsigned int i, vec<N, T>& row) const
      {
		for (int b=0; b<N; b++)
			row(b) = val[(b*M)+i];
      }

      void set_ith_row(unsigned int i, const vec<N, T>& row)
      {
		for (int b=0; b<N; b++)
			val[(b*M)+i] = row(b);
      }

      mat<N, M, T> transpose() const
      {
        mat<N, M, T>  trans;

		for (int c=0; c<N; c++)
		{
			for (int d=0; d<M; d++)
			{
				trans(d,c) = val[(d*M)+c];
			}
		}
        return  trans;
      }

    protected:
      T val[M*N];   // column major
    };

    typedef mat<3, 3, float>    mat3x3f;
    typedef mat<3, 3, double>   mat3x3d;

    typedef mat<4, 4, float>    mat4x4f;
    typedef mat<4, 4, double>   mat4x4d;

  } // math
} // kmuvcl

#include "operator.hpp"

#endif // #ifndef KMUVCL_GRAPHICS_MAT_HPP
