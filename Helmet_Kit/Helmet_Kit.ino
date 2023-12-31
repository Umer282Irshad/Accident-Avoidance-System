                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        #include <ESP8266WiFi.h>
#include <espnow.h>
#define inputpin 3
// REPLACE WITH RECEIVER MAC Address
//94:B9:7E:D5:C7:08
uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0xD5, 0xC7, 0x08};

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 1
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id;
    int IR;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;
int previousState = 0;
int currentState = 0;
// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(inputpin,INPUT);
  digitalWrite(inputpin,LOW);
  pinMode(inputpin,FUNCTION_3);
  pinMode(inputpin,INPUT);
  
  myData.id = BOARD_ID;
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}
 
void loop() {
  // Button will produce no signal when stand is UP
  if(digitalRead(inputpin) == LOW){
    currentState = 1;
  }else{
    currentState = 0;
  }
  // Data will be sent after 3s or when the stand moves up or down
  if ((millis() - lastTime) > timerDelay || previousState != currentState) {
    // Set values to send
    myData.IR = currentState;
    previousState = currentState;
    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
}
