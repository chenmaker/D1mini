#include <SPI.h>
#include "RF24.h"
#include <SparkFun_ADXL345.h> 
RF24 rf24(D0, D8); // CE腳, CSN腳

const byte addr[] = "1Node";
String car="BMX8888";
ADXL345 adxl = ADXL345(); 
void ADXL_ISR(); 

//GPS
#define DebugSerial Serial
#include <SoftwareSerial.h>
SoftwareSerial GpsSerial(D4,D3);

struct
{
    char GPS_Buffer[80];
    bool isGetData;     //是否获取到GPS数据
    bool isParseData;   //是否解析完成
    char UTCTime[11];   //UTC时间
    char latitude[11];  //纬度
    char N_S[2];        //N/S
    char longitude[12]; //经度
    char E_W[2];        //E/W
    bool isUsefull;     //定位信息是否有效
} Save_Data;

const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;

void setup()    //初始化内容
{ 
//Serial.begin(9600);
//nrf24
  rf24.begin();
  rf24.setChannel(83);          // 設定頻道編號
  rf24.openWritingPipe(addr);   // 設定通道位址
  rf24.setPALevel(RF24_PA_MAX); // 設定廣播功率
  rf24.setDataRate(RF24_1MBPS); // 設定傳輸速率
  rf24.stopListening();         // 停止偵聽；設定成發射模式
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
//GPS
  GpsSerial.begin(9600);
  DebugSerial.begin(9600);
  Save_Data.isGetData = false;
  Save_Data.isParseData = false;
  Save_Data.isUsefull = false;
}

void loop(){
//ADXL345
  int x,y,z; 
  adxl.readAccel(&x, &y, &z); 
  byte interrupts = adxl.getInterruptSource();   
//判斷是否撞擊力大於10G
if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
    adxl.readAccel(&x, &y, &z);   
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.println(z); 
//GPS
  gpsRead();       //获取GPS数据
  parseGpsBuffer();//解析GPS数据
  printGpsBuffer();//输出解析后的数据
//String NS=Save_Data.N_S;
  String NN=Save_Data.latitude;
//String EW=Save_Data.E_W;
  String EE=Save_Data.longitude;
  String times=String(int(Save_Data.UTCTime)+80000);
  //String msg=NS+NN+EW+EE+times+car; 
    String msg=NN+EE+times+car; 
    Serial.print("msg="); 
    Serial.println(msg);     
    rf24.write(&msg[0], 40);
    delay(300);
    }
if (rf24.available()>0) {
    char emsg[128] = "";
    rf24.read(&emsg, sizeof(emsg));
    Serial.println(emsg); // 顯示訊息內容
}
