#include <ArduinoJson.h> 

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#define SSID "your ssid" 
#define PASS "your password" 
#define DST_IP "139.59.9.23" //api.openweathermap.org 's IP Address
  
void setup()
{     
  Serial1.begin(115200); 
  Serial1.setTimeout(5000);
  Serial.begin(115200);   
  Serial1.println("AT+RESTORE"); 
  delay(1000);
  Serial.println("Init");     
 
  boolean connected=false;
  for(int i=0;i<5;i++){
    if(connectWiFi()){
      connected = true;
      Serial.println("Connected to WiFi...");
      break;
    }
  }
  if (!connected){
    Serial.println("Coudn't connect to WiFi.");
    while(1);
  }  
  delay(5000); 
  Serial1.println("AT+CIPMODE=0"); 
  Serial1.println("AT+CIPMUX=1");   
}   

void loop()
{ 
      
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial1.println(cmd);
  Serial.println(cmd);  
  if(Serial1.find("Error")) return;  
  cmd = "GET /data/2.5/forecast/city?id=1798524&APPID=**********apikey************ \r\n";   //this is pudong id,ohter see  http://bulk.openweathermap.org/sample/ city.list.json
  Serial1.print("AT+CIPSEND=");
  Serial1.println(cmd.length());
  if(Serial1.find(">")){
    Serial.print(">");
  }else{
    Serial1.println("AT+CIPCLOSE");   
    Serial.println("connection timeout");
   delay(1000);
   return;
  }
  Serial1.print(cmd);
  Serial.print(cmd);
  unsigned int i = 0; //timeout counter
  int n = 1; // char counter
  char json[200]="{";
  while (!Serial1.find("\"main\":{")){} 
  while (i<2000) {
    if(Serial1.available()) {
      char c = Serial1.read();
      json[n]=c;
      if(c=='}') break;
      n++;
      i=0;
    }
    i++;
  }
  Serial.println(json);
  DynamicJsonBuffer jsonBuffer;     
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  double temp = root["temp"];
  double pressure = root["pressure"];
  double humidity = root["humidity"];
  temp -= 273.15;   
     
  u8g.firstPage();  
  do {
  u8g.setFont(u8g_font_unifont); 
  u8g.drawStr( 0, 20, "temp");
  u8g.setPrintPos(70, 20);   
  u8g.print(temp);
  u8g.print(" ");
  u8g.print("c");
  
  u8g.drawStr( 0, 42, "Humidity");
  u8g.setPrintPos(70,42); 
  u8g.print(humidity);
  u8g.print(" ");
  u8g.print("%");
  
  u8g.drawStr( 0,63, "Pressure");
  u8g.setPrintPos(68, 63); 
  u8g.print(pressure); 
  } while( u8g.nextPage() ); 
  delay(900000);
 
}
  
boolean connectWiFi()
{
  Serial1.println("AT+CWMODE=3");
  delay(2000);
  Serial1.println("AT+RST");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  Serial1.println(cmd);
  delay(2000);
  if(Serial1.find("OK")){
    Serial.println("OK, Connected to WiFi.");
    return true;
  }else{
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
}


