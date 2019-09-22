//输出解析后的数据
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
