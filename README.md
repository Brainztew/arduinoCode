## Om projktetet:
Jag har gjort en Adruino modul som samlar in fukt och temp data som sedan skickar vidare till en LCD skärm. Finns status markörer som Grön led för skicka data status=ok (200)<br>
Ifall något fel inträffar med givare eller servern så tänds röd led och ifall datum inte kan hämtas syns det även i skärmen.

för att starta lokalt ändra WiFi Client till detta: <br>
Där XX XXX är din lokala ip adress <br>
char serverAdress[] = "192.168.XX.XXX";<br>
int port = 3000;<br>
WiFiClient wifi;<br>

Färgkod:<br>
1 mellan snabb grön led = skickar live temp och fukt som sparas som en variabel <br>
3 snabba och en långsam grön led = skickar data till backend för lagring i databas<br>
Röd lampa lyser = Fel på nätverk eller DHT sensor<br>
Gul lampa lyser = skickar data var 30 minut till backend för lagring i databas<br>

## Det som användes:
Arduino R4 WiFi<br>
1602IIC skärm<br>
DHT11 sensor <br>
2 Knappar<br>
3 Led lampor (Gul, Röd, Grön)<br>
Ett gäng kablar och några resistorer<br>
Kopplingsdäck<br>

## Länkar:
Backend: https://github.com/Brainztew/ArduinoBackend <br>
Frontend: https://github.com/Brainztew/ArduinoFrontend <br>
Ardunino: https://github.com/Brainztew/arduinoMagic
