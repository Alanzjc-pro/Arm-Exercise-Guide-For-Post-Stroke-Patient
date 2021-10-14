# Arm-Exercise-Guide-For-Post-Stroke-Patient

## Background
Assessments of post-stroke rehabilitation exercises by clinical expert is not always available or accessible to the patients due to some limitations or constraints. A presence of an intelligent system using machine learning (ML) approach that can guide through the exercises by classifying them and providing useful feedback can be a great help to tackle the issue, without the need of full supervision from a therapist. This is especially targeted for mild-symptom patients who can self direct their movements. Hence, keep tracking the progress towards recovery can be achieved.

## Our Approach
We designed a low-cost wearable IMU device (3-axis accelerometer and gyroscope) to classfy four user-defined arm exercises, including straight push, arm rotation, bicep lift and arm swing. The overal design flow can be seen below:
* Hardware Prototype Development
* Data Processing and Collection
* Machine Learning Model Training and Validation
* Arduino Nano BLE sense function implementation
* Web GUI design for exercise analysis

### Hardware Prototype Development
6-axis IMU sensor is utilized to collect kinematic information of the user during exercises. 2.4 GHz wireless communication antenna is utilised to transmit and send data to GUI. The Arduino Nano 33 BLE sense has incorporated both IMU and BLE modules as the microcontroller. Extra modules, such as SD card reader, along with push button and LED are used for data collection and hardware control. All components are powered by 5V power bank with 2.1A current output.<br />
<img width="162" alt="IrfanHardware" src="https://user-images.githubusercontent.com/72474193/137358998-e012eb85-5044-4e3c-a437-a9876bb58831.png">

### Data Processing and Collection
We firstly use a 4-tap moving average filter to remove the motion noise of accelerometer and then measure corresponding Eular angles (pitch and roll) as additional input features.

In the experiement data collection, we considered three potential failure cases (shaking,low speed etc) for each exercise to provide more useful exercise feedbacks. Besides, three members did all exercises seperately in the different environment settings, resulting in more convinced datasets for real-world applications. To remove any bias for ML model training and guarantee reliable accuracy, 100 datasets of each output are collected equally by all members, resulting in totally 1600 datasets.

### Machine Learning Model
Initally, the total 1600 datasets are spilt into training data (70%) and test data (30%). For hyperparameter tuning, 20% training data are split for validation data. The sequence of collected datasets with sensor readings are converted to the statistical features (mean, median, max, min and std) as ML inputs and also are annotated as corresponding exercises, resulting in totally 40 input features as a feature vector with 1 target label. With the use of encoder, the string target labels are converted to the binary matrix for neural network training and integer values for standard ML models training.

Four types of ML models are investigated to classify 16 exercise outputs, including KNN, SVM, DT and MLP. Specific hyperparameters in each models are tuned on the validation sets to obtain the optimal hyperparamters. Finally, we retrain models with the optimal hyperparameters for performance analysis on unseen test data.<br />

![ML](https://user-images.githubusercontent.com/72474193/137360602-038635db-d512-4c5c-8375-dfc1620ce8d8.png)


Table demonstrates the performance analysis of 4 best trained ML models on 471 unseen test data. Overall, all models achieve high test accuracy, precision, recall and F1 score. DT can perform slightly better than MLP but the MLP model offers more flexibility in the architecture optimization. Although KNN achieves similar performance like the SVM model, the distance-based method relies on the feature prepossessing to reduce the input dimension for less computation and memory usage, which makes them harder to implement in the small memory Arduino. In three kernels of the SVM model, the linear SVM achieves the best performance in all metrics.
In terms of flexibility and accuracy, the linear SVM is converted and implemented in Arduino for real-scenario testing. <br/>
|Type of ML models|Test Accuracy| Precison | Recall | F1 score|
|---|---|---|---|---|
|MLP|0.85|0.88|0.85|0.85|
|DT| 0.88|0.89|0.88|0.88|
|KNN| 0.95|0.96|0.95|0.95|
|SVM (RGF Kernel)| 0.96|0.95|0.95|0.95|
|SVM (polynomial Kernel)| 0.95|0.96|0.95|0.95|
|SVM (linear Kernel)| 0.96|0.96|0.96|0.96|

### Arduino Testing
We firstly used several buffer arrays to measure the statistic features from processed sensor signals and then all feature results are inputted to a linear SVM model to output the classification results. This was the hardware interrupt triggered envent, which required the user to press the button for activation. The BLE communication was set up to transmit the real-time classification outputs to a web GUI.

### Web GUI
It consists of two pages, home and data analysis. 

#### Home
The connect button is pressed to scan the nearby advertised BLE peripherals. Additional start button is used to control when to record the exercise with the countdown timer (6 second counter). When the stop button is pressed, the classification result will be sent from Arduino and then be displayed in the Detected Exercise block with simultaneous text and animated gif images as the feedback instruction. Besides, the user can select which and how many exercises to do by selecting the option in a dropdown menu and moving a slide bar. According to the number and type of detected exercises, the pie chart will output 3 types of results, such as Failure, Correct and Misclassify. When the Store button is pressed, all exercise results can be stored into the local server and then be loaded later for the performance analysis. <br/>
![Home](https://user-images.githubusercontent.com/72474193/137363408-987724ae-aa0f-4e6a-9a6b-e3f542cbaf11.png)

#### Data analysis
There are 3 main parts of data visualisation page, as shown in Figure 17. The first part is the bar plots that shows the comparison of the exercise counting of the chosen exercise type between the previous record and the current performance result. The previous record is shown using load function. The second part is the Performance Analysis that consists of 4 performance metrics, such as number of exercise movements performed by the user, model accuracy that shows how accurate ML model performs in classifying the correct chosen exercise type against miss-classifying other exercise type, the percentage of correct exercise performed by the user against any failure cases, and what the most failure case performed by the user is. The last part is Future Suggestion that shows some narrative feedback according to the comparison of previous record and the current performance result, and feedback for current most failure case to improve on next time. <br/>
![Visualization](https://user-images.githubusercontent.com/72474193/137363487-17e08d72-28f3-4bf9-aa03-11a952d7ea76.png)


### Conclusion
* Low cost (< 60 £) and low power (mW)
* The overall performance in the real-world testing : 84% accuracy
* Really nice interface for data analysis and feedback

### Future World
* Collect more datasets from different people to improve the robustness of ML model.
* Add more functionalities of GUI to provide more useful feedback.
* Ask and invite clinial experts and patients to test the whole system.
