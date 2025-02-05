#include <ESP8266WiFi.h>  //Wi-Fi module management library
#include <ESPAsyncWebServer.h>  //Async web server library
#include <DateTime.h>  //Time and date library
#include <EEPROM.h>  //EEPROM storage memory library
#include <DHT.h>  //Temperature and humidity sensor library
#include <FS.h>  //File server library
#define HEATER 5  //Naming the heater output pin
#define COOLER 4  //Naming the cooler output pin
#define FOG_MAKER 0  //Naming the fog maker output pin
#define FAN 2  //Naming the fan output pin
#define BULB 14  //Naming the bulb output pin
#define WATER_PUMP 12  //Naming the water pump output pin
#define SENSOR 13  //Naming the DHT22 sensor input pin
#define PHOTOCELL A0  //Naming the photoresistor input pin
#define BUZZER 15  //Naming the buzzer output pin

AsyncWebServer server(80);  //Setting the web server on port 80
DHT dht(SENSOR, DHT22);  //Setting the DHT22 sensor on the corresponding pin
String AP_name = "";  //Access point name storage variable
String AP_pass = "";  //Access point password storage variable
bool heater = false;  //Heater status (on or off)
bool cooler = false;  //Cooler status (on or off)
bool fog_maker = false;  //Fog maker status (on or off)
bool fan = false;  //Fan status (on or off)
bool bulb = false;  //Bulb status (on or off)
bool water_pump = false;  //Water pump status (on or off)
bool period = false;  //24 hour period
bool ap_change = false;  //Setting the access point at runtime
bool dark_mode = false;  //Dark mode status (on or off)
int temperature = 0;  //Temperature storage variable
int humidity = 0;  //Humidity storage variable
short brightness = 0;  //Brightness storage variable
short uptime = 0;  //Runtime storage variable
short min_temperature = 300;  //Minimum temperature storage variable
short max_temperature = -50;  //Maximum temperature storage variable
short min_humidity = 100;  //Minimum humidity storage variable
short max_humidity = 0;  //Maximum humidity storage variable
short old_min_temperature = 300;  //Yesterday minimum temperature storage variable
short old_max_temperature = -50;  //Yesterday maximum temperature storage variable
short old_min_humidity = 100;  //Yesterday minimum humidity storage variable
short old_max_humidity = 0;  //Yesterday maximum humidity storage variable
short old_hour = 0;  //Previous hour storage variable
String set_min_temperature = "0";  //Minimum set temperature
String set_max_temperature = "0";  //Maximum set temperature
String set_min_humidity = "0";  //Minimum set humidity
String set_max_humidity = "0";  //Maximum set humidity
String delay_time = "";  //Delay time value
String error_time = "3600";  //Error time value
String scale = "";  //Temperature scale
short heater_timer = -1;  //Heater startup delay
short cooler_timer = -1;  //Cooler startup delay
short fog_maker_timer = -1;  //Fog maker startup delay
short fan_timer = -1;  //Fan startup delay
short heater_error = 0;
short cooler_error = 0;
short fog_maker_error = 0;
short fan_error = 0;
String language = "";  //Panel language
bool buzzer = true;

String processor(const String& var) {
  if (var == "THEME")  //Replace theme name in html page
    if (dark_mode)
      return "dark";  //When the dark theme is active
    else
      return "light";  //When the light theme is active
  else if (var == "THEME-COLOR")  //Replace theme color in html page
    if (dark_mode)
      return "#1a344d";  //When the dark theme is active
    else
      return "#66cdaa";  //When the light theme is active
  return String();
}

void setup() {  //The codes of this function are executed once when the module is being turned on
  pinMode(BUZZER, OUTPUT);  //Adjusting the buzzer pin in output mode
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
  delay(200);
  Serial.begin(9600);  //Setting up a serial monitor with a baud rate of 9600
  EEPROM.begin(512);  //Setting up EEPROM memory with a maximum of 512 bytes
  Eeprom_read();  //Read stored information from EEPROM memory
  WiFi.softAP(AP_name, AP_pass);  //Setting up the access point
  DateTime.begin();  //Setting up the internal clock and date
  SPIFFS.begin();  //Setting up SPIFFS memory to access the web server files
  dht.begin();  //Setting up the DHT22 temperature and humidity module
  pinMode(HEATER, OUTPUT);  //Adjusting the heater pin in output mode
  pinMode(COOLER, OUTPUT);  //Adjusting the cooler pin in output mode
  pinMode(FOG_MAKER, OUTPUT);  //Adjusting the fog maker pin in output mode
  pinMode(FAN, OUTPUT);  //Adjusting the fan pin in output mode
  pinMode(BULB, OUTPUT);  //Adjusting the bulb pin in output mode
  pinMode(WATER_PUMP, OUTPUT);  //Adjusting the water pump pin in output mode
  pinMode(PHOTOCELL, INPUT);  //Adjusting the photoresistor pin in input mode
  digitalWrite(HEATER, HIGH);  //Turning off the heater pin (active low)
  digitalWrite(COOLER, HIGH);  //Turning off the cooler pin (active low)
  digitalWrite(FOG_MAKER, HIGH);  //Turning off the fog maker pin (active low)
  digitalWrite(FAN, HIGH);  //Turning off the fan pin (active low)
  digitalWrite(BULB, HIGH);  //Turning off the bulb pin (active low)
  digitalWrite(WATER_PUMP, HIGH);  //Turning off the water pump pin (active low)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/' page
    if (request->hasParam("lang")) {  //If new status is requested
      language = request->getParam("lang")->value();
      Eeprom_write();  //Save changes in EEPROM memory
    }
    if (language == "pr")  //If the language of the panel was persian
      request->send(SPIFFS, "/pr/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "en")  //If the language of the panel was english
      request->send(SPIFFS, "/en/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "ar")  //If the language of the panel was arabic
      request->send(SPIFFS, "/ar/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "fr")  //If the language of the panel was french
      request->send(SPIFFS, "/fr/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "ge")  //If the language of the panel was german
      request->send(SPIFFS, "/ge/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "jp")  //If the language of the panel was japanese
      request->send(SPIFFS, "/jp/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "ru")  //If the language of the panel was russian
      request->send(SPIFFS, "/ru/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "tr")  //If the language of the panel was turkish
      request->send(SPIFFS, "/tr/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "sp")  //If the language of the panel was spanish
      request->send(SPIFFS, "/sp/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else if (language == "ch")  //If the language of the panel was chinese
      request->send(SPIFFS, "/ch/status.html", String(), false, processor);  //Display 'status.html' file from SPIFFS memory
    else
      request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/output", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/output' page
    if (language == "pr")  //If the language of the panel was persian
      request->send(SPIFFS, "/pr/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "en")  //If the language of the panel was english
      request->send(SPIFFS, "/en/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "ar")  //If the language of the panel was arabic
      request->send(SPIFFS, "/ar/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "fr")  //If the language of the panel was french
      request->send(SPIFFS, "/fr/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "ge")  //If the language of the panel was german
      request->send(SPIFFS, "/ge/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "jp")  //If the language of the panel was japanese
      request->send(SPIFFS, "/jp/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "ru")  //If the language of the panel was russian
      request->send(SPIFFS, "/ru/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "tr")  //If the language of the panel was turkish
      request->send(SPIFFS, "/tr/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "sp")  //If the language of the panel was spanish  //If the language of the panel was spanish
      request->send(SPIFFS, "/sp/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else if (language == "ch")  //If the language of the panel was chinese
      request->send(SPIFFS, "/ch/output.html", String(), false, processor);  //Display 'output.html' file from SPIFFS memory
    else
      request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/control' page
    if (language == "pr")  //If the language of the panel was persian
      request->send(SPIFFS, "/pr/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "en")  //If the language of the panel was english
      request->send(SPIFFS, "/en/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "ar")  //If the language of the panel was arabic
      request->send(SPIFFS, "/ar/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "fr")  //If the language of the panel was french
      request->send(SPIFFS, "/fr/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "ge")  //If the language of the panel was german
      request->send(SPIFFS, "/ge/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "jp")  //If the language of the panel was japanese
      request->send(SPIFFS, "/jp/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "ru")  //If the language of the panel was russian
      request->send(SPIFFS, "/ru/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "tr")  //If the language of the panel was turkish
      request->send(SPIFFS, "/tr/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "sp")  //If the language of the panel was spanish
      request->send(SPIFFS, "/sp/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else if (language == "ch")  //If the language of the panel was chinese
      request->send(SPIFFS, "/ch/control.html", String(), false, processor);  //Display 'control.html' file from SPIFFS memory
    else
      request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/settings' page
    if (request->hasParam("ap_name") && request->hasParam("ap_pass") && request->hasParam("min_temperature") && request->hasParam("max_temperature") && request->hasParam("min_humidity") && request->hasParam("max_humidity") && request->hasParam("delay_time") && request->hasParam("scale")) {  //When the user wants to change the settings
      if (language == "pr") {  //If the language of the panel was persian
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("نام وای فای نمی تواند خالی باشد.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("حداکثر طول نام شبکه ۱۶ نویسه می باشد.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("حداکثر طول کلمه عبور ۱۶ نویسه می باشد.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("حداقل طول کلمه عبور ۸ نویسه می باشد.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("حداقل دما نمی تواند خالی باشد.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("حداکثر دما نمی تواند خالی باشد.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("حداقل دما نمی تواند از حداکثر دما بیشتر باشد.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("حداقل دما نمی تواند از ۳۰۰ درجه بیشتر باشد.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("حداکثر دما نمی تواند از ۳۰۰ درجه بیشتر باشد.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("حداقل دما نمی تواند از ۵۰- درجه کمتر باشد.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("حداکثر دما نمی تواند از ۵۰- درجه کمتر باشد.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("حداقل رطوبت نمی تواند خالی باشد.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("حداکثر رطوبت نمی تواند خالی باشد.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("حداقل طوبت نمی تواند از حداکثر رطوبت بیشتر باشد.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("حداقل رطوبت نمی تواند از ۱۰۰ درجه بیشتر باشد.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("حداکثر رطوبت نمی تواند از ۱۰۰ درجه بیشتر باشد.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("حداقل رطوبت نمی تواند از ۰ درجه کمتر باشد.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("حداکثر رطوبت نمی تواند از ۰ درجه کمتر باشد.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_humidity != request->getParam("min_humidity")->value() || set_max_humidity != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("تغییرات با موفقیت ذخیره شد.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "en") {  //If the language of the panel was english
        if (request->getParam("ap_name")->value() == "") //When the access point name was empty
          request->send_P(200, "text/plain", String("Wifi name cannot be empty.").c_str()); //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16) //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("The maximum length of the network name is 16 characters.").c_str()); //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16) //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("The maximum length of the password is 16 characters.").c_str()); //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0) //When the The number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("The minimum length of the password is 8 characters.").c_str()); //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "") //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("Minimum temperature cannot be empty.").c_str()); //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "") //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("Maximum temperature cannot be empty.").c_str()); //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt()) //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("The minimum temperature cannot exceed the maximum temperature.").c_str()); //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300) //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("The minimum temperature cannot be more than 300 degrees.").c_str()); // Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300) //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("The maximum temperature cannot exceed 300 degrees.").c_str()); // Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50) //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Minimum temperature cannot be lower than -50 degrees.").c_str()); // Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50) //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("The maximum temperature cannot be lower than -50 degrees.").c_str()); // Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "") //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("Minimum humidity cannot be empty.").c_str()); //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "") //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("Maximum humidity cannot be empty.").c_str()); //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt()) //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("The minimum temperature can be greater than the maximum humidity.").c_str()); //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100) //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("The minimum humidity cannot be more than 100 degrees.").c_str()); // Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100) //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("The maximum humidity cannot exceed 100 degrees.").c_str()); // Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0) //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Minimum humidity cannot be less than 0 degree.").c_str()); // Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0) //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Maximum humidity cannot be lower than 0 degree.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Changes saved successfully.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "ar") {  //If the language of the panel was arabic
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("الاسم وای فای لا يمكن أن يكون فارغًا.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("الحد الأقصى لطول اسم الشبكة هو 16 حرفًا.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("الحد الأقصى لطول كلمة المرور هو 16 حرفًا.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("الحد الأدنى لطول كلمة المرور هو 8 أحرف.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة الدنيا فارغة.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة القصوى فارغة.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة الدنيا أكبر من درجة الحرارة القصوى.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة الدنيا أكبر من 300 درجة.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة القصوى أكبر من 300 درجة.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة الدنيا أقل من -50 درجة.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("لا يمكن أن تكون درجة الحرارة القصوى أقل من -50 درجة.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة الدنيا فارغة.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة القصوى فارغة.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة الدنيا أكبر من الرطوبة القصوى.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة الدنيا أكبر من 100 درجة.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة القصوى أكبر من 100 درجة.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة الدنيا أقل من 0 درجة.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("لا يمكن أن تكون الرطوبة القصوى أقل من 0 درجة.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("تم حفظ التغييرات بنجاح.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "fr") {  //If the language of the panel was french
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("Le nom du point d'accès ne peut pas être vide.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("La longueur maximale du nom du réseau est de 16 caractères.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("La longueur maximale du mot de passe est de 16 caractères.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("La longueur minimale du mot de passe est de 8 caractères.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("La température minimale ne peut pas être vide.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("La température maximale ne peut pas être vide.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("La température minimale ne peut pas être supérieure à la température maximale.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("La température minimale ne peut pas être supérieure à 300 degrés.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("La température maximale ne peut pas être supérieure à 300 degrés.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("La température minimale ne peut pas être inférieure à -50 degrés.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("La température maximale ne peut pas être inférieure à -50 degrés.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("L'humidité minimale ne peut pas être vide.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("L'humidité maximale ne peut pas être vide.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("L'humidité minimale ne peut pas être supérieure à l'humidité maximale.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("L'humidité minimale ne peut pas être supérieure à 100 %.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("L'humidité maximale ne peut pas être supérieure à 100 %.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("L'humidité minimale ne peut pas être inférieure à 0 %.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("L'humidité maximale ne peut pas être inférieure à 0 %.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Modifications enregistrées avec succès.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "ch") {  //If the language of the panel was chinese
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("无线接入点名称不能为空。").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("网络名称的最大长度为16个字符。").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("密码的最大长度为16个字符。").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("密码的最小长度为8个字符。").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("最低温度不能为空。").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("最高温度不能为空。").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("最低温度不能高于最高温度。").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("最低温度不能超过300度。").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("最高温度不能超过300度。").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("最低温度不能低于-50度。").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("最高温度不能低于-50度。").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("最低湿度不能为空。").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("最高湿度不能为空。").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("最低湿度不能高于最高湿度。").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("最低湿度不能超过100%。").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("最高湿度不能超过100%。").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("最低湿度不能低于0%。").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("最高湿度不能低于0%。").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("更改已成功保存。").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "ge") {  //If the language of the panel was german
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("Der WLAN-Name darf nicht leer sein.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("Die maximale Länge des Netzwerknamens beträgt 16 Zeichen.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("Die maximale Länge des Passworts beträgt 16 Zeichen.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("Die Mindestlänge des Passworts beträgt 8 Zeichen.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("Die Mindesttemperatur darf nicht leer sein.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("Die Höchsttemperatur darf nicht leer sein.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("Die Mindesttemperatur darf nicht höher als die Höchsttemperatur sein.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Die Mindesttemperatur darf 300 Grad nicht überschreiten.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Die Höchsttemperatur darf 300 Grad nicht überschreiten.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Die Mindesttemperatur darf nicht unter -50 Grad liegen.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Die Höchsttemperatur darf nicht unter -50 Grad liegen.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("Die Mindestfeuchtigkeit darf nicht leer sein.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("Die Höchstfeuchtigkeit darf nicht leer sein.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("Die Mindestfeuchtigkeit darf nicht höher als die Höchstfeuchtigkeit sein.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Die Mindestfeuchtigkeit darf 100% nicht überschreiten.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Die Höchstfeuchtigkeit darf 100% nicht überschreiten.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Die Mindestfeuchtigkeit darf nicht unter 0% liegen.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Die Höchstfeuchtigkeit darf nicht unter 0% liegen.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Änderungen erfolgreich gespeichert.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "jp") {  //If the language of the panel was japanese
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("ワイヤレス名が空です。").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("ネットワーク名の最大長は16文字です。").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("パスワードの最大長は16文字です。").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("パスワードの最小長は8文字です。").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("最小温度は空であってはなりません。").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("最大温度は空であってはなりません。").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("最小温度は最大温度より高くてはなりません。").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("最小温度は300度を超えてはいけません。").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("最大温度は300度を超えてはいけません。").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("最小温度は-50度未満であってはなりません。").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("最大温度は-50度未満であってはなりません。").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("最小湿度は空であってはなりません。").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("最大湿度は空であってはなりません。").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("最小湿度は最大湿度より高くてはなりません。").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("最小湿度は100%を超えてはいけません。").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("最大湿度は100%を超えてはいけません。").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("最小湿度は0%未満であってはなりません。").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("最大湿度は0%未満であってはなりません。").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("変更は正常に保存されました。").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "ru") {  //If the language of the panel was russian
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("Имя Wi-Fi не может быть пустым.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("Максимальная длина имени сети составляет 16 символов.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("Максимальная длина пароля составляет 16 символов.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("Минимальная длина пароля составляет 8 символов.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("Минимальная температура не может быть пустой.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("Максимальная температура не может быть пустой.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("Минимальная температура не может быть больше максимальной температуры.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Минимальная температура не может превышать 300 градусов.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Максимальная температура не может превышать 300 градусов.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Минимальная температура не может быть ниже -50 градусов.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Максимальная температура не может быть ниже -50 градусов.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("Минимальная влажность не может быть пустой.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("Максимальная влажность не может быть пустой.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("Минимальная влажность не может быть выше максимальной влажности.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Минимальная влажность не может превышать 100%.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Максимальная влажность не может превышать 100%.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Минимальная влажность не может быть ниже 0%.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Максимальная влажность не может быть ниже 0%.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Изменения успешно сохранены.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "sp") {  //If the language of the panel was spanish
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("El nombre del Wi-Fi no puede estar vacío.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("La longitud máxima del nombre de la red es de 16 caracteres.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("La longitud máxima de la contraseña es de 16 caracteres.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("La longitud mínima de la contraseña es de 8 caracteres.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("La temperatura mínima no puede estar vacía.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("La temperatura máxima no puede estar vacía.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("La temperatura mínima no puede ser mayor que la temperatura máxima.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("La temperatura mínima no puede ser mayor de 300 grados.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("La temperatura máxima no puede ser mayor de 300 grados.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("La temperatura mínima no puede ser inferior a -50 grados.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("La temperatura máxima no puede ser inferior a -50 grados.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("La humedad mínima no puede estar vacía.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("La humedad máxima no puede estar vacía.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("La humedad mínima no puede ser mayor que la humedad máxima.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("La humedad mínima no puede ser mayor del 100%.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("La humedad máxima no puede ser mayor del 100%.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("La humedad mínima no puede ser inferior al 0%.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("La humedad máxima no puede ser inferior al 0%.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Los cambios se guardaron correctamente.").c_str());  //Display the message when changes are successful
        }
      }
      if (language == "tr") {  //If the language of the panel was turkish
        if (request->getParam("ap_name")->value() == "")  //When the access point name was empty
          request->send_P(200, "text/plain", String("Wi-Fi adı boş olamaz.").c_str());  //Display error the access point name is empty
        else if (request->getParam("ap_name")->value().length() > 16)  //When the number of characters in the access point name is more than 16 characters
          request->send_P(200, "text/plain", String("Ağ adının maksimum uzunluğu 16 karakterdir.").c_str());  //Display error message when there are too many characters of access point name
        else if (request->getParam("ap_pass")->value().length() > 16)  //When the number of characters in the access point password is more than 16 characters
          request->send_P(200, "text/plain", String("Şifre için maksimum uzunluk 16 karakterdir.").c_str());  //Display error message when there are too many characters of access point password
        else if (request->getParam("ap_pass")->value().length() < 8 && request->getParam("ap_pass")->value().length() != 0)  //When the number of characters of the access point password is less than 8 characters
          request->send_P(200, "text/plain", String("Şifrenin minimum uzunluğu 8 karakterdir.").c_str());  //Display the error message when access point password is short
        else if (request->getParam("min_temperature")->value() == "")  //When the minimum temperature field is empty
          request->send_P(200, "text/plain", String("Minimum sıcaklık boş olamaz.").c_str());  //Display error the minimum temperature is empty
        else if (request->getParam("max_temperature")->value() == "")  //When the maximum temperature field is empty
          request->send_P(200, "text/plain", String("Maksimum sıcaklık boş olamaz.").c_str());  //Display error the maximum temperature is empty
        else if (request->getParam("min_temperature")->value().toInt() > request->getParam("max_temperature")->value().toInt())  //If the minimum temperature is greater than the maximum temperature
          request->send_P(200, "text/plain", String("Minimum sıcaklık maksimum sıcaklıktan büyük olamaz.").c_str());  //Display error the minimum temperature is higher than the maximum temperature
        else if (request->getParam("min_temperature")->value().toInt() > 300)  //When the minimum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Minimum sıcaklık 300 dereceden fazla olamaz.").c_str());  //Display of minimum temperature error is more than 300 degrees
        else if (request->getParam("max_temperature")->value().toInt() > 300)  //When the maximum temperature is more than 300 degrees
          request->send_P(200, "text/plain", String("Maksimum sıcaklık 300 dereceden fazla olamaz.").c_str());  //Display of maximum temperature error is more than 300 degrees
        else if (request->getParam("min_temperature")->value().toInt() < -50)  //When the minimum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Minimum sıcaklık -50 dereceden az olamaz.").c_str());  //Display of minimum temperature error is less than -50 degrees
        else if (request->getParam("max_temperature")->value().toInt() < -50)  //When the maximum temperature is less than -50 degrees
          request->send_P(200, "text/plain", String("Maksimum sıcaklık -50 dereceden az olamaz.").c_str());  //Display of maximum temperature error is less than -50 degrees
        else if (request->getParam("min_humidity")->value() == "")  //When the minimum humidity field is empty
          request->send_P(200, "text/plain", String("Minimum nem boş olamaz.").c_str());  //Display error the minimum humidity is empty
        else if (request->getParam("max_humidity")->value() == "")  //When the maximum humidity field is empty
          request->send_P(200, "text/plain", String("Maksimum nem boş olamaz.").c_str());  //Display error the maximum humidity is empty
        else if (request->getParam("min_humidity")->value().toInt() > request->getParam("max_humidity")->value().toInt())  //If the minimum humidity is greater than the maximum humidity
          request->send_P(200, "text/plain", String("Minimum nem maksimum nemden büyük olamaz.").c_str());  //Display error the minimum humidity is higher than the maximum humidity
        else if (request->getParam("min_humidity")->value().toInt() > 100)  //When the minimum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Minimum nem %100'den fazla olamaz.").c_str());  //Display of minimum humidity error is more than 100 percent
        else if (request->getParam("max_humidity")->value().toInt() > 100)  //When the maximum humidity is more than 100 percent
          request->send_P(200, "text/plain", String("Maksimum nem %100'den fazla olamaz.").c_str());  //Display of maximum humidity error is more than 100 percent
        else if (request->getParam("min_humidity")->value().toInt() < 0)  //When the minimum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Minimum nem %0'dan az olamaz.").c_str());  //Display of minimum humidity error is less than 0 percent
        else if (request->getParam("max_humidity")->value().toInt() < 0)  //When the maximum humidity is less than 0 percent
          request->send_P(200, "text/plain", String("Maksimum nem %0'dan az olamaz.").c_str());  //Display of maximum humidity error is less than 0 percent
        else {
          if (AP_name != request->getParam("ap_name")->value() || AP_pass != request->getParam("ap_pass")->value()) {  //When the name or password access point  have been changed
            AP_name = request->getParam("ap_name")->value();  //Storage the new access point name
            AP_pass = request->getParam("ap_pass")->value();  //Storage the new access point password
            Eeprom_write();  //Saving new data in EEPROM memory
            ap_change = true;  //Adjust again access point
          }
          if (set_min_temperature != request->getParam("min_temperature")->value() || set_max_temperature != request->getParam("max_temperature")->value()) {  //When the set minimum and maximum temperature values ​​were changed
            set_min_temperature = request->getParam("min_temperature")->value();  //Storage the new minimum temperature value
            set_max_temperature = request->getParam("max_temperature")->value();  //Storage the new maximum temperature value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (set_min_temperature != request->getParam("min_humidity")->value() || set_max_temperature != request->getParam("max_humidity")->value()) {  //When the set minimum and maximum humidity values ​​were changed
            set_min_humidity = request->getParam("min_humidity")->value();  //Storage the new minimum humidity value
            set_max_humidity = request->getParam("max_humidity")->value();  //Storage the new maximum humidity value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (delay_time != request->getParam("delay_time")->value()) {  //When the delay time values ​​were changed
            delay_time = request->getParam("delay_time")->value();  //Storage the new delay time value
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          if (scale != request->getParam("scale")->value()) {  //When the temperature scale values ​​were changed
            scale = request->getParam("scale")->value();  //Storage the new temperature scale name
            min_temperature = 300;
            max_temperature = -50;
            Eeprom_write();  //Saving new data in EEPROM memory
          }
          request->send_P(200, "text/plain", String("Değişiklikler başarıyla kaydedildi.").c_str());  //Display the message when changes are successful
        }
      }
    }
    else  //When the user does not intend to make changes
      if (language == "pr")  //If the language of the panel was persian
        request->send(SPIFFS, "/pr/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "en")  //If the language of the panel was english
        request->send(SPIFFS, "/en/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "ar")  //If the language of the panel was arabic
        request->send(SPIFFS, "/ar/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "fr")  //If the language of the panel was french
        request->send(SPIFFS, "/fr/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "ge")  //If the language of the panel was german
        request->send(SPIFFS, "/ge/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "jp")  //If the language of the panel was japanese
        request->send(SPIFFS, "/jp/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "ru")  //If the language of the panel was russian
        request->send(SPIFFS, "/ru/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "tr")  //If the language of the panel was turkish
        request->send(SPIFFS, "/tr/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "sp")  //If the language of the panel was spanish
        request->send(SPIFFS, "/sp/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else if (language == "ch")  //If the language of the panel was chinese
        request->send(SPIFFS, "/ch/settings.html", String(), false, processor);  //Display 'settings.html' file from SPIFFS memory
      else
        request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/language", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/language' page
    request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/about", HTTP_GET, [](AsyncWebServerRequest * request) {  //When the user requests the '/about' page
    if (language == "pr")  //If the language of the panel was persian
      request->send(SPIFFS, "/pr/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "en")  //If the language of the panel was english
      request->send(SPIFFS, "/en/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "ar")  //If the language of the panel was arabic
      request->send(SPIFFS, "/ar/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "fr")  //If the language of the panel was french
      request->send(SPIFFS, "/fr/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "ge")  //If the language of the panel was german
      request->send(SPIFFS, "/ge/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "jp")  //If the language of the panel was japanese
      request->send(SPIFFS, "/jp/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "ru")  //If the language of the panel was russian
      request->send(SPIFFS, "/ru/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "tr")  //If the language of the panel was turkish
      request->send(SPIFFS, "/tr/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "sp")  //If the language of the panel was spanish
      request->send(SPIFFS, "/sp/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else if (language == "ch")  //If the language of the panel was chinese
      request->send(SPIFFS, "/ch/about.html", String(), false, processor);  //Display 'about.html' file from SPIFFS memory
    else
      request->send(SPIFFS, "/language.html", String(), false, processor);  //Display 'language.html' file from SPIFFS memory
  });
  server.on("/assets/image/logo.png", HTTP_GET, [](AsyncWebServerRequest * request) { //When the user requests the logo image
    request->send(SPIFFS, "/assets/image/logo.png", "image/png");  //Display 'logo.png' file from SPIFFS memory
  });
  server.on("/ap_name", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request access point name value
    request->send_P(200, "text/plain", String(AP_name).c_str());  //Send access point name value
  });
  server.on("/ap_pass", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request access point password value
    request->send_P(200, "text/plain", String(AP_pass).c_str());  //Send access point password value
  });
  server.on("/set_min_temperature", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request set minimum temperature value
    request->send_P(200, "text/plain", String(set_min_temperature).c_str());  //Send set minimum temperature value
  });
  server.on("/set_max_temperature", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request set maximum temperature value
    request->send_P(200, "text/plain", String(set_max_temperature).c_str());  //Send set maximum temperature value
  });
  server.on("/set_min_humidity", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request set minimum humidity value
    request->send_P(200, "text/plain", String(set_min_humidity).c_str());  //Send set minimum humidity value
  });
  server.on("/set_max_humidity", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request set maximum humidity value
    request->send_P(200, "text/plain", String(set_max_humidity).c_str());  //Send set maximum humidity value
  });
  server.on("/delay_time", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request delay time value
    request->send_P(200, "text/plain", String(delay_time).c_str());  //Send delay time value
  });
  server.on("/scale", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request temperature scale name
    request->send_P(200, "text/plain", String(scale).c_str());  //Send temperature scale name
  });
  server.on("/heater", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request heater status (on or off)
    if (heater)  //If the heater was on
      request->send_P(200, "text/plain", String("on").c_str());  //Send status on
    else
      request->send_P(200, "text/plain", String("off").c_str());  //Send status off
  });
  server.on("/cooler", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request cooler status (on or off)
    if (cooler)  //If the cooler was on
      request->send_P(200, "text/plain", String("on").c_str());  //Send status on
    else
      request->send_P(200, "text/plain", String("off").c_str());  //Send status off
  });
  server.on("/fog_maker", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request fog maker status (on or off)
    if (fog_maker)  //If the fog maker was on
      request->send_P(200, "text/plain", String("on").c_str());  //Send status on
    else
      request->send_P(200, "text/plain", String("off").c_str());  //Send status off
  });
  server.on("/fan", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request fan status (on or off)
    if (fan)  //If the fan was on
      request->send_P(200, "text/plain", String("on").c_str());  //Send status on
    else
      request->send_P(200, "text/plain", String("off").c_str());  //Send status off
  });
  server.on("/bulb", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request and change bulb status (on or off)
    if (request->hasParam("status")) {  //If new status is requested
      if (request->getParam("status")->value() == "on") {  //If on status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        digitalWrite(BULB, LOW);  //Turning on the bulb pin (active low)
        bulb = true;  //Storage the new state of the lamp (on or off)
        Serial.println("bulb => ON");
      }
      else if (request->getParam("status")->value() == "off") {  //If off status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        digitalWrite(BULB, HIGH);  //Turning off the bulb pin (active low)
        bulb = false;  //Storage the new state of the lamp (on or off)
        Serial.println("bulb => OFF");
      }
    }
    else {
      if (bulb)  //If the lamp was on
        request->send_P(200, "text/plain", String("on").c_str());  //Send status on
      else
        request->send_P(200, "text/plain", String("off").c_str());  //Send status off
    }
  });
  server.on("/water_pump", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request and change water pump status (on or off)
    if (request->hasParam("status")) {  //If new status is requested
      if (request->getParam("status")->value() == "on") {  //If on status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        digitalWrite(WATER_PUMP, LOW);  //Turning on the water pump pin (active low)
        water_pump = true;  //Storage the new state of the water pump (on or off)
        Serial.println("water_pump => ON");
      }
      else if (request->getParam("status")->value() == "off") {  //If off status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        digitalWrite(WATER_PUMP, HIGH);  //Turning off the water pump pin (active low)
        water_pump = false;  //Storage the new state of the water pump (on or off)
        Serial.println("water_pump => OFF");
      }
    }
    else {
      if (water_pump)  //If the water pump was on
        request->send_P(200, "text/plain", String("on").c_str());  //Send status on
      else
        request->send_P(200, "text/plain", String("off").c_str());  //Send status off
    }
  });
  server.on("/dark_mode", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request and change dark mode status (on or off)
    if (request->hasParam("status")) {  //If new status is requested
      if (request->getParam("status")->value() == "on") {  //If on status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        dark_mode = true;  //Storage the new state of the dark mode (on or off)
      }
      else if (request->getParam("status")->value() == "off") {  //If off status is requested
        request->send_P(200, "text/plain", String("ok").c_str());  //Display success message
        dark_mode = false;  //Storage the new state of the dark mode (on or off)
      }
    }
    else {
      if (dark_mode)  //If the dark mode was on
        request->send_P(200, "text/plain", String("on").c_str());  //Send status on
      else
        request->send_P(200, "text/plain", String("off").c_str());  //Send status off
    }
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request temperature value
    if (temperature > 300 || temperature < -50)  //If the temperature and humidity sensor is not connected
      request->send_P(200, "text/plain", String("--").c_str());  //Display unknown value for temperature
    else if (scale == "celsius")  //If the temperature scale is celsius
      request->send_P(200, "text/plain", String(String(temperature) + ".C").c_str());  //Send temperature value based on celsius scale
    else if (scale == "fahrenheit")  //If the temperature scale is fahrenheit
      request->send_P(200, "text/plain", String(String(temperature) + ".F").c_str());  //Send temperature value based on fahrenheit scale
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request humidity value
    if (humidity > 100 || humidity < 0)  //If the temperature and humidity sensor is not connected
      request->send_P(200, "text/plain", String("--").c_str());  //Display unknown value for humidity
    else
      request->send_P(200, "text/plain", String(String(humidity) + "%").c_str());  //Send humidity value based on percentage
  });
  server.on("/error", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request humidity value
    if (error_time.toInt() <= heater_error && temperature < set_min_temperature.toInt() - 3) {
      if (language == "pr")
        request->send_P(200, "text/plain", String("اختلال در عملکرد دستگاه بخاری.").c_str());
      else if (language == "en")
        request->send_P(200, "text/plain", String("Malfunction of the heater.").c_str());
      else if (language == "ar")
        request->send_P(200, "text/plain", String("عطل في السخان.").c_str());
      else if (language == "ch")
        request->send_P(200, "text/plain", String("加热器故障。").c_str());
      else if (language == "fr")
        request->send_P(200, "text/plain", String("Dysfonctionnement du chauffage.").c_str());
      else if (language == "ge")
        request->send_P(200, "text/plain", String("Fehlfunktion der Heizung.").c_str());
      else if (language == "jp")
        request->send_P(200, "text/plain", String("ヒーターの故障。").c_str());
      else if (language == "ru")
        request->send_P(200, "text/plain", String("Неисправность отопителя.").c_str());
      else if (language == "sp")
        request->send_P(200, "text/plain", String("Mal funcionamiento del calentador.").c_str());
      else if (language == "tr")
        request->send_P(200, "text/plain", String("Isıtıcının arızası.").c_str());
    }
    else if (error_time.toInt() <= cooler_error && temperature > set_max_temperature.toInt() + 3) {
      if (language == "pr")
        request->send_P(200, "text/plain", String("اختلال در عملکرد دستگاه کولر.").c_str());
      else if (language == "en")
        request->send_P(200, "text/plain", String("Malfunction of the cooler.").c_str());
      else if (language == "ar")
        request->send_P(200, "text/plain", String("عطل في المكيف.").c_str());
      else if (language == "ch")
        request->send_P(200, "text/plain", String("冷却器故障。").c_str());
      else if (language == "fr")
        request->send_P(200, "text/plain", String("Dysfonctionnement du climatiseur.").c_str());
      else if (language == "ge")
        request->send_P(200, "text/plain", String("Fehlfunktion des Kühlers.").c_str());
      else if (language == "jp")
        request->send_P(200, "text/plain", String("クーラーの故障。").c_str());
      else if (language == "ru")
        request->send_P(200, "text/plain", String("Неисправность охладителя.").c_str());
      else if (language == "sp")
        request->send_P(200, "text/plain", String("Mal funcionamiento del enfriador.").c_str());
      else if (language == "tr")
        request->send_P(200, "text/plain", String("Klimanın arızası.").c_str());
    }
    else if (error_time.toInt() <= fog_maker_error && humidity < set_min_humidity.toInt() - 5) {
      if (language == "pr")
        request->send_P(200, "text/plain", String("اختلال در عملکرد دستگاه مه ساز.").c_str());
      else if (language == "en")
        request->send_P(200, "text/plain", String("Malfunction of the fog maker.").c_str());
      else if (language == "ar")
        request->send_P(200, "text/plain", String("عطل في آلة الضباب.").c_str());
      else if (language == "ch")
        request->send_P(200, "text/plain", String("雾机故障。").c_str());
      else if (language == "fr")
        request->send_P(200, "text/plain", String("Dysfonctionnement de la machine à brouillard.").c_str());
      else if (language == "ge")
        request->send_P(200, "text/plain", String("Fehlfunktion der Nebelmaschine.").c_str());
      else if (language == "jp")
        request->send_P(200, "text/plain", String("フォグマシンの故障。").c_str());
      else if (language == "ru")
        request->send_P(200, "text/plain", String("Неисправность машины для создания тумана.").c_str());
      else if (language == "sp")
        request->send_P(200, "text/plain", String("Mal funcionamiento de la máquina de niebla.").c_str());
      else if (language == "tr")
        request->send_P(200, "text/plain", String("Buhar makinesinin arızası.").c_str());
    }
    else if (error_time.toInt() <= fan_error && humidity > set_max_humidity.toInt() + 5) {
      if (language == "pr")
        request->send_P(200, "text/plain", String("اختلال در عملکرد دستگاه فن.").c_str());
      else if (language == "en")
        request->send_P(200, "text/plain", String("Malfunction of the fan.").c_str());
      else if (language == "ar")
        request->send_P(200, "text/plain", String("عطل في المروحة.").c_str());
      else if (language == "ch")
        request->send_P(200, "text/plain", String("风扇故障。").c_str());
      else if (language == "fr")
        request->send_P(200, "text/plain", String("Dysfonctionnement du ventilateur.").c_str());
      else if (language == "ge")
        request->send_P(200, "text/plain", String("Fehlfunktion des Ventilators.").c_str());
      else if (language == "jp")
        request->send_P(200, "text/plain", String("ファンの故障。").c_str());
      else if (language == "ru")
        request->send_P(200, "text/plain", String("Неисправность вентилятора.").c_str());
      else if (language == "sp")
        request->send_P(200, "text/plain", String("Mal funcionamiento del ventilador.").c_str());
      else if (language == "tr")
        request->send_P(200, "text/plain", String("Fanın arızası.").c_str());
    }
    else
      request->send_P(200, "text/plain", String("").c_str());
  });
  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request brightness value based on percentage
    if (brightness > 100)  //If the photoresistor is not connected
      request->send_P(200, "text/plain", String("--").c_str());  //Display unknown value for brightness
    else
      request->send_P(200, "text/plain", String(String(brightness) + "%").c_str());  //Send brightness value
  });
  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request uptime value
    if (uptime < 0)  //If the uptime value is not valid
      request->send_P(200, "text/plain", String("--").c_str());  //Display unknown value for uptime
    else
      request->send_P(200, "text/plain", String(String(uptime) + "H").c_str());  //Send uptime based on hour
  });
  server.on("/min_max_temperature", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request minimum and maximum temperature value
    if (min_temperature == 300 || max_temperature == -50)  //If the minimum or maximum temperature values ​​not valid
      request->send_P(200, "text/plain", String("-- &nbsp --").c_str());  //Display unknown value for minimum and maximum temperature
    else if (scale == "celsius")  //If the temperature scale is celsius
      request->send_P(200, "text/plain", String(String(String(min_temperature) + ".C &nbsp ") + String(String(max_temperature) + ".C")).c_str());  //Send minimum and maximum temperature values based on celsius scale
    else if (scale == "fahrenheit")  //If the temperature scale is fahrenheit
      request->send_P(200, "text/plain", String(String(String(min_temperature) + ".F &nbsp ") + String(String(max_temperature) + ".F")).c_str());  //Send minimum and maximum temperature values based on fahrenheit scale
  });
  server.on("/min_max_humidity", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request minimum and maximum humidity value
    if (min_humidity == 100 || max_humidity == 0)  //If the minimum or maximum humidity values ​​not valid
      request->send_P(200, "text/plain", String("-- &nbsp --").c_str());  //Display unknown value for minimum and maximum humidity
    else
      request->send_P(200, "text/plain", String(String(String(min_humidity) + "% &nbsp ") + String(String(max_humidity) + "%")).c_str());  //Send minimum and maximum humidity value based on percentage
  });
  server.on("/assets/style/bootstrap.css", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request bootstrap style file
    request->send(SPIFFS, "/assets/style/bootstrap.css", "text/css");  //Send 'bootstrap.css' file from SPIFFS memory
  });
  server.on("/assets/style/font.css", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request fonts file
    request->send(SPIFFS, "/assets/style/font.css", "text/css");  //Send 'font.css' file from SPIFFS memory
  });
  server.on("/assets/script/bootstrap.js", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request bootstrap javascript file
    request->send(SPIFFS, "/assets/script/bootstrap.js", "text/css");  //Send 'bootstrap.js' file from SPIFFS memory
  });
  server.on("/assets/font/B-Koodak.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request 'B Koodak' font
    request->send(SPIFFS, "/assets/font/B-Koodak.ttf", "application/x-font-ttf");  //Send 'B-Koodak.ttf' font file from SPIFFS memory
  });
  server.on("/assets/font/7-Segment.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request 'Seven segment' font
    request->send(SPIFFS, "/assets/font/7-Segment.ttf", "application/x-font-ttf");  //Send '7-Segment.ttf' font file from SPIFFS memory
  });
  server.on("/assets/font/Nunito.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {  //Request 'Nunito' font
    request->send(SPIFFS, "/assets/font/Nunito.ttf", "application/x-font-ttf");  //Send 'Nunito.ttf' font file from SPIFFS memory
  });
  server.begin();  //Setting up a async web server
}

void loop() {  //As long as the module is active, the codes inside this function will be executed regularly
  if (scale == "celsius")  //If the temperature scale is celsius
    temperature = (int)dht.readTemperature();  //Get the new temperature in celsius
  else if (scale == "fahrenheit")  //If the temperature scale is fahrenheit
    temperature = (int)dht.readTemperature(true);  //Get the new temperature in fahrenheit
  humidity = (int)dht.readHumidity();  //Get new humidity value and convert to integer
  brightness = map(analogRead(PHOTOCELL), 0, 1023, 0, 100);  //Get new brightness value and convert to percentage
  if (temperature > 300 && humidity > 100) {
    delay(3000);
    if (scale == "celsius")  //If the temperature scale is celsius
      temperature = (int)dht.readTemperature();  //Get the new temperature in celsius
    else if (scale == "fahrenheit")  //If the temperature scale is fahrenheit
      temperature = (int)dht.readTemperature(true);  //Get the new temperature in fahrenheit
    humidity = (int)dht.readHumidity();  //Get new humidity value and convert to integer
    brightness = map(analogRead(PHOTOCELL), 0, 1023, 0, 100);  //Get new brightness value and convert to percentage
    if (temperature > 300 && humidity > 100) {
      Serial.println("Restart the module.");
      ESP.reset();
    }
  }
  DateTimeParts datetime = DateTime.getParts();  //Get internal time and date
  if (old_hour != datetime.getHours()) {  //If an hour has passed
    old_hour = datetime.getHours();  //Update the previous clock value
    uptime++;  //Add an hour to uptime
  }
  if (uptime < 24) {  //If 24 hours have not passed since the module was turned on
    if (temperature < min_temperature && temperature <= 300 && temperature >= -50)  //If the current temperature is lower than the minimum temperature
      min_temperature = temperature;  //Consider the current temperature as the minimum temperature
    if (temperature > max_temperature && temperature <= 300 && temperature >= -50)  //If the current temperature is upper than the maximum temperature
      max_temperature = temperature;  //Consider the current temperature as the maximum temperature
    if (humidity < min_humidity && humidity <= 100 && humidity >= 0)  //If the current humidity is lower than the minimum humidity
      min_humidity = humidity;  //Consider the current humidity as the minimum humidity
    if (humidity > max_humidity && humidity <= 100 && humidity >= 0)  //If the current humidity is upper than the maximum humidity
      max_humidity = humidity;  //Consider the current humidity as the maximum humidity
    old_min_temperature = min_temperature;  //Since 24 hours have not passed, consider the minimum temperature of yesterday the minimum temperature of today
    old_max_temperature = max_temperature;  //Since 24 hours have not passed, consider the maximum temperature of yesterday the maximum temperature of today
    old_min_humidity = min_humidity;  //Since 24 hours have not passed, consider the minimum humidity of yesterday the minimum humidity of today
    old_max_humidity = max_humidity;  //Since 24 hours have not passed, consider the maximum humidity of yesterday the maximum humidity of today
  }
  else if (uptime % 24 == 0 && period == false) {  //If a period of 24 hours has passed
    min_temperature = old_min_temperature;  //Update the minimum temperature to the minimum temperature of the last 24 hours
    max_temperature = old_max_temperature;  //Update the maximum temperature to the maximum temperature of the last 24 hours
    min_humidity = old_min_humidity;  //Update the minimum humidity to the minimum humidity of the last 24 hours
    max_humidity = old_max_humidity;  //Update the maximum humidity to the maximum humidity of the last 24 hours
    old_min_temperature = 300;  //Reset yesterday minimum temperature
    old_max_temperature = -50;  //Reset yesterday maximum temperature
    old_min_humidity = 100;  //Reset yesterday minimum humidity
    old_max_humidity = 0;  //Reset yesterday maximum humidity
    period = true;  //Confirm the passage of a 24-hour period
  }
  else {  //If more than 24 hours have passed since the module was turned on
    if (temperature < old_min_temperature && temperature <= 300 && temperature >= -50)  //If the current temperature is lower than the yesterday minimum temperature
      old_min_temperature = temperature;  //Update yesterday minimum temperature to the current temperature
    if (temperature > old_max_temperature && temperature <= 300 && temperature >= -50)  //If the current temperature is upper than the yesterday maximum temperature
      old_max_temperature = temperature;  //Update yesterday maximum temperature to the current temperature
    if (humidity < old_min_humidity && humidity <= 100 && humidity >= 0)  //If the current humidity is lower than the yesterday minimum humidity
      old_min_humidity = humidity;  //Update yesterday minimum humidity to the current humidity
    if (humidity > old_max_humidity && humidity <= 100 && humidity >= 0)  //If the current humidity is upper than the yesterday maximum humidity
      old_max_humidity = humidity;  //Update yesterday maximum humidity to the current humidity
    if (uptime % 24 != 0 && period)  //If more than a 24-hour period has passed
      period = false;  //Violate the passage of a 24-hour period
  }
  if (ap_change) {  //If an access point update is requested
    WiFi.softAP(AP_name, AP_pass);  //Access point update
    ap_change = false;  //Return the connection point update request to the previous state
  }
  if (Serial.available()) {
    String STR = Serial.readString();
    STR.remove(STR.length() - 1, 1);
    String str = STR;
    str.toLowerCase();
    Serial.print("Golkhune> ");
    Serial.println(STR);
    if (str == "status") {
      Serial.print("Temperature: ");
      if (temperature > 300)
        Serial.println("--");
      else if (scale == "celsius") {
        Serial.print(temperature);
        Serial.println("°C");
      }
      else if (scale == "fahrenheit") {
        Serial.print(temperature);
        Serial.println("°F");
      }
      Serial.print("Humidity: ");
      if (humidity > 100)
        Serial.println("--");
      else {
        Serial.print(humidity);
        Serial.println("%");
      }
      Serial.print("Brightness: ");
      if (brightness > 100)
        Serial.println("--");
      else {
        Serial.print(brightness);
        Serial.println("%");
      }
      Serial.print("Uptime: ");
      if (uptime < 0)
        Serial.println("--");
      else {
        Serial.print(uptime);
        Serial.println("H");
      }
      Serial.print("Minimum temperature: ");
      if (min_temperature == 300)
        Serial.println("--");
      else if (scale == "celsius") {
        Serial.print(min_temperature);
        Serial.println("°C");
      }
      else if (scale == "fahrenheit") {
        Serial.print(min_temperature);
        Serial.println("°F");
      }
      Serial.print("Maximum temperature: ");
      if (max_temperature == -50)
        Serial.println("--");
      else if (scale == "celsius") {
        Serial.print(max_temperature);
        Serial.println("°C");
      }
      else if (scale == "fahrenheit") {
        Serial.print(max_temperature);
        Serial.println("°F");
      }
      Serial.print("Minimum humidity: ");
      if (min_humidity == 100)
        Serial.println("--");
      else {
        Serial.print(min_humidity);
        Serial.println("%");
      }
      Serial.print("Maximum humidity: ");
      if (max_humidity == 0)
        Serial.println("--");
      else {
        Serial.print(max_humidity);
        Serial.println("%");
      }
      Serial.println();
    }
    else if (str == "output") {
      Serial.print("Heater: ");
      if (heater)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("Cooler: ");
      if (cooler)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("Fog_maker: ");
      if (fog_maker)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("Fan: ");
      if (fan)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("Bulb: ");
      if (bulb)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("Water_pump: ");
      if (water_pump)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.println();
    }
    else if (str == "control") {
      Serial.print("bulb => ");
      if (bulb)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.print("water_pump => ");
      if (water_pump)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.println();
    }
    else if (str == "bulb") {
      digitalWrite(BULB, bulb);
      bulb = !bulb;
      Serial.print("bulb => ");
      if (bulb)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.println();
    }
    else if (str == "water_pump") {
      digitalWrite(WATER_PUMP, water_pump);
      water_pump = !water_pump;
      Serial.print("water_pump => ");
      if (water_pump)
        Serial.println("ON");
      else
        Serial.println("OFF");
      Serial.println();
    }
    else if (str == "settings") {
      Serial.print("access_point_name: ");
      Serial.println(AP_name);
      Serial.print("access_point_pass: ");
      Serial.println(AP_pass);
      Serial.print("set_min_temperature: ");
      Serial.println(set_min_temperature);
      Serial.print("set_max_temperature: ");
      Serial.println(set_max_temperature);
      Serial.print("set_min_humidity: ");
      Serial.println(set_min_humidity);
      Serial.print("set_max_humidity: ");
      Serial.println(set_max_humidity);
      Serial.print("delay_time: ");
      Serial.print(delay_time);
      Serial.println("s");
      Serial.print("temperature_scale: ");
      Serial.println(scale);
      Serial.println();
    }
    else if (str.startsWith("access_point_name=")) {
      STR = STR.substring(18);
      if (STR == "") {
        Serial.println("Access point name cannot be empty.");
        Serial.println();
      }
      else if (STR.length() > 16) {
        Serial.println("Access point name cannot be longer than 16 characters.");
        Serial.println();
      }
      else {
        AP_name = STR;
        Eeprom_write();
        Serial.println("Access point name saved successfully.");
        Serial.println();
        ap_change = true;
      }
    }
    else if (str.startsWith("access_point_pass=")) {
      STR = STR.substring(18);
      if (STR.length() < 8 && STR != "") {
        Serial.println("Access point password cannot be less than 8 characters.");
        Serial.println();
      }
      else if (STR.length() > 16) {
        Serial.println("Access point password cannot be longer than 16 characters.");
        Serial.println();
      }
      else {
        AP_pass = STR;
        Eeprom_write();
        Serial.println("Access point password saved successfully.");
        Serial.println();
        ap_change = true;
      }
    }
    else if (str.startsWith("set_min_temperature=")) {
      STR = STR.substring(20);
      if (!Number_check(STR, false)) {
        Serial.println("Please enter a numeric value.");
        Serial.println();
      }
      else if (STR.toInt() > 150) {
        Serial.println("The set minimum temperature should not be longer than 150 degrees.");
        Serial.println();
      }
      else if (STR.toInt() < -50) {
        Serial.println("The set minimum temperature should not be lower than -50 degrees.");
        Serial.println();
      }
      else {
        set_min_temperature = STR;
        Eeprom_write();
        Serial.println("Set minimum temperature saved successfully.");
        Serial.println();
      }
    }
    else if (str.startsWith("set_max_temperature=")) {
      STR = STR.substring(20);
      if (!Number_check(STR, false)) {
        Serial.println("Please enter a numeric value.");
        Serial.println();
      }
      else if (STR.toInt() > 150) {
        Serial.println("The set maximum temperature should not be longer than 150 degrees.");
        Serial.println();
      }
      else if (STR.toInt() < -50) {
        Serial.println("The set maximum temperature should not be lower than -50 degrees.");
        Serial.println();
      }
      else {
        set_max_temperature = STR;
        Eeprom_write();
        Serial.println("Set maximum temperature saved successfully.");
        Serial.println();
      }
    }
    else if (str.startsWith("set_min_humidity=")) {
      STR = STR.substring(17);
      if (!Number_check(STR, true)) {
        Serial.println("Please enter a numeric value.");
        Serial.println();
      }
      else if (STR.toInt() > 100) {
        Serial.println("The set minimum humidity cannot be longer than 100 percentage.");
        Serial.println();
      }
      else {
        set_min_humidity = STR;
        Eeprom_write();
        Serial.println("Set minimum humidity saved successfully.");
        Serial.println();
      }
    }
    else if (str.startsWith("set_max_humidity=")) {
      STR = STR.substring(17);
      if (!Number_check(STR, true)) {
        Serial.println("Please enter a numeric value.");
        Serial.println();
      }
      else if (STR.toInt() > 100) {
        Serial.println("The set maximum humidity cannot be longer than 100 percentage.");
        Serial.println();
      }
      else {
        set_max_humidity = STR;
        Eeprom_write();
        Serial.println("Set maximum humidity saved successfully.");
        Serial.println();
      }
    }
    else if (str.startsWith("delay_time=")) {
      STR = STR.substring(11);
      if (!Number_check(STR, true)) {
        Serial.println("Please enter a numeric value.");
        Serial.println();
      }
      else if (STR != "0" && STR != "5" && STR != "10" && STR != "15" && STR != "20") {
        Serial.println("The delay time can be 0, 5, 10, 15, or 20 seconds");
        Serial.println();
      }
      else {
        delay_time = STR;
        Eeprom_write();
        Serial.println("Delay time saved successfully.");
        Serial.println();
      }
    }
    else if (str.startsWith("temperature_scale=")) {
      str = str.substring(18);
      if (str != "celsius" && str != "fahrenheit") {
        Serial.println("The temperature scale can only be celsius or fahrenheit.");
        Serial.println();
      }
      else {
        scale = str;
        min_temperature = 300;
        max_temperature = -50;
        Eeprom_write();
        Serial.println("Temperature scale saved successfully.");
        Serial.println();
      }
    }
    else if (str == "about") {
      Serial.println("Program name: Golkhune");
      Serial.println("Version number: 1");
      Serial.println("Publication date: 09/15/2024");
      Serial.println("Developer: Alireza Moshfeghi");
      Serial.println("Email: moshfeghi79@gmail.com");
      Serial.println("Github: https://github.com/alireza-moshfeghi/Golkhune_ESP8266");
      Serial.println();
    }
    else if (str == "help") {
      Serial.println("status - Display temperature, humidity, brightness and other values.");
      Serial.println("output - Display the status of the outputs (on or off).");
      Serial.println("control - Display the status of the bulb and water pump (on or off).");
      Serial.println("bulb - Turn the bulb on or off.");
      Serial.println("water_pump - Turn the water pump on or off.");
      Serial.println("settings - Show applied settings.");
      Serial.println("access_point_name=... - Set new name for the access point.");
      Serial.println("access_point_pass=... - Set new password for the access point.");
      Serial.println("set_min_temperature=... - Set the minimum greenhouse temperature.");
      Serial.println("set_max_temperature=... - Set the maximum greenhouse temperature.");
      Serial.println("set_min_humidity=... - Set the minimum greenhouse humidity.");
      Serial.println("set_max_humidity=... - Set the maximum greenhouse humidity.");
      Serial.println("delay_time=... - Set the delay time for starting the outputs.");
      Serial.println("temperature_scale=... - Set the temperature measurement scale.");
      Serial.println("about - Description of the program.");
      Serial.println();
    }
    else {
      Serial.println("The entered command is not valid.");
      Serial.println("Enter the 'help' command.");
      Serial.println();
    }
  }
  if (temperature < set_min_temperature.toInt() && !heater) {  //If the temperature is lower than the set value
    if (set_min_temperature != "0" && set_max_temperature != "0" && temperature <= 300)
    {
      if (heater_timer == -1)  //If the timer is off
        heater_timer = delay_time.toInt();  //Enable delay timer
      else if (heater_timer == 0) {  //If the delay time has elapsed
        digitalWrite(HEATER, LOW);  //Turn on the heater (active low)
        heater_timer = -1;  //Stop the timer
        heater = true;  //Set the heater status variable to active
        Serial.println("heater => ON");
      }
      else
        heater_timer = heater_timer - 1;  //Decrease the timer
    }
  }
  else if (temperature >= set_min_temperature.toInt() && heater) {  //If the heater was on
    if (temperature <= 300)
    {
      if (heater_timer == -1)  //If the timer is off
        heater_timer = delay_time.toInt();  //Enable delay timer
      if (heater_timer == 0) {  //If the delay time has elapsed
        digitalWrite(HEATER, HIGH);  //Turn off the heater (active low)
        heater_timer = -1;  //Stop the timer
        heater = false;  //Set the heater status variable to deactivate
        Serial.println("heater => OFF");
      }
      else
        heater_timer = heater_timer - 1;  //Decrease the timer
    }
  }
  else if (heater_timer != -1)  //If the timer was active
    heater_timer = -1;  //Stop the timer
  if (temperature > set_max_temperature.toInt() && !cooler) {  //If the temperature is upper than the set value
    if (set_min_temperature != "0" && set_max_temperature != "0" && temperature <= 300)
    {
      if (cooler_timer == -1)  //If the timer is off
        cooler_timer = delay_time.toInt();  //Enable delay timer
      else if (cooler_timer == 0) {  //If the delay time has elapsed
        digitalWrite(COOLER, LOW);  //Turn on the cooler (active low)
        cooler_timer = -1;  //Stop the timer
        cooler = true;  //Set the cooler status variable to active
        Serial.println("cooler => ON");
      }
      else
        cooler_timer = cooler_timer - 1;  //Decrease the timer
    }
  }
  else if (temperature <= set_max_temperature.toInt() && cooler) {
    if (temperature <= 300)
    {
      if (cooler_timer == -1)  //If the timer is off
        cooler_timer = delay_time.toInt();  //Enable delay timer
      else if (cooler_timer == 0) {  //If the delay time has elapsed
        digitalWrite(COOLER, HIGH);  //Turn off the cooler (active low)
        cooler_timer = -1;  //Stop the timer
        cooler = false;  //Set the cooler status variable to deactivate
        Serial.println("cooler => OFF");
      }
      else
        cooler_timer = cooler_timer - 1;  //Decrease the timer
    }
  }
  else if (cooler_timer != -1)  //If the timer was active
    cooler_timer = -1;  //Stop the timer
  if (humidity < set_min_humidity.toInt() && !fog_maker) {  //If the humidity is lower than the set value
    if (set_min_humidity != "0" && set_max_humidity != "0" && humidity <= 100)
    {
      if (fog_maker_timer == -1)  //If the timer is off
        fog_maker_timer = delay_time.toInt();  //Enable delay timer
      else if (fog_maker_timer == 0) {  //If the delay time has elapsed
        digitalWrite(FOG_MAKER, LOW);  //Turn on the fog maker (active low)
        fog_maker_timer = -1;  //Stop the timer
        fog_maker = true;  //Set the fog maker status variable to active
        Serial.println("fog_maker => ON");
      }
      else
        fog_maker_timer = fog_maker_timer - 1;  //Decrease the timer
    }
  }
  else if (humidity >= set_min_humidity.toInt() && fog_maker) {
    if (humidity <= 100)
    {
      if (fog_maker_timer == -1)  //If the timer is off
        fog_maker_timer = delay_time.toInt();  //Enable delay timer
      else if (fog_maker_timer == 0) {  //If the delay time has elapsed
        digitalWrite(FOG_MAKER, HIGH);  //Turn off the fog maker (active low)
        fog_maker_timer = -1;  //Stop the timer
        fog_maker = false;  //Set the fog maker status variable to deactivate
        Serial.println("fog_maker => OFF");
      }
      else
        fog_maker_timer = fog_maker_timer - 1;  //Decrease the timer
    }
  }
  else if (fog_maker_timer != -1)  //If the timer was active
    fog_maker_timer = -1;  //Stop the timer
  if (humidity > set_max_humidity.toInt() && !fan) {  //If the humidity is upper than the set value
    if (set_min_humidity != "0" && set_max_humidity != "0" && humidity <= 100)
    {
      if (fan_timer == -1)  //If the timer is off
        fan_timer = delay_time.toInt();  //Enable delay timer
      else if (fan_timer == 0) {  //If the delay time has elapsed
        digitalWrite(FAN, LOW);  //Turn on the fan (active low)
        fan_timer = -1;  //Stop the timer
        fan = true;  //Set the fan status variable to active
        Serial.println("fan => ON");
      }
      else
        fan_timer = fan_timer - 1;  //Decrease the timer
    }
  }
  else if (humidity <= set_max_humidity.toInt() && fan) {  //If the humidity is upper than the set value
    if (humidity <= 100)
    {
      if (fan_timer == -1)  //If the timer is off
        fan_timer = delay_time.toInt();  //Enable delay timer
      else if (fan_timer == 0) {  //If the delay time has elapsed
        digitalWrite(FAN, HIGH);  //Turn off the fan (active low)
        fan_timer = -1;  //Stop the timer
        fan = false;  //Set the fan status variable to deactivate
        Serial.println("fan => OFF");
      }
      else
        fan_timer = fan_timer - 1;  //Decrease the timer
    }
  }
  else if (fan_timer != -1)  //If the timer was active
    fan_timer = -1;  //Stop the timer
  if (heater) {
    if (heater_error < error_time.toInt())
      heater_error++;
    else if (temperature < set_min_temperature.toInt() - 3){
      digitalWrite(BUZZER, buzzer);
      buzzer = !buzzer;
    }
  }
  else if (heater_error != 0) {
    heater_error = 0;
    digitalWrite(BUZZER, LOW);
    buzzer = true;
  }
  if (cooler) {
    if (cooler_error < error_time.toInt())
      cooler_error++;
    else if (temperature > set_max_temperature.toInt() + 3) {
      digitalWrite(BUZZER, buzzer);
      buzzer = !buzzer;
    }
  }
  else if (cooler_error != 0) {
    cooler_error = 0;
    digitalWrite(BUZZER, LOW);
    buzzer = true;
  }
  if (fog_maker) {
    if (fog_maker_error < error_time.toInt())
      fog_maker_error++;
    else if (humidity < set_min_humidity.toInt() - 5) {
      digitalWrite(BUZZER, buzzer);
      buzzer = !buzzer;
    }
  }
  else if (fog_maker_error != 0) {
    fog_maker_error = 0;
    digitalWrite(BUZZER, LOW);
    buzzer = true;
  }
  if (fan) {
    if (fan_error < error_time.toInt())
      fan_error++;
    else if (humidity > set_max_temperature.toInt() + 5) {
      digitalWrite(BUZZER, buzzer);
      buzzer = !buzzer;
    }
  }
  else if (fan_error != 0) {
    fan_error = 0;
    digitalWrite(BUZZER, LOW);
    buzzer = true;
  }
  delay(1000);  //Pause for 1000 milliseconds
}

void Eeprom_read() {  //EEPROM memory read function
  AP_name = "";  //Delete the name of the previous access point
  AP_pass = "";  //Delete the password of the previous access point
  set_min_temperature = "";  //Delete the previously set minimum temperature value
  set_max_temperature = "";  //Delete the previously set maximum temperature value
  set_min_humidity = "";  //Delete the previously set minimum humidity value
  set_max_humidity = "";  //Delete the previously set maximum humidity value
  delay_time = "";  //Delete the previously delay time value
  scale = "";  //Delete the previously temperature scale name
  language = "";  //Delete the previously language name
  if (EEPROM.read(0) == 1 && EEPROM.read(1) == 3 && EEPROM.read(2) == 7 && EEPROM.read(3) == 9 && EEPROM.read(4) == 1 && EEPROM.read(5) == 1 && EEPROM.read(6) == 1 && EEPROM.read(7) == 5) {
    short index = 8;  //Place the index in the start position
    short cursor = 0;  //Place the cursor in the start position
    while (index < 512) {  //Continue until the index reaches 512
      if (EEPROM.read(index) == 0)  //If the index reaches the delimiter character
        cursor++;  //Move the cursor forward one unit
      else if (cursor == 1)  //If the cursor was at position 1
        AP_name += (char)EEPROM.read(index);  //Read the value of the access point name from the EEPROM memory
      else if (cursor == 2)  //If the cursor was at position 2
        AP_pass += (char)EEPROM.read(index);  //Read the value of the access point password from the EEPROM memory
      else if (cursor == 3)  //If the cursor was at position 3
        set_min_temperature += (char)EEPROM.read(index);  //Read the value of the set minimum temperature from the EEPROM memory
      else if (cursor == 4)  //If the cursor was at position 4
        set_max_temperature += (char)EEPROM.read(index);  //Read the value of the set maximum temperature from the EEPROM memory
      else if (cursor == 5)  //If the cursor was at position 5
        set_min_humidity += (char)EEPROM.read(index);  //Read the value of the set minimum humidity from the EEPROM memory
      else if (cursor == 6)  //If the cursor was at position 6
        set_max_humidity += (char)EEPROM.read(index);  //Read the value of the set maximum humidity from the EEPROM memory
      else if (cursor == 7)  //If the cursor was at position 7
        delay_time += (char)EEPROM.read(index);  //Read the value of the delay time from the EEPROM memory
      else if (cursor == 8)  //If the cursor was at position 8
        scale += (char)EEPROM.read(index);  //Read the value of the temperature scale name from the EEPROM memory
      else if (cursor == 9)  //If the cursor was at position 9
        language += (char)EEPROM.read(index);  //Read the value of the language name from the EEPROM memory
      else  //When none of the data is found
        break;  //Exit the loop
      index++;  //Move the index forward by one unit
    }
  }
  else {
    AP_name = "Golkhune";  //Return the access point name to the default state
    AP_pass = "";  //Return the access point password to the default state
    set_min_temperature = "0";  //Return the set minimum temperature value to the default state
    set_max_temperature = "0";  //Return the set maximum temperature value to the default state
    set_min_humidity = "0";  //Return the set minimum humidity value to the default state
    set_max_humidity = "0";  //Return the set maximum humidity value to the default state
    delay_time = "10";  //Return the delay time value to the default state
    scale = "celsius";  //Return the temperature scale name to the default state
    language = "";  //Return the language name to the default state
    Eeprom_write();  //Save changes in EEPROM memory
  }
}

void Eeprom_write() {  //EEPROM memory write function
  EEPROM.write(0, 1);
  EEPROM.write(1, 3);
  EEPROM.write(2, 7);
  EEPROM.write(3, 9);
  EEPROM.write(4, 1);
  EEPROM.write(5, 1);
  EEPROM.write(6, 1);
  EEPROM.write(7, 5);
  short index = 8;  //Place the index in the start position
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < AP_name.length(); i++) {  //Separate the characters of the access point name
    EEPROM.write(index, (int)AP_name[i]);  //Save access point name in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < AP_pass.length(); i++) {  //Separate the characters of the access point password
    EEPROM.write(index, (int)AP_pass[i]);  //Save access point password in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < String(set_min_temperature).length(); i++) {  //Separate the characters of the set minimum temperature value
    EEPROM.write(index, (int)String(set_min_temperature)[i]);  //Save set minimum temperature value in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < set_max_temperature.length(); i++) {  //Separate the characters of the set maximum temperature value
    EEPROM.write(index, (int)set_max_temperature[i]);  //Save set maximum temperature value in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < String(set_min_humidity).length(); i++) {  //Separate the characters of the set minimum humidity value
    EEPROM.write(index, (int)String(set_min_humidity)[i]);  //Save set minimum humidity value in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < set_max_humidity.length(); i++) {  //Separate the characters of the set maximum humidity value
    EEPROM.write(index, (int)set_max_humidity[i]);  //Save set maximum humidity value in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < delay_time.length(); i++) {  //Separate the characters of the delay time value
    EEPROM.write(index, (int)delay_time[i]);  //Save delay time value in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < scale.length(); i++) {  //Separate the characters of the temperature scale name
    EEPROM.write(index, (int)scale[i]);  //Save temperature scale name in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  for (int i = 0; i < language.length(); i++) {  //Separate the characters of the language name
    EEPROM.write(index, (int)language[i]);  //Save language name in EEPROM memory
    index++;  //Move the index forward by one unit
  }
  EEPROM.write(index, 0);  //Insert the space character
  index++;  //Move the index forward by one unit
  EEPROM.commit();  //Final record of changes in EEPROM memory
}

bool Number_check(String value, bool percentage) {
  bool num = true;
  if (value == "")
    num = false;
  else if (value[0] == '-' && percentage == false) {
    for (int i = 1; i < value.length(); i++) {
      if (value[i] != '0' && value[i] != '1' && value[i] != '2' && value[i] != '3' && value[i] != '4')
        if (value[i] != '5' && value[i] != '6' && value[i] != '7' && value[i] != '8' && value[i] != '9')
          num = false;
    }
  }
  else {
    for (int i = 0; i < value.length(); i++) {
      if (value[i] != '0' && value[i] != '1' && value[i] != '2' && value[i] != '3' && value[i] != '4')
        if (value[i] != '5' && value[i] != '6' && value[i] != '7' && value[i] != '8' && value[i] != '9')
          num = false;
    }
  }
  return num;
}
