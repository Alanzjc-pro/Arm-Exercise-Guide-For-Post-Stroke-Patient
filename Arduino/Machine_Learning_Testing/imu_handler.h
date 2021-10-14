#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

// Called by the main loop to produce some output based on the x and y values
extern  bool ReadIMU(tflite::ErrorReporter* error_reporter);
extern void Result(float* input, int len);
extern TfLiteStatus SetupIMU(tflite::ErrorReporter* error_reporter);
 void get_Result();
 void print_Result();
 void clear_buffer();
 void store_Result();
 float stds(float val[], float avg, int cnt);
 
extern int begin_idx;
 
