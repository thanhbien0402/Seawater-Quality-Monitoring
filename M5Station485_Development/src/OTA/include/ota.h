#ifndef OTA_H
#define OTA_H

#include <ESP32httpUpdate.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

// const char* currentFirmwareTitle   = "MyFirmware";
// const char* currentFirmwareVersion = "1.0.0";

extern volatile bool otaUpdateTriggered;
extern char receivedFwTitle[32];
extern char receivedFwVersion[16];

void performOTAUpdate(const char* fwTitle, const char* fwVersion);
void handleOTAUpdate();
void otaUpdateTask(void *pvParameters);

// // Callback xử lý tin nhắn MQTT
// void mqttCallback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("MQTT Message arrived on topic: ");
//   Serial.println(topic);
//   String message;
//   for (unsigned int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }
//   Serial.println("Payload: " + message);
  
//   // Phân tích JSON của payload
//   DynamicJsonDocument doc(512);
//   DeserializationError error = deserializeJson(doc, message);
//   if (error) {
//     Serial.print("JSON parse error: ");
//     Serial.println(error.f_str());
//     return;
//   }
  
//   // Kiểm tra xem có key "fw_title" và "fw_version" hay không
//   if (doc.containsKey("fw_title") && doc.containsKey("fw_version")) {
//     const char* fwTitle   = doc["fw_title"];
//     const char* fwVersion = doc["fw_version"];
//     Serial.print("Received fw_title: ");
//     Serial.println(fwTitle);
//     Serial.print("Received fw_version: ");
//     Serial.println(fwVersion);
    
//     // So sánh phiên bản mới với phiên bản hiện tại (nếu khác thì trigger OTA)
//     if (strcmp(currentFirmwareVersion, fwVersion) != 0) {
//       Serial.println("Có firmware mới. Trigger OTA update.");
//       strncpy(receivedFwTitle, fwTitle, sizeof(receivedFwTitle)-1);
//       strncpy(receivedFwVersion, fwVersion, sizeof(receivedFwVersion)-1);
//       otaUpdateTriggered = true;
//     } else { 
//       Serial.println("Firmware đã được cập nhật.");
//     }
//   }
// }

// // Hàm reconnect MQTT
// void reconnectMQTT() {
//   while (!mqttClient.connected()) {
//     Serial.print("Kết nối đến ThingsBoard MQTT...");
//     if (mqttClient.connect("ESP32", accessToken, "")) {
//       Serial.println(" đã kết nối");
//       // Subscribe vào topic attributes
//       mqttClient.subscribe("v1/devices/me/attributes");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(mqttClient.state());
//       Serial.println(" thử lại sau 5 giây");
//       delay(5000);
//     }
//   }
// }

// // Hàm thực hiện OTA update
// void performOTAUpdate(const char* fwTitle, const char* fwVersion) {
//   // Xây dựng URL OTA theo định dạng của ThingsBoard:
//   // https://demo.thingsboard.io/api/v1/$ACCESS_TOKEN/firmware?title=$TITLE&version=$VERSION
//   String otaURL = "https://demo.thingsboard.io/api/v1/";
//   otaURL += accessToken;
//   otaURL += "/firmware?title=";
//   otaURL += fwTitle;
//   otaURL += "&version=";
//   otaURL += fwVersion;
//   Serial.println("Thực hiện OTA update. URL: " + otaURL);
  
//   // Tạo client HTTPS (cho demo sử dụng setInsecure, production cần setCACert)
//   WiFiClientSecure client;
//   client.setInsecure();
  
//   // Lưu ý: truyền đối tượng client vào hàm update để sử dụng kết nối HTTPS
//   t_httpUpdate_return ret = ESPhttpUpdate.update(otaURL.c_str());
//   switch (ret) {
//     case HTTP_UPDATE_FAILED:
//       Serial.printf("OTA Update thất bại (%d): %s\n",
//                     ESPhttpUpdate.getLastError(),
//                     ESPhttpUpdate.getLastErrorString().c_str());
//       break;
//     case HTTP_UPDATE_NO_UPDATES:
//       Serial.println("Không có cập nhật OTA mới.");
//       break;
//     case HTTP_UPDATE_OK:
//       Serial.println("OTA Update thành công.");
//       break;
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Khởi động...");

//   // Kết nối WiFi
//   WiFi.begin(ssid, password);
//   Serial.print("Đang kết nối đến WiFi");
//   while(WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi đã kết nối!");
  
//   // Cấu hình MQTT
//   mqttClient.setServer(thingsboardServer, thingsboardPort);
//   mqttClient.setCallback(mqttCallback);
// }

// void loop() {
//   if (!mqttClient.connected()) {
//     reconnectMQTT();
//   }
//   mqttClient.loop();
  
//   // Nếu có yêu cầu OTA update từ MQTT
//   if (otaUpdateTriggered) {
//     performOTAUpdate(receivedFwTitle, receivedFwVersion);
//     otaUpdateTriggered = false;
//   }
  
//   delay(1000);
// }

#endif // OTA_H