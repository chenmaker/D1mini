#include <SoftwareSerial.h>
SoftwareSerial BT(D4,D3);
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);           //定义波特率9600
 BT.begin(9600);
  Serial.println("Wating...");
}

void loop() {
  // put your main code here, to run repeatedly:
 while (BT.available()) {   
     Serial.write(BT.read());//收到GPS数据则通过串口输出
  }
}
