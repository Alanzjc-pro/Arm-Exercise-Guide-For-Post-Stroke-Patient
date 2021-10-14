
//'Bicep_FL': 1, 'Bicep_FP': 2, 'Bicep_FS': 3, 'Bicep': 0, 'Rotation_FL': 5, 'Rotation': 4, 'Straight_FL': 9, 'Straight_FP': 10,
//'Straight_FS': 11, 'Straight': 8, 'Swing_FL': 13, 'Swing_FP': 14, 'Swing_FS': 15, 'Swing': 12, 'Rotation_FR': 6, 'Rotation_FS': 7
// What exercises are supported.
//Debugging system variable definition
int OutState=0;
int LEDPin=8;
int buttonPin=7;
int buttonNew;
int buttonOld=1;
int flag=0;


constexpr int Bicep = 0;
constexpr int Bicep_FL = 1;
constexpr int Bicep_FP = 2;
constexpr int Bicep_FS = 3;
constexpr int Rotation = 4;
constexpr int Rotation_FL = 5;
constexpr int Rotation_FR =  6;
constexpr int Rotation_FS = 7;
constexpr int Straight = 8;
constexpr int Straight_FL = 9;
constexpr int Straight_FP = 10;
constexpr int Straight_FS = 11;
constexpr int Swing = 12;
constexpr int Swing_FL = 13;
constexpr int Swing_FP = 14;
constexpr int Swing_FS = 15;
