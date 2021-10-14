<!DOCTYPE html>
  <html>
    <head>
      <title>Getting Started with p5.ble.js</title>
      <script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/p5.js"></script>
      <script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.7.2/addons/p5.dom.min.js"></script>
      <script src="https://unpkg.com/p5ble@latest/dist/p5.ble.min.js"></script>
    </head>

    <body>
      <script>
      //BLE  
      const ArmUuid = "00002234-0000-1000-8000-00805f9b34fb";
      const resultUuid = "00001111-0000-1000-8000-00805f9b34fb";
      const txUuid = "00002A3D-0000-1000-8000-00805f9b34fb";
      const startUuid = "00002222-0000-1000-8000-00805f9b34fb";
      const updateUuid = "00003333-0000-1000-8000-00805f9b34fb";
      let resultCharacteristic;
      let txCharacteristic;
      let startCharacteristic;
      let updateCharacteristic;
      let result = 16;
      let start_butt;
      let slider;
      let val;
      let bgSlider;
      let tcounter= 0;
      let fcounter= 0;
      let mcounter = 0;
      let bgColor = 220;
      let timer = 6;
      let Start = false;
      let Start_signal = false;
      let state = 0;
      let load = 0;
      let activate = false;
      let result_prev = 20;
      let update_prev  = 0;
      let update = 0;
      let sel;
      let exerciseName = ['Bicep','Bicep_FL','Bicep_FP','Bicep_FS','Rotation','Rotation_FL','Rotation_FR','Rotation_FS','Straight','Straight_FL','Straight_FP','Straight_FS','Swing','Swing_FL','Swing_FP','Swing_FS','Misclassify'];
      let acc_prev;
      let acc_now;
      let total_prev=0;
      let total_now=0;
      let correct_prev;
      let correct_now;
      let indexOfLargestPrev;
      let failure_prev = 0;
      let failure_now = 0;
      let idxNow;
      // Graph Setup
      const W = 600, H = 400;
      const data = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
      const colour = [[255,255,255],[0,200,255],[255,0,0],[0,255,0]];

 let s0 = 'A correct bicep arm lift exercise is detected. Please continue.';
      let s1 = 'A bicep lift exercise with low speed failure is detected. Try to strengthen the muscle and lift the bicep within 5 seconds period.';
      let s2 = 'A bicep lift exercise with partial failure is detected. Try to strengthen the muscle and lift the bicep up to 180deg.';
      let shake = 'A shaky movement is detected. Please keep your arm stable!';
      let s4 = 'A correct arm rotation exercise is detected. Please continue.';
      let s5 = 'An arm rotation with low speed failure is detected. Try to strengthen the muscle and rotate within 5 seconds period.';
      let s6 = 'An arm rotation with partial failure is detected. Try to strengthen the muscle and rotate up to 90deg.';
      let s8 = 'A correct straight push exercise is detected. Please continue.';
      let s9= 'A straight push with low speed failure is detected. Try to strengthen the muscle and rotate within 5 seconds period.';
      let s10 = 'A straight push with partial failure is detected. Try to strengthen the muscle and push with arm perpendicular to table.';
      let s12 = 'A correct arm swing exercise is detected. Please continue.';
      let s13= 'An arm swing with low speed failure is detected. Try to strengthen the muscle and swing within 5 seconds period.';
      let s14 = 'An arm swing with partial failure is detected. Try to strengthen the muscle and swing arm up to right shoulder.';
      let s15 = 'Congradulation, you finished this';
      let s16 = 'No worry, you can do better in the next time. Please look at the failure suggestion!';
      let s17 = 'No improvement is seen in the ';
    let Ex1 = [];
    let data1 = [];
    function preload() {
     data1 = loadTable("http://127.0.0.1:8895/Straight Push_3-10-22-14.csv", "csv", "header")
     }
      function setup() {
        // Create a p5ble class
        Arm = new p5ble();
        createCanvas(W, H);
        slider = createSlider(0, 400, 200); // min, max, start
        slider.size(100,280); // width and height
        slider.position(W, 0); // x and y
        bg = loadImage('Arm.jpg');
        textSize(20);
        textAlign(CENTER, CENTER);
        // Create a 'Connect' button
        const connectButton5 = createButton('Home');
        connectButton5.mousePressed(HomePage); 
        const connectButton4 = createButton('Visualization');
        connectButton4.mousePressed(changePage);  
        const connectButton = createButton('Connect');
        connectButton.mousePressed(connectToBle);
        const connectButton2 = createButton('Start');
        connectButton2.mousePressed(write1); 
        const connectButton3 = createButton('Stop');
        connectButton3.mousePressed(write0);  
        const connectButton6 = createButton('Store');
        connectButton6.mousePressed(SaveResult);  
        
        for (let r = 0; r < 1; r++)
          for (let c = 0; c < data1.getColumnCount(); c++)
              append(Ex1, data1.getNum(r, c));
        
        sel = createSelect();
        sel.option('Straight Push');
        sel.option('Rotation');
        sel.option('Bicep');
        sel.option('Swing');
        
        table = new p5.Table();

        table.addColumn('Bicep');
        table.addColumn('Bicep_FL');
        table.addColumn('Bicep_FP');
        table.addColumn('Bicep_FS');
        table.addColumn('Rotation');
        table.addColumn('Rotation_FL');
        table.addColumn('Rotation_FP');
        table.addColumn('Rotation_FS');
        table.addColumn('Straight');
        table.addColumn('Straight_FL');
        table.addColumn('Straight_FP');
        table.addColumn('Straight_FS');
        table.addColumn('Swing');
        table.addColumn('Swing_FL');
        table.addColumn('Swing_FP');
        table.addColumn('Swing_FS');
        table.addColumn('Misclassify');
      }

      function connectToBle() {
        // Connect to a device by passing the service UUID
        Arm.connect(ArmUuid, gotCharacteristics);
      }
      
      function changePage(){
        state = 1;
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(160, 235); 
      }
      
      function HomePage(){
        state = 0;
      }

      function SaveResult(){        
        let newRow = table.addRow();
        newRow.setNum('Bicep',data[0]);
        newRow.setNum('Bicep_FL',data[1]);
        newRow.setNum('Bicep_FP',data[2]);
        newRow.setNum('Bicep_FS',data[3]);
        newRow.setNum('Rotation',data[4]);
        newRow.setNum('Rotation_FL',data[5]);
        newRow.setNum('Rotation_FP',data[6]);
        newRow.setNum('Rotation_FS',data[7]);
        newRow.setNum('Straight',data[8]);
        newRow.setNum('Straight_FL',data[9]);
        newRow.setNum('Straight_FP',data[10]);
        newRow.setNum('Straight_FS',data[11]);
        newRow.setNum('Swing',data[12]);
        newRow.setNum('Swing_FL',data[13]);
        newRow.setNum('Swing_FP',data[14]);
        newRow.setNum('Swing_FS',data[15]);
        newRow.setNum('Misclassify',data[16]);
        saveTable(table, val+'_'+month()+'-'+day()+'-'+hour()+'-'+minute()+'.csv');
        data = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
      }
      

      // A function that will be called once got characteristics
      function gotCharacteristics(error, characteristics) {
        if (error) console.log('error: ', error);
        console.log('characteristics: ', characteristics);
        resultCharacteristic = characteristics[0];
        startCharacteristic = characteristics[1];
        txCharacteristic = characteristics[2];
        updateCharacteristic = characteristics[3];
        
        // Read the value of the first characteristic
        Arm.read(resultCharacteristic, gotValue);
        Arm.read(startCharacteristic,gotStart);
        Arm.read(updateCharacteristic,gotUpdate);
      }

      function gotValue(error, value) {
        if (error) console.log('error:', error);
        result = value;
        //console.log('result:',result);
        Arm.read(resultCharacteristic,gotValue);
      }

      function gotStart(error, value) {
        if (error) console.log('error:', error);
        start_butt = value;
        if(start_butt ==1){
          Start_signal = true;
        }
        else if(start_butt == 0 && Start == false){
          Start_signal = false;
          timer = 6;
        }
        else{
          Start_signal = false;
        }
        //console.log('start:',start_butt);
        Arm.read(startCharacteristic,gotStart);
      }

     function gotUpdate(error, value) {
        if (error) console.log('error:', error);
         update = value;
        Arm.read(updateCharacteristic,gotUpdate);
      }

      function write1(){
         Start = true;
         Arm.write(txCharacteristic, 1);
      }
      function write0(){
         Arm.write(txCharacteristic, 0);
      }

      function draw() {
        val = sel.value();
        if (state ==0){
        background(bg);

        if(result_prev!=result){
          activate = true;
        }
        if(update_prev!=update){
          activate = true;
        }  
          
        fill(255);
        rect(20,20,300,30);
        rect(20,82,355,105);
        rect(400,150,200,30)
        fill(colour[1]);
        rect(466,180,66,30);
        fill(colour[2]);
        rect(400,180,66,30); 
        fill(colour[3]);
        rect(530,180,70,30)
        fill(0);
        text('No. Exercise:',460,165);
        c = int(slider.value()/40);
        text(c,W-50,165);
        textSize(12);
        text('Correct',500,195);
        text('Failure',435,195);
        text('Misclassify',570,195)  
        fill(255);
        circle(W-50, 50, 160);
        if (Start == true || Start_signal == true){
        textSize(80);
        fill(0);
        text(timer,W-50,50);
        textSize(20);
        time();
        }
        fill(0);
        textSize(16);
        text('Detected Exercise:',110,35);
        textStyle(BOLD);
        text('Narrative Feedback:',110,95);
        textStyle(NORMAL);
        //feedback();
        if (result == 0){
        text('Bicep',260,35);
        textSize(13);
        text(s0, 20, 85, 350, 100);
        text('Correct Bicep',80,210);
        }
        else if(result == 1){
        text('Bicep_FL',260,35);
        textSize(13);
        text(s1, 20, 85, 350, 100);
        text('Bicep_FL',80,210);
        text('Correct Bicep',215,210);
        }
        else if(result==2){
        text('Bicep_FP',260,35);
        textSize(13);
        text(s2, 20, 85, 350, 100);
        text('Bicep_FP',80,210);
        text('Correct Bicep',215,210);
        }
        else if(result==3){
        text('Bicep_FS',260,35);
        textSize(13);
        text(shake, 20, 85, 350, 100);
        text('Bicep_FS',80,210);
        text('Correct Bicep',215,210);
        }
        else if(result==4){
        text('Rotation',260,35);
        textSize(13);
        text(s4, 20, 85, 350, 100);
        text('Correct Rotation',80,210);
        }
        else if(result==5){
        text('Rotation_FL',260,35);
        textSize(13);
        text(s5, 20, 85, 350, 100);
        text('Rotation_FL',80,210);
        text('Correct Rotation',215,210);
        }
        else if(result==6){
        text('Rotation_FR',260,35);
        textSize(13);
        text(s6, 20, 85, 350, 100);
        text('Rotation_FR',80,210);
        text('Correct Rotation',215,210);
        }
        else if(result==7){
        text('Rotation_FS',260,35);
        textSize(13);
        text(shake, 20, 85, 350, 100);
        text('Rotation_FS',80,210);
        text('Correct Rotation',215,210);
        }
        else if(result==8){
        text('Straight',260,35);
        textSize(13);
        text(s8, 20, 85, 350, 100);
        text('Correct Straight',80,210);
        }
        else if(result==9){
        text('Straight_FL',260,35);
        textSize(13);
        text(s9, 20, 85, 350, 100);
        text('Straight_FL',80,210);
        text('Correct Straight',215,210);
        }
        else if(result==10){
        text('Straight_FP',260,35);
        textSize(13);
        text(s10, 20, 85, 350, 100);
        text('Straight_FP',80,210);
        text('Correct Straight',215,210);
        }
        else if(result==11){
        text('Straight_FS',260,35);
        textSize(13);
        text(shake, 20, 85, 350, 100);
        text('Straight_FS',80,210);
        text('Correct Straight',215,210);
        }
        else if(result==12){
        text('Swing',260,35);
        textSize(13);
        text(s12, 20, 85, 350, 100);
        text('Correct Swing',80,210);
        }
        else if(result==13){
        text('Swing_FL',260,35);
        textSize(13);
        text(s13, 20, 85, 350, 100);
        text('Swing_FL',80,210);
        text('Correct Swing',215,210);
        }
        else if(result==14){
        text('Swing_FP',260,35);
        textSize(13);
        text(s14, 20, 85, 350, 100);
        text('Swing_FP',80,210);
        text('Correct Swing',215,210);
        }
        else if(result==15){
        text('Swing_FS',260,35);
        textSize(13);
        text(shake, 20, 85, 350, 100);
        text('Swing_FS',80,210);
        text('Correct Swing',215,210);
        }  
        else{
        text('No activity',260,35);
        }
          
        if (activate == true){
        if (result == 0){
        gif_createImg = createImg("Bicep.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(160, 235);
        if(val == 'Bicep'){
          tcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result == 1){
        gif_createImg = createImg("Bicep_FL.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Bicep.gif");
        gif_createImg.position(160, 235);
        if(val == 'Bicep'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==2){
        gif_createImg = createImg("Bicep_FP.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Bicep.gif");
        gif_createImg.position(160, 235);
        if(val == 'Bicep'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==3){
        gif_createImg = createImg("Bicep_FS.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Bicep.gif");
        gif_createImg.position(160, 235);
        if(val == 'Bicep'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==4){
        gif_createImg = createImg("Rotation.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(160, 235);
        if(val == 'Rotation'){
          tcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==5){
        gif_createImg = createImg("Rotation_FL.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Rotation.gif");
        gif_createImg.position(160, 235);
        if(val == 'Rotation'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==6){
        gif_createImg = createImg("Rotation_FR.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Rotation.gif");
        gif_createImg.position(160, 235);
        if(val == 'Rotation'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==7){
        gif_createImg = createImg("Rotation_FS.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Rotation.gif");
        gif_createImg.position(160, 235);
        if(val == 'Rotation'){
          fcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==8){
        gif_createImg = createImg("Straight.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(160, 235);
        if(val == 'Straight Push'){
          tcounter++;
          data[result] ++; 
        }else{
          mcounter++;
          data[16] ++; 
        }
        }
        else if(result==9){
        gif_createImg = createImg("Straight_FL.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Straight.gif");
        gif_createImg.position(160, 235);
        if(val == 'Straight Push'){
          fcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==10){
        gif_createImg = createImg("Straight_FP.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Straight.gif");
        gif_createImg.position(160, 235); 
        if(val == 'Straight Push'){
          fcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==11){
        gif_createImg = createImg("Straight_FS.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Straight.gif");
        gif_createImg.position(160, 235);
        if(val == 'Straight Push'){
          fcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==12){
        gif_createImg = createImg("Swing.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("blank.gif");
        gif_createImg.position(160, 235);
         if(val == 'Swing'){
          tcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==13){
        gif_createImg = createImg("Swing_FL.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Swing.gif");
        gif_createImg.position(160, 235);
         if(val == 'Swing'){
          fcounter++;
           data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==14){
        gif_createImg = createImg("Swing_FP.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Swing.gif");
        gif_createImg.position(160, 235);
        if(val == 'Swing'){
          fcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }
        else if(result==15){
        gif_createImg = createImg("Swing_FS.gif");
        gif_createImg.position(20, 235);
        gif_createImg = createImg("Swing.gif");
        gif_createImg.position(160, 235);
        if(val == 'Swing'){
          fcounter++;
          data[result] ++;
        }else{
          mcounter++;
          data[16] ++;
        }
        }  
        else{
        text('No activity',260,35);
        }
        activate = false;
        }
          rest_c = c -tcounter-fcounter-mcounter;
          angles = [(360/c)*rest_c,(360/c)*tcounter,(360/c)*(fcounter),(360/c)*(mcounter)];
          pieChart(180, angles);
      if (rest_c == 0){
          tcounter = 0;
          fcounter = 0;
          mcounter = 0;
        }
          result_prev = result;
          update_prev = update;
        }
        else{
        background(255);       
        textSize(18);
        textStyle(BOLD);
        text(val,150,10);
        text('Performance Analysis',450,10);
        text('Future suggestion',450,235);
        
        textSize(12);
        text('Type of exercise',50,30);
        text('Previous',150,30);
        text('Current',250,30);
        text('Metric',360,30);         
        text('Previous',465,30);
        text('Current',560,30);
        text('Comparison:',340,255);
        text('Suggestion for failure:',367,310);
        textStyle(NORMAL);
        text('No.experiment',360,63);
        text('Model Accuracy',360,109);
        text('% Correct Exercise',360,155);
        text('Most Failure Case',360,201);
        line(0, 225, W, 225);
        line(300, 0, 300, H);
        line(0,20,W,20);
        line(0,40,W,40);
        line(100,20,100,225);
        line(200,20,200,225);
        line(300,245,W,245);
        line(300,300,W,300);
        line(425,20,425,225);
        line(512.5,20,512.5,225);
        line(300,86.25,W,86.25);
        line(300,132.5,W,132.5);
        line(300,178.75,W,178.75);
        if (data1) {
          for (let i = 0; i < 17; i++) {
          let x = 100;
          let y = 45+(i* 10.6);
          let w = Ex1[i] * 10;
          let h = 9.5;
          fill(150, 20, 20)
          rect(x, y, w, h);
          fill(0)
          textSize(10);
          text( Ex1[i], x  + 95, y + 4)
          }
        }
          for (let i = 0; i < 17; i++) {
          let x = 100;
          let y = 45+(i* 10.6);
          let w = data[i] * 10;
          let h = 9.5;
          fill(100, 200, 20)
          rect(x+100, y, w, h);
          fill(0)
          textSize(10);
          text(exerciseName[i], x  - 55, y + 4)
          text( data[i] , x  +195, y + 4)
          }

          if(val == 'Bicep'){
            total_prev = Ex1[0]+Ex1[1]+Ex1[2]+Ex1[3]+Ex1[16];
            total_new = data[0]+data[1]+data[2]+data[3]+data[16];
            failure_prev = Ex1[1]+Ex1[2]+Ex1[3];
            failure_now = data[1]+data[2]+data[3];
            acc_prev = (1-(Ex1[16]/(total_prev+Ex1[16])))*100;
            acc_now = (1-(data[16]/(total_new+data[16])))*100;
            correct_prev = (Ex1[0]/total_prev)*100;
            correct_now = (data[0]/total_new)*100;
            indexOfLargestPrev = 1;
            idxNow = 1;
            for (let f = 1; f < 4; f++) {
              if (Ex1[f] > Ex1[indexOfLargestPrev]) {
                indexOfLargestPrev = f;
              }
              if (data[f] > data[idxNow]) {
                idxNow = f;
              }
            }
          } else if (val == 'Rotation'){
            total_prev = Ex1[4]+Ex1[5]+Ex1[6]+Ex1[7]+Ex1[16];
            total_new = data[4]+data[5]+data[6]+data[7]+data[16];
            failure_prev = Ex1[5]+Ex1[6]+Ex1[7];
            failure_now = data[5]+data[6]+data[7];
            acc_prev = (1-(Ex1[16]/(total_prev+Ex1[16])))*100;
            acc_now = (1-(data[16]/(total_new+data[16])))*100;
            correct_prev = (Ex1[4]/total_prev)*100;
            correct_now = (data[4]/total_new)*100;
            indexOfLargestPrev = 5;
            idxNow = 5;
            for (let f = 1; f < 4; f++) {
              if (Ex1[f+4] > Ex1[indexOfLargestPrev]) {
                indexOfLargestPrev = f+4;
              }
              if (data[f+4] > data[idxNow]) {
                idxNow= f+4;
              }
            }
          } else if (val == 'Straight Push'){
            total_prev = Ex1[8]+Ex1[9]+Ex1[10]+Ex1[11]+Ex1[16];
            total_new = data[8]+data[9]+data[10]+data[11]+data[16];
            failure_prev = Ex1[9]+Ex1[10]+Ex1[11];
            failure_now = data[9]+data[10]+data[11];
            acc_prev = (1-(Ex1[16]/(total_prev+Ex1[16])))*100;
            acc_now = (1-(data[16]/(total_new+data[16])))*100;
            correct_prev = (Ex1[8]/total_prev)*100;
            correct_now = (data[8]/total_new)*100;
            indexOfLargestPrev =9;
            idxNow = 9;
            for (let f = 1; f < 4; f++) {
              if (Ex1[f+8] > Ex1[indexOfLargestPrev]) {
                indexOfLargestPrev = f+8;
              }
              if (data[f+8] > data[idxNow]) {
                idxNow = f+8;
              }
            }
          } else if (val == 'Swing'){
            total_prev = Ex1[12]+Ex1[13]+Ex1[14]+Ex1[15]+Ex1[16];
            failure_prev = Ex1[13]+Ex1[14]+Ex1[15];
            failure_now = data[13]+data[14]+data[15];
            total_new = data[12]+data[13]+data[14]+data[15]+data[16];
            acc_prev = (1-(Ex1[16]/(total_prev+Ex1[16])))*100;
            acc_now = (1-(data[16]/(total_new+data[16])))*100;
            correct_prev = (Ex1[12]/total_prev)*100;
            correct_now = (data[12]/total_new)*100;
            indexOfLargestPrev = 13;
            idxNow= 13;
            for (let f = 1; f < 4; f++) {
              if (Ex1[f+12] > Ex1[indexOfLargestPrev]) {
                indexOfLargestPrev = f+12;
              }
              if (data[f+12] > data[idxNow]) {
                idxNow = f+12;
              }
            }
          }
       textSize(11);
        textStyle(NORMAL);
        text(total_prev,465,63);
        text(total_new,555,63);
        text(int(acc_prev)+'%',465,109);
        text(int(acc_now)+'%',555,109);
        text(int(correct_prev)+'%',465,155);
        text(int(correct_now)+'%',555,155);
        if(failure_prev>0){
          text(exerciseName[indexOfLargestPrev],465,201);
        }else{
          text('No Failure',465,201);
        }
        if(failure_now>0){
          text(exerciseName[idxNow],555,201);
        }else{
          text('No Failure',555,201);
        }
        if (int(correct_now) > int(correct_prev)){
          text(s15+' '+val+' '+'exercise with higher accuracy than your last time. Keep going!',330,260,230,40);
        }
        else if(int(correct_now) == int(correct_prev)){
          text(s17+val+' '+'exercise. Keep fighting!',380,245,220,40)
        } 
        else{
          text(s16,380,240,220,40);
        }
        textSize(13);
        if(failure_now>0){
          if(exerciseName[idxNow] == 'Bicep_FL'){
            text(s1,330,300,200,200);
          }else if(exerciseName[idxNow] == 'Bicep_FP'){
            text(s2,330,300,200,200);
          }else if(exerciseName[idxNow] == 'Bicep_FS'||exerciseName[idxNow] == 'Straight_FS'||exerciseName[idxNow] == 'Rotation_FS'||exerciseName[idxNow] == 'Swing_FS'){
            text(shake,330,300,200,100);
          }else if(exerciseName[idxNow] == 'Rotation_FL'){
             text(s5,330,300,200,100);
          }else if(exerciseName[idxNow] == 'Rotation_FR'){
            text(s6,330,300,200,100);        
           }else if(exerciseName[idxNow] == 'Straight_FL'){
            text(s9,330,300,200,100);        
           }else if(exerciseName[idxNow] == 'Straight_FP'){
            text(s10,330,300,200,100);        
           }else if(exerciseName[idxNow] == 'Swing_FL'){
            text(s13,330,300,200,100);        
           }else if(exerciseName[idxNow] == 'Swing_FP'){
            text(s15,330,300,200,100);        
           }else{
            text('No suggestion is provided as you perform better in this '+val+' '+'exercise.',330,310,200,100);  
         }
        }
        }   
      }
        function time(){
        if (frameCount % 60 == 0 && timer > 0) {
            timer --;
        }
        if (timer == 0) {
        timer = 6;
        Start = false;
        Start_signal = false;
        }
        }
        
    function pieChart(diameter, data) {
        var lastAngle = 0;
    for (var i = 0; i < data.length; i++) {
        var gray = map(i, 0, data.length, 50, 150);
        fill(colour[i]);
        arc(W-90, H-90, diameter, diameter, lastAngle,           lastAngle+radians(angles[i]));
        lastAngle += radians(angles[i]);
    }
    }
      </script>
    </body>
  </html>

