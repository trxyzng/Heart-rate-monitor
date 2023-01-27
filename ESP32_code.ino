
#include <WiFi.h>       //include wifi library       
#include <PubSubClient.h>  //include mqtt library
#include "time.h"      //include time library
#include "MAX30100_PulseOximeter.h"       //include sensor library

#define period 1000       //time between publish 
uint32_t previous = 0;    // store time from last publish

const char* ssid = "ASUS";                //wifi name
const char* password = "10032001";        //wifi password

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "broker.mqtt-dashboard.com";        //mqtt server address
#define msg_size  20        //max size of payload 
char msg[msg_size];         //payload

PulseOximeter pox;          //initial sensor
int h = 0;        //store heart rate value
int s = 0;        //store SpO2 value

struct tm timeinfo;
const char* ntpServer = "pool.ntp.org";       //time server
const long  gmtOffset_sec = 60*60*7;          //adjust base on timezone
const int   daylightOffset_sec = 0;
char hour[3];
char minute[3];
char second[3];
char day[3];
char month[3];
char year[3];



void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.println("Try to connect to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_sensor(){
        Serial.print("Start the pulse oximeter sensor..");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);        //set LED current
}

void setup_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_hoacchitrung_nguyendinhhongquan", "test", "test", "hoacchitrung_nguyendinhhongquan", 2, true, "no", false)) {  //clientID, user, pass, willtopic, willqos, willretain, willmessage, clean session
      Serial.println("Connected to mqtt broker");                               //last will message is "no"
      client.publish("hoacchitrung_nguyendinhhongquan", "on", "true");
      Serial.println("Publish message: on");
      //client.setKeepAlive(1.5);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 0.5 seconds");
      delay(500);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setup_mqtt();
  setup_sensor();
}

void loop(){
  int a= millis();
    while(!WiFi.status() || !client.connected() ){
      pox.shutdown();
      Serial.println("Wifi lost. Try to recconnect");
      setup_wifi();
      setup_mqtt();
      setup_sensor();
      //pox.resume();
    }

    getLocalTime(&timeinfo);
    pox.update();
    if (millis() - previous > period) {
      h = round(pox.getHeartRate());
      s = round(pox.getSpO2());
      if (h < 40 || s < 80) {
        Serial.printf("%dbpm %d", h, s);
        Serial.print("%");
        Serial.printf("\nData error");
        client.publish("hoacchitrung_nguyendinhhongquan", "");                    //if data is error, send a blank payload to keep connection
        Serial.println("\nPublish msg: null");
      }
      else {
        strftime(hour,3, "%H", &timeinfo);
        strftime(minute,3, "%M", &timeinfo);
        strftime(second,3, "%S", &timeinfo);
        strftime(day,3, "%d", &timeinfo);
        strftime(month,3, "%m", &timeinfo);
        strftime(year,3, "%y", &timeinfo);
        snprintf (msg, msg_size, "%s%s%s%s%s%s%d%d", hour, minute, second, day, month, year, h, s);     //if data is normal, send whole data to broker
        Serial.printf("%dbpm %d", h, s);
        Serial.print("%");
        Serial.printf("\nPublish message: %s\n", msg);
        client.publish("hoacchitrung_nguyendinhhongquan", msg);
        int b = millis();
        Serial.printf("Time to send data: %dms\n", b-a);
      }
      previous = millis();
    }
}
