#include <WiFi.h>
#include <AsyncUDP.h>
#include <Arduino.h>
#include <esp_log.h>
#include<driver/i2s.h>

// networking
const char* ssid = "Stain";
const char* password = "b$VJ518175";
const int port = 5000;
AsyncUDP udp_socket;

//peripherals
const int MIC_SCK = GPIO_NUM_5;
const int MIC_WS = GPIO_NUM_4;
const int MIC_SD = GPIO_NUM_3;

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate =  16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_LSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128
  };

i2s_pin_config_t pin_config = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };

//buffer for I2s data and UDP transmission
uint8_t mic_data[128];
int buffer_size = sizeof(mic_data);
size_t bytes_read;

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

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  
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

  bytes_read = i2s_read(I2S_NUM_0, mic_data, buffer_size, &bytes_read, portMAX_DELAY);
  if(bytes_read == ESP_OK){
      Serial.println("broadcasting  data");
      udp_socket.broadcast(mic_data, bytes_read);
    }
  else{
      Serial.printf("Error reading I2S data: %d\n", bytes_read);
    }
}
