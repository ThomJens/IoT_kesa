#include <WiFi.h>
#include <WiFiClient.h>
#include <esp_now.h>

typedef struct data_struct {
  int id;
  char paikka[5];
  float h;
  float t;
} data_struct;

data_struct myData;

// Luodaan pari data_structia listaa varten, jotta voidaan erottaa sisä ja ulko mittaukset toisistaan
data_struct sisa;
data_struct ulko;

data_struct vastaanotettu[2] = {sisa, ulko};

WiFiServer server(80);

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // varataan muisti
  memcpy(&myData, incomingData, sizeof(myData));
  memcpy(vastaanotettu[myData.id - 1].paikka, myData.paikka, sizeof(myData.paikka));

  vastaanotettu[myData.id - 1].t = myData.t;
  vastaanotettu[myData.id - 1].h = myData.h;

  Serial.print(myData.id);
  Serial.print(":");
  Serial.print(myData.paikka);
  Serial.print(":");
  Serial.print(myData.t);
  Serial.print(":");
  Serial.print(myData.h);
  Serial.println();
}

void setup() {
  // 115200 on baud rate, eli kuinka monta bittiä siirretään sekunnissa
  Serial.begin(115200);
    
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  delay(2000);
}
