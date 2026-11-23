#include <MatrizLed.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <FS.h>
#include <LittleFS.h>

const char* ap_ssid = "ESP8266-Hotspot";
const char* ap_password = "password123";

// Create a web server object on port 80
ESP8266WebServer server(80);

// Onboard LED is usually connected to GPIO 2 (D4 on NodeMCU)
const int ledPin = 2;
bool ledState = false;
String html;
bool states[5][5]; 

/*
 * Contador con varias pantallas
 */

MatrizLed pantalla;
int contador = 0;

void fillRow(int r) {
  int i = 0;
  for(i = 0; i < 8; i++) {
    //for(j = 0; j < 8; j++) {
      pantalla.setLed(0, i, r, true);
    //}
  }
}

void setup_matrix() {
  //pantalla.begin(12, 11, 10, 1); // dataPin, clkPin, csPin, numero de matrices de 8x8
  pantalla.begin(D7, D5, D8, 1);
  pantalla.rotar(false);

  pantalla.borrar();
  
}

void fileSetup() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // 2. Open the text file for reading (note the leading slash)
  File file = LittleFS.open("/esfera.html", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // 3. Read and print contents
  Serial.println("File contents:");
  while (file.available()) {
    html += (char)file.read();
  }
  Serial.println();

  // 4. Close the file
  file.close();
}

void loop_matrix() { 
  pantalla.escribirFraseScroll("Tane te amo", 120);
  /*pantalla.setLed(0, 0, 0, true);
  pantalla.setLed(0, 0, 1, false);
  pantalla.setLed(0, 0, 2, false);
  pantalla.setLed(0, 0, 3, false);
  pantalla.setLed(0, 0, 4, false);
  pantalla.setLed(0, 0, 5, false);
  pantalla.setLed(0, 0, 6, false);
  pantalla.setLed(0, 0, 7, true);
  
  pantalla.setLed(0, 1, 1, false);*/
  //pantalla.setLed(0, 1, 1, true);
  //pantalla.encender();
  //pantalla.escribirCifra(contador);
  //contador++;
  delay(1000);
}



// Set your desired Access Point credentials
// Password must be at least 8 characters long (or set to "" for an open network)


// Function to generate the HTML page


// Handler for the root URL ("/")
void handleRoot() {
  server.send(200, "text/html", html);
}

// Handler for toggling the LED ("/toggle")
void handleToggle() {
  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  bool state = states[x][y];
  states[x][y] = !state;
  pantalla.setLed(0, x, y, !state);
  
  //server.sendHeader("Location", "/");
  //server.send(303);
  server.send(200, "text/html", "");
}

void handleOnAll() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      states[i][j] = true;
      pantalla.setLed(0, i, j, true);
    }
  }

  server.send(200, "text/html", "");
}

void handleOffAll() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      states[i][j] = false;
      pantalla.setLed(0, i, j, false);
    }
  }

  server.send(200, "text/html", "");
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the LED pin as an output and turn it off initially
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Set Wi-Fi mode to Access Point and start the hotspot
  Serial.println("\nStarting Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  // Print the AP IP address (Defaults to 192.168.4.1)
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/toggleOn", handleOnAll);
  server.on("/toggleOff", handleOffAll);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

  fileSetup();

  setup_matrix();
}

void loop() {
  server.handleClient();
}


