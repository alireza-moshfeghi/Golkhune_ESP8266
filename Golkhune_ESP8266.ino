#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DateTime.h>
#include <DateTimeTZ.h>
#include <ESPDateTime.h>
#include <TimeElapsed.h>
#include <DHT.h>
#define HEATER 5
#define COOLER 4
#define FOG_MAKER 0
#define FAN 2
#define BULB 14
#define WATER_PUMP 12
#define SENSOR 13
#define PHOTOCELL A0

AsyncWebServer server(80);
DHT dht(SENSOR, DHT22);
String AP_name = "Golkhune";
String AP_pass = "";
bool heater = false;
bool cooler = false;
bool fog_maker = false;
bool fan = false;
bool bulb = false;
bool water_pump = false;
int temperature = 0;
int humidity = 0;
short brightness = 0;
short uptime = 0;
short min_temperature = 150;
short max_temperature = -50;
short min_humidity = 100;
short max_humidity = 0;
short old_hour = 0;

void setup() {
  WiFi.softAP(AP_name, AP_pass);
  Serial.begin(9600);
  DateTime.begin();
  SPIFFS.begin();
  dht.begin();
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(FOG_MAKER, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(BULB, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(PHOTOCELL, INPUT);
  digitalWrite(HEATER, HIGH);
  digitalWrite(COOLER, HIGH);
  digitalWrite(FOG_MAKER, HIGH);
  digitalWrite(FAN, HIGH);
  digitalWrite(BULB, HIGH);
  digitalWrite(WATER_PUMP, HIGH);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/status.html", String(), false);
  });
  server.on("/output", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/output.html", String(), false);
  });
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/control.html", String(), false);
  });
  server.on("/heater", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (heater == true)
      request->send_P(200, "text/plain", String("on").c_str());
    else
      request->send_P(200, "text/plain", String("off").c_str());
  });
  server.on("/cooler", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (cooler == true)
      request->send_P(200, "text/plain", String("on").c_str());
    else
      request->send_P(200, "text/plain", String("off").c_str());
  });
  server.on("/fog_maker", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (fog_maker == true)
      request->send_P(200, "text/plain", String("on").c_str());
    else
      request->send_P(200, "text/plain", String("off").c_str());
  });
  server.on("/fan", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (fan == true)
      request->send_P(200, "text/plain", String("on").c_str());
    else
      request->send_P(200, "text/plain", String("off").c_str());
  });
  server.on("/bulb", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("status")) {
      if (request->getParam("status")->value() == "on") {
        request->send_P(200, "text/plain", String("ok").c_str());
        digitalWrite(BULB, LOW);
        bulb = true;
      }
      else if (request->getParam("status")->value() == "off") {
        request->send_P(200, "text/plain", String("ok").c_str());
        digitalWrite(BULB, HIGH);
        bulb = false;
      }
    }
    else {
      if (bulb == true)
        request->send_P(200, "text/plain", String("on").c_str());
      else
        request->send_P(200, "text/plain", String("off").c_str());
    }
  });
  server.on("/water_pump", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("status")) {
      if (request->getParam("status")->value() == "on") {
        request->send_P(200, "text/plain", String("ok").c_str());
        digitalWrite(WATER_PUMP, LOW);
        water_pump = true;
      }
      else if (request->getParam("status")->value() == "off") {
        request->send_P(200, "text/plain", String("ok").c_str());
        digitalWrite(WATER_PUMP, HIGH);
        water_pump = false;
      }
    }
    else {
      if (water_pump == true)
        request->send_P(200, "text/plain", String("on").c_str());
      else
        request->send_P(200, "text/plain", String("off").c_str());
    }
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (temperature > 150 || temperature < -50)
      request->send_P(200, "text/plain", String("--").c_str());
    else
      request->send_P(200, "text/plain", String(String(temperature) + ".C").c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (humidity > 100 || humidity < 0)
      request->send_P(200, "text/plain", String("--").c_str());
    else
      request->send_P(200, "text/plain", String(String(humidity) + "%").c_str());
  });
  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (brightness > 100 || brightness <= 0)
      request->send_P(200, "text/plain", String("--").c_str());
    else
      request->send_P(200, "text/plain", String(String(brightness) + "%").c_str());
  });
  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (uptime < 0)
      request->send_P(200, "text/plain", String("--").c_str());
    else
      request->send_P(200, "text/plain", String(String(uptime) + "H").c_str());
  });
  server.on("/min_max_temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (min_temperature == 150 || max_temperature == -50)
      request->send_P(200, "text/plain", String("-- &nbsp --").c_str());
    else
      request->send_P(200, "text/plain", String(String(String(min_temperature) + ".C &nbsp ") + String(String(max_temperature) + ".C")).c_str());
  });
  server.on("/min_max_humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (min_humidity == 100 || max_humidity == 0)
      request->send_P(200, "text/plain", String("-- &nbsp --").c_str());
    else
      request->send_P(200, "text/plain", String(String(String(min_humidity) + "% &nbsp ") + String(String(max_humidity) + "%")).c_str());
  });
  server.on("/assets/style/bootstrap.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/style/bootstrap.css", "text/css");
  });
  server.on("/assets/style/font.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/style/font.css", "text/css");
  });
  server.on("/assets/script/bootstrap.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/script/bootstrap.js", "text/css");
  });
  server.on("/assets/image/menu.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/menu.png", "image/png");
  });
  server.on("/assets/image/brightness.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/brightness.png", "image/png");
  });
  server.on("/assets/image/brightness2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/brightness2.png", "image/png");
  });
  server.on("/assets/image/uptime.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/uptime.png", "image/png");
  });
  server.on("/assets/image/uptime2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/uptime2.png", "image/png");
  });
  server.on("/assets/image/humidity.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/humidity.png", "image/png");
  });
  server.on("/assets/image/humidity2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/humidity2.png", "image/png");
  });
  server.on("/assets/image/temperature.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/temperature.png", "image/png");
  });
  server.on("/assets/image/temperature2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/temperature2.png", "image/png");
  });
  server.on("/assets/image/graph.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/graph.png", "image/png");
  });
  server.on("/assets/image/graph2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/graph2.png", "image/png");
  });
  server.on("/assets/image/snow.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/snow.png", "image/png");
  });
  server.on("/assets/image/snow2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/snow2.png", "image/png");
  });
  server.on("/assets/image/fire.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/fire.png", "image/png");
  });
  server.on("/assets/image/fire2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/fire2.png", "image/png");
  });
  server.on("/assets/image/cloud.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/cloud.png", "image/png");
  });
  server.on("/assets/image/cloud2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/cloud2.png", "image/png");
  });
  server.on("/assets/image/fan.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/fan.png", "image/png");
  });
  server.on("/assets/image/fan2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/fan2.png", "image/png");
  });
  server.on("/assets/image/bulb.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/bulb.png", "image/png");
  });
  server.on("/assets/image/bulb2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/bulb2.png", "image/png");
  });
  server.on("/assets/image/water.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/water.png", "image/png");
  });
  server.on("/assets/image/water2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/water2.png", "image/png");
  });
  server.on("/assets/image/moon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/moon.png", "image/png");
  });
  server.on("/assets/image/sun.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/sun.png", "image/png");
  });
  server.on("/assets/image/power.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/power.png", "image/png");
  });
  server.on("/assets/image/power2.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/image/power2.png", "image/png");
  });
  server.on("/assets/font/B-Koodak.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/font/B-Koodak.ttf", "application/x-font-ttf");
  });
  server.on("/assets/font/7-Segment.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/font/7-Segment.ttf", "application/x-font-ttf");
  });
  server.on("/assets/font/Arial-Round.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/assets/font/Arial-Round.ttf", "application/x-font-ttf");
  });

  server.begin();
}

void loop() {
  temperature = (int)dht.readTemperature();
  humidity = (int)dht.readHumidity();
  brightness = map(analogRead(PHOTOCELL), 0, 1023, 0, 100);
  DateTimeParts datetime = DateTime.getParts();
  if (old_hour != datetime.getHours()) {
    old_hour = datetime.getHours();
    uptime++;
  }
  if (uptime < 24) {
    if (temperature < min_temperature && temperature <= 150 && temperature >= -50)
      min_temperature = temperature;
    if (temperature > max_temperature && temperature <= 150 && temperature >= -50)
      max_temperature = temperature;
    if (humidity < min_humidity && humidity <= 100 && humidity >= 0)
      min_humidity = humidity;
    if (humidity > max_humidity && humidity <= 100 && humidity >= 0)
      max_humidity = humidity;
  }
  delay(250);
}
