#include "sysdraw.hpp"
#include "helper_macros.hpp"

#include <iostream>

void AnimeDraw::Timer(int value) {
  PD_UNUSED_PARAM(value);
  if (cur_time > all_time) {
    if (!crit_out) {
      swt_but = false;
    } else {
      std::exit(0);
    }
  }
  glutPostRedisplay();
  if (swt_but) cur_time += time_step;
  glutTimerFunc(50, callbacks::wrap_timer, 0);
}

void AnimeDraw::Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //カラーバッファ,デプスバッファ指定
  // 光源と視点が一緒に動かず固定されている場合
  // const GLfloat light0pos[4] = {lightpos[0][0],
  //                               lightpos[0][1],
  //                               lightpos[0][2],
  //                               lightpos[0][3]};
  // glLightfv(GL_LIGHT0, GL_POSITION, light0pos);

  if (swt_but) LoadParticleDat();

  for (const auto& ptcl : Particle) {
    if (IsDrawnObject(ptcl)) RenderSphere(ptcl);
  }

  ChangeCrossSection();
  //DrawCubic();
  //Drawxyz();

  const float col[][3] {{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
  DrawAxis(0.02, 0.3, col);
  RenderCurTime();
  Dump2Jpg();
  glutSwapBuffers();
}

void AnimeDraw::KeyBoard(unsigned char key, int x, int y) {
  PD_UNUSED_PARAM(x);
  PD_UNUSED_PARAM(y);

  const auto wid = glutGet(GLUT_WINDOW_WIDTH), hei = glutGet(GLUT_WINDOW_HEIGHT);
  switch(key)
    {
    case 'b':
      swt_but = true;
      break;
    case 'B':
      cut_adv = true;
      break;
    case 's':
      swt_but = false;
      break;
    case 'S':
      cut_adv = false;
      break;
    case 'q':
      swt_but = true;
      cut_but = false;
      break;
    case 'Q':
      swt_but = false;
      cut_but = false;
      break;
    case 'x':
      swt_but  = false;
      cut_adv  = cut_but = true;
      ChangeNormalVector(0);
      Resize(wid, hei);
      break;
    case 'y':
      swt_but  = false;
      cut_adv  = cut_but = true;
      ChangeNormalVector(1);
      Resize(wid, hei);
      break;
    case 'z':
      swt_but = false;
      cut_adv = cut_but = true;
      ChangeNormalVector(2);
      Resize(wid, hei);
      break;
    case 'c':
      swt_but = false;
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

void AnimeDraw::PrintDrawInfo() {
  std::cout << "b restart drawing"       << std::endl;
  std::cout << "s stop drawing"          << std::endl;
  std::cout << "c start cutting(skew)"   << std::endl;
  std::cout << "x start cutting(x axis)" << std::endl;
  std::cout << "y start cutting(y axis)" << std::endl;
  std::cout << "z start cutting(z axis)" << std::endl;
  std::cout << "B restart cutting"       << std::endl;
  std::cout << "S stop cutting"          << std::endl;
  std::cout << "q quit cutting mode"     << std::endl;
  std::cout << "X look along x axis"     << std::endl;
  std::cout << "Y look along y axis"     << std::endl;
  std::cout << "Z look along z axis"     << std::endl;
  std::cout << "h change visible object."<< std::endl;
}
