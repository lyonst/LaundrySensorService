void checkForWebRequest()
{ 
  // Read the first line of the request
  String req;
  while (Serial.available())
  {
    req += char(Serial.read());
  }

  if (req.length() == 0)
  {
    return;
  }
  
  if (req.indexOf("/sensors/laundry/0") != -1)
  {
    sendStatus(washerAlarm, washerAlarmTime);
  }
  else if (req.indexOf("/sensors/laundry/1") != -1)
  {
    sendStatus(dryerAlarm, dryerAlarmTime);
  }
  else if (req.indexOf("/sensors/time") != -1)
  {
    sendTime();
  }
  else if (req.indexOf("/sensors/settime") != -1)
  {
    settime(req);
  }
  else if (req.indexOf("/sensors/raw0") != -1)
  {
    sendRawValue(washerPin);
  }
  else if (req.indexOf("/sensors/raw1") != -1)
  {
    sendRawValue(dryerPin);
  }
  else
  {
    Serial.print("HTTP/1.1 400 ERROR\r\n");
  }
}

void sendStatus(boolean sensor, time_t alarmTime)
{
  String s = generateHeaders();

  String s2 = "{\"Status\": {\"Running\":";
  s2 += sensor ? "\"true\"" : "\"false\"";
  s2 += ", \"TimeStamp\":\"";

  char buf[25];
  sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d", year(alarmTime), month(alarmTime), day(alarmTime), hour(alarmTime), minute(alarmTime), second(alarmTime));
  
  s2 += buf;
  s2 += "\"}}";

  s += "Content-Length: " + String(s2.length()) + "\r\n\r\n";
  s += s2;

  sendResponse(s);
}

void sendTime()
{
  String s = generateHeaders();

  time_t currentTime = now();

  char buf[25];
  sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d", year(currentTime), month(currentTime), day(currentTime), hour(currentTime), minute(currentTime), second(currentTime));

  String s2 = "<TimeStamp xmlns:i=\"http://www.w3.org/2001/XMLSchema-instance\">";
  s2 += buf;
  s2 += "</TimeStamp>";

  s += "Content-Length: " + String(s2.length()) + "\r\n";
  s += "\r\n";
  s += s2;

  sendResponse(s);
}

void sendRawValue(int pin)
{
  String s = generateHeaders();

  char buf[5];
  sprintf(buf, "%d", analogRead(pin));

  String s2 = "<RawValue xmlns:i=\http://www.w3.org/2001/XMLSchema-instance\">";
  s2 += buf;
  s2 += "</RawValue>";

  s += "Content-Length: " + String(s2.length()) + "\r\n";
  s += "\r\n";
  s += s2;

  sendResponse(s);
}

String generateHeaders()
{
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Cache-Control: no-cache\r\n";
  s += "Pragma: no-cache\r\n";
  s += "Content-Type: application/xml; charset=utf-8\r\n";
  s += "Expires: -1\r\n";
 
  return s;
}

void sendResponse(String s)
{
  int start = 0;

  while (start < s.length())
  {
    Serial.print(s.substring(start, start + 20));
    start += 20;
    delay(500);
  }
}

void settime(String s)
{
  String timeStr = s.substring(s.lastIndexOf("/") + 1);

  int year = timeStr.substring(0, 2).toInt() * 100 + timeStr.substring(2, 4).toInt();

  TimeElements tm;
  tm.Year = timeStr.substring(0, 2).toInt() * 100 + timeStr.substring(2, 4).toInt() - 1970;
  tm.Month = timeStr.substring(5, 7).toInt();
  tm.Day = timeStr.substring(8, 10).toInt();
  tm.Hour = timeStr.substring(11, 13).toInt();
  tm.Minute = timeStr.substring(14, 16).toInt();
  tm.Second = timeStr.substring(17, 19).toInt();
  
  setTime(makeTime(tm));
}

