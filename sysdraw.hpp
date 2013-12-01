#pragma once

#include <cmath>
#include <sstream>
#include <vector>
#include <bitset>
#include <fstream>
#include <GL/freeglut.h>
#include "jpegout.hpp"

class drawsys{
private:
  //color
  struct color_t{
    GLfloat p[3];
  };
  std::vector<color_t>p_color;
  //light
  struct light_t{
    GLfloat l[4];
  };
  std::vector<light_t>lightpos; 

  std::bitset<15> draw_crit;
  int draw_crit_max;
  int draw_crit_base;
  enum{
    draw_crit_mask = 0x7ffe
  };
  
  int cubeedge[12][2];
  GLdouble vertex[8][3]; //立方体の各頂点

  double* p_fovy;
  double* p_perscenter;
  double* p_center2eye;
  double* p_base_z;
  
  //time events
  int cur_time;
  int time_step;
  int all_time;
  
  //cutting events
  int swt_but;
  int cut_but;
  int cut_adv;
  int cut_axis;

  //particle system
  struct particle{
    double r[3]; //position
    int prop;    //property
  };
  double scL,invL;
  double prad;
  std::vector<particle> Particle;
  int seedN;
  int pN;
  
  //util
  char* cur_dir;
  std::ifstream fin;
  
  //out jpeg
  bool crit_out;
  Jpegout jpgout;
public:
  drawsys(char* cur_dir,bool crit_out){
    this->cur_dir = cur_dir;
    this->crit_out = crit_out;
  };
  ~drawsys(){};
  void SetParamParticle(double,double,int,int);
  void SetParamTime(int,int);
  void SetCallBackFunc() const;
  void SetWindow() const;
  void SetColor(GLfloat*);
  void SetLightPos(GLfloat*);
  void GetMouseInfo(double*_,double*,double*,double*);

  void InitCube();
  void InitCut();
  void InitWindowSys(int argc,char* argv[]) const;
  void InitColor() const;
  
  void FileManag();
  
  void Timer(int value);
  void LoadParticleDat();
  void Display();
  void Resize(int,int) const;
  void KeyBoard(unsigned char, int ,int);
  void ChgDrawObject();
  void Drawxyz();
  
  void PrintDrawInfo() const;

  void Execute(){
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutMainLoop();//無限ループ
  }

  int Pow_n(int,int) const;
};

extern drawsys* Drawsys;

namespace drwsys_clbck{
  extern void disp_callback(void);
  extern void timer_callback(int value);
  extern void resize_callback(int w,int h);
  extern void keyboard_callback(unsigned char key,int x,int y);
}
