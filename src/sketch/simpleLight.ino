#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266NetBIOS.h>
#include <FS.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266LLMNR.h>
#include <NeoPixelBrightnessBus.h>
#include <GyverButton.h>

#define CLEAR_EEPROM 0

#define MODE_AMOUNT 5
#define MODE 0
#define BTN_PIN 0
#define MAIN_LOOP 5
#define MAX_LENGTH_SSID_PASS 32

uint16_t NUM_LEDS       = 100;
byte this_mode          = 0;
byte LIGHT_SAT          = 100;
byte COLOR_SPEED        = 100;
byte FIRE_SPEED         = 15;
int RAINBOW_PERIOD      = 3;
int LIGHT_COLOR         = 0;
int FIRE_COLOR          = 0;
float RAINBOW_STEP      = 5.5;
byte BRIGHTNESS         = 200;
boolean ONstate         = true;
boolean reboot          = false;
boolean AP_MODE, first;
unsigned long main_timer, color_timer, rainbow_timer, eeprom_timer;
int this_color, networksCount;
float rainbow_steps;

const int led = LED_BUILTIN;
char ssid[MAX_LENGTH_SSID_PASS] = "SimpleSSID";
char pass[MAX_LENGTH_SSID_PASS] = "SimplePass";
const char* version = "v1.0";
const char* modelNumber = "KitchenLight";
const char* host = "https://antiit.ru";
const char* hostNS = "antiit.ru";
const uint16_t httpsPort = 443;
const char* update_file = "/arduino/kitchen-light.json";
const char* update_path = "/arduino/firmware";
const char* modelName = "SimpleLight";
const char* hostName = "SimpleLight";
char* password = "SimpleLightPassword";

AsyncWebServer server(80);

MDNSResponder mdns;

NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>* strip = NULL;

IPAddress softApIp(192, 168, 42, 1);
IPAddress subnet(255, 255, 255, 0);

GButton butt1(BTN_PIN);

void setup(void) {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  if (CLEAR_EEPROM)
  {
    saveSettings();
    Serial.println(F("EEPROM saved with default settings"));
  }
  readSettings();
  Serial.println(F("Get settings"));
  pixelCountChanged(NUM_LEDS);
  animationTick();
  networksCount = WiFi.scanNetworks();
  WiFi.hostname(hostName);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  int tryNum = 1;
  while (!WiFi.isConnected() && tryNum++ < 10) {
    Serial.println(F("Try connect to WiFi"));
    if (tryNum == 10){
      tryNum++;
      break;
    } else {
      digitalWrite(led, 1);
      delay(1000);
      digitalWrite(led, 0);
    }
  }
  if (tryNum != 11) {
    Serial.println(F("Connected to WiFi"));
    WiFi.setAutoReconnect(true);
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());
    Serial.print(F("DNS #1, #2 IP: "));
    WiFi.dnsIP().printTo(Serial);
    Serial.print(", ");
    WiFi.dnsIP(1).printTo(Serial);
    Serial.println();
    AP_MODE = false;
    MDNS.begin(hostName, WiFi.localIP());
    MDNS.addService("http", "tcp", 80);
    LLMNR.begin(hostName);
    Serial.println(F("Started MDNS"));
    NBNS.begin(hostName);
    Serial.println(F("Started NETBIOS"));
  } else {
    WiFi.mode(WIFI_AP);
    Serial.println(F("Not connected to WiFi"));
    Serial.println(F("Starting Access Point"));
    WiFi.hostname(hostName);
    WiFi.softAPConfig(softApIp, softApIp, subnet);
    WiFi.softAP(modelName, password);
    server.reset();
    Serial.println(F("Started DNS server"));
    AP_MODE = true;
  }
  server.serveStatic("/index.html", SPIFFS, "/www/index.html");
  server.serveStatic("/assets/", SPIFFS, "/assets/", "max-age=86400");
  server.serveStatic("/views/", SPIFFS, "/views/", "max-age=86400");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/index.html");
  });
  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println(F("Server handle settings"));
    handleSettings(request);
  });
  server.on("/info.json", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(F("Server handle json"));
    handleInfoJson(request);
  });
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "updatefail" : "updateok");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    Serial.println(filename);
    Serial.setDebugOutput(true);
    SPIFFS.end();

    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Serial.println(maxSketchSpace);
    if (filename == "spiffs.bin") {
      if(!Update.begin(maxSketchSpace, U_FS)) {
        Update.printError(Serial);
        SPIFFS.begin();
      }
    } else {
      if(!Update.begin(maxSketchSpace, U_FLASH)) {
        Update.printError(Serial);
        SPIFFS.begin();
      }
    }

    Update.runAsync(true);

    if(Update.write(data, len) != len) {
      Update.printError(Serial);
      SPIFFS.begin();
    }
    
    if(final) {
      if(Update.end(true)){
        Serial.println("Update Success: Rebooting...");
        reboot = true;
      } else {
        Update.printError(Serial);
        SPIFFS.begin();
      }
      Serial.setDebugOutput(false);
    }
  });
  server.onNotFound(handleNotFound);
  if (SPIFFS.begin()) {
    Serial.println(F("Started SPIFFS"));
    server.begin();
    Serial.println(F("Started Web Server:"));
    if (AP_MODE) {
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println(WiFi.localIP());
    }
  } else {
    Serial.println(F("ERROR! NO SPIFFS!"));
  }
  eeprom_timer = millis();
}

void loop(void) {
  buttonTick();
  animationTick();
  if (!AP_MODE) {
    MDNS.update();
  }
  eepromTick();
  if (reboot) {
    ESP.restart();
  }
}

// Animations
void animationTick() {
  if (strip != NULL) { 
    if (ONstate) {
      strip->SetBrightness(BRIGHTNESS);
      if (millis() - main_timer > MAIN_LOOP) {
        switch (this_mode) {
          case 0:
            for (int i = 0; i < NUM_LEDS; i++) strip->SetPixelColor(i, RgbColor((uint8_t)255));
            break;
          case 1: for (int i = 0; i < NUM_LEDS; i++) strip->SetPixelColor(i, HsbColor(LIGHT_COLOR / (float)360, LIGHT_SAT / (float)100, (float)1));
            break;
          case 2:
            if (millis() - color_timer > (256 - COLOR_SPEED)) {
              color_timer = millis();
              if (++this_color > 255) this_color = 0;
            }
            for (int i = 0; i < NUM_LEDS; i++) strip->SetPixelColor(i, HsbColor(this_color / (float)255, LIGHT_SAT / (float)100, (float)1));
            break;
          case 3:
            if (millis() - rainbow_timer > 30) {
              rainbow_timer = millis();
              this_color += RAINBOW_PERIOD;
              if (this_color > 255) this_color = 0;
              if (this_color < 0) this_color = 255;
            }
            rainbow_steps = this_color;
            for (int i = 0; i < NUM_LEDS; i++) {
              strip->SetPixelColor(i, HsbColor(rainbow_steps / (float)255, (float)1, (float)1));
              rainbow_steps += RAINBOW_STEP;
              if (rainbow_steps > 255) rainbow_steps = 0 + (rainbow_steps - 255);
              if (rainbow_steps < 0) rainbow_steps = 255 + rainbow_steps;
            }
            break;
          case 4:
            RgbColor color = RgbColor(HsbColor(FIRE_COLOR / (float)360, (float)1, (float)1));
            int r = color.R, g = color.G, b = color.B;
            int speed = random(1000, 7000) / FIRE_SPEED;
            if (millis() - color_timer > speed) {
              for(int i = 0; i < NUM_LEDS; i++) {
                int flickerR = 0;
                int flickerG = 0;
                int flickerB = 0;
                if (r >= g && r >= b) { //Красные оттенки
                  flickerR = random(0, r / 4);
                  flickerG = random(0, g);
                  flickerB = random(0, b);
                } else if (g >= r && g >= b) { //Зелёные оттенки
                  flickerR = random(0, r);
                  flickerG = random(0, g / 4);
                  flickerB = random(0, b);
                } else if (b >= r && b >= g) { //Синие оттенки
                  flickerR = random(0, r);
                  flickerG = random(0, g);
                  flickerB = random(0, b / 4);
                }
                int r1 = r - flickerR;
                int g1 = g - flickerG;
                int b1 = b - flickerB;
                if(r1<0) r1=0;
                if(g1<0) g1=0;
                if(b1<0) b1=0;
                strip->SetPixelColor(i, RgbColor(r1, g1, b1));
              }
              color_timer = millis();
            }
            break;
        }

        strip->Show();
        main_timer = millis();
      }
    }
  }
}

// Button
void buttonTick() {
  butt1.tick();
  if (butt1.isSingle()) {
    digitalWrite(led, 0);
    Serial.println(F("Button pressed"));
    if (++this_mode >= MODE_AMOUNT) { 
      this_mode = 0;
    }
    digitalWrite(led, 1);
  }
}


// EEPROM
void readSettings() {
  int eepromlong = (MAX_LENGTH_SSID_PASS * 2) + 7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t) + 50;
  EEPROM.begin(eepromlong);
  first = EEPROM.read(0);
  if (first == 0) {
    saveSettings();
  }
  this_mode = EEPROM.read(1);
  COLOR_SPEED = EEPROM.read(2);
  BRIGHTNESS = EEPROM.read(3);
  ONstate = EEPROM.read(4);
  FIRE_SPEED = EEPROM.read(5);
  LIGHT_SAT = EEPROM.read(6);
  EEPROM.get(7, RAINBOW_PERIOD); 
  EEPROM.get(7 + sizeof(int), LIGHT_COLOR);
  EEPROM.get(7 + (sizeof(int) * 2), FIRE_COLOR);
  EEPROM.get(7 + (sizeof(int) * 3), RAINBOW_STEP);
  EEPROM.get(7 + (sizeof(int) * 3) + sizeof(float), NUM_LEDS);
  EEPROM.get(7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t), ssid);
  EEPROM.get(7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t) + sizeof(ssid), pass);
  EEPROM.end();
  Serial.println(F("Settings readed"));
}

void saveSettings() {
  int eepromlong = (MAX_LENGTH_SSID_PASS * 2) + 7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t) + 50;
  EEPROM.begin(eepromlong);
  EEPROM.write(0, 1);
  EEPROM.write(1, this_mode);
  EEPROM.write(2, COLOR_SPEED);
  EEPROM.write(3, BRIGHTNESS);
  EEPROM.write(4, ONstate);
  EEPROM.write(5, FIRE_SPEED);
  EEPROM.write(6, LIGHT_SAT);
  EEPROM.put(7, RAINBOW_PERIOD);
  EEPROM.put(7 + sizeof(int), LIGHT_COLOR);
  EEPROM.put(7 + (sizeof(int) * 2), FIRE_COLOR);
  EEPROM.put(7 + (sizeof(int) * 3), RAINBOW_STEP);
  EEPROM.put(7 + (sizeof(int) * 3) + sizeof(float), NUM_LEDS);
  EEPROM.put(7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t), ssid);
  EEPROM.put(7 + (sizeof(int) * 3) + sizeof(float) + sizeof(uint16_t) + sizeof(ssid), pass);
  EEPROM.commit();
  EEPROM.end();
  Serial.println(F("Settings saved"));
}

void eepromTick() {
  if (millis() - eeprom_timer > 30000) {
    eeprom_timer = millis();
    saveSettings();
    Serial.println(ESP.getFreeHeap());
  }
}


//WEB
void handleSettings(AsyncWebServerRequest *request) {
  digitalWrite(led, 1);
  boolean error = false;
  boolean reload = false;
  for (int i = 0; i < request->args(); i++) {
    if (strcmp(request->argName(i).c_str(), "mode") == 0) {
      if (request->arg(i).toInt() <= MODE_AMOUNT && request->arg(i).toInt() != this_mode) {
        this_mode = request->arg(i).toInt();
        reload = true;
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "brightness") == 0) {
      if (request->arg(i).toInt() != BRIGHTNESS) {
        BRIGHTNESS = request->arg(i).toInt();
        request->send(200, "text/plain", "ok");
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "state") == 0) {
      if (request->arg(i) == "on") {
        ONstate = true;
        reload = true;
      } else if (request->arg(i) == "off") {
        ONstate = false;
        reload = true;
        strip->SetBrightness(0);
        for (int i = 0; i < NUM_LEDS; i++) strip->SetPixelColor(i, HsbColor((float)0, (float)0, (float)0));
        strip->Show();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "light_color") == 0) {
      if (request->arg(i).toInt() != LIGHT_COLOR) {
        LIGHT_COLOR = request->arg(i).toInt();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "color_speed") == 0) {
      if (request->arg(i).toInt() != COLOR_SPEED) {
        COLOR_SPEED = request->arg(i).toInt();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "fire_speed") == 0) {
      if (request->arg(i).toInt() != FIRE_SPEED) {
        FIRE_SPEED = request->arg(i).toInt();
      } else {
        error = true;
      }
    }  else if (strcmp(request->argName(i).c_str(), "fire_color") == 0) {
      if (request->arg(i).toInt() != FIRE_COLOR) {
        FIRE_COLOR = request->arg(i).toInt();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "rainbow_period") == 0) {
      if (request->arg(i).toInt() != RAINBOW_PERIOD) {
        RAINBOW_PERIOD = request->arg(i).toInt();   
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "rainbow_step") == 0) {
      if (request->arg(i).toInt() != RAINBOW_STEP) {
        RAINBOW_STEP = request->arg(i).toInt();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "light_sat") == 0) {
      if (request->arg(i).toInt() != LIGHT_SAT) {
        LIGHT_SAT = request->arg(i).toInt();
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "num_leds") == 0) {
      if (request->arg(i).toInt() != NUM_LEDS) {
        NUM_LEDS = request->arg(i).toInt();
        reboot = true;
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "ssid") == 0) {
      if (request->arg(i).length() <= MAX_LENGTH_SSID_PASS) {
        request->arg(i).toCharArray(ssid, request->arg(i).length() + 1);
        reboot = true;
      } else {
        error = true;
      }
    } else if (strcmp(request->argName(i).c_str(), "pass") == 0) {
      if (request->arg(i).length() <= MAX_LENGTH_SSID_PASS) {
        request->arg(i).toCharArray(pass, request->arg(i).length() + 1);
        reboot = true;
      } else {
        error = true;
      }
    }
  }
  if (error) {
    request->send(400);
  } else if (reload) {
    saveSettings();
    request->send(200, "text/plain", "reload");
  } else if (reboot) {
    saveSettings();
    request->send(200, "text/plain", "reboot");
  } else {
    saveSettings();
    request->send(200, "text/plain", "ok");
  }
  digitalWrite(led, 0);
}

void handleInfoJson(AsyncWebServerRequest *request) {
  String output = "";
  StaticJsonDocument<1200> doc;
  doc["this_mode"] = this_mode;
  doc["brightness"] = BRIGHTNESS;
  doc["onstate"] = ONstate;
  doc["light_color"] = LIGHT_COLOR;
  doc["color_speed"] = COLOR_SPEED;
  doc["rainbow_period"] = RAINBOW_PERIOD;
  doc["rainbow_step"] = RAINBOW_STEP;
  doc["light_sat"] = LIGHT_SAT;
  doc["fire_speed"] = FIRE_SPEED;
  doc["fire_color"] = FIRE_COLOR;
  doc["num_leds"] = NUM_LEDS;
  doc["ssid"] = ssid;
  doc["pass"] = pass;
  JsonArray ssids = doc.createNestedArray("ssids");
  String currSsid(ssid);
  for (int i = 0; i < networksCount; i++)
  {
    String name = WiFi.SSID(i);
    if (currSsid != name) {
      ssids.add(WiFi.SSID(i));
    }
  }
  serializeJson(doc, output);
  doc.clear();
  request->send(200, "text/json", output);
}

void handleNotFound(AsyncWebServerRequest *request) {
  digitalWrite(led, 1);
  request->send(404, "text/plain", "File Not Found");
  digitalWrite(led, 0);
}

// Dynamic pixels count
void pixelCountChanged(uint16_t newCount) {
  if (strip != NULL) {
    delete strip;
  }
  strip = new NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>(newCount);
  strip->Begin();
}