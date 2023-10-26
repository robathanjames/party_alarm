#include <Arduino.h>
#include <user_interface.h>
#include <espnow.h>
#include "shared_defs.h"

#define PARTY_STOP_PIN D2

const uint8_t disco_mac[6] = {0xa4,0xe5,0x7c,0xbc,0xdb,0xe9};  // Use const instead of static
const uint8_t speaker_mac[6] = {0x84,0xf3,0xeb,0xb2,0x32,0x9e};
const uint8_t light1_mac[6] = {0x80,0x64,0x6f,0xb7,0xd5,0x93};
const uint8_t light2_mac[6] = {0x80,0x64,0x6f,0xb6,0x86,0x3e};

void init_esp_now() {
  if (esp_now_init() != 0) {  // Check for errors in ESP-NOW initialization
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
}

void add_peer(const uint8_t* mac) {
  if (esp_now_add_peer(mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0) {  // Check for errors when adding peers
    Serial.println("Failed to add peer");
  }
}

void stop_party() {
  uint8_t op = OP_STOP_DISCOTIME;
  esp_now_send(disco_mac, &op, 1);
  op = OP_PAUSE_MUSIC;
  esp_now_send(speaker_mac, &op, 1);
  op = OP_LIGHTS_ON;
  esp_now_send(light1_mac, &op, 1);
  esp_now_send(light2_mac, &op, 1);
}

void start_party() {
  uint8_t op = OP_PLAY_MUSIC_WITH_DISCOTIME;
  esp_now_send(speaker_mac, &op, 1);
  op = OP_LIGHTS_OFF;
  esp_now_send(light1_mac, &op, 1);
  esp_now_send(light2_mac, &op, 1);
  delay(3000);
  op = OP_DISCOTIME;
  esp_now_send(disco_mac, &op, 1);
}

void setup() {
  Serial.begin(115200);  // Initialize Serial for debugging
  
  init_esp_now();  // Initialize ESP-NOW
  
  add_peer(disco_mac);
  add_peer(speaker_mac);
  add_peer(light1_mac);
  add_peer(light2_mac);

  pinMode(PARTY_STOP_PIN, INPUT_PULLUP);  // Use INPUT_PULLUP mode for button
  
  // Debounce logic
  delay(50);
  if (digitalRead(PARTY_STOP_PIN)) {
    stop_party();
  } else {
    start_party();
  }

  ESP.deepSleep(0, WAKE_RF_DISABLED);
}

void loop() {
  // Nothing here
}
