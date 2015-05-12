#pragma once
#include <vector>
#include <bitset>
#include <fstream>
#include <array>
#include <GL/freeglut.h>

class Jpegout;

class DrawSys{
  enum{
    SEED_N = 4,
  };

  template<int val, int n>
  struct power{
    static const int ret = val * power<val, n - 1>::ret;
  };
  
  template<int val>
  struct power<val, 0>{
    static const int ret = val;
  };
  
  std::vector<std::array<GLfloat, 3> > p_color;
  std::vector<std::array<GLfloat, 4> > lightpos; 
  std::bitset<15>  draw_crit;
  std::vector<int> draw_crit_mask;
  
  int draw_crit_max, draw_crit_base;
  bool chem_is_drawn;
  
  int cubeedge[12][2];
  GLdouble vertex[8][3];
  
  GLfloat nv[3], cut_plane = 0.5;

  float *p_fovy, *p_perscenter, *p_center2eye, *p_base_z;

  std::string cur_dir;
  bool crit_out;
  
  int cur_time, time_step, all_time;
  bool swt_but, cut_but, cut_adv;

  //particle system
  struct particle{
    float r[3]; //position
    int prop;    //property
    bool chem;
  };
  float scL, invL, prad, invhL, box_size[3], inv_box_size[3];
  std::vector<particle> Particle;
  int pN;

  void *font = GLUT_BITMAP_TIMES_ROMAN_24;
  
  std::ifstream fin;
  
  Jpegout *jpgout;
  
  void Drawxyz();
  void DrawCubic();
  void DrawAxis(float   , float, const float[][3]);
  void DrawSubAxis(float, float, const float[3]);

  void RenderCurTime();
  void RenderString2D(const char *,float,float);
  void RenderString3D(const char *,const float[3]);

  void RenderSphere(const particle& prtcl);
  bool IsDrawnObject(const particle& prtcl);
  
  void ChangeNormalVector(int i);
  void ChangeCrossSection();
  
public:
  DrawSys(const std::string& cur_dir_, const bool crit_out_);
  ~DrawSys();
  void SetParams();
  void AllocateResource();

  void SetCallBackFunc() const;
  void SetColor(const GLfloat*);
  void SetLightPos(const GLfloat*);
  void GetMouseInfo(float*, float*, float*, float*);

  void InitCube();
  void InitWindow(int argc, char* argv[]) const;
  void InitColor() const;
  bool InitGlew() const;
  
  void FileOpen();
  
  void Timer(int value);
  void LoadParticleDat();
  void Display();
  void Resize(int,int) const;
  void KeyBoard(unsigned char, int ,int);
  void ChgDrawObject();

  
  void PrintDrawInfo() const;

  void Execute(){
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutMainLoop();
  }

  int Pow_n(int, int) const;
};

namespace callbacks{
  extern DrawSys* drawsys;
  extern void wrap_display();
  extern void wrap_timer(int value);
  extern void wrap_resize(int w, int h);
  extern void wrap_keyboard(unsigned char key, int x, int y);
}
