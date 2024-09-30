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
int buttonPin = 13;
int buttonState = 1;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

char serverAdress[] = "192.168.0.215";
int port = 3000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);

WiFiServer server(80);

IPAddress ip;
unsigned long previousTempMillis = 0;
unsigned long previousLiveMillis = 0;
const long tempInterval = 1800000; 
const long liveInterval = 30000;   

void setup() {
  pinMode(sendDataLedOK, OUTPUT);
  pinMode(sendDataLedNOTOK, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  dht.begin();
  client.setTimeout(3000);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.println("Ansluter till WIFI...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  ip = WiFi.localIP();
  Serial.print("IP Adress: ");
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
  delay(30000);
  setScreen(temperature, humidity);
  buttonState = digitalRead(buttonPin);

  unsigned long currentMillis = millis();
  
  
  if (currentMillis - previousTempMillis >= tempInterval) {
    previousTempMillis = currentMillis;
    sendTempToBackend(temperature, humidity);
  }
  
  if (currentMillis - previousLiveMillis >= liveInterval) {
    previousLiveMillis = currentMillis;
    sendLiveTempToBackend(temperature, humidity);
    Serial.print(currentMillis / 1000);
    Serial.println(" sekunder");
  }
  
  if (buttonState == LOW) {
    Serial.println("Tryck på knapp!");
    delay(50);
    sendTempToBackend( temperature,  humidity);
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

  String postData = "{\"temp\": \"" + String(temperature) + "\", \"humidity\": \"" + String(humidity) + "\"}";
  Serial.println("Skickar data till databasen!");
  client.beginRequest();
  client.post("/temp");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-length", postData.length());

  client.beginBody();
  client.print(postData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  Serial.print("Temperature = ");
  Serial.println(temperature);
  Serial.print("Humidity = ");
  Serial.println(humidity);

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
    Serial.print("Error kunde inte skicka data!");
    digitalWrite(sendDataLedNOTOK, HIGH);
  }
}

void sendLiveTempToBackend( float temperature, float humidity) {
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Kunde inte läsa från DHT sensor!");
    digitalWrite(sendDataLedNOTOK, HIGH);
    delay(5000);
    digitalWrite(sendDataLedNOTOK, LOW);
    return;
  }

  String postData = "{\"temp\": \"" + String(temperature) + "\", \"humidity\": \"" + String(humidity) + "\"}";
  Serial.println("Skickar LiveTemp");
  client.beginRequest();
  client.post("/live");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-length", postData.length());

  client.beginBody();
  client.print(postData);
  client.endRequest();


  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  Serial.print("Temperature = ");
  Serial.println(temperature);
  Serial.print("Humidity = ");
  Serial.println(humidity);

  if (statusCode == 200) {
    digitalWrite(sendDataLedOK, HIGH);
    delay(1000);
    digitalWrite(sendDataLedOK, LOW);
    digitalWrite(sendDataLedNOTOK, LOW);
  } else {
    Serial.print("Error kunde inte skicka data!");
    digitalWrite(sendDataLedNOTOK, HIGH);
  }
}

void clearLcdScreen() {
  lcd.setCursor(0, 0);
  lcd.print("                                           ");
  lcd.setCursor(0, 1);
  lcd.print("                                           ");
}

void setScreen(float temperature, float humidity) {
  clearLcdScreen();
  lcd.setCursor(0, 0);
  lcd.print("H=");
  lcd.print(humidity);
  lcd.print("%T=");
  lcd.print(temperature);
  lcd.print("C");

  client.get("/time");
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  lcd.setCursor(0, 1);
  if ( statusCode == 200) {
    lcd.print(response);
    digitalWrite(sendDataLedNOTOK, LOW);
  }
  else {
    lcd.print("Couldn't gettime");
    digitalWrite(sendDataLedNOTOK, HIGH);
  }
  
}