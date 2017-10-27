#include "sysdraw.hpp"
#include "helper_macros.hpp"

#include <iostream>

void SlideDraw::Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //カラーバッファ,デプスバッファ指定
  // 光源と視点が一緒に動かず固定されている場合
  // const GLfloat light0pos[4] = {lightpos[0][0],
  //                               lightpos[0][1],
  //                               lightpos[0][2],
  //                               lightpos[0][3]};
  // glLightfv(GL_LIGHT0, GL_POSITION, light0pos);

  for (const auto& ptcl : Particle) {
    if (IsDrawnObject(ptcl)) RenderSphere(ptcl);
  }

  ChangeCrossSection();
  // DrawCubic();
  // Drawxyz();

  const float col[][3] {{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
  DrawAxis(0.02, 0.3, col);
  RenderCurTime();
  glutSwapBuffers();
}

void SlideDraw::Timer(int value) {
  PD_UNUSED_PARAM(value);
  glutPostRedisplay();
  glutTimerFunc(50, callbacks::wrap_timer, 0);
}

void SlideDraw::KeyBoard(unsigned char key, int x, int y) {
  PD_UNUSED_PARAM(x);
  PD_UNUSED_PARAM(y);

  const int wid = glutGet(GLUT_WINDOW_WIDTH), hei = glutGet(GLUT_WINDOW_HEIGHT);
  const int offset_time = SKIP_FRAME_NUM * time_step;
  switch(key) {
    case 'n':
      if ((cur_time <= all_time) && (cur_time >= 0)) {
        LoadParticleDat();
        cur_time += time_step;
        Resize(wid, hei);
      }
      break;
    case 'N':
      if (((cur_time + offset_time) <= all_time) && ((cur_time - offset_time) >= 0)) {
        SkipFileLines(fin, SKIP_FRAME_NUM, 1);
        LoadParticleDat();
        cur_time += time_step;
        Resize(wid, hei);
      }
      break;
    case 'p':
      if ((cur_time <= all_time) && (cur_time >= 0)) {
        SkipFileLines(fin, 1, -1);
        LoadParticleDat();
        cur_time += time_step;
        SkipFileLines(fin, 1, -1);
        Resize(wid, hei);
      }
      break;
    case 'P':
      if (((cur_time + offset_time) <= all_time) && ((cur_time - offset_time) >= 0)) {
        SkipFileLines(fin, SKIP_FRAME_NUM, -1);
        LoadParticleDat();
        cur_time += time_step;
        SkipFileLines(fin, 1, -1);
        Resize(wid, hei);
      }
      break;
    case 'q':
      cut_but = false;
      break;
    case 'Q':
      cut_but = false;
      break;
    case 'S':
      cut_adv = false;
      break;
    case 'x':
      cut_adv  = cut_but = true;
      ChangeNormalVector(0);
      Resize(wid, hei);
      break;
    case 'y':
      cut_adv  = cut_but = true;
      ChangeNormalVector(1);
      Resize(wid, hei);
      break;
    case 'z':
      cut_adv = cut_but = true;
      ChangeNormalVector(2);
      Resize(wid, hei);
      break;
    case 'c':
      cut_adv = cut_but = true;
      ChangeNormalVector(3);
      Resize(wid, hei);
      break;
    case 'X':
      ChangeLookDirection(0);
      Resize(wid, hei);
      break;
    case 'Y':
      ChangeLookDirection(1);
      Resize(wid, hei);
      break;
    case 'Z':
      ChangeLookDirection(2);
      Resize(wid, hei);
      break;
    case 'h':
      swt_but = false;
      ChgDrawObject();
      Resize(wid, hei);
      break;
    case 'H':
      chem_is_drawn = false;
      Resize(wid,hei);
      break;
    case 'A':
      chem_is_drawn = true;
      Resize(wid,hei);
      break;
    default:
      break;
    }
}

void SlideDraw::PrintDrawInfo() {
  std::cout << "n frameを1進める。		" << std::endl;
  std::cout << "N frameを4進める。		" << std::endl;
  std::cout << "p frameを1戻す。　		" << std::endl;
  std::cout << "P frameを4戻す。　		" << std::endl;
  std::cout << "x start cutting(x axis)		" << std::endl;
  std::cout << "y start cutting(y axis)		" << std::endl;
  std::cout << "z start cutting(z axis)		" << std::endl;
  std::cout << "B restart cutting		" << std::endl;
  std::cout << "S stop cutting			" << std::endl;
  std::cout << "q quit cutting mode		" << std::endl;
  std::cout << "X look along x axis		" << std::endl;
  std::cout << "Y look along y axis		" << std::endl;
  std::cout << "Z look along z axis		" << std::endl;
  std::cout << "h change visible object.	" << std::endl;
}
