#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <string.h>
#include <sys/uio.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ad_log.h"

typedef char* CharT;

//std::string jsonString =
//    "{\"elapse\":14966,\"iface_name\":\"response_to_client\",\"in_out\":\"out\",\"resp_code\":0,\"resp_text\":{\"ad_response\":{\"bid\":[{\"ad_network_id\":\"MTSSP2017011100002\",\"adid\":\"\",\"adm\":\"<div style=\\\"width:640px; height:100px;overflow:hidden;position:relative;\\\"><a href=\\\"http://t2.paiyue.com/clk/nihao?pr=ND9TfQKOzG1PiyNLlADThtp0jGu0qXs9MJKIXTVfwHL4.UMtEwuoiWObaIF5NOmhUD1o4kC2DUAwlIJPQuydBp9Y5kpfG_ee_u13VkQY8ZYIxLLhQKAQ1fPZiZPPsRNLUcNo.bmY09x77Rxy489IWviXilp6Hfsh_z1D7aqetlqQGBMgXC9wwOuZToVOdEmcbAD2z7mcICYNl.qOGAAdFGQdDLeS2ZPzvXuUikMtslFQ3BugGjV0FaPrpZmC_OSLqcxoXb9YUEKr&km=5zRsiDONqOssKR701UgPjv\\\" target=\\\"_blank\\\"><img border=\\\"0\\\" width=\\\"640\\\" height=\\\"100\\\" src=\\\"http://c4.py0.cc/2016/252/0659/11503.png\\\" alt=\\\"\\\"/></a></div>\",\"clk_through_url\":\"http://t2.paiyue.com/clk/nihao?pr=ND9TfQKOzG1PiyNLlADThtp0jGu0qXs9MJKIXTVfwHL4.UMtEwuoiWObaIF5NOmhUD1o4kC2DUAwlIJPQuydBp9Y5kpfG_ee_u13VkQY8ZYIxLLhQKAQ1fPZiZPPsRNLUcNo.bmY09x77Rxy489IWviXilp6Hfsh_z1D7aqetlqQGBMgXC9wwOuZToVOdEmcbAD2z7mcICYNl.qOGAAdFGQdDLeS2ZPzvXuUikMtslFQ3BugGjV0FaPrpZmC_OSLqcxoXb9YUEKr&km=5zRsiDONqOssKR701UgPjv\",\"crid\":\"58559\",\"exp\":128,\"id\":\"TEST_DSP_RESP_impid_0\",\"imp_track_urls\":[\"WND3iwAH1EEAAAAAAAAAALGrZQ4k4JMima_jVA==\"],\"impid\":\"impid\",\"price\":500,\"seat\":\"1\",\"w\":86231200}],\"id\":\"e7dc7a40-0e1b-11e7-b77f-c3a997f607b8\"},\"error_code\":0,\"msg\":\"\"},\"service\":\"ssp_service\"}";
std::string jsonString = "this is a trace message";

void test_log() {
  LOG(trace, "a_pid", "normal") << jsonString;
  LOG(debug, "a_pid", "normal") << jsonString;
  LOG(info, "a_pid", "normal") << jsonString;
  LOG(warn, "a_pid", "normal") << jsonString;
  LOG(error, "a_pid", "normal") << jsonString;
  LOG(fatal, "a_pid", "normal") << jsonString;
  LOG(report, "a_pid", "normal") << jsonString;
  LOG(trace, "a_pid", "normal") << jsonString;
  LOG(debug, "a_pid", "normal") << jsonString;
  LOG(info, "a_pid", "normal") << jsonString;
}

void test_empty_log() {
  LOG(trace, "a_pid", "normal");
  LOG(debug, "a_pid", "normal");
  LOG(info, "a_pid", "normal");
  LOG(warn, "a_pid", "normal");
  LOG(error, "a_pid", "normal");
  LOG(fatal, "a_pid", "normal");
  LOG(report, "a_pid", "normal");
  LOG(trace, "a_pid", "normal");
  LOG(debug, "a_pid", "normal");
  LOG(info, "a_pid", "normal");
}

void loop_test_log(int num) {
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < 10000; j++) {
      test_log();
    }
  }
}

int main(int argc, char* argv[]) {

  mtad::adlog::AdLog::Instance()->Init(true, mtad::adlog::develop, "logs", "server_name");
  SET_LOG_LEVEL(trace)

//  const std::string vvv = "vav";
//  LOG_INFO << (char)'a';
//  LOG_INFO << (char*)"abc";
//  LOG_INFO << true;
//  LOG_INFO << (signed char)'a';
//  LOG_INFO << (unsigned char)'a';
//  LOG_INFO << (short)888;
//  LOG_INFO << (unsigned short)888;
//  LOG_INFO << (int)999;
//  LOG_INFO << (unsigned int)999;
//  LOG_INFO << (long)100;
//  LOG_INFO << (unsigned long)100;
//  LOG_INFO << (long long)1000;
//  LOG_INFO << (unsigned long long)1000;
//  LOG_INFO << (float)1000.23;
//  LOG_INFO << (double)1001.123;
//  LOG_INFO << (long double)100000.123;
//  LOG_INFO << std::string("def");
//  LOG_INFO << vvv;
//  LOG_INFO << (void*)&vvv;

  int thread_num = 0;
  if (argc == 2) {
    thread_num = atoi(argv[1]);
  }

//  jsonString += jsonString;
//  jsonString += jsonString;
//  jsonString += jsonString;

  int loop_num = 10;
  std::cout << "thread num: " << thread_num << std::endl;
  std::vector<std::thread> array;
  for (int i = 0; i < thread_num; i++) {
    array.push_back(std::thread(loop_test_log, loop_num));
  }

  timeval tv1;
  gettimeofday(&tv1, NULL);
  for (int i = 0; i < thread_num; i++) {
    array[i].join();
  }

  timeval tv2;
  gettimeofday(&tv2, NULL);

  std::cout << "seconds: " << tv2.tv_sec - tv1.tv_sec << std::endl;
  std::cout << "milliseconds: " << tv2.tv_usec - tv1.tv_usec << std::endl;
  float t = (tv2.tv_sec - tv1.tv_sec) * 1000000.0 + (tv2.tv_usec - tv1.tv_usec);
  std::cout << "speed: " << "10.0 * " << thread_num << " * " << loop_num
            << " * 1000000.0 / " << t << " = "
            << 10.0 * thread_num * loop_num * 1000000.0 / t << "(W) per seconds"
            << std::endl;

  std::cout << "Before Stop....." << std::endl;
  mtad::adlog::AdLog::Instance();
  std::cout << "End Stop!!!" << std::endl;
  return 0;
}

