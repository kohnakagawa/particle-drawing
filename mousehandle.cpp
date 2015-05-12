#include "sysdraw.hpp"
#include "mousehandle.hpp"
#include <iostream>

void callbacks::wrap_mclick(int but, int state, int x, int y){
  callbacks::mousehandle->MouseClick(but, state, x, y);
}
void callbacks::wrap_mmotion(int x, int y){
  callbacks::mousehandle->MouseMotion(x, y);
}
void callbacks::wrap_mwheel(int wheel_n, int direct, int x, int y){
  callbacks::mousehandle->MouseWheel(wheel_n, direct, x, y);
}

MouseHandle::MouseHandle(float center_x, float center_y, float center_z, float eDist, float fov){
  eyeDistance  = eDist;
    
  persCenter[0] = center_x;
  persCenter[1] = center_y;
  persCenter[2] = center_z;
    
  fovy = fov;

  phi   = M_PI * 0.35;
  theta = M_PI * 0.3;
    
  center2eye[0] = eyeDistance*cos(phi) * sin(theta);
  center2eye[1] = eyeDistance*sin(phi) * sin(theta);
  center2eye[2] = eyeDistance*cos(theta);

  ebase_z[0] = cos(phi) * sin(theta - 0.5 * M_PI);
  ebase_z[1] = sin(phi) * sin(theta - 0.5 * M_PI);
  ebase_z[2] = cos(theta - 0.5 * M_PI);
    
  x_bef = y_bef = but = 0;
}

void MouseHandle::RotPersVect(float* vec, float* rot_axis, float theta)
{
  quaternion quate_vec(0.0, vec[0], vec[1], vec[2]);
  quaternion quate_axis(theta, rot_axis);
  quaternion quate_temp = quate_vec * quate_axis;
  quate_axis.conj();
  quate_vec = quate_axis * quate_temp;
  quate_vec.ret_array3(vec);
}

void MouseHandle::MouseClick(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    but = 1;
    if(state == GLUT_DOWN){
	x_bef = x;
	y_bef = y;
      }
    break;
  default:
    break;
  }
  if (state == GLUT_UP) but = 0;
}

void MouseHandle::MouseMotion(int x, int y)
{
  const int  width = glutGet(GLUT_WINDOW_WIDTH);
  const int  height = glutGet(GLUT_WINDOW_HEIGHT);
  if(but == 1){
    const float dx = float (x - x_bef);
    const float dy = float (y - y_bef);
    const float theta_u = dx * 0.005;
    const float theta_w = -dy * 0.005;
      
    float vec_u[3] = {ebase_z[0], ebase_z[1], ebase_z[2]};
    float vec_w[3] = {0.0, 0.0, 0.0};
    CrossProduct(center2eye, vec_u, vec_w);

    RotPersVect(center2eye, vec_u, theta_u);
    RotPersVect(center2eye, vec_w, theta_w);
    RotPersVect(ebase_z,    vec_w, theta_w);

    callbacks::wrap_resize(width, height);
  }
}

void MouseHandle::MouseWheel(int wheel_n,int direct,int x,int y){
  const int  width  = glutGet(GLUT_WINDOW_WIDTH);
  const int  height = glutGet(GLUT_WINDOW_HEIGHT);
  std::cout << wheel_n << " " << direct << std::endl;
  fovy -= direct * 1.5;
  callbacks::wrap_resize(width, height);
}
