#ifndef KMUCS_GRAPHICS_TRANSFORM_HPP
#define KMUCS_GRAPHICS_TRANSFORM_HPP

#include <cmath>
#include <vector>
#include "vec.hpp"
#include "mat.hpp"
#include "operator.hpp"

namespace kmuvcl
{
    namespace math
    {
#ifndef M_PI
        const float M_PI = 3.14159265358979323846f;
#endif

        template <typename T>
        mat<4, 4, T> translate(T dx, T dy, T dz)
        {
            mat<4, 4, T> translateMat;
            translateMat(0, 3) = dx;
            translateMat(1, 3) = dy;
            translateMat(2, 3) = dz;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (i == j) {
                        translateMat(j,i) = 1;
                    }
                    
                }
            }

            return translateMat;
        }

        template <typename T>
        mat<4, 4, T> rotate(T angle, T x, T y, T z)
        {
            mat<4, 4, T> rotateMat;

            double radian = angle * (float)(M_PI / 180);
            double r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
            double ux = x * 1/r;
            double uy = y * 1/r;
            double uz = z * 1/r;
            
            rotateMat(3, 3) = 1;
            rotateMat(0, 0) = cos(radian) + pow(ux,2) * (1 - cos(radian));
            rotateMat(0, 1) = (ux * uy)*(1 - cos(radian)) - uz * sin(radian);
            rotateMat(0, 2) = (ux * uz) * (1 - cos(radian)) + (uy * sin(radian));
            rotateMat(1, 0) = (uy * ux) * (1 - cos(radian)) + (uz * sin(radian));
            rotateMat(1, 1) = cos(radian) + (uy * uy) * (1 - cos(radian));
            rotateMat(1, 2) = (uy * uz) * (1 - cos(radian)) - (ux * sin(radian));
            rotateMat(2, 0) = (uz * ux)* (1 - cos(radian)) - (uy * sin(radian));
            rotateMat(2, 1) = (uz * uy)* (1 - cos(radian)) + (ux * sin(radian));
            rotateMat(2, 2) = cos(radian) + (uz * uz) * 1 - cos(radian);
            return rotateMat;
        }

        template<typename T>
        mat<4, 4, T> scale(T sx, T sy, T sz)
        {
            mat<4, 4, T> scaleMat;

            scaleMat(0, 0) = sx;
            scaleMat(1, 1) = sy;
            scaleMat(2, 2) = sz;
            scaleMat(3, 3) = 1;

            return scaleMat;
        }

        template<typename T>
        mat<4, 4, T> lookAt(T eyeX, T eyeY, T eyeZ, T centerX, T centerY, T centerZ, T upX, T upY, T upZ)
        {
            mat<4, 4, T> viewMat;

            T composeX = eyeX;
            T composeY = eyeY;
            T composeZ = eyeZ;
            T forwardX = eyeX - centerX;
            T forwardY = eyeY - centerY;
            T forwardZ = eyeZ - centerZ;


            double vect = sqrt(pow(forwardX, 2) + pow(forwardY, 2) + pow(forwardZ, 2));

            T camzx = forwardX / vect;
            T camzy = forwardY / vect;
            T camzz = forwardZ / vect;

            T camxx = (upY * camzz) - (upZ * camzy);
            T camxy = (upZ * camzx) - (upX * camzz);
            T camxz = (upX * camzy) - (upY * camzx);
            
            T camyx = (camzy * camxz) - (camzz * camxy);
            T camyy = (camzz * camxx) - (camzx * camxx);
            T camyz = (camzx * camxy) - (camzy * camxx);


            T Camaxis[4][4] = {
                {camxx,     camxy,   camxz,   0.0},
                {camyx,     camyy,   camyz,   0.0 },
                {camzx,     camzy,   camzz,   0.0},
                {0.0,     0.0,   0.0,   1.0}
            };
            
            T Compose[4][4] = {
                {1.0,     0.0,   0.0,   -composeX},
                {0.0,     1.0,   0.0,   -composeY},
                {0.0,     0.0,   1.0,   -composeZ},
                {0.0,     0.0,   0.0,   1.0}
            };

            T product[4][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                    for (int inner = 0; inner < 4; inner++) {
                        viewMat(row, col) += Camaxis[row][inner] * Compose[inner][col];
                    }
                }                
            }

            return viewMat;
        }

        template<typename T>
        mat<4, 4, T> ortho(T left, T right, T bottom, T top, T nearVal, T farVal)
        {
            mat<4, 4, T> orthoMat;

            orthoMat(0, 0) = 2 / (right - left);
            orthoMat(1, 1) = 2 / (top - bottom);
            orthoMat(2, 2) = -(2 / (farVal - nearVal));
            orthoMat(0, 3) = -((right + left) / (right - left));
            orthoMat(1, 3) = -((top + bottom) / (top - bottom));
            orthoMat(2, 3) = -((farVal + nearVal) / (farVal - nearVal));
            orthoMat(3, 3) = 1;


            // TODO: Fill up this function properly 

            return orthoMat;
        }

        template<typename T>
        mat<4, 4, T> frustum(T left, T right, T bottom, T top, T nearVal, T farVal)
        {
            mat<4, 4, T> frustumMat;
            frustumMat(0, 0) = 2 * nearVal / (right - left);
            frustumMat(0, 2) = (right + left) / (right - left);
            frustumMat(1, 1) = 2 * nearVal / (top - bottom);
            frustumMat(1, 2) = (top + bottom) / (top - bottom);
            frustumMat(2, 2) = -((farVal + nearVal) / (farVal - nearVal));
            frustumMat(2, 3) = -((2 * farVal * nearVal) / (farVal - nearVal));
            frustumMat(3, 2) = -1;

            // TODO: Fill up this function properly 

            return frustumMat;
        }

        template<typename T>
        mat<4, 4, T> perspective(T fovy, T aspect, T zNear, T zFar)
        {
            T radian = (fovy/2) * (float)(M_PI / 180);

            T  top = tan(radian) * zNear;
            T  right = top * aspect;
            // TODO: Fill up this function properly 

            return frustum(-right, right, -top, top, zNear, zFar);
        }
    }
}
#endif