#include <Arduino.h>

#include <TensorFlowLite.h>
#include "Exercise_predictor.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "model.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "imu_handler.h"
#include "constant.h" 
#include <math.h> 

namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
int inference_count = 0;
constexpr int tensor_arena_size = 70 * 1024;
uint8_t tensor_arena[tensor_arena_size];
TfLiteTensor* model_input = nullptr;
TfLiteTensor* model_output = nullptr;
int input_length;
}  // namespace

void setup() {
  Serial.begin(9600);
  //button
  pinMode(buttonPin, INPUT);
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(arm_model2_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
   }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena,
                                     tensor_arena_size, error_reporter);


  model_input = interpreter->input(0);
  if ((model_input->dims->size != 2) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != 33) ||
      (model_input->type != kTfLiteFloat32)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }
  // Allocate memory from the tensor_arena for the model's tensors.
    interpreter->AllocateTensors();
  input_length = 32;
  
  TfLiteStatus setup_status = SetupIMU(error_reporter);
   if (setup_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Set up failed\n");
  }
  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

void loop() {
 buttonNew = digitalRead(buttonPin);

   if(buttonOld==0 && buttonNew==1){
    if(OutState==0){
      digitalWrite(LEDPin,HIGH);
      flag=1;
      OutState=1;
      
    }
    else{
      digitalWrite(LEDPin,LOW);
      flag=0;
      OutState=0; 
      input_length = model_input->bytes / sizeof(float);
       Result(model_input->data.f,input_length); 
       float val= 0;
       interpreter->Invoke();
       model_output = interpreter-> output(0);
       Serial.println("Seiral");
       Serial.print(model_output->data.int8[0]);       Serial.print('\t');
       Serial.print(model_output->data.int8[1]);       Serial.print('\t');
       Serial.print(model_output->data.int8[2]);       Serial.print('\t');
       Serial.print(model_output->data.int8[3]);       Serial.print('\t');
       Serial.print(model_output->data.int8[4]);       Serial.print('\t');
       Serial.print(model_output->data.int8[5]);       Serial.print('\t');
       Serial.print(model_output->data.int8[6]);       Serial.print('\t');
       Serial.print(model_output->data.int8[7]);       Serial.print('\t');
       Serial.print(model_output->data.int8[8]);       Serial.print('\t');
       Serial.print(model_output->data.int8[9]);       Serial.print('\t');
       Serial.print(model_output->data.f[10],5);       Serial.print('\t');
       Serial.print(model_output->data.f[11],5);       Serial.print('\t');
       Serial.print(model_output->data.f[12],5);       Serial.print('\t');
       Serial.print(model_output->data.f[13],5);       Serial.print('\t');
       Serial.print(model_output->data.f[14],5);       Serial.print('\t');
       Serial.println(model_output->data.int8[16]);
       float threshold = 0;
      Serial.print("Output Size:");
      Serial.println(model_output->dims->data[1]);
      //Serial.print("Output:");
      //Serial.println(val);
      }
  } 
 if(flag==1){
   bool got_data = ReadIMU(error_reporter);
   if (!got_data) return;
 }
   buttonOld=buttonNew;
  delay(50);
}
