#pragma once
#include <cmath>

class MouseHandle{
  struct quaternion{
    double t, x, y, z;
    inline quaternion(){
      t = x = y = z = 0.0;
    }
    inline explicit quaternion(const double a[]) : t(0.0), x(a[0]), y(a[1]), z(a[2]) {}
    inline quaternion(const double t_, const double x_, const double y_, const double z_):t(t_), x(x_), y(y_), z(z_){}
    inline quaternion(const double radian, double axis[]) {
      const double inv_dr = 1.0 / sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
      axis[0] *= inv_dr;
      axis[1] *= inv_dr;
      axis[2] *= inv_dr;

      const double sss = std::sin(0.5 * radian);
      
      t = std::cos(0.5 * radian);
      x = sss * axis[0];
      y = sss * axis[1];
      z = sss * axis[2];
    }
    
    inline const quaternion operator*(const quaternion& obj) const {
      const quaternion ret(t * obj.t - x * obj.x - y * obj.y - z * obj.z,
			   t * obj.x + x * obj.t + y * obj.z - z * obj.y,
			   t * obj.y + y * obj.t + z * obj.x - x * obj.z,
			   t * obj.z + z * obj.t + x * obj.y - y * obj.x);
      return ret;
    }
    
    inline void conj() {
      x *= -1.0;
      y *= -1.0;
      z *= -1.0;
    }
    
    inline void ret_array3(double vec[3]){
      vec[0] = x; vec[1] = y; vec[2] = z;
    }
  };
  
  double phi, theta, fovy;
  double eyeDistance;
  double persCenter[3];
  double center2eye[3];  
  double ebase_z[3];
  
  int x_bef, y_bef;
  int but;
  
  void CrossProduct(const double* a, const double* b, double* c) const {
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
  }
public:
  MouseHandle(double, double, double, double, double);

  double* RetFovy(){return &fovy;};
  double* RetPersCent(){return persCenter;};
  double* RetCenter2eye(){return center2eye;};
  double* RetEbaseZ(){return ebase_z;};

  void RotPersVect(double*, double*, double);
  void MouseClick(int, int, int, int);
  void MouseMotion(int, int);
  void MouseWheel(int, int, int, int);
  
};

namespace callbacks{
  extern MouseHandle* mousehandle;
  extern void wrap_mclick(int but, int state, int x, int y);
  extern void wrap_mmotion(int x, int y);
  extern void wrap_mwheel(int wheel_n, int direct, int x, int y);
}
