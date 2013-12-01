#include "sysdraw.hpp"
#include "mouse_handling_rev.hpp"
#include <iostream>

drawsys* Drawsys;
mouse_handle* MouseHandle;

void Warning(){
  std::cout << "usage:" << std::endl;
  std::cout << "argv[1] = target directory." << std::endl;
  std::cout << "argv[2] = 0 or 1." << std::endl;
  std::cout << "argv[2] = 0: There are no jpeg files." << std::endl;
  std::cout << "argv[2] = 1: There are jpeg files in the target directory." << std::endl;
  exit(1);
}

int main(int argc, char* argv[]){
  if(argc < 3) Warning();
  char* cur_dir = argv[1];
  int buff = atoi(argv[2]);
  if(buff > 1 || buff < 0){
    std::cout << buff << std::endl;
    std::cout << "argv[2] should be 0 or 1." << std::endl;
    Warning();
  }
  bool crit_out = (bool)buff;

  std::stringstream ss;
  ss << cur_dir << "/macro_data.txt";
  std::ifstream fin(ss.str().c_str());
  double scL,prad;
  int seedN,pN;
  int wN,lN;
  int all_time,time_step;
  fin >> wN >> lN >> scL >> prad >> all_time >> time_step;
  pN = wN + lN;
  seedN = 3;

  //instance
  Drawsys = new drawsys (cur_dir,crit_out);
  Drawsys->SetParamParticle(scL,prad,seedN,pN);
  Drawsys->SetParamTime(all_time,time_step);

  //file manager
  Drawsys->FileManag();
  
  //color set
  GLfloat water_c[] = {0.000, 0.749, 1.000};
  GLfloat hyphil_c[] = {1.000, 0.188, 0.188};
  GLfloat hyphob_c[] = {1.000, 0.843, 0.000};
  Drawsys->SetColor(water_c);
  Drawsys->SetColor(hyphil_c);
  Drawsys->SetColor(hyphob_c);
  
  //light set
  GLfloat light0pos[] = { 3.0, 4.0, 5.0, 1.0 };
  Drawsys->SetLightPos(light0pos);

  //cube set
  Drawsys->InitCube();

  //cutting set
  Drawsys->InitCut();
  
  //window set
  Drawsys->SetWindow();
  Drawsys->InitWindowSys(argc,argv);
  
  //mouse util
  MouseHandle		   = new mouse_handle (0.5,0.5,0.5,6.,30.);
  double*	fovy	   = MouseHandle->RetFovy();
  double*	persCenter = MouseHandle->RetPersCent();
  double*	center2eye = MouseHandle->RetCenter2eye();
  double*	ebase_z    = MouseHandle->RetEbaseZ();
  Drawsys->GetMouseInfo(fovy,persCenter,center2eye,ebase_z);

  //set call back fuction
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
