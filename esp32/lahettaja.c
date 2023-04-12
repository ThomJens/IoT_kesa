#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11
#include <esp_now.h>
#include <WiFi.h>

DHT dht(DHTPIN, DHTTYPE);

// vastaanottimen macosoite
uint8_t broadcastAddress1[] = {0x7C, 0x9E, 0xBD, 0xF1, 0x52, 0x68};

typedef struct data_struct {
  int id;
  char paikka[5];
  float h;
  float t;
} data_struct;

// Tallennetaan vastaanottimen  tiedot.
esp_now_peer_info_t peerInfo;

// Tulostaa macosoitteen, kuinka monta bittiä lähetettiin ja onnistuiko lähetys.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Kopioi lähettäjän mac osoite stringiksi.
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // 115200 on baud rate, eli kuinka monta bittiä siirretään sekunnissa
  Serial.begin(115200);
  dht.begin();
 
  WiFi.mode(WIFI_STA);
 
  // Tarkastaa jos esp:n alustus onnistui.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // liitetään vastaanottimeen.
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  data_struct lahetin = {.id = 2, .paikka = "sisa", .h = dht.readHumidity(), .t = dht.readTemperature()};

  // Tarkastaa jos luku epäonnistui ja yrittää lukea datan uudestaan, jos näin on.
  if (isnan(lahetin.h) || isnan(lahetin.t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  // Tarkastaa onnistuiko datan lähetys
  esp_err_t result = esp_now_send(0, (uint8_t *) &lahetin, sizeof(data_struct));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
