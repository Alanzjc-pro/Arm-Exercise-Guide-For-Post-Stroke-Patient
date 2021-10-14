#include "Exercise_predictor.h"
constexpr int kExerciseCount = 16;
constexpr int kNoexercise = 16;

namespace {
// State for the averaging algorithm we're using.
float prediction_history[kExerciseCount] = {};
}  // namespace

//'Bicep_FL': 1, 'Bicep_FP': 2, 'Bicep_FS': 3, 'Bicep': 0, 'Rotation_FL': 5, 'Rotation': 4, 'Straight_FL': 9, 'Straight_FP': 10,
//'Straight_FS': 11, 'Straight': 8, 'Swing_FL': 13, 'Swing_FP': 14, 'Swing_FS': 15, 'Swing': 12, 'Rotation_FR': 6, 'Rotation_FS': 7
 int PredictExercise(float* output){
  // Record the latest predictions in our rolling history buffer.
  int max_predict_index = 16;
  for (int i = 0; i < kExerciseCount; ++i) {
    prediction_history[i] = output[i];
    
  }
    return max_predict_index;
 }
