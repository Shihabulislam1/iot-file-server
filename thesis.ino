#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

//const char* ssid = "Bus Security";
//const char* password = "12121212";
const char* ssid = "404_ERROR";
const char* password = "404notfound";
const char* serverUrl = "http://192.168.0.101:8080";

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;

int retryCount = 3; // Number of retry attempts

#define vibrationPin D5 //for vibration sensor
String vibrationMessage;

#define DHTPIN D1  // Pin where the DHT11 sensor is connected
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define BUTTON_PIN D2  // Pin where the push button is connected
#define BUZZER_PIN D3  // Pin where the buzzer is connected



bool buttonState = false;
bool lastButtonState = false;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(vibrationPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

float generateRandomValue() {
  // Generate a random value between 0 and 1000 (scaled to 0-10)
  int randomValue = random(1001);
  
  // Convert the random value to a float and divide by 100 to get values between 0 and 10
  float scaledValue = (float)randomValue / 100.0;

  return scaledValue;
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      int vibrationState = digitalRead(vibrationPin);
      bool isVibrationDetected = (vibrationState == HIGH);
      float vibrationValue;
      if (isVibrationDetected) {
          vibrationMessage = "true"; // Store message when vibration is detected
          vibrationValue = generateRandomValue();
        } else {
          vibrationMessage = "false"; // Store message when no vibration is detected
          vibrationValue=0;
        }

      if (!isnan(humidity) && !isnan(temperature)) {
       StaticJsonDocument<200> jsonDocument;
        jsonDocument["temperature"] = temperature;
        jsonDocument["humidity"] = humidity;
        jsonDocument["vibration"] = vibrationMessage;
        jsonDocument["vibrationValue"] = vibrationValue;
        
        String jsonString;
        serializeJson(jsonDocument, jsonString);

        Serial.println(jsonString);

          
          WiFiClient client;
          HTTPClient http;
          http.begin(client, serverUrl);
          http.addHeader("Content-Type", "application/json");
          int httpCode = http.POST(jsonString);
          Serial.print("HTTP Response code: ");
          Serial.println(httpCode);

          
            // HTTP request successful
            String payload = http.getString();
            Serial.println("Response payload: " + payload);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();

            
          
            // Beep the buzzer upon successful data transmission
            digitalWrite(BUZZER_PIN, HIGH);
            delay(500); // Adjust beep duration if needed
            digitalWrite(BUZZER_PIN, LOW);
            
            
          
           http.end();
        
       
      } else {
        Serial.println("Failed to read from DHT sensor");
      }
    }
  }
  lastButtonState = buttonState;
}
