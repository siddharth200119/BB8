#include <WiFi.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include <UDP.h>
#include "esp_camera.h"

//network credentials
const char* ssid = "Stain";
const char* password = "b$VJ518175";

//UDP
const int udpPORT = 5000;
const char* receiver_ip = "192.168.1.2";
WiFiUDP udp_socket;

//camera settings
camera_config_t camera_config;
const int frameWidth = 160;
const int frameHeight = 120;
const int frameSize = frameWidth * frameHeight;

//buffers for frame data
uint8_t frame_buffer[frameSize];
uint8_t transmit_buffer[frameSize];

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

  //init camera
  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = 5;
  camera_config.pin_d1 = 18;
  camera_config.pin_d2 = 19;
  camera_config.pin_d3 = 21;
  camera_config.pin_d4 = 36;
  camera_config.pin_d5 = 39;
  camera_config.pin_d6 = 34;
  camera_config.pin_d7 = 35;
  camera_config.pin_xclk = 0;
  camera_config.pin_pclk = 22;
  camera_config.pin_vsync = 25;
  camera_config.pin_href = 23;
  camera_config.pin_sccb_scl = 27;
  camera_config.pin_sccb_sda = 26;
  camera_config.pin_pwdn = 32;
  camera_config.pin_reset = -1;
  camera_config.xclk_freq_hz = 15000000;
  camera_config.pixel_format = PIXFORMAT_YUV422; // Raw RGB format
  camera_config.frame_size = FRAMESIZE_QVGA; // Reduced frame size
  camera_config.jpeg_quality = 12; // Not applicable for raw format
  camera_config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  camera_config.fb_count = 1;

  esp_err_t err = esp_camera_init(&camera_config);
  if(err != ESP_OK){
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
  else{
    Serial.println("camera initialized");
    }
  
  udp_socket.begin(udpPORT);
  Serial.println("UDP server started");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
      Serial.println("WIFI Disconnected. Attempting to reconnect: ");
      connect_wifi(ssid, password);
    }

  //capture frame
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb) {
      Serial.println("cam init failed");
      return;
    }

   //extract raw image
   memcpy(frame_buffer, fb->buf, frameSize);

   //prepare data for transmission
   memcpy(transmit_buffer, frame_buffer, frameSize);
  
   udp_socket.beginPacket(receiver_ip, udpPORT);
   udp_socket.write(transmit_buffer, frameSize);
   udp_socket.endPacket();

   //release the frame buffer
   esp_camera_fb_return(fb);
   delay(100);
}
