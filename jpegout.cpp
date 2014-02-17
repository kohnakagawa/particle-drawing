//使い方
/*
glutSwapBuffers()を呼び出す直前に以下の用に挿入する。

PrepSavingImage();
SnapijgImage();
SaveImgJpeg(current_time,all_time_steps,current_dir);

縦2048ピクセル以上の大きさの画像はdeclineする。
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/freeglut.h>
#include <jpeglib.h>
#include "jpegout.hpp"

void Jpegout::PrepSavingImage()
{
  const int wid = glutGet(GLUT_WINDOW_WIDTH);
  const int hei = glutGet(GLUT_WINDOW_HEIGHT);  
  if(wid == b_wid && hei == b_hei) return;
  if(hei > MAX_HEIGHT){
    std::cout << "too large size." << std::endl;
    exit(1); //強制終了
  }

  b_wid = wid;
  b_hei = hei;
  ijg_buffer.clear();
  ijg_buffer.reserve(3*wid*hei);
}

void Jpegout::SnapijgImage(void)
{
  glReadBuffer(GL_FRONT); //参考http://www.khronos.org/opengles/sdk/1.1/docs/man/glPixelStorei.xml
  //フレームバッファの内容を取得  
  //temp_p_buf = &ijg_buffer[0];
  std::vector<float>::iterator it_b = ijg_buffer.begin();
  ptr_buffer = &*it_b;
  glReadPixels(0,0,b_wid,b_hei,GL_RGB,GL_UNSIGNED_BYTE,ptr_buffer);
}

void Jpegout::SaveImgJpeg(int current_time,int all_time_steps, char* current_dir, int time_step)//参考:http://www.math.meiji.ac.jp/~mk/labo/2008/opengl-glut/node10.html
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW img[MAX_HEIGHT];
  FILE *outfile;
  JSAMPLE*  fram_buffer = (JSAMPLE*)ptr_buffer;
  
  /*  const int numb_digit = static_cast<int>(log10(all_time_steps/time_step)) + 1;
  std::stringstream ss;
  ss << current_dir  << "/time" << std::setw(numb_digit) << std::setfill('0') << (int)current_time/time_step << ".jpeg";*/
  
  std::stringstream ss;
  ss << current_dir << "/time" << std::setw(numb_digit) << std::setfill('0') << jpeg_time << ".jpeg";
  
  //JPEGオブジェクトの初期化
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  
  //ファイルを開く
  outfile = fopen(ss.str().c_str(), "wb");
  jpeg_stdio_dest(&cinfo,outfile);

  //パラメータの設定
  cinfo.image_width = b_wid;
  cinfo.image_height = b_hei;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  //デフォルト値の設定
  jpeg_set_defaults(&cinfo);
  
  //画質の設定
  jpeg_set_quality(&cinfo, 100, TRUE);
  
  //圧縮の開始
  jpeg_start_compress(&cinfo, TRUE);

  for(int i=0; i< b_hei; i++){
      img[i] = fram_buffer + (b_hei - i) * 3 * b_wid;
    }

  jpeg_write_scanlines(&cinfo, img, b_hei);
  jpeg_finish_compress(&cinfo); //圧縮の終了
  jpeg_destroy_compress(&cinfo);//JPEGオブジェクトの破棄
  fclose(outfile); //ファイルを閉じる

  jpeg_time++;
}
