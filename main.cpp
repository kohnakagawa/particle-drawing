#include "sysdraw.hpp"
#include "mousehandle.hpp"
#include <iostream>
#include <string>


drawsys* Drawsys;
mouse_handle* MouseHandle;

namespace {
  void Warning(){
    std::cout << "usage:" << std::endl;
    std::cout << "argv[1] = target directory." << std::endl;
    std::cout << "argv[2] = 0 or 1." << std::endl;
    std::cout << "argv[2] = 0: There are no jpeg files." << std::endl;
    std::cout << "argv[2] = 1: There are jpeg files in the target directory." << std::endl;
    exit(1);
  }
}

int main(int argc, char* argv[]){
  if(argc < 3) Warning();
  char* cur_dir = argv[1];
  int buff = atoi(argv[2]);
  if(buff > 1 || buff < 0) Warning();
  
  bool crit_out = (bool)buff;

  std::string str = cur_dir;
  str += "/macro_data.txt";
  std::ifstream fin(str.c_str());
  
  double scL,prad;
  int wN,lN;
  int all_time,time_step;
  fin >> wN >> lN >> scL >> prad >> all_time >> time_step;
  const int pN = wN + lN;
  const int seedN = 4;

  //drawing system
  Drawsys = new drawsys (cur_dir,crit_out);
  Drawsys->SetParamParticle(scL,prad,seedN,pN);
  Drawsys->SetParamTime(all_time,time_step);

  //mouse util
  MouseHandle		   = new mouse_handle (0.5,0.5,0.5,6.,17.);
  double*	fovy	   = MouseHandle->RetFovy();
  double*	persCenter = MouseHandle->RetPersCent();
  double*	center2eye = MouseHandle->RetCenter2eye();
  double*	ebase_z    = MouseHandle->RetEbaseZ();
  Drawsys->GetMouseInfo(fovy,persCenter,center2eye,ebase_z);

  //file manager
  Drawsys->FileManag();
  
  //color set
  GLfloat water_c[] = {0.000, 0.749, 1.000};
  GLfloat hyphil_c[] = {1.000, 0.188, 0.188};
  GLfloat hyphob_c[] = {1.000, 0.843, 0.000};
  GLfloat reacted_c[] = {0.498,1.000,0.000};
  Drawsys->SetColor(water_c);
  Drawsys->SetColor(hyphil_c);
  /*  Drawsys->SetColor(hyphob_c);
      Drawsys->SetColor(reacted_c);*/
  Drawsys->SetColor(reacted_c);
  Drawsys->SetColor(hyphob_c);
  
  //light set
  //GLfloat light0pos[] = { 3.0,4.0, 5.0,1.};
  GLfloat light0pos[] = { 0.,0.,-1.,1.};
  Drawsys->SetLightPos(light0pos);

  //cube set
  Drawsys->InitCube();

  //cutting set
  Drawsys->InitCut();
  
  //window set
  Drawsys->SetWindow();
  Drawsys->InitWindowSys(argc,argv);

  //glew init
  Drawsys->InitGlew();
  
  //set call_back fuction
  Drawsys->SetCallBackFunc();

  //init window color
  Drawsys->InitColor();

  //print mouse handle info
  Drawsys->PrintDrawInfo();

  //Execution
  Drawsys->Execute();

  //delete objects
  std::cout << "delete objects" << std::endl;
  delete Drawsys;
  delete MouseHandle;

  return 0;
}
