#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

const char* mqtt_username = NULL;
const char* mqtt_password = NULL;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
DFRobotDFPlayerMini myDFPlayer;

IPAddress ip(192, 168, 178, 77);
byte mqttserver[] = { 192, 168, 178, 156 };
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(mqttserver, 1883, callback, ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  String content = "";
  char character;
  for (int num = 0; num < length; num++) {
    character = payload[num];
    content.concat(character);
  }
  Serial.println(topic);
  Serial.println(content);
  if (!strcmp(topic,"mp3/cmd/play")){
    myDFPlayer.play(content.toInt());
  } else if (!strcmp(topic,"mp3/cmd/pause")){
    myDFPlayer.pause();
  } else if (!strcmp(topic,"mp3/cmd/start")){
    myDFPlayer.start();
  } else if (!strcmp(topic,"mp3/volume/set")){
    myDFPlayer.volume(content.toInt());
  }
}
void setup() {
  Ethernet.init(10);  // Most Arduino shields
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  if (client.connect("Arduini Mega 2560", mqtt_username, mqtt_password)) {
    client.publish("mp3player/status", "hello world");
    Serial.println("connected");
  }
  Serial3.begin(9600);
  if (!myDFPlayer.begin(Serial3)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println("DFPlayer Mini online");
  client.subscribe("mp3/cmd/play");
  client.subscribe("mp3/cmd/pause");
  client.subscribe("mp3/cmd/start");
//  next
//  previous
//  loop
//  playFolder
//  enableLoopAll
//  disableLoopAll
//  playMp3Folder
//  advertise
//  stopAdvertise
//  playLargeFolder
//  loopFolder
//  randomAll
//  enableLoop
//  disableLoop
  client.subscribe("mp3/volume/set");
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
}

void loop() {
  client.loop();
}
