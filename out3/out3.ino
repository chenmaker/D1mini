#include <SPI.h>
#include "RF24.h"
#include <SparkFun_ADXL345.h> 
RF24 rf24(D0, D8); // CE腳, CSN腳

const byte addr[] = "1Node";
//const char msg[] = "E2256.2569W22120.5698CAR:BMX-88888";
const char msg[] = "";
//String msg[]="";
ADXL345 adxl = ADXL345(); 
void ADXL_ISR(); 
void setup() {
  Serial.begin(9600);
  rf24.begin();
  rf24.setChannel(83);       // 設定頻道編號
  rf24.openWritingPipe(addr); // 設定通道位址
  rf24.setPALevel(RF24_PA_MAX);   // 設定廣播功率
  rf24.setDataRate(RF24_1MBPS); // 設定傳輸速率
  rf24.stopListening();       // 停止偵聽；設定成發射模式
  //ADXL345
  adxl.powerOn(); 
  adxl.setRangeSetting(16); 
  adxl.setSpiBit(0);                  
  adxl.setActivityXYZ(1, 0, 0);       
  adxl.setActivityThreshold(250);      
  adxl.setInactivityXYZ(1, 0, 0);     
  adxl.setInactivityThreshold(75);    
  adxl.setTimeInactivity(2);         
  adxl.setTapDetectionOnXYZ(0, 0, 1); 
  adxl.setTapThreshold(50);           
  adxl.setTapDuration(15);          
  adxl.setDoubleTapLatency(80);       
  adxl.setDoubleTapWindow(200);       
  adxl.InactivityINT(1);
  adxl.ActivityINT(1); 
}

void loop() {
  Serial.println("READ");
  int x,y,z; 
  adxl.readAccel(&x, &y, &z); 
  byte interrupts = adxl.getInterruptSource();   
if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
    adxl.readAccel(&x, &y, &z);   
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.println(z); 
     String NS="N";
     String NN="2200.56";
     String EW="E";
     String EE="1200.50";
     String times="190520";
     String car="BMX-8888";
     String any=NS+NN+EW+EE+" "+times+car;     
     // Serial.println(sizeof(NS)); 
     //rf24.write(&any, sizeof(any));
     rf24.write(&any[0], 40);
    delay(300);
    }
    delay(300);
}
