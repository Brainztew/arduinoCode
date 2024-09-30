#include <WiFiS3.h>
#include "settings.h"

int led = 2;
String ledStatus = "OFF";

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

WiFiServer server(80);

void setup() {
  pinMode(led, OUTPUT);
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial.println("Ansluter till WIFI...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Adress: ");
  Serial.println(ip);

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available();

  if (client) {
    Serial.println("Ny klient ansluten!");
    digitalWrite(led, HIGH);
    String request = client.readStringUntil('\r');
    Serial.println(request);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<html><body>");
    client.println("<h1>hello, world!</h1>");
    client.println("</body></html>");
    client.println();

    delay(10);

    client.stop();
    Serial.println("Klient har kopplat ifrån!");

    digitalWrite(led, LOW);
  }

  delay(500);

}
