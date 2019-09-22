#include <SPI.h>
#include "RF24.h"
#include <SparkFun_ADXL345.h> 
RF24 rf24(D0, D8); // CE腳, CSN腳

const byte addr[] = "1Node";
String msg="";
String car="BMX-8888";
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
    char UTCTime[11];       //UTC时间
    char latitude[11];      //纬度
    char N_S[2];        //N/S
    char longitude[12];     //经度
    char E_W[2];        //E/W
    bool isUsefull;     //定位信息是否有效
} Save_Data;

const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;


void setup()    //初始化内容
{ //Serial.begin(9600);
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
  
    //GPS
    GpsSerial.begin(9600);          //定义波特率9600，和我们店铺的GPS模块输出的波特率一致
    DebugSerial.begin(9600);

    Save_Data.isGetData = false;
    Save_Data.isParseData = false;
    Save_Data.isUsefull = false;
}

void loop()     //主循环
{  //GPS
    gpsRead();  //获取GPS数据
    parseGpsBuffer();//解析GPS数据
    printGpsBuffer();//输出解析后的数据
    String NS=Save_Data.N_S;
    String NN=Save_Data.latitude;
    String EW=Save_Data.E_W;
    String EE=Save_Data.longitude;
    String times=Save_Data.UTCTime;
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
    String msg=NS+NN+EW+EE+times+car;     
    rf24.write(&msg[0], 40);
    delay(300);
    }
}

void errorLog(int num)
{
    DebugSerial.print("ERROR");
    DebugSerial.println(num);
    while (1)
    {
  //      digitalWrite(L, HIGH);
        delay(300);
  //      digitalWrite(L, LOW);
        delay(300);
    }
}

void printGpsBuffer()
{
    if (Save_Data.isParseData)
    {
        Save_Data.isParseData = false;

        DebugSerial.print("Save_Data.UTCTime = ");
        DebugSerial.println(Save_Data.UTCTime);

        if(Save_Data.isUsefull)
        {
            Save_Data.isUsefull = false;
            DebugSerial.print("Save_Data.latitude = ");
            DebugSerial.println(Save_Data.latitude);
            DebugSerial.print("Save_Data.N_S = ");
            DebugSerial.println(Save_Data.N_S);
            DebugSerial.print("Save_Data.longitude = ");
            DebugSerial.println(Save_Data.longitude);
            DebugSerial.print("Save_Data.E_W = ");
            DebugSerial.println(Save_Data.E_W);
        }
        else
        {
            DebugSerial.println("GPS DATA is not usefull!");
        }

    }
}

void parseGpsBuffer()
{
    char *subString;
    char *subStringNext;
    if (Save_Data.isGetData)
    {
        Save_Data.isGetData = false;
        DebugSerial.println("**************");
        DebugSerial.println(Save_Data.GPS_Buffer);


        for (int i = 0 ; i <= 6 ; i++)
        {
            if (i == 0)
            {
                if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
                    errorLog(1);    //解析错误
            }
            else
            {
                subString++;
                if ((subStringNext = strstr(subString, ",")) != NULL)
                {
                    char usefullBuffer[2]; 
                    switch(i)
                    {
                        case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;   //获取UTC时间
                        case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;   //获取UTC时间
                        case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;  //获取纬度信息
                        case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;   //获取N/S
                        case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break; //获取纬度信息
                        case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;   //获取E/W

                        default:break;
                    }

                    subString = subStringNext;
                    Save_Data.isParseData = true;
                    if(usefullBuffer[0] == 'A')
                        Save_Data.isUsefull = true;
                    else if(usefullBuffer[0] == 'V')
                        Save_Data.isUsefull = false;

                }
                else
                {
                    errorLog(2);    //解析错误
                }
            }
        }
    }
}


void gpsRead() {
    while (GpsSerial.available())
    {
        gpsRxBuffer[ii++] = GpsSerial.read();
        if (ii == gpsRxBufferLength)clrGpsRxBuffer();
    }

    char* GPS_BufferHead;
    char* GPS_BufferTail;
    if ((GPS_BufferHead = strstr(gpsRxBuffer, "$GPRMC,")) != NULL || (GPS_BufferHead = strstr(gpsRxBuffer, "$GNRMC,")) != NULL )
    {
        if (((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL) && (GPS_BufferTail > GPS_BufferHead))
        {
            memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);
            Save_Data.isGetData = true;

            clrGpsRxBuffer();
        }
    }
}

void clrGpsRxBuffer(void)
{
    memset(gpsRxBuffer, 0, gpsRxBufferLength);      //清空
    ii = 0;
}
