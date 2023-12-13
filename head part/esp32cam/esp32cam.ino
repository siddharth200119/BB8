#include <WiFi.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include <UDP.h>

//network credentials
const char* ssid = "Stain";
const char* password = "b$VJ518175";

//UDP
const int udpPORT = 5000;
const char* receiver_ip = "192.168.1.2";
WiFiUDP udp_socket;

//test message
const char* test = "test";
int msg_size = sizeof(test);

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
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable   detector
  connect_wifi(ssid, password);

  udp_socket.begin(udpPORT);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.println("WIFI Disconnected. Attempting to reconnect: ");
      connect_wifi(ssid, password);
    }

   udp_socket.beginPacket(receiver_ip, udpPORT);
   udp_socket.write(reinterpret_cast<const uint8_t*>(test), msg_size);
   udp_socket.endPacket();
}
