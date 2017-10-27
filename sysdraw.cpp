#include <iostream>
#include <cstdlib>
#include <sstream>
#include <GL/glew.h>
#include <limits>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "sysdraw.hpp"
#include "jpegout.hpp"
#include "mousehandle.hpp"

#include "helper_macros.hpp"

void callbacks::wrap_display() {
  callbacks::drawsys->Display();
}

void callbacks::wrap_timer(int value) {
  callbacks::drawsys->Timer(value);
}

void callbacks::wrap_resize(int w, int h) {
  callbacks::drawsys->Resize(w, h);
}

void callbacks::wrap_keyboard(unsigned char key, int x, int y) {
  callbacks::drawsys->KeyBoard(key, x, y);
}

DrawSys::DrawSys(const std::string& cur_dir_,
                 const bool crit_out_,
                 const int b_time) : beg_time(b_time), jpgout(new Jpegout()) {
  draw_crit_max  = power(2, SEED_N);
  draw_crit_base = draw_crit_max - 1;
  chem_is_drawn  = true;

  nv[0] = 1.0; nv[1] = 0.0, nv[2] = 0.0;

  p_fovy = p_perscenter = p_center2eye = p_base_z = nullptr;

  cur_dir  = cur_dir_;
  crit_out = crit_out_;

  cur_time = time_step = all_time = 0;
  swt_but  = true; cut_adv = false; cut_but = false;

  scL = invL = prad = invhL = 0.0;
  box_size[0] = box_size[1] = box_size[2] = 0.0;
  inv_box_size[0] = inv_box_size[1] = inv_box_size[2] = 0.0;

  pN  = 0;
}

DrawSys::~DrawSys() {}

void DrawSys::SetParams() {
  const std::string f_name = cur_dir + "/macro_data.txt";
  std::ifstream fin(f_name.c_str());

  if (!fin) {
    std::cerr << "File I/O error! \n";
    std::cerr << f_name.c_str() << " No such file \n";
    std::cerr << __FILE__ << " " << __LINE__ << std::endl;
    std::exit(1);
  }

  int wN = 0, lN = 0;
  fin >> wN >> lN >> scL >> prad >> all_time >> time_step;
  pN = wN + lN;
  all_time -= 3 * time_step;

  static_assert(SEED_N <= 15, "the number of max particle seed is 15");

  invL = 1.0 / scL;
  invhL = 0.5 * invL;

  prad *= invL;

  Particle.resize(pN);
  p_color.reserve(SEED_N);

  draw_crit.set();

  for (int i = 0; i < SEED_N; i++) {
    draw_crit_mask.push_back(power(2, i));
  }
}

void DrawSys::SetCallBackFunc() const {
  glutDisplayFunc(callbacks::wrap_display);
  glutTimerFunc(50, callbacks::wrap_timer, 0);
  glutReshapeFunc(callbacks::wrap_resize);
  glutMouseFunc(callbacks::wrap_mclick);
  glutMotionFunc(callbacks::wrap_mmotion);
  glutMouseWheelFunc(callbacks::wrap_mwheel);
  glutKeyboardFunc(callbacks::wrap_keyboard);
}

void DrawSys::SetColor(const GLfloat* col) {
  p_color.emplace_back(std::array<GLfloat, 3> {col[0], col[1], col[2]});
}

void DrawSys::SetLightPos(const GLfloat* pos) {
  lightpos.emplace_back(std::array<GLfloat ,4> {pos[0], pos[1], pos[2], pos[3]});
}

void DrawSys::GetMouseInfo(float* fov_,
                           float* perscent,
                           float* cent2eye,
                           float* base_z) {
  p_fovy       = fov_;
  p_perscenter = perscent;
  p_center2eye = cent2eye;
  p_base_z     = base_z;
};

void DrawSys::InitWindow(int argc, char* argv[], const char* win_name) const {
  glutInitWindowPosition(800, 400);
  glutInitWindowSize(1300, 1100);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(win_name);
}

void DrawSys::InitCube() {
  //down square
  vertex[0][0] = -0.5; vertex[0][1] = -0.5; vertex[0][2] = -0.5;
  vertex[1][0] = 0.5;  vertex[1][1] = -0.5; vertex[1][2] = -0.5;
  vertex[2][0] = 0.5;  vertex[2][1] = 0.5;  vertex[2][2] = -0.5;
  vertex[3][0] = -0.5; vertex[3][1] = 0.5;  vertex[3][2] = -0.5;

  //up square
  vertex[4][0] = -0.5; vertex[4][1] = -0.5; vertex[4][2] = 0.5;
  vertex[5][0] = 0.5;  vertex[5][1] = -0.5; vertex[5][2] = 0.5;
  vertex[6][0] = 0.5;  vertex[6][1] = 0.5;  vertex[6][2] = 0.5;
  vertex[7][0] = -0.5; vertex[7][1] = 0.5;  vertex[7][2] = 0.5;

  //edge connection
  cubeedge[0][0] = 0; cubeedge[0][1] = 1;
  cubeedge[1][0] = 1; cubeedge[1][1] = 2;
  cubeedge[2][0] = 2; cubeedge[2][1] = 3;
  cubeedge[3][0] = 3; cubeedge[3][1] = 0;
  cubeedge[4][0] = 4; cubeedge[4][1] = 5;
  cubeedge[5][0] = 5; cubeedge[5][1] = 6;
  cubeedge[6][0] = 6; cubeedge[6][1] = 7;
  cubeedge[7][0] = 7; cubeedge[7][1] = 4;
  cubeedge[8][0] = 0; cubeedge[8][1] = 4;
  cubeedge[9][0] = 1; cubeedge[9][1] = 5;
  cubeedge[10][0] = 2; cubeedge[10][1] = 6;
  cubeedge[11][0] = 3; cubeedge[11][1] = 7;
}

void DrawSys::InitColor() const {
  glClearColor(1.0, 1.0, 1.0, 1.0); //(Red,Green,Blue,A) Aは透明度 ウィンドウを塗りつぶす色を指定
  glEnable(GL_DEPTH_TEST); //デプスバッファ使用

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_LIGHTING); //光源使用
  glEnable(GL_LIGHT0); //光源0を設定
}

void DrawSys::FileOpen() {
  boost::filesystem::path path_cur_dir(cur_dir);
  boost::filesystem::path path_filename("particle_data.txt");
  boost::filesystem::path path_f_full = path_cur_dir / path_filename;

  const auto str = path_f_full.string();
  fin.open(str);

  if (!fin) {
    std::cerr << "File I/O error!" << std::endl;
    std::cerr << str << " No such file" << std::endl;
    std::cerr << __FILE__ << " " << __LINE__ << std::endl;
    std::exit(1);
  }

  if (beg_time >= all_time){
    std::cerr << "beg_time is larger than all_time. \n";
    std::exit(1);
  }

  SkipFileLines(fin, beg_time / time_step, 1);
}

DrawSys::particle DrawSys::ParseDataLine(const std::string& line) const {
  std::vector<std::string> v;
  boost::algorithm::split(v, line, boost::algorithm::is_space());
  if (v.size() < 9) {
    std::cerr << "Few input data\n";
    std::cerr << __FILE__ << " " << __LINE__ << std::endl;
    std::exit(1);
  }
  const DrawSys::particle prtcl {
    {std::stof(v[0]), std::stof(v[1]), std::stof(v[2])},
    std::atoi(v[6].c_str()),
    static_cast<bool>(std::atoi(v[8].c_str())),
  };
  return prtcl;
}

void DrawSys::LoadParticleDat() {
  for (int i = 0; i < pN; i++) {
    std::string line;
    std::getline(fin, line);
    Particle[i] = ParseDataLine(line);

    if (box_size[0] < Particle[i].r[0]) box_size[0] = Particle[i].r[0];
    if (box_size[1] < Particle[i].r[1]) box_size[1] = Particle[i].r[1];
    if (box_size[2] < Particle[i].r[2]) box_size[2] = Particle[i].r[2];
  }

#if 0
  //adjust image to remove PBC
  inv_box_size[0] = 1.0 / box_size[0];
  inv_box_size[1] = 1.0 / box_size[1];
  inv_box_size[2] = 1.0 / box_size[2];
  float offset[] {9.0, 5.0, 0.0};
  for (int i = 0; i < pN; i++) {
    Particle[i].r[0] += offset[0];
    Particle[i].r[1] += offset[1];
    Particle[i].r[2] += offset[2];

    Particle[i].r[0] -= floor(Particle[i].r[0] * inv_box_size[0]) * box_size[0];
    Particle[i].r[1] -= floor(Particle[i].r[1] * inv_box_size[1]) * box_size[1];
    Particle[i].r[2] -= floor(Particle[i].r[2] * inv_box_size[2]) * box_size[2];
  }
#endif

  box_size[0] *= invL;
  box_size[1] *= invL;
  box_size[2] *= invL;
  inv_box_size[0] = 1.0 / box_size[0];
  inv_box_size[1] = 1.0 / box_size[1];
  inv_box_size[2] = 1.0 / box_size[2];

  for (int i = 0; i < pN; i++) {
    Particle[i].r[0] *= invL;
    Particle[i].r[1] *= invL;
    Particle[i].r[2] *= invL;

    Particle[i].r[0] -= box_size[0] * 0.5;
    Particle[i].r[1] -= box_size[1] * 0.5;
    Particle[i].r[2] -= box_size[2] * 0.5;
  }
}

void DrawSys::Drawxyz() {
  const float axis_pos[] {0.0, 0.0, 0.0};
  glPushMatrix();
  glTranslatef(axis_pos[0], axis_pos[1], axis_pos[2]);
  glBegin(GL_LINES);

  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(1.5, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);

  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 1.5, 0.0);
  glVertex3f(0.0, 0.0, 0.0);

  glColor3f(0.0 , 0.0, 1.0);
  glVertex3f(0.0, 0.0, 1.5);
  glVertex3f(0.0, 0.0, 0.0);
  glEnd();
  glPopMatrix();
}

void DrawSys::DrawCubic() {
  const GLfloat color[] {0.0, 0.0, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
  glBegin(GL_LINES);
  for (int i = 0; i < 12; ++i) {
    glVertex3fv(vertex[cubeedge[i][0]]);
    glVertex3fv(vertex[cubeedge[i][1]]);
  }
  glEnd();
}

void DrawSys::DrawAxis(float d, float s, const float col[][3]) {
  const float origin[] {-0.95, 0.0, 0.0};

  glPushMatrix();
  glTranslatef(origin[0], origin[1], origin[2]);
  DrawSubAxis(d,s,col[0]);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(origin[0], origin[1], origin[2]);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  DrawSubAxis(d,s,col[1]);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(origin[0], origin[1], origin[2]);
  glRotatef(-90.0, 0.0, 0.0, 1.0);
  DrawSubAxis(d,s,col[2]);
  glPopMatrix();

  //Render Axis name
  /*const float n_pos[3][3] = {{1.23,0.0,0.0},{0.0,1.23,0.0},{0.0,0.0,1.23}};
    const float black[3] = {0.0, 0.0, 0.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,black);
    RenderString3D("X",n_pos[0]);
    RenderString3D("Y",n_pos[1]);
    RenderString3D("Z",n_pos[2]);*/
}

void DrawSys::DrawSubAxis(float d, float s, const float col[3]) {
  glBegin(GL_QUAD_STRIP);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);
  for (float i = 0; i <= 6.0; i += 1.0) {
    const float t = i * 2.0 * M_PI / 6.0;
    glNormal3f(std::cos(t)    , 0.0, std::sin(t)    );
    glVertex3f((d * std::cos(t) ), 0.0, (d * std::sin(t) ) );
    glVertex3f((d * std::cos(t) ), s  , (d * std::sin(t) ) );
  }
  glEnd();
  glTranslatef(0.0, s, 0.0);
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  glutSolidCone(2.0 * d, 4.0 * d, 5, 5);
}

void DrawSys::Resize(int w, int h) const {
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION); //透視変換行列設定
  glLoadIdentity(); //変換行列初期化

  gluPerspective(*p_fovy, float(w) / float(h), 0.1f, 200.0f); //透視投影　カメラの画角、アスペクト比を指定

  glMatrixMode(GL_MODELVIEW); //モデルビュー変換行列設定
  glLoadIdentity();

  //光源と視点が一緒に動く場合
  const GLfloat light0pos[4] = {lightpos[0][0],
                                lightpos[0][1],
                                lightpos[0][2],
                                lightpos[0][3]};
  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  //

  gluLookAt(p_perscenter[0] + p_center2eye[0],
            p_perscenter[1] + p_center2eye[1],
            p_perscenter[2] + p_center2eye[2],
            p_perscenter[0],
            p_perscenter[1],
            p_perscenter[2],
            p_base_z[0],
            p_base_z[1],
            p_base_z[2]);
}

void DrawSys::ChgDrawObject() {
  draw_crit_base++;
  if (draw_crit_base == draw_crit_max) draw_crit_base = 1;

  for (int i = 0; i < SEED_N; i++) {
    draw_crit[i] = (draw_crit_base & draw_crit_mask[i]) == draw_crit_mask[i];
  }
}

void DrawSys::RenderCurTime() {
  std::stringstream ss;
  ss << "time = " << cur_time ;
  glColor3d(0.0, 0.0, 0.0);
  RenderString2D(ss.str().c_str(), 0.8, 0.8);
  glPopAttrib();
}

void DrawSys::RenderString2D(const char *str, float x, float y) {
  glWindowPos2f(x,y);

  while (*str) {
    glutBitmapCharacter(font, *str);
    ++str;
  }
}

void DrawSys::RenderString3D(const char *str, const float r[3]) {
  glRasterPos3f(r[0], r[1], r[2]);
  glutBitmapString(font,
                   reinterpret_cast<const unsigned char*>(str));
}

void DrawSys::RenderSphere(const particle& prtcl) {
  const int prop = prtcl.prop;
  GLfloat color[] {p_color[prop][0], p_color[prop][1], p_color[prop][2]};
  if (!prtcl.chem && (prop == 2)) {
    color[0] = p_color[4][0];
    color[1] = p_color[4][1];
    color[2] = p_color[4][2];
  }

  if (!prtcl.chem && (prop == 1)) {
    color[0] = p_color[5][0];
    color[1] = p_color[5][1];
    color[2] = p_color[5][2];
  }

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
  glPushMatrix();
  glTranslated(prtcl.r[0], prtcl.r[1], prtcl.r[2]);
  glutSolidSphere(prad, 10, 10);
  glPopMatrix();
}

bool DrawSys::IsDrawnObject(const particle& prtcl) {
  if (!cut_but) {
#if 0
    bool ret = draw_crit[prtcl.prop];
    if (chem_is_drawn) ret &= prtcl.chem;
#else
    bool ret = draw_crit[prtcl.prop];
    if (chem_is_drawn) ret &= prtcl.chem;
    if (prtcl.prop == 2 && prtcl.chem) ret = false;
#endif
    return ret;
  } else {
    const float in_prod = nv[0] * prtcl.r[0] * inv_box_size[0] + nv[1] * prtcl.r[1] * inv_box_size[1] + nv[2] * prtcl.r[2] * inv_box_size[2];
    bool ret = (in_prod < cut_plane) && draw_crit[prtcl.prop];

#if 0
    if (chem_is_drawn) ret &= prtcl.chem;
#else
    if (chem_is_drawn) {
      if((prtcl.prop == 1) || (prtcl.prop == 0)) {
        ret &= prtcl.chem;
      } else if(prtcl.prop == 2) {
        ret &= true;
      }
    }
#endif

    return ret;
  }
}

void DrawSys::ChangeNormalVector(int i) {
  nv[0] = nv[1] = nv[2] = 0.0;
  switch(i) {
  case 0:
    nv[0] = 1.0;
    break;
  case 1:
    nv[1] = 1.0;
    break;
  case 2:
    nv[2] = 1.0;
    break;
  case 3:
    nv[0] = nv[1] = nv[2] = 1.0 / std::sqrt(3.0);
    break;
  default:
    break;
  }
}

void DrawSys::ChangeCrossSection() {
  static float sign = 1.0f;
  if(cut_plane >= 0.2 ) {
    sign = -1.0f;
  } else if(cut_plane <= -0.2) {
    sign = 1.0f;
  }
  if (cut_adv) cut_plane += sign * 0.03;
}

void DrawSys::Dump2Jpg() {
  if (crit_out) {
    jpgout->GetWindowInfo();
    jpgout->SnapijgImage();
    jpgout->SaveImgJpeg(cur_time, all_time, cur_dir, time_step);
  }
}

void DrawSys::ChangeLookDirection(const int i) {
  p_center2eye[0] = p_center2eye[1] = p_center2eye[2] = 0.5;
  p_center2eye[i] = 6.0;
  p_base_z[0] = p_base_z[1] = p_base_z[2] = 0.0;
  if (i == 2) {
    p_base_z[1] = 1.0;
  } else {
    p_base_z[2] = 1.0;
  }
}

bool DrawSys::InitGlew() const {
  const GLenum err = glewInit();
  if (err != GLEW_OK){
    std::cerr << glewGetErrorString(err) << std::endl;
    return false;
  }
  return true;
}

void DrawSys::SkipFileLines(std::ifstream& fin, const size_t frame_num, const int sign) {
  size_t line = 0;
  if (sign > 0) {
    cur_time += frame_num * time_step;
    line = frame_num * pN;
  } else {
    cur_time -= frame_num * time_step;
    line = cur_time / time_step * pN;
    fin.seekg(std::ios::beg);
  }
  for (size_t i=0; i<line; i++) {
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

