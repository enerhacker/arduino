#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <EthernetClient.h>

//Ethernet és MQTT beállítások
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 2, 115);
EthernetClient ethClient;
PubSubClient client(server, 1883, ethClient);

//Szenzorok pin beállításai
#define ABLAK1_PIN 2
#define ABLAK2_PIN 3
#define AJTO1_PIN 7
#define DHT11_PIN 5
#define PIR_PIN 6
#define LDR_PIN A0

//Szenzorok változói
int ablak1 = 0;
int ablak2 = 0;
int ajto1 = 0;
float hofok = 0.0;
int motion = 0;
int feny = 0;

//DHT11 objektum létrehozása
DHT dht(DHT11_PIN, DHT11);

void setup() {
  //Ethernet inicializálása
  Ethernet.begin(mac);

  //MQTT inicializálása
  client.setServer(server, 1883);
  client.connect("eloter");

  //Szenzorok inicializálása
  pinMode(ABLAK1_PIN, INPUT_PULLUP);
  pinMode(ABLAK2_PIN, INPUT_PULLUP);
  pinMode(AJTO1_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  //DHT11 inicializálása
  dht.begin();
}

void loop() {
  //Szenzorok olvasása
  ablak1 = digitalRead(ABLAK1_PIN);
  ablak2 = digitalRead(ABLAK2_PIN);
  ajto1 = digitalRead(AJTO1_PIN);
  hofok = dht.readTemperature();
  motion = digitalRead(PIR_PIN);
  feny = analogRead(LDR_PIN);

  // Ablak és ajtó állapot küldése MQTT-n
  client.publish("eloter/ablak1", String(ablak1 == HIGH ? "Nyitva" : "Zárva").c_str());
  client.publish("eloter/ablak2", String(ablak2 == HIGH ? "Nyitva" : "Zárva").c_str());
  client.publish("eloter/ajto1", String(ajto1 == HIGH ? "Nyitva" : "Zárva").c_str());

  // DHT11 szenzoradat küldése MQTT-n keresztül
  client.publish("eloter/hofok", String(hofok).c_str());

  // PIR szenzoradat küldése MQTT-n keresztül
  client.publish("eloter/motion", String(motion).c_str());

  // LDR szenzor érték alapján MQTT üzenet küldése
  int ldr_value = analogRead(LDR_PIN);
  int ldr_status = (ldr_value > 10) ? 1 : 0;
  client.publish("eloter/csengo", String(ldr_status).c_str());

  // Kis várakozás
  delay(1000);
}