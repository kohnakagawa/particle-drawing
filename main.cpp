#include "sysdraw.hpp"
#include "mousehandle.hpp"
#include <iostream>
#include <string>

DrawSys*     callbacks::drawsys;
MouseHandle* callbacks::mousehandle;

namespace {
  void warning(){
    std::cout << "usage:" << std::endl;
    std::cout << "argv[1] = target directory." << std::endl;
    std::cout << "argv[2] = 0 or 1." << std::endl;
    std::cout << "argv[2] = 0: There are no jpeg files." << std::endl;
    std::cout << "argv[2] = 1: There are jpeg files in the target directory." << std::endl;
    exit(1);
  }
}

int main(int argc, char* argv[]){
  if(argc < 3) warning();
  const std::string cur_dir = argv[1];
  const int buff = atoi(argv[2]);
  if(buff > 1 || buff < 0) warning();
  const bool crit_out = static_cast<bool>(buff);

  //drawing system
  callbacks::drawsys = new DrawSys (cur_dir, crit_out);
  callbacks::drawsys->SetParams();
  callbacks::drawsys->AllocateResource();

  callbacks::mousehandle = new MouseHandle (0.0, 0.0, 0.0, 6.0, 17.0);
  callbacks::drawsys->GetMouseInfo(callbacks::mousehandle->RetFovy(), 
				   callbacks::mousehandle->RetPersCent(), 
				   callbacks::mousehandle->RetCenter2eye(),
				   callbacks::mousehandle->RetEbaseZ());

  callbacks::drawsys->FileOpen();
  
  const GLfloat water_c[]	= {0.000, 0.749, 1.000};
  const GLfloat hyphil_c[]	= {1.000, 0.188, 0.188};
  const GLfloat hyphob_c[]	= {1.000, 1.000, 0.000};
  const GLfloat reacted_c[]	= {0.000, 0.500, 0.000};
  callbacks::drawsys->SetColor(water_c);
  callbacks::drawsys->SetColor(hyphil_c);
  callbacks::drawsys->SetColor(hyphob_c);
  callbacks::drawsys->SetColor(reacted_c);
  
  const GLfloat light0pos[] = {0.0, 0.0, -1.0, 1.0};
  callbacks::drawsys->SetLightPos(light0pos);

  callbacks::drawsys->InitCube();
  callbacks::drawsys->InitWindow(argc, argv);
  callbacks::drawsys->InitGlew();
  
  callbacks::drawsys->SetCallBackFunc();
  callbacks::drawsys->InitColor();

  callbacks::drawsys->PrintDrawInfo();
  callbacks::drawsys->Execute();

  std::cout << "delete objects" << std::endl;
  delete callbacks::drawsys;
  delete callbacks::mousehandle;
}
