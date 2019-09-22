//获取GPS数据
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
