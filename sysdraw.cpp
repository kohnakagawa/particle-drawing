#include <cstdlib>
#include <iostream>
#include "sysdraw.hpp"
#include "mousehandle.hpp"

void drwsys_clbck::disp_callback(void){
  Drawsys->Display();
}
void drwsys_clbck::timer_callback(int value){
  Drawsys->Timer(value);
}
void drwsys_clbck::resize_callback(int w,int h){
  Drawsys->Resize(w,h);
}
void drwsys_clbck::keyboard_callback(unsigned char key,int x,int y){
  Drawsys->KeyBoard(key,x,y);
}

void drawsys::SetParamParticle(double scL,double prad,int seedN,int pN){
  this->scL   = scL;
  this->prad  = prad;
  this->seedN = seedN;
  this->pN    = pN;
  
  if(seedN > 15){
    std::cout << "the number of max particle seed is 15" << std::endl;
    exit(1);
  }

  invL = 1./scL;
  this->prad *= invL;

  Particle.resize(pN);
  p_color.reserve(seedN);
  
  draw_crit.set(); //all true
  draw_crit_max = Pow_n(2,seedN);
  draw_crit_base = draw_crit_max - 1;
  
  for(int i=0; i<seedN; i++){
    int base_b = Pow_n(2,i);
    draw_crit_mask.push_back(base_b);
  }
}

void drawsys::SetParamTime(int all_time,int time_step){
  this->all_time = all_time;
  this->time_step = time_step;
  this->all_time -= 3*time_step;

  cur_time = 0;
}

void drawsys::SetCallBackFunc() const {
  glutDisplayFunc(drwsys_clbck::disp_callback); 
  glutTimerFunc(50,drwsys_clbck::timer_callback,0);
  glutReshapeFunc(drwsys_clbck::resize_callback);
  glutMouseFunc(moushandl_clbck::mouseclick_callback);
  glutMotionFunc(moushandl_clbck::mousemotion_callback);
  glutMouseWheelFunc(moushandl_clbck::mousewheel_callback);
  glutKeyboardFunc(drwsys_clbck::keyboard_callback);
}

void drawsys::SetColor(GLfloat* col){
  color_t buf;
  buf.p[0] = col[0];
  buf.p[1] = col[1];
  buf.p[2] = col[2];
  p_color.push_back(buf);
}

void drawsys::SetLightPos(GLfloat* pos){
  light_t lig;
  lig.l[0] = pos[0];
  lig.l[1] = pos[1];
  lig.l[2] = pos[2];
  lig.l[3] = pos[3];
  lightpos.push_back(lig);
}

void drawsys::GetMouseInfo(double* fov_,double* perscent,double* cent2eye,double* base_z)
{
  p_fovy       = fov_;
  p_perscenter = perscent;
  p_center2eye = cent2eye;
  p_base_z     = base_z;
};

void drawsys::InitWindowSys(int argc,char* argv[]) const {
  glutInit(&argc, argv); //OpenGL初期化
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //ディスプレイの表示モードを指定
  glutCreateWindow(argv[1]); //ウィンドウを開く
}

void drawsys::InitCube(){
  vertex[0][0] = 0.;
  vertex[0][1] = 0.;
  vertex[0][2] = 0.;
  
  vertex[1][0] = 1.;
  vertex[1][1] = 0.;
  vertex[1][2] = 0.;

  vertex[2][0] = 1.;
  vertex[2][1] = 1.;
  vertex[2][2] = 0.;

  vertex[3][0] = 0.;
  vertex[3][1] = 1.;
  vertex[3][2] = 0.;

  vertex[4][0] = 0.;
  vertex[4][1] = 0.;
  vertex[4][2] = 1.;

  vertex[5][0] = 1.;
  vertex[5][1] = 0.;
  vertex[5][2] = 1.;

  vertex[6][0] = 1.;
  vertex[6][1] = 1.;
  vertex[6][2] = 1.;

  vertex[7][0] = 0.;
  vertex[7][1] = 1.;
  vertex[7][2] = 1.;

  cubeedge[0][0] = 0;
  cubeedge[0][1] = 1;
  
  cubeedge[1][0] = 1;
  cubeedge[1][1] = 2;

  cubeedge[2][0] = 2;
  cubeedge[2][1] = 3;

  cubeedge[3][0] = 3;
  cubeedge[3][1] = 0;

  cubeedge[4][0] = 4;
  cubeedge[4][1] = 5;

  cubeedge[5][0] = 5;
  cubeedge[5][1] = 6;

  cubeedge[6][0] = 6;
  cubeedge[6][1] = 7;

  cubeedge[7][0] = 7;
  cubeedge[7][1] = 4;

  cubeedge[8][0] = 0;
  cubeedge[8][1] = 4;

  cubeedge[9][0] = 1;
  cubeedge[9][1] = 5;

  cubeedge[10][0] = 2;
  cubeedge[10][1] = 6;

  cubeedge[11][0] = 3;
  cubeedge[11][1] = 7;
}

void drawsys::InitCut(){
  swt_but  = 0;
  cut_but  = 0;
  cut_adv  = 0;
  cut_axis = 0;
}

void drawsys::SetWindow() const{
  glutInitWindowPosition(200,400); //ウィンドウの位置を指定
  glutInitWindowSize(1200,900); //ウィンドウサイズ指定
}

void drawsys::InitColor() const{
  glClearColor(1.0,1.0,1.0,1.0); //(Red,Green,Blue,A) Aは透明度 ウィンドウを塗りつぶす色を指定
  glEnable(GL_DEPTH_TEST); //デプスバッファ使用
  glEnable(GL_LIGHTING); //光源使用
  glEnable(GL_LIGHT0); //光源0を設定
}

void drawsys::FileManag(){
  std::stringstream ss;
  ss << cur_dir << "/particle_data.txt";
  fin.open(ss.str().c_str());
}

void drawsys::Timer(int value){
  if (cur_time > all_time){
    if(crit_out == false){
      swt_but = 1;
    }else{
      std::cout << "delete objects" << std::endl;
      delete MouseHandle;
      delete this;
      exit(0);
    }
  }
  glutPostRedisplay();
  if(swt_but == 0) cur_time += time_step;
  glutTimerFunc(50,drwsys_clbck::timer_callback, 0);
}

void drawsys::Drawxyz(){
  glBegin(GL_LINES);

  glColor3d(0,1,0);
  glVertex2d(-100,0);
  glVertex2d(100, 0);

  glColor3d(1,0,0);
  glVertex2d(0,0);
  glVertex2d(0,100);

  glColor3d(0,0,1);
  glVertex3d(0,0,-100);
  glVertex3d(0,0, 100);
  glEnd();
}

void drawsys::Resize(int w,int h) const {
  glViewport(0,0,w,h); //ビューポート範囲指定　この場合ウィンドウ全体
  
  glMatrixMode(GL_PROJECTION); //透視変換行列設定
  glLoadIdentity(); //変換行列初期化
  
  gluPerspective(*p_fovy, (double)w / (double)h, 0.1, 200.0); //透視投影　カメラの画角、アスペクト比を指定

  glMatrixMode(GL_MODELVIEW); //モデルビュー変換行列設定
  glLoadIdentity();

  gluLookAt(p_perscenter[0] + p_center2eye[0],
	    p_perscenter[1] + p_center2eye[1],
	    p_perscenter[2] + p_center2eye[2],
	    p_perscenter[0],
	    p_perscenter[1],
	    p_perscenter[2],
	    p_base_z[0],
	    p_base_z[1],
	    p_base_z[2]);
  //  gluLookAt(5.0,4.0,3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0); 

}

void drawsys::Display(){
  static double cut_plane = 1.0;
  static double cut_skew = 3.0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //カラーバッファ,デプスバッファ指定
  GLfloat light0pos[4];
  light0pos[0] = lightpos[0].l[0];
  light0pos[1] = lightpos[0].l[1];
  light0pos[2] = lightpos[0].l[2];
  light0pos[3] = lightpos[0].l[3];

  glLightfv(GL_LIGHT0, GL_POSITION, light0pos); //光源0指定 視点を設定した後で指定する。
  
  if(swt_but == 0){
      LoadParticleDat(); //粒子の座標データを読み込む
  }
  switch(cut_but)
    {
    case 0://普通に描画
      for(int i=0; i<pN; i++){
	const int p_prop = Particle[i].prop;
	GLfloat color[3];
	color[0] = p_color[p_prop].p[0];
	color[1] = p_color[p_prop].p[1];
	color[2] = p_color[p_prop].p[2];
	
	if(draw_crit[p_prop] == true){
	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	  glPushMatrix();
	  glTranslated(Particle[i].r[0],Particle[i].r[1],Particle[i].r[2]);
	  glutSolidSphere(prad,10,10);
	  glPopMatrix();
	}
      }
      break;
    case 1://ある軸についてcut
      if(cut_plane < 0.0) cut_plane += 1.0;
      for(int i=0; i<pN; i++){
	const int p_prop = Particle[i].prop;
	GLfloat color[3];
	color[0] = p_color[p_prop].p[0];
	color[1] = p_color[p_prop].p[1];
	color[2] = p_color[p_prop].p[2];
	
	if(Particle[i].r[cut_axis] < cut_plane && draw_crit[p_prop] == true){
	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	  glPushMatrix();
	  glTranslated(Particle[i].r[0],Particle[i].r[1],Particle[i].r[2]);
	  glutSolidSphere(prad,10,10);
	  glPopMatrix();
	}
      }
      if(cut_adv == 0) cut_plane -= 0.01;
      break;
    case 2://斜めにカット
      if(cut_skew < 0.0) cut_skew += 3.0;
      for(int i=0; i<pN; i++){
	const int p_prop = Particle[i].prop;
	GLfloat color[3];
	color[0] = p_color[p_prop].p[0];
	color[1] = p_color[p_prop].p[1];
	color[2] = p_color[p_prop].p[2];
	double cut_plane_skew = Particle[i].r[0] + Particle[i].r[1] + Particle[i].r[2];
	
	if(cut_plane_skew < cut_skew && draw_crit[p_prop] == true){
	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	  glPushMatrix();
	  glTranslated(Particle[i].r[0],Particle[i].r[1],Particle[i].r[2]);
	  glutSolidSphere(prad,10,10);
	  glPopMatrix();
	}
      }
      if(cut_adv == 0) cut_skew -= 0.05;
      break;
    }  
  
  //立方体描画
  glColor3d(0.0, 0.0, 0.0);
  glBegin(GL_LINES);
  for (int i = 0; i < 12; ++i) {
    glVertex3dv(vertex[cubeedge[i][0]]);
    glVertex3dv(vertex[cubeedge[i][1]]);
  }
  glEnd();

  //xyz軸描画
  //Drawxyz();
  if(cur_time != 0 && crit_out == true){
    jpgout.PrepSavingImage();
    jpgout.SnapijgImage();
    jpgout.SaveImgJpeg(cur_time,all_time,cur_dir,time_step);
  }

  glutSwapBuffers();
}

void drawsys::KeyBoard(unsigned char key,int x,int y){
  const int wid = glutGet(GLUT_WINDOW_WIDTH);
  const int hei = glutGet(GLUT_WINDOW_HEIGHT);
  switch(key)
    {
    case 'b':
      swt_but = 0; //描画を再開する
      break;
    case 'B':
      cut_adv = 0; //断面カットを再開する。
      break;
    case 's':
      swt_but = 1; //描画をストップする
      break;
    case 'S':
      cut_adv = 1;//断面カットを一時停止する。
      break;
    case 'q'://断面カットモードを中断し、描画にもどる。
      swt_but = 0;
      cut_but = 0;
      break;
    case 'x':
      swt_but = 1; //描画を一度中断する。
      cut_adv = 0; //断面カット再開
      cut_but = 1; 
      cut_axis = 0;//x軸にそってカット
      break;
    case 'y': 
      swt_but = 1; //描画を一度中断する。
      cut_adv = 0; //断面カット再開
      cut_but = 1; 
      cut_axis = 1;//y軸にそってカット
      break;
    case 'z':
      swt_but = 1; //描画を一度中断する。
      cut_adv = 0; //断面カット再開
      cut_but = 1; 
      cut_axis = 2;//z軸にそってカット
      break;
    case 'X':
      p_center2eye[0] = 6.0;
      p_center2eye[1] = 0.5;
      p_center2eye[2] = 0.5;
      p_base_z[0] = 0.0;
      p_base_z[1] = 0.0;
      p_base_z[2] = 1.0;
      Resize(wid,hei);
      break;
    case 'Y':
      p_center2eye[0] = 0.5;
      p_center2eye[1] = 6.0;
      p_center2eye[2] = 0.5;
      p_base_z[0] = 0.0;
      p_base_z[1] = 0.0;
      p_base_z[2] = 1.0;
      Resize(wid,hei);
      break;
    case 'Z':
      p_center2eye[0] = 0.5;
      p_center2eye[1] = 0.5;
      p_center2eye[2] = 6.0;
      p_base_z[0] = 0.0;
      p_base_z[1] = 1.0;
      p_base_z[2] = 0.0;
      Resize(wid,hei);
      break;
    case 'c':
      swt_but = 1; //描画を一度中断する。
      cut_adv = 0; //断面カット再開
      cut_but = 2; //斜めにカット
      break;
    case 'h':
      swt_but = 1;
      ChgDrawObject();
      Resize(wid,hei);
      break;
    default:
      break;		  
    }
}

void drawsys::PrintDrawInfo() const {
  std::cout << "b restart drawing" << std::endl;
  std::cout << "s stop drawing" << std::endl;
  std::cout << "c start cutting(skew)" << std::endl;
  std::cout << "x start cutting(x axis)" << std::endl;
  std::cout << "y start cutting(y axis)" << std::endl;
  std::cout << "z start cutting(z axis)" << std::endl;
  std::cout << "B restart cutting" << std::endl;
  std::cout << "S stop cutting" << std::endl;
  std::cout << "q quit cutting mode" << std::endl;
  std::cout << "X look along x axis" << std::endl;
  std::cout << "Y look along y axis" << std::endl;
  std::cout << "Z look along z axis" << std::endl;
  std::cout << "h change visible object." << std::endl;
}

void drawsys::LoadParticleDat(){
  for(int i=0; i<pN; i++){
    fin >> Particle[i].r[0] >> Particle[i].r[1] >> Particle[i].r[2] >> Particle[i].prop;
  }
  for(int i=0; i<pN; i++){
    Particle[i].r[0] *= invL;
    Particle[i].r[1] *= invL;
    Particle[i].r[2] *= invL;
  }
}

void drawsys::ChgDrawObject(){
  draw_crit_base++;
  if(draw_crit_base == draw_crit_max) draw_crit_base = 1;
  
  for(int i=0; i<seedN; i++){
    draw_crit[i] = ((draw_crit_base&draw_crit_mask[i])==draw_crit_mask[i]);
  }
}

int drawsys::Pow_n(int x, int n) const {
  int a = 1;
  while(n > 0){
    if(n % 2 == 0){
      x = x*x;
      n = n>>1;
    }else{
      a = a*x;
      n--;
    }
  }
  return a;
}
