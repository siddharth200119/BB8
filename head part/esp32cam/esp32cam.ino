#include <WiFi.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

const char* ssid = "Stain";
const char* password = "b$VJ518175";

void connect_wifi(const char* ssid, const char* password){
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   Serial.print("\nConnecting ");

   while(WiFi.status() != WL_CONNECTED){
      Serial.print(".. ");
      delay(100);
    }

   const char ip = WiFi.localIP();
   Serial.println("\nConnected to the WiFi network");
   Serial.print("Local ESP32 IP: ");
   Serial.println(ip);
  }


void setup() {
  Serial.begin(115200);
  delay(100);

  connect_wifi(ssid, password);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.println("WIFI Disconnected. Attempting to reconnect: ");
      connect_wifi(ssid, password);
    }
}
