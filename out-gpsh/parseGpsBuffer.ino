//解析GPS数据
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
