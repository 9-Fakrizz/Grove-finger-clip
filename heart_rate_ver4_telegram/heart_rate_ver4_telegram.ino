#include <Wire.h>
#include "rgb_lcd.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MLX90614.h>
#include <ArduinoJson.h>

#define RELAY 5

// Replace with your network credentials
const char* ssid = "CARETUNU";
const char* password = "98765432";

// Replace with your LINE Notify token
// const char* lineToken = "v2qQQD4CEM5H5KZjPkhAvL5gmF2DwiYz5bDX2j3MKkf";

// Google Script ID
const char* googleScriptID = "AKfycbx4c5cODWixfnZiMbi33Ku9Ihmu5b8bNGKF90ChDzI8u18oFn0qRSpvEOAz5JrQ_LXwMg";

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
rgb_lcd lcd;

// Updated 06.13.2024
const char* root_ca = \
 "-----BEGIN PRIVATE KEY-----\n"\
 "nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDJ9v6adTca7pdv\n"\
 "BH+4jL/iBRHtKM4/LOxWGitfSs+LuA1EzTVbP4Rda6zg4dzQGakUx3ho2eEGxER/\n" \
 "vUvoUqnisVvsrvRaoRfleZzq2hXaePrUggPMMUxCElVDgh+zkh4R1BTFJBIE8cqO\n" \
 "ARiR7FmhYTqNa9tVzsmHEL8AORollJvZEcpreGZFU0/yprTEulAWuPL/ePCleTz4\n" \
 "qo4spvX4FxTa6r1AJlIQMTY0zFKXtH4hz4P70duRll2YUI0DllQG7UMlTnSYu5ga\n" \
 "kOYsCJspIg5blUoF0TDODrP6UtmheWm5P70Grr9bzVxUGjyu8BepBmGNi5dlWj2H\n" \
 "QQtrmZupAgMBAAECggEAIrRYJLd+nBiimfeWcOJyuIZxgaSw6Ccyh0sg59OFnXcn\n" \
 "hIqUoBTmjF0LzGrTVEkMhl8OYEoMQmrocGDZUdcejS834hBgOdCTuCFxJuQJvExs\n" \
 "oaaju2s/NN1MnHiYNPWnSB3+4a9kj0fzK/qhO7X24uW/t3xwqKqLc7ObdpDZXtp/\n" \
 "dCrZqOCPyZY12EEZmIew3YG8/Gwf/O9n8W/546VmdkOSQKgyuoexsTVvZCrotNMQ\n" \
 "FrhsecnfGdwo8i8QysQu4pjAKRj8LkVW/9oWnWifp5uUOgWBLVQP3gxATv1gVayB\n" \
 "i3B6+4y7OJT/vY/nB4RNyQrDTJw8HTaNJuRJHOpFrwKBgQD7A5vqUnOztEAWuPwV\n" \
 "FuShy8LA+JAdbWzCXtNo8uOatV1p+rRr05diVK5oGeMp9m+M4DYiMWCWduK5Hf0E\n" \
 "D4xTCV7BjJTeJ6NxP/CtvtnVT38eVBn1KYMDzip6rfQaGoG+Tz3odLXCeazmUEa6\n" \
 "itsF6YM3z1q+ycA5CFgeHsEcVwKBgQDN+fkXGtIOu0Qt3CK41FNKZwuNds6JhPdI\n" \
 "pnVsUXR5DxlzIYjcmPTRUZ708OIdU6HfxMaUrBkpd8jeFcfvtQl6QtGm0v712tg4\n" \
 "6/3JkomwdxNAGf3yZjpwjrox7XVe7NK/UUYY5yJmY2OdxbrI7mRxl08kzTGAEQ5G\n" \
 "EYMTH0gH/wKBgQCPsrs5Y8ZU5CWkXHYDZL2yWkG90w49qn0OsNZ6SEJAOP+nQhyz\n" \
 "dcg+RJgrhFZlmWy2y0xyOq4+tQSKUT6daoE8sCqMxvwaSbrphF82OAHgecgeBqtt\n" \
 "jDwym336CUerAe2wkxCv2NCM6rcuilpTRFaqhcnW01/xxPHENKCtbZHDtQKBgD2/\n" \
 "pbQOf1CZRVw2NVaRmJKih60WI97INNXbyTH6zV8Jbsi8EeqIFES4Il4t5KuECfAq\n" \
 "bpndagGrflVN1E2BSWEUrQNFF9gejlmqLcADJTH40JWT9TR4/yYFM76KAHi/qIJg\n" \
 "XBJVIFPvkcqXn2JjTUtcYAaZWTCabFwHHfNaml6DAoGAQcxVSBixACwU1wK813Mi\n" \
 "2UtFRSIbsaMmZirf+aSSE7fYDXNyMispB6IorXWnB3NqSPmsYUVJUQ8ljwYBC6Lt\n" \
 "kSQddx63Gd3u/6ginl6hZl+9P8Kt99NMh/8fK++bWlwbafPpw4EmZx4VB/RIHYko\n" \
 "MD2pdewX021XwWcOccdTK4s==\n" \
 "-----END PRIVATE KEY-----\n";

// void sendLineNotify(const String &message) {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     http.begin("https://notify-api.line.me/api/notify");
//     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//     http.addHeader("Authorization", String("Bearer ") + lineToken);
//     String payload = "message=" + message;
//     int httpResponseCode = http.POST(payload);

//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("Response: " + response);
//     } else {
//       Serial.println("Error on sending POST: " + String(httpResponseCode));
//     }
//     http.end();
//   } else {
//     Serial.println("Error in WiFi connection");
//   }
// }

const char* botToken = "YOUR_TELEGRAM_BOT_TOKEN"; // Replace with your bot token
const char* chatID = "YOUR_CHAT_ID";              // Replace with your chat ID

void sendTelegramMessage(const String &message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("https://api.telegram.org/bot") + botToken + "/sendMessage";

    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = "chat_id=" + String(chatID) + "&text=" + message;
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Telegram Response: " + response);
    } else {
      Serial.println("Error sending to Telegram: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}

// Function to follow redirects until the final URL
String followRedirects(String url) {
  HTTPClient http;
  http.begin(url, root_ca);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpResponseCode = http.GET();
  String payload = http.getString();

  while (httpResponseCode == HTTP_CODE_FOUND || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY || httpResponseCode == HTTP_CODE_TEMPORARY_REDIRECT) {
    // Get the redirected URL
    url = http.getLocation();
    http.end();
    http.begin(url, root_ca);
    httpResponseCode = http.GET();
    payload = http.getString();
  }
  
  http.end();
  return url;
}

// Function to send data to Google Sheets
void sendDataToGoogleSheets(float sensor1, float sensor2) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Your Google Script URL
    String url = String("https://script.google.com/macros/s/") + googleScriptID + "/exec";
    url = followRedirects(url);

    // Prepare JSON payload
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["sensor1"] = sensor1;
    jsonDoc["sensor2"] = sensor2;

    String payload;
    serializeJson(jsonDoc, payload);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void reset_heart_rate_sensor() {
  digitalWrite(RELAY, LOW);
  delay(1000);
  digitalWrite(RELAY, HIGH);
}

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
int heart_rate_value = 0;
bool start = false;
long timer0 = 0;
long timer1 = 0;
long timer2 = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  lcd.begin(16, 2);
  lcd.setRGB(150, 105, 255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting Up ..");
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  mlx.begin();                     //init mlx90614 temp
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connect WIFI ..");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected WiFi");
  lcd.clear();
  timer0 = millis();
  timer1 = millis();
  timer2 = millis();
}

void loop() {
  // every 2 minute go to trig heart rate sensor
  float temperature = mlx.readAmbientTempC() + 4.50;
  if (millis() - timer0 >= 60000) {
    Serial.println("Resetting...");
    delay(1000);
    reset_heart_rate_sensor();
    timer0 = millis();
  }
  if (heart_rate_value > 81) {
    lcd.setRGB(colorR, colorG, colorB);
    sendTelegramMessage("Warning!!, Heart Rate Exceeded 81 BPM. ");
  } else {
    lcd.setRGB(150, 105, 255);
  }
  if (temperature > 38) {
    sendTelegramMessage("Warning!!, Temperature is too high. ");
  }

  if (millis() - timer2 >= 5000  && heart_rate_value > 59) {
    // Send data to Google Sheets
    sendDataToGoogleSheets(temperature, heart_rate_value);
    timer2 = millis();
  }

  if (millis() - timer1 >= 5000) {
    String str;
    Wire.requestFrom(0xA0 >> 1, 1);    // request 1 byte from slave device
    while (Wire.available()) {          // slave may send less than requested
      unsigned char c = Wire.read();   // receive heart rate value (a byte)
      Serial.print(c, DEC);         // print heart rate value
      str += c;
    }
    Serial.println();
    heart_rate_value = str.toInt() - 15;
    Serial.println("heart_rate_value >> " + String(heart_rate_value));
    timer1 = millis();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Heart: ");
  if(heart_rate_value > 59){
    lcd.print(heart_rate_value);
  }
  else{
    lcd.print("--");
  }
  lcd.print(" BPM.");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" *C");
  delay(1000);
}
