void checkSensor(int pin, int *pVal, unsigned long *plastTime, boolean *pAlarm, time_t *palarmTime)
{
  
  *pVal = analogRead(pin);
 
  if (*pVal > 300) {
    
    *plastTime = millis() + alarmTime;

    if (!*pAlarm) {
      delay(10000);
      *pVal = analogRead(pin);

      if (*pVal > 300)
      {
        *pAlarm = true;
        *palarmTime = now();
      }
    }
  }
  else {
    if ((long)(millis() - *plastTime) >= 0 && *pAlarm) {
      *pAlarm = false;
      *palarmTime = now();
    }
  }
}

