#include "sysdraw.hpp"
#include "mousehandle.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

std::unique_ptr<DrawSys    > callbacks::drawsys(nullptr);
std::unique_ptr<MouseHandle> callbacks::mousehandle(nullptr);

namespace {
  enum{
    CORRECT_OPTTAG_NUM = 8,
  };
  
  void print_usege_info(){
    std::cerr << "usage:					" << std::endl;
    std::cerr << "-h (Print usage info)				" << std::endl;
    std::cerr << "-i (Input directory) directory name		" << std::endl;
    std::cerr << "-t (Draw type) slide or anime		        " << std::endl;
    std::cerr << "-o (Output jpeg files) 1(true) or 0(false)	" << std::endl;
    std::cerr << "-b (Begin time)				" << std::endl;
    std::exit(1);
  }
  
  std::vector<std::string> 
  get_argvs(const int argc, char* argvs_[]){
    std::vector<std::string> argvs;
    for(int i=1; i<argc; i++) argvs.push_back(argvs_[i]);
    return argvs;
  }
  
  bool input_is_correct(const std::vector<std::string>& argvs){
    for(auto it = argvs.cbegin(); it != argvs.cend(); ++it) 
      if(*it == "-h") return false;
    
    if(argvs.size() != CORRECT_OPTTAG_NUM) return false;
    return true;
  }

  void dump_err_unknown_opt(const std::string& opt,
			    const std::string& tag)
  {
    std::cerr << "unkown option " << opt << std::endl;;
    if(tag == "-i") std::cerr << "-i directory name		" << std::endl;
    if(tag == "-t") std::cerr << "-t drawing type (slide/anime)	" << std::endl;
    if(tag == "-o") std::cerr << "-o jpeg output (1/0)		" << std::endl;
    if(tag == "-b") std::cerr << "-b begin time			" << std::endl;
    exit(1);
  }
  
  void get_options(std::map<std::string, std::string>& tag_opts,
		   const std::vector<std::string>& argvs)
  {
    for(size_t i=0; i<argvs.size(); i += 2) 
      tag_opts[ argvs[i] ] = argvs[i + 1];
    
    const auto end_it = tag_opts.cend();
    const bool tag_is_correct = (tag_opts.find("-i") != end_it) && (tag_opts.find("-t") != end_it) && (tag_opts.find("-o") != end_it) && (tag_opts.find("-b") != end_it);
    if(tag_is_correct){
      const bool flag_t_correct = (tag_opts["-t"] == "slide" ) || (tag_opts["-t"] == "anime");
      const bool flag_o_correct = (tag_opts["-o"] == "0"     ) || (tag_opts["-o"] == "1"    );
      const bool flag_b_correct = ( atoi(tag_opts["-b"].c_str() ) >= 0);
      if(!flag_t_correct) dump_err_unknown_opt("-t", tag_opts["-t"]);      
      if(!flag_o_correct) dump_err_unknown_opt("-o", tag_opts["-o"]);
      if(!flag_b_correct) dump_err_unknown_opt("-b", tag_opts["-b"]);
    }else{
      print_usege_info();      
    }
  }
}

int main(int argc, char* argv[]){
  const auto argvs = get_argvs(argc, argv);
  if(!input_is_correct(argvs) ) print_usege_info();
  std::map<std::string, std::string> tag_opts;
  get_options(tag_opts, argvs);

  const auto cur_dir  = tag_opts["-i"];
  const auto crit_out = static_cast<bool>( atoi(tag_opts["-o"].c_str() ) );
  const auto beg_time = std::atoi(tag_opts["-b"].c_str() );

  if(tag_opts["-t"] == "anime"){
    callbacks::drawsys.reset(new AnimeDraw (cur_dir, crit_out, beg_time) );
  }else if(tag_opts["-t"] == "slide") {
    callbacks::drawsys.reset(new SlideDraw (cur_dir, crit_out, beg_time) );
  }else{
    std::exit(1);
  }

  callbacks::drawsys->SetParams();
  
  callbacks::mousehandle.reset(new MouseHandle (0.0, 0.0, 0.0, 6.0, 17.0));
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
  callbacks::drawsys->InitWindow(argc, argv, cur_dir.c_str() );
  callbacks::drawsys->InitGlew();
  
  callbacks::drawsys->SetCallBackFunc();
  callbacks::drawsys->InitColor();

  callbacks::drawsys->PrintDrawInfo();
  callbacks::drawsys->Execute();
}
