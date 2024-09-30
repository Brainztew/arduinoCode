/*   WiFiClient client = server.available();

  if (client) {
    Serial.println("Ny klient ansluten!");
    String request = client.readStringUntil('\r');
    Serial.println(request);

    if (request.indexOf("/ON") != -1) {
      ledStatus = "ON";
      Serial.println("TÄND LAMPAN");
      digitalWrite(led,HIGH);
    } else if (request.indexOf("/OFF") != -1) {
      ledStatus = "OFF";
      Serial.println("SLÄCK LAMPAN");
      digitalWrite(led,LOW); 
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<html><body>");
    if (ledStatus == "OFF") {
      client.println("<h1>LED IS TURNED OFF!</h1>");
      client.print("<a href='http://");
;      client.print(ip);
      client.println("/ON'>TURN ON</a>");
    } else {
      client.println("<h1>LED IS TURNED ON!</h1>");
       client.print("<a href='http://");
      client.print(ip);
      client.println("/OFF'>TURN OFF</a>");
    }
    
    client.println("</body></html>");
    client.println();

    delay(2000);

    client.stop();
    Serial.println("Klient har kopplat ifrån!");

  }

  delay(500); */