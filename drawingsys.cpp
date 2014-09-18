#include <fstream>
#include "drawingsys.h"



void drawingsys::LoadParam(char* c_dir){
  double L,prad;
  int wN,lN,all_time,time_step;
  std::string fname = c_dir;
  fname += "macro_data.txt";
  std::ifstream fin(fname.c_str());
  fin >> wN >> lN >> L >> prad >> all_time >> time_step;
  
}
