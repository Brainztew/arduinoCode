#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include "settings.h"

#define DHTPIN 9
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int sendDataLedOK = 10;
int sendDataLedNOTOK = 11;
int sendToDbLED = 2;
int buttonPin = 4;
int buttonPinSendToBackend = 3;
int buttonState = 1;
int buttonSendToBackendState = 1;
int lastButtonSendToBackendState = 1;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

char serverAdress[] = "seahorse-app-ql4ym.ondigitalocean.app";
int port = 443;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);


IPAddress ip;
unsigned long previousTempMillis = 0;
unsigned long previousLiveMillis = 0;
unsigned long previousScreenMillis = 0;
const long tempInterval = 1800000; 
const long liveInterval = 30000;
const long screenInterval = 30000;
bool autoSending = false;   

void setup() {
  pinMode(sendDataLedOK, OUTPUT);
  pinMode(sendDataLedNOTOK, OUTPUT);
  pinMode(sendToDbLED, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPinSendToBackend, INPUT);
  Serial.begin(9600);
  dht.begin();
  client.setTimeout(5000); 
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.println("Connecting to WIFI...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  //server.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MagicSensor 2024");
  lcd.setCursor(0, 1);
  lcd.print("Values in 5 sec!");
  delay(5000);
  setScreen(temperature, humidity);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  buttonState = digitalRead(buttonPin);
  buttonSendToBackendState = digitalRead(buttonPinSendToBackend);

  unsigned long currentMillis = millis();

  if (currentMillis - previousScreenMillis >= screenInterval) {
    previousScreenMillis = currentMillis;
    setScreen(temperature, humidity);
  }
  if (autoSending && currentMillis - previousTempMillis >= tempInterval) {
    previousTempMillis = currentMillis;
    sendTempToBackend(temperature, humidity);
  }
  
  if (currentMillis - previousLiveMillis >= liveInterval) {
    previousLiveMillis = currentMillis;
    sendLiveTempToBackend(temperature, humidity);
    Serial.print(currentMillis / 1000);
    Serial.println(" seconds");
  }
  
  if (buttonState == LOW) {
    Serial.println("Press button, sending manually to DB!");
    delay(50);
    sendTempToBackend(temperature, humidity); 
  }
  if (buttonSendToBackendState == LOW && lastButtonSendToBackendState == HIGH) {
    delay(500);
    autoSending = !autoSending;
    digitalWrite(sendToDbLED, autoSending ? HIGH : LOW);
    if (autoSending) {
    Serial.println("Sending data to backend timer ON");}
    else {
    Serial.println("Sending data to backend timer OFF");
    }
    
  }
}

void sendTempToBackend(float temperature, float humidity) {
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(sendDataLedNOTOK, HIGH);
    delay(5000);
    digitalWrite(sendDataLedNOTOK, LOW);
    return;
  }

  String postData = "{\"temp\": \"" + String(temperature, 2) + "\", \"humidity\": \"" + String(humidity, 2) + "\"}";
  Serial.println("Sending data to the database!");
  
  client.stop(); 
  delay(100); 

  unsigned long startMillis = millis();
  client.beginRequest();
  client.post("/temp");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", postData.length());

  client.beginBody();
  client.print(postData);
  client.endRequest();

  while (!client.available() && (millis() - startMillis) < 5000) {
  }

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  if (statusCode == 200) {
    digitalWrite(sendDataLedNOTOK, LOW);
    digitalWrite(sendDataLedOK, HIGH);
    delay(300);
    digitalWrite(sendDataLedOK, LOW);
    delay(300);
    digitalWrite(sendDataLedOK, HIGH);
    delay(300);
    digitalWrite(sendDataLedOK, LOW);
    delay(300);
    digitalWrite(sendDataLedOK, HIGH);
    delay(300);
    digitalWrite(sendDataLedOK, LOW);
    delay(300);
    digitalWrite(sendDataLedOK, HIGH);
    delay(3000);
    digitalWrite(sendDataLedOK, LOW);
  } else {
    Serial.print("Error: Could not send data!");
    digitalWrite(sendDataLedNOTOK, HIGH);
  }
}

void sendLiveTempToBackend(float temperature, float humidity) {
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(sendDataLedNOTOK, HIGH);
    delay(5000);
    digitalWrite(sendDataLedNOTOK, LOW);
    return;
  }

  String postData = "{\"temp\": \"" + String(temperature, 2) + "\", \"humidity\": \"" + String(humidity, 2) + "\"}";
  Serial.println("Sending live temperature data");
  
  client.stop(); 
  delay(100); 

  unsigned long startMillis = millis();
  client.beginRequest();
  client.post("/live");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", postData.length());

  client.beginBody();
  client.print(postData);
  client.endRequest();

  while (!client.available() && (millis() - startMillis) < 5000) {
  }

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  if (statusCode == 200) {
    digitalWrite(sendDataLedOK, HIGH);
    delay(1000);
    digitalWrite(sendDataLedOK, LOW);
  } else {
    Serial.print("Error: Could not send data!");
    digitalWrite(sendDataLedNOTOK, HIGH);
  }
}

void clearLcdScreen() {
  lcd.setCursor(0, 0);
  lcd.print("                "); 
  lcd.setCursor(0, 1);
  lcd.print("                "); 
}

void setScreen(float temperature, float humidity) {
  clearLcdScreen();
  lcd.setCursor(0, 0);
  lcd.print("H=");
  lcd.print(humidity);
  lcd.print("%T=");
  lcd.print(temperature);
  lcd.print("C");

  
  client.stop(); 
  delay(100); 

  unsigned long startMillis = millis();
  client.get("/time");

  while (!client.available() && (millis() - startMillis) < 5000) {
  }

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  lcd.setCursor(0, 1);
  if (statusCode == 200) {
    lcd.print(response);
    Serial.println("Printing time!");
    digitalWrite(sendDataLedNOTOK, LOW);
  } else {
    lcd.print("Couldn't get time");
    Serial.println("Couldn't get time");
    digitalWrite(sendDataLedNOTOK, HIGH);

  }
}