//Smart Robotic Arm system
//Nov 2022
/*
The code is programmed in such way that it works best with a LabVIEW interface
However, it's still totally practical to run the program using Arduino IDE only,
   but it's highly recommanded to uncomment every Serial.println() if you want to run it that way.
*/

#include <SPI.h>  
#include <Pixy.h>
#include <Servo.h>

Pixy pixy;

Servo servo_claw;
Servo servo_bottom;
Servo servo_joint1;
Servo servo_joint2;

int pot_claw_A0 = A0;
int pot_botton_A1 = A1;
int pot_joint1_A2 = A2;
int pot_joint2_A3 = A3;

int value_claw = 20;
int value_joint1 = 65;
int value_joint2 = 60;
int value_botton = 50;

String command;
String value_claw_temp,value_botton_temp;
String value_joint1_temp,value_joint2_temp; 

int fstIndex = 0;
int secIndex = 0;
int thirdIndex = 0;

int mode=1;


int modeFunc(){
  //Serial.println("Mode?");
  while (Serial.available()) {
     Serial.read();
    }
  while(Serial.available() == 0){}
  mode = Serial.parseInt();
  return mode;
}

void setup(){
  servo_claw.attach(7);
  servo_joint2.attach(8);
  servo_joint1.attach(9);
  servo_bottom.attach(10);
  
  //set default positions:
  servo_claw.write(value_claw);
  servo_joint1.write(value_joint1);
  servo_joint2.write(value_joint2);
  servo_bottom.write(value_botton);

  Serial.begin(9600);
  mode = modeFunc();
  if(mode==2){
      //Serial.print("Starting...\n");
      pixy.init();
      //Serial.print("-------------Scanning-------------\n");
  }
  //Serial.println(mode);
  /*
  if (mode==1){
    Serial.println("Hybrid - angles?");
    Serial.println("Example: 0,80,90,90");
    }
  else if (mode==0){Serial.println("Manual");} */
}

void loop(){

//Manual mode
  if (mode==0){ 
      int pot_inputs[4];
      pot_inputs[0] = analogRead(pot_claw_A0);
      pot_inputs[1] = analogRead(pot_botton_A1);
      pot_inputs[2] = analogRead(pot_joint1_A2);
      pot_inputs[3] = analogRead(pot_joint2_A3);

      value_claw   = map(pot_inputs[0], 400, 900, 18, 43);
      value_botton = map(pot_inputs[1], 1023, 400, 50, 150);
      value_joint1 = map(pot_inputs[2], 1023, 500, 50, 140);
      value_joint2 = map(pot_inputs[3], 700, 200, 45, 120);
      value_claw   = constrain(value_claw,18,43);
      value_botton = constrain(value_botton,50,150);
      value_joint1 = constrain(value_joint1,40,140);
      value_joint2 = constrain(value_joint2,45,100);
  		
      servo_claw.write(value_claw);
      servo_bottom.write(value_botton);
      servo_joint1.write(value_joint1);
      servo_joint2.write(value_joint2);
  }

//Hybrid mode
  command = "";
  if (mode==1&&Serial.available()>0){
    //Serial.println("--------------------------------");
    command = Serial.readStringUntil('\n');
 
    fstIndex = command.indexOf(',');
    secIndex = command.indexOf(',',fstIndex + 1);
    thirdIndex = command.indexOf(',',secIndex + 1);
    
    value_claw_temp = command.substring(0,fstIndex);
    value_botton_temp = command.substring(fstIndex+1,secIndex);
    value_joint1_temp = command.substring(secIndex+1,thirdIndex);
    value_joint2_temp = command.substring(thirdIndex+1);
    
    value_claw =  value_claw_temp.toInt();
    value_botton = value_botton_temp.toInt();
    value_joint1 = value_joint1_temp.toInt();
    value_joint2 = value_joint2_temp.toInt();
      
    /*
    Serial.println(command);
    Serial.println("bottonPin8: "+value_botton_temp);
    Serial.println("join1Pin9: "+value_joint1_temp);
    Serial.println("join2Pin10: "+value_joint2_temp);
    */
    

      value_claw   = constrain(value_claw,0,1);
      value_botton = constrain(value_botton,50,150);
      value_joint1 = constrain(value_joint1,40,140);
      value_joint2 = constrain(value_joint2,45,100);

      if(value_claw==1) {
        servo_claw.write(40);
        //Serial.println("claw: close");
      } else{servo_claw.write(20);
      //Serial.println("claw: open");
      }

	  delay(300);
    servo_bottom.write(value_botton);
    delay(300);
    servo_joint1.write(value_joint1);
    delay(300);
    servo_joint2.write(value_joint2);
    delay(300);

  }

  //Automatic mode
  if(mode==2){
    static int i = 0; //number of frames
    int j; int sig;
    uint16_t blocks;

    blocks = pixy.getBlocks();
      if (blocks){
      i++;
      if (i%50==0){ //Check once 50 frames
          sig = pixy.blocks[j].signature;   
          if(sig==1){ //If the target is detected 
            Serial.println(sig);
            delay(3000);
            value_claw   = constrain(value_claw,18,46);
            value_botton = constrain(value_botton,50,150);
            value_joint1 = constrain(value_joint1,40,140);
            value_joint2 = constrain(value_joint2,45,100);

            //Rise the arm
            for (value_joint1 = 65; value_joint1 <= 100; value_joint1 += 1) { 
              servo_joint1.write(value_joint1);              
              delay(20);                       
            }
            
            //Turn buttom to scan
            for (value_botton = 50; value_botton <= 146; value_botton += 1) {
              servo_bottom.write(value_botton);
              delay(25); 
            }
            
            
            //Lower the arm
            for (value_joint1 = 100; value_joint1 >= 44; value_joint1 -= 1) { 
              servo_joint1.write(value_joint1);              
              delay(20);                       
            }

            //Lower joint2(head)
            for (value_joint2 = 60; value_joint2 <= 74; value_joint2 += 1) {
              servo_joint2.write(value_joint2);
              delay(25); 
            }

            //Close claw
            for (value_claw = 20; value_claw <= 40; value_claw += 1) { 
              servo_claw.write(value_claw);              
              delay(20);                       
            }
            delay(500);

            //Rise joint2(head)
            for (value_joint2 = 74; value_joint2 >= 60; value_joint2 -= 1) { 
              servo_joint2.write(value_joint2);              
              delay(20);                       
            }

            //Rise the arm
            for (value_joint1 = 44; value_joint1 <= 65; value_joint1 += 1) { 
              servo_joint1.write(value_joint1);              
              delay(20);                       
            }

            //Turn bottom back   
            for (value_botton = 146; value_botton >= 50; value_botton -= 1) {
              servo_bottom.write(value_botton);
              delay(25); 
            }

            //Open claw to drop the object
            for (value_claw = 40; value_claw >= 20; value_claw -= 1) { 
                servo_claw.write(value_claw);              
                delay(20);                       
            }
            
          }
         
      }
    }

  }

  delay(5);
}