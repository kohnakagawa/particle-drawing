#include "mouse_handling_rev.hpp"
#include "sysdraw.hpp"

void moushandl_clbck::mouseclick_callback(int but, int state, int x, int y){
  MouseHandle->MouseClick(but,state,x,y);
}
void moushandl_clbck::mousemotion_callback(int x,int y){
  MouseHandle->MouseMotion(x,y);
}
void moushandl_clbck::mousewheel_callback(int wheel_n,int direct,int x,int y){
  MouseHandle->MouseWheel(wheel_n,direct,x,y);
}

void mouse_handle::RotPersVect(double* vec, double* rot_axis, double theta)
{
  quaternion quate_vec = {0.0,0.0,0.0,0.0};
  quaternion quate_axis = {0.0,0.0,0.0,0.0}; 
  quaternion quate_temp = {0.0,0.0,0.0,0.0};

  GenQuate(vec,&quate_vec);
  MakeRotationalQuate(theta,rot_axis, &quate_axis);

  ProductQuate(&quate_vec,&quate_axis,&quate_temp);
  ConjuQuate(&quate_axis);
  ProductQuate(&quate_axis,&quate_temp,&quate_vec);

  vec[0] = quate_vec.x;
  vec[1] = quate_vec.y;
  vec[2] = quate_vec.z;
}

void mouse_handle::MouseClick(int button, int state, int x, int y)
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

void mouse_handle::MouseMotion(int x, int y)
{
  const int  width = glutGet(GLUT_WINDOW_WIDTH);
  const int  height = glutGet(GLUT_WINDOW_HEIGHT);
  if (but == 1){
      double dx = double (x-x_bef);
      double dy = double (y-y_bef);
      double theta_u = dx * 0.005;
      double theta_w = -dy * 0.005;
      
      double vec_u[3] = {ebase_z[0],ebase_z[1],ebase_z[2]};
      double vec_w[3] = {0.0, 0.0, 0.0};

      CrossProduct(center2eye,vec_u,vec_w);

      RotPersVect(center2eye, vec_u, theta_u);
      RotPersVect(center2eye, vec_w, theta_w);
      RotPersVect(ebase_z,vec_w, theta_w);

      drwsys_clbck::resize_callback(width,height);
    }
}

void mouse_handle::MouseWheel(int wheel_n,int direct,int x,int y){
  const int  width = glutGet(GLUT_WINDOW_WIDTH);
  const int  height = glutGet(GLUT_WINDOW_HEIGHT);
  fovy -= direct*1.5;
  drwsys_clbck::resize_callback(width,height);
}

void mouse_handle::CrossProduct(const double* a,const double* b,double* c) const
{
  c[0] = a[1]*b[2]-a[2]*b[1];
  c[1] = a[2]*b[0]-a[0]*b[2];
  c[2] = a[0]*b[1]-a[1]*b[0];
}

void mouse_handle::Normaliz(double* a) const
{
  const double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  const double inverse_norm = 1.0/norm;

  a[0] *=inverse_norm;
  a[1] *=inverse_norm;
  a[2] *=inverse_norm;
}

void mouse_handle::ConjuQuate(quaternion* a) const
{
  a->x *= -1.;
  a->y *= -1.;
  a->z *= -1.;
}

void mouse_handle::GenQuate(const double* a, quaternion* ret) const
{
  ret->t = 0.;
  ret->x = a[0];
  ret->y = a[1];
  ret->z = a[2];
}

void mouse_handle::ProductQuate(const quaternion* left,const quaternion* right, quaternion* ret) const
{     
  double   d1, d2, d3, d4;      
	      
  d1   =  left->t * right->t;
  d2   = -left->x * right->x;
  d3   = -left->y * right->y;
  d4   = -left->z * right->z;
  ret->t = d1+ d2+ d3+ d4;
       
  d1   =  left->t * right->x;
  d2   =  right->t * left->x;
  d3   =  left->y * right->z;
  d4   = -left->z * right->y;
  ret->x =  d1+ d2+ d3+ d4; 
       
  d1   =  left->t * right->y;
  d2   =  right->t * left->y;
  d3   =  left->z * right->x;
  d4   = -left->x * right->z;
  ret->y =  d1+ d2+ d3+ d4;
       
  d1   =  left->t * right->z;
  d2   =  right->t * left->z;
  d3   =  left->x * right->y;
  d4   = -left->y * right->x;
  ret->z =  d1+ d2+ d3+ d4;
}

void mouse_handle::MakeRotationalQuate(double radian, double* rot_axis, quaternion* ret) const
{      
  ret->t = ret->x = ret->y = ret->z = 0.;
       
  Normaliz(rot_axis);

  double ccc   = cos(0.5 * radian);      
  double sss   = sin(0.5 * radian);      
       
  ret->t = ccc;      
  ret->x = sss * rot_axis[0];
  ret->y = sss * rot_axis[1];  
  ret->z = sss * rot_axis[2];  
}      
