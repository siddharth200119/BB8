#include <WiFi.h>
#include <AsyncUDP.h>
#include <I2S.h>

// networking
const char* ssid = "Stain";
const char* password = "b$VJ518175";
const int port = 5000;
AsyncUDP udp_socket;

//peripherals
const int MIC_SCK = GPIO_NUM_5;
const int MIC_WS = GPIO_NUM_4;
const int MIC_SD = GPIO_NUM_3;

//buffer for I2s data and UDP transmission
uint8_t mic_data[128];
int buffer_size = sizeof(mic_data);

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
  if(udp_socket.listen(port)){
    Serial.print("UDP server listening on port: ");
    Serial.println(port);
    udp_socket.onPacket([](AsyncUDPPacket packet){
      Serial.print("Received UDP packet from: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.println(packet.remotePort());

      for(int i=0; i<packet.length(); i++){
          Serial.print((char)packet.data()[i]);
        }
      Serial.println();
      });
    }
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.println("WIFI Disconnected. Attempting to reconnect: ");
      connect_wifi(ssid, password);
    }

  udp_socket.broadcast("test");
}
