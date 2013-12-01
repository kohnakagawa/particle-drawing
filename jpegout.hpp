#pragma once

#include <vector>

class Jpegout{
private:
  enum{
    MAX_HEIGHT = 2048
  };
  int b_wid,b_hei;
  std::vector<float> ijg_buffer;
  float* ptr_buffer;
public:
  void PrepSavingImage();
  void SnapijgImage(void);
  void SaveImgJpeg(int current_time,int all_time_steps, char* current_dir, int time_step);
};



