#pragma once
#include <cmath>

class mouse_handle{
private:
  struct quaternion{
    double t;
    double x;
    double y;
    double z;
  };
  
  double phi;
  double theta;
  double fovy;
  
  double center2eye[3];
  double eyeDistance;
  double persCenter[3];
  double ebase_z[3];
  
  int x_bef,y_bef;

  int but;
public:
  mouse_handle(double center_x,double center_y,double center_z,double eDist,double fov){
    eyeDistance  = eDist;
    
    persCenter[0] = center_x;
    persCenter[1] = center_y;
    persCenter[2] = center_z;
    
    fovy = fov;

    phi = M_PI*0.35;
    theta = M_PI*0.3;
    
    center2eye[0] = eyeDistance*cos(phi)*sin(theta);
    center2eye[1] = eyeDistance*sin(phi)*sin(theta);
    center2eye[2] = eyeDistance*cos(theta);

    ebase_z[0] = cos(phi)*sin(theta-0.5*M_PI);
    ebase_z[1] = sin(phi)*sin(theta-0.5*M_PI);
    ebase_z[2] = cos(theta-0.5*M_PI);
    
    but = 0;
  };
  ~mouse_handle(){};
  void CrossProduct(const double*,const double*,double*) const;
  void Normaliz(double*) const;
  void ConjuQuate(quaternion*) const;
  void GenQuate(const double*,quaternion*) const;
  void ProductQuate(const quaternion*,const quaternion*, quaternion*) const;
  void MakeRotationalQuate(double,double*,quaternion*) const;

  double* RetFovy(){return &fovy;};
  double* RetPersCent(){return persCenter;};
  double* RetCenter2eye(){return center2eye;};
  double* RetEbaseZ(){return ebase_z;};

  void RotPersVect(double*, double*,double);
  void MouseClick(int,int,int,int);
  void MouseMotion(int,int);
  void MouseWheel(int wheel_n,int direct,int x,int y);
};

extern mouse_handle* MouseHandle;

namespace moushandl_clbck{
  extern void mouseclick_callback(int but, int state, int x, int y);
  extern void mousemotion_callback(int x,int y);
  extern void mousewheel_callback(int wheel_n,int direct,int x,int y);
}
