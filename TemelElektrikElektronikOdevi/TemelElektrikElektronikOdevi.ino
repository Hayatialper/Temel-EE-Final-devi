#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C ekran(0x27,16,2);


const int ledPin = 2;
const char* ssid = "Wifi SSID'si";
const char* password = "Wifi şifresi";
const char* chatgpt_token = "OpenAI API Key'i buraya koyuyoruz";
String res = "";

void setup() {
  Serial.begin(115200);
  ekran.init();
  ekran.backlight();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
ekran.print("esp32 hazir");
delay(3000);
ekran.clear();

  while (!Serial);

  // wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  Serial.print("Ask your Question : ");
  while (!Serial.available());
  while (Serial.available())
  {
    char add = Serial.read();
    res = res + add;
    delay(1);
  }
  int len = res.length();
  res = res.substring(0, (len - 1));
  res = "\"" + res + "\"";
  Serial.println(res);

  HTTPClient https;

  //Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);

    String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": ") + res + String(", \"temperature\": 1, \"max_tokens\": 100}"); //Instead of TEXT as Payload, can be JSON as Paylaod

    //Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https.POST(payload);

    // httpCode will be negative on error
    // file found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      //Serial.println(payload);

      DynamicJsonDocument doc(1024);


      deserializeJson(doc, payload);
      String Answer = doc["choices"][0]["text"];
      Answer = Answer.substring(2);
      Serial.print("Answer : "); Serial.println(Answer);
      int uzunluk = Answer.length();
      int ortaIndex = uzunluk / 2;
      int boslukIndex = -1;

      for (int i = ortaIndex; i < uzunluk; i++) {
        if (Answer.charAt(i) == ' ') {
          boslukIndex = i;
          break;
        }
      }

      String ilkKisim;
      String ikinciKisim;
      
      if (boslukIndex != -1) {
         ilkKisim = Answer.substring(0, boslukIndex);
         ikinciKisim = Answer.substring(boslukIndex + 1);
      } else   {
        ilkKisim = Answer.substring(0, ortaIndex);
        ikinciKisim = Answer.substring(ortaIndex);

      }
      ekran.setCursor(1,0);
      ekran.print(ilkKisim);
      ekran.setCursor(0,1);
      ekran.print(ikinciKisim);
      delay(5000);
      ekran.clear();
    }
    else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      ekran.setCursor(0,1);
      ekran.print("Err");
      delay(3000);
      ekran.clear();
    }
    https.end();
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  Serial.println("Wait 10s before next round...");
  res = "";
  delay(10000);
}

