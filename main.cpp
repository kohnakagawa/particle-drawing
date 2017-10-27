#include "sysdraw.hpp"
#include "mousehandle.hpp"
#include "cmdline.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <pthread.h>

std::unique_ptr<DrawSys    > callbacks::drawsys(nullptr);
std::unique_ptr<MouseHandle> callbacks::mousehandle(nullptr);

// NOTE:
// These two functions are needed to link libpthread.so at Ubuntu 14.04.
namespace {
  void* simpleFunc(void*) {
    return NULL;
  }

  void forcePThreadLink() {
    pthread_t t1;
    pthread_create(&t1, NULL, simpleFunc, NULL);
  }
}

int main(int argc, char* argv[]){
  cmdline::parser arg_parser;
  arg_parser.add<std::string>("input", 'i', "(Input directory) directory name", true);
  arg_parser.add<std::string>("type", 't', "(Draw type) slide or anime", true);
  arg_parser.add<bool>("out", 'o', "(Output jpeg files) 1(true) or 0(false)", false, false);
  arg_parser.add<int>("begin_time", 'b', "(Begin time)", false, 0);
  arg_parser.parse_check(argc, argv);

  const auto cur_dir  = arg_parser.get<std::string>("input");
  const auto type     = arg_parser.get<std::string>("type");
  const auto crit_out = arg_parser.get<bool>("out");
  const auto beg_time = arg_parser.get<int>("begin_time");

  if(type == "anime") {
    callbacks::drawsys.reset(new AnimeDraw(cur_dir, crit_out, beg_time));
  } else if (type == "slide") {
    callbacks::drawsys.reset(new SlideDraw(cur_dir, crit_out, beg_time));
  } else {
    std::cerr << "Unknown execution mode type\n";
    std::exit(1);
  }

  callbacks::drawsys->SetParams();

  callbacks::mousehandle.reset(new MouseHandle(0.0, 0.0, 0.0, 6.0, 17.0));
  callbacks::drawsys->GetMouseInfo(callbacks::mousehandle->RetFovy(),
                                   callbacks::mousehandle->RetPersCent(),
                                   callbacks::mousehandle->RetCenter2eye(),
                                   callbacks::mousehandle->RetEbaseZ());

  callbacks::drawsys->FileOpen();

  const GLfloat water_c[]   = {0.000, 0.749, 1.000};
  const GLfloat hyphil_c[]	= {1.000, 0.188, 0.188};
  const GLfloat hyphob_c[]	= {1.000, 1.000, 0.000};
  const GLfloat catal_c[]   = {0.545, 0.000, 0.545};
  const GLfloat reacted_c[]	= {0.000, 0.500, 0.000};
  const GLfloat reacted_h[] = {1.000, 0.500, 0.000};
  callbacks::drawsys->SetColor(water_c);
  callbacks::drawsys->SetColor(hyphil_c);
  callbacks::drawsys->SetColor(hyphob_c);
  callbacks::drawsys->SetColor(catal_c);
  callbacks::drawsys->SetColor(reacted_c);
  callbacks::drawsys->SetColor(reacted_h);

  const GLfloat light0pos[] = {0.0, 0.0, -1.0, 1.0};
  callbacks::drawsys->SetLightPos(light0pos);

  callbacks::drawsys->InitCube();
  callbacks::drawsys->InitWindow(argc, argv, cur_dir.c_str());
  callbacks::drawsys->InitGlew();

  callbacks::drawsys->SetCallBackFunc();
  callbacks::drawsys->InitColor();

  callbacks::drawsys->PrintDrawInfo();
  callbacks::drawsys->Execute();
}
