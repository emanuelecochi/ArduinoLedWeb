#include <SPI.h>
#include <Ethernet.h>

// Create an array of bytes to specify the mac address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// Create an array of bytes to specify the IP address
IPAddress ip(192, 168, 1, 100);
// Creates a server object that listens on port specified
EthernetServer server(80);
// Create a client object
EthernetClient client;
// dataRX contains the request HTTP of the client
String dataRX = "";
// checkString check if dataRX contains the word "status"
int checkString = 0;
// statusLed contains the status of the led
// received from the client (0 or 1)
int statusLed = 0;

// readRequest check if there are data be read
bool readRequest(EthernetClient& client) {
  dataRX = "";
  // An HTTP request ends with a blank line
  bool currentLineIsBlank = true;
  // Check continuously that the client is connected
  while (client.connected()) {
    // Check if there are bytes available for reading
    if (client.available()) {
      char c = client.read();
      dataRX += c;
      // If the sending of the HTTP request is completed,
      // then the server sends the response
      if (c == '\n' && currentLineIsBlank) {
        Serial.print(dataRX);
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;     // new row
      } else if (c != '\r') {
        currentLinesBlank = false;    // character of new line
      }
    }
  }
  return false;
}

void writeResponse() {
  // Header of the response HTTP
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text / html");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  checkString = dataRX.indexOf("status");
  if (checkString >= 0) {
    statusLed = dataRX.charAt(checkString + 7);
    if (statusLed == 49) {
      digitalWrite(9, HIGH);
      client.println("1");
    }
    else if (statusLed == 48) {
      digitalWrite(9, LOW);
      client.println("0");
    }
  }
}

void setup() {
  Ethernet.begin(mac, ip);  // Initialize the shield with the mac and ip
  server.begin();           // Initialize the server object
  Serial.begin(9600);       // Initialize serial communications with the PC
  pinMode(9, OUTPUT);       // Define pin 9 as OUTPUT
  digitalWrite(9, LOW);     // Initialize the digital pin 9 LOW
}

void loop() {
  // Gets a client that is connected to the
  // server and has data available for reading
  client = server.available();
  if (client) {
    bool success = readRequest(client);
    if (success) {
      writeResponse();
    }
    // Wait 1 ms so that the response arrivals the browser of the client
    delay(1);
    // Close connection
    client.stop();
  }
}
