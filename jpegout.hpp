#pragma once

#include <vector>
#include <cmath>

class Jpegout{
private:
  enum{
    MAX_HEIGHT = 2048
  };
  int b_wid,b_hei;
  std::vector<float> ijg_buffer;
  float* ptr_buffer;
  int jpeg_time;
  const int MAX_TIME;
  int numb_digit;
public:
  Jpegout(int max_time):MAX_TIME(max_time){
    jpeg_time=0;
    numb_digit = static_cast<int>(log10(MAX_TIME))+1;
  };
  void PrepSavingImage();
  void SnapijgImage(void);
  void SaveImgJpeg(int current_time,int all_time_steps, char* current_dir, int time_step);
  int GetJpegTime(void){
    return jpeg_time;
  }
};



