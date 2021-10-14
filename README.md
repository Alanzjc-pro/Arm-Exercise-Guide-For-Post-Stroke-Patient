# Arm-Exercise-Guide-For-Post-Stroke-Patient

## Background
Assessments of post-stroke rehabilitation exercises by clinical expert is not al- ways available or accessible to the patients due to some limitations or constraints. A presence of an intelligent system using machine learning (ML) approach that can guide through the exercises by classifying them and providing useful feedback can be a great help to tackle the issue, without the need of full supervision from a thera- pist. This is especially targeted for mild-symptom patients who can self direct their movements. Hence, keep tracking the progress towards recovery can be achieved.

## Our Approach
We designed a low-cost wearable IMU device (3-axis accelerometer and gyroscope) to classfy four user-defined arm exercises, including straight push, arm rotation, bicep lift and arm swing. The overal design flow can be seen below:
* Hardware Prototype Development
* Data Processing and Collection
* Machine Learning Model Training and Validation
* Arduino Nano BLE sense function implementation
* Web GUI design for exercise analysis

### Hardware Prototype Development
6-axis IMU sensor is utilized to collect kinematic information of the user during exercises. 2.4 GHz wireless communi- cation antenna is utilised to transmit and send data to GUI. The Arduino Nano 33 BLE sense has incorporated all IMU and BLE modules as the microcontroller. Extra modules, such as SD card reader, along with push button and LED are used for data collection and hardware control. All components are powered by 5V power bank with 2.1A current output.<br />
<img width="162" alt="IrfanHardware" src="https://user-images.githubusercontent.com/72474193/137358998-e012eb85-5044-4e3c-a437-a9876bb58831.png">

### Data Processing and Collection
We firstly use a 4-tap moving average filter to remove the motion noise of accelerometer and then measure corresponding Eular angles (pitch and roll) as additional input features.

In the experiement data collection, we considered 3 potential failure cases (shaking,low speed etc) for each exercise to provide more useful exercise feedbacks. Besides, three members did all exercises seperately in the different environment settings, resulting in convinced datasets for real-world applications. To remove any bias for ML model training and guarantee reliable accuracy, 100 datasets of each output are collected equally by all members, resulting in totally 1600 datasets.

### Machine Learning Model
Initally, the total 1600 datasets are spilt into training data (70%) and test data (30%). For hyperparameter tuning, 20% training data are split for validation data. The sequence of collected datasets with sensor readings are converted to the statistical features (mean, median, max, min and std) as ML inputs and also are annotated as corresponding exer- cises, resulting in totally 40 input features as a feature vector with 1 target label.

Four types of ML models are investigated to classify 16 exercise outputs, including KNN, SVM, DT and MLP. Specific hyperparameters in each models are tuned on the validation sets to obtain the optimal hyperparamters. Finally, we retrain models with the optimal hyperparameters for performance analysis on unseen test data.<br />

![ML](https://user-images.githubusercontent.com/72474193/137360602-038635db-d512-4c5c-8375-dfc1620ce8d8.png)

