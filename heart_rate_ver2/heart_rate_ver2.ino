#include <Wire.h>
#include "rgb_lcd.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MLX90614.h>
#include <ArduinoJson.h>

#define RELAY 5

// Replace with your network credentials
const char* ssid = "Ok";
const char* password = "q12345678";

// Replace with your LINE Notify token
const char* lineToken = "59ysy6LlZxDyVhzIEwZ2dGkPVdIMg9AoVSEnHqtPNDH";

// Google Script ID
const char* googleScriptID = "AKfycbxflLf4fcwYo1JI1eE5XVSO8V952FL9LdB_FqRae86ObAzA0znyoespf-1-KE6w9caXOw";

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
rgb_lcd lcd;

// Updated 06.13.2024
const char* root_ca = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCQkBIa61kTTWh9\n" \
"jBsRzmTejFVQxMIjWtu3xtxKHjYtKe3Z+B099W1YtbJ0y0o+pTsvuI089ZsuXLru\n" \
"MFUThxCD0k8bfHY4dXNd2rEi/F6oSq9IiulYcdZx9J15RbBmuCyaOvCWaHf4GovO\n" \
"D9jBBxeOTgK1jHEAgNqOg1ESHZMe/epAj/laQPN+xHxGQXXDOw7FLNz7IsZ9WIsc\n" \
"RhHJxgKovXsWJ/3LqtHBo33tXXuptDDD3qVRUaTZXJMeYWf7iWrim+1T5vAdh5BO\n" \
"8+/Om0rENWhXi0y0AT6Nr9KgSfCP8d0SBREwv1KTb8vytQFbBjHzZcBQHI70aFE+\n" \
"Xiw8yEG3AgMBAAECggEAPY7L1uCuzpRg0U1O3Zc4xreVQM54KNvwweIRPhJ9WWAI\n" \
"Y4/Oy+pthJ/ifO9wDxsHB/wbUID5cpWUGAdGwwj5lFiCzR6DrOj+llGgqlgy6pDa\n" \
"8RYN5VacTWwvfkuZoTP98jL07edQZrrPG5EN6KxfcH5xkuNZ3y//YT4z0HX9hu1O\n" \
"wG5M6hhGi6iUSe0zhGNgSioO4Z+RkR5u57zOmeFVfUo0BJF+vMmH8TNCk0loTIxb\n" \
"RjFynQ4G2HMDc1YznY61ewa5Nef980uJS2NSQwM0rmc3JpHkq3ABVVFC/X/9Wm2m\n" \
"j+olS1y4NnyyULLsxP15Ru8hNINcTrcM3eu/FejcQQKBgQDKICDORKfgL9JrlpM3\n" \
"tRIFfDGYZwphl2CpGjHSPhdjQOvyMjfJUIQmJni9wk2Z9OwFQlOCHmWKRoWBfT1K\n" \
"2PksZg80NE3vPgn9tVW7m9HjmhvE7S/oLLf65IqyHYQqXLs2GmkjpwnVSkDoYodi\n" \
"tNz5gYp+CW70dX6Jx1/XbVomIQKBgQC3GDI1WH9NOrec/NU+SdVxVUn3Jj2XGP2G\n" \
"My1oVUaVqGIYJv2YYpnMTWTIfhlhPksQ1Yl6mQZSdAAs6rk3rPe1rY+Xm4StOASP\n" \
"2UjNrvPcJXNBtAJdBYqpvRBkeBFFpCfKoL/iqbI6Q38ciPvuxQlEcJGIFp+BeGDm\n" \
"DIgKi9K81wKBgGnxgyB6rf99MW7BIvMNDqAfY6N9Rty2Sqz4FOsdLocQrVtqZQE5\n" \
"GbshaSIVaTVsQhXTmQ/XvONIaYvgyDKmHZze9dMoIz1mKDh6RrCqUM5apXmH1Rd9\n" \
"8QUpq2UGwZKuvlXya++LC0VnwTPNCSd8o2KIOr4rVaP+vc+b2CmDoL/hAoGAKEWj\n" \
"LYakoYkLv/UAtm2pC/+VgE4h9+0gPnddQniXSO27fdJ9rxno+D4Br/PEg1YfTrmZ\n" \
"PqUP9ezb6QVL7goWhhiB3/fuC+epYTaKw+r6am62VjpQ1sMUR9d1FpQuQGIl+nS3\n" \
"IBTxRc141P7wYwlly27zI7rOJL0kmxUgiylMFgsCgYBxvt/hDW2PRL1czrtvPC4D\n" \
"40EsfnGu+eYzu7s481YUpc7tpgVaALQQBv3EzBpwQFfQs50I0cpba/a6KWfu8YJc\n" \
"v64r54CYKSG/8nFE4NttBePmExqdE/hw2pJdmlLCdRKJCIieoZNOAK4fgBniwOyz\n" \
"3E6ATfIfVeW0hf78xEz5AA==\n" \
"-----END PRIVATE KEY-----\n";

void sendLineNotify(const String &message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://notify-api.line.me/api/notify");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", String("Bearer ") + lineToken);
    String payload = "message=" + message;
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
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
  digitalWrite(RELAY, HIGH);
  delay(1000);
  digitalWrite(RELAY, LOW);
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
  digitalWrite(RELAY, LOW);
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
  float temperature = mlx.readAmbientTempC();
  if (millis() - timer0 >= 60000) {
    Serial.println("Resetting...");
    delay(1000);
    reset_heart_rate_sensor();
    timer0 = millis();
  }
  if (heart_rate_value > 81) {
    lcd.setRGB(colorR, colorG, colorB);
    sendLineNotify("Warning!!, Heart Rate Exceeded 81 BPM. ");
  } else {
    lcd.setRGB(150, 105, 255);
  }
  if (temperature > 38) {
    sendLineNotify("Warning!!, Temperature is too high. ");
  }

  if (millis() - timer2 >= 5000) {
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
    heart_rate_value = str.toInt();
    Serial.println("heart_rate_value >> " + String(heart_rate_value));
    timer1 = millis();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Heart: ");
  lcd.print(heart_rate_value);
  lcd.print(" BPM.");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" *C");
  delay(1000);
}
