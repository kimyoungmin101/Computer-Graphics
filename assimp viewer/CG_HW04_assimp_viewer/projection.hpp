#pragma once

aiCamera camera;

namespace kmuvcl
{
  float left;
  float right;
  float bottom;
  float top;
  aiMatrix4x4 out;

  void ortho(float left, float right, float bottom, float top, aiMatrix4x4& out)
  {
    float far    = camera.mClipPlaneFar;
    float near   = camera.mClipPlaneNear;

    out.a1 = 2/(right-left);
    out.a2 = 0.0f;
    out.a3 = 0.0f;
    out.a4 = -(right+left)/(right-left);
    
    out.b1 = 0.0f;
    out.b2 = 2/(top-bottom);
    out.b3 = 0.0f;
    out.b4 = -(top+bottom)/(top-bottom);

    out.c1 = 0.0f;
    out.c2 = 0.0f;
    out.c3 = -2/(far-near);
    out.c4 = -(far+near)/(far-near);

    out.d1 = 0.0f;
    out.d2 = 0.0f;
    out.d3 = 0.0f;
    out.d4 = 1.0f;
  }

  void frustum(float left, float right, float bottom, float top, 
              aiMatrix4x4& out)
  {
    float far    = camera.mClipPlaneFar;
    float near   = camera.mClipPlaneNear;
    
    out.a1 = (2*near)/(right - left);
    out.a2 = 0.0f;
    out.a3 = (right + left)/(right - left);
    out.a4 = 0.0f;
    
    out.b1 = 0.0f;
    out.b2 = (2*near)/(top - bottom);
    out.b3 = (top + bottom)/(top - bottom);
    out.b4 = 0.0f;

    out.c1 = 0.0f;
    out.c2 = 0.0f;
    out.c3 = -(far + near)/(far - near);
    out.c4 = -(2*far*near)/(far - near);

    out.d1 =  0.0f;
    out.d2 =  0.0f;
    out.d3 = -1.0f;
    out.d4 =  0.0f;
  }

  void perspective(aiMatrix4x4& out)
  {
    float far    = camera.mClipPlaneFar;
    float near   = camera.mClipPlaneNear;
    float aspect = camera.mAspect;
    float fovx   = camera.mHorizontalFOV;

    // std::cout << "aspect: " << aspect << std::endl;
    // std::cout << "fovx: " << fovx << std::endl;

    float right  = near*tan(fovx/2.0);
    float top    = aspect*right;

    frustum(-right, right, -top, top, out);
  }
};
