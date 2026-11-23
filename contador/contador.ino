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

const int FILAS = 5;
const int COLUMNAS = 5;

const int VELOCIDAD = 150; 

const int TOTAL_DIAGONALES = FILAS + COLUMNAS - 1;

int id_anim = -1;

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


// Handler for the root URL ("/")
void handleRoot() {
  server.send(200, "text/html", html);
}

// Handler for toggling the LED ("/toggle")
void handleToggle() {
  id_anim = -1;
  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  bool state = states[x][y];
  states[x][y] = !state;
  pantalla.setLed(0, x, y, !state);
  
  //server.sendHeader("Location", "/");
  //server.send(303);
  server.send(200, "text/html", "");
}

void handleAnim() {
  id_anim = server.arg("id").toInt();
  server.send(200, "text/html", "");
}

void handleOnAll() {
  id_anim = -1;
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      states[i][j] = true;
      pantalla.setLed(0, i, j, true);
    }
  }

  server.send(200, "text/html", "");
}

void handleOffAll() {
  id_anim = -1;

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
  server.on("/anim", handleAnim);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

  fileSetup();

  setup_matrix();
}

void anim1() {
  // 1. Animación hacia ABAJO (de la fila 0 a la 4)
  for (int y = 0; y < FILAS; y++) {
    dibujarLineaHorizontal(y, true);  // Enciende la línea
    delay(VELOCIDAD);
    dibujarLineaHorizontal(y, false); // Apaga la línea antes de pasar a la siguiente
  }

  // 2. Animación hacia ARRIBA (de la fila 3 a la 1)
  // Empezamos en 3 y terminamos en 1 para no repetir los extremos (4 y 0) y que el rebote sea fluido
  for (int y = FILAS - 2; y > 0; y--) {
    dibujarLineaHorizontal(y, true);  // Enciende la línea
    delay(VELOCIDAD);
    dibujarLineaHorizontal(y, false); // Apaga la línea
  }
}

void anim2() {
  // 1. Animación hacia la DERECHA (de la columna 0 a la 4)
  for (int x = 0; x < COLUMNAS; x++) {
    dibujarLineaVertical(x, true);  // Enciende la línea vertical
    delay(VELOCIDAD);
    dibujarLineaVertical(x, false); // Apaga la línea antes de pasar a la siguiente
  }

  // 2. Animación hacia la IZQUIERDA (de la columna 3 a la 1)
  // Evita repetir los extremos (4 y 0) para mantener un rebote fluido
  for (int x = COLUMNAS - 2; x > 0; x--) {
    dibujarLineaVertical(x, true);  // Enciende la línea vertical
    delay(VELOCIDAD);
    dibujarLineaVertical(x, false); // Apaga la línea
  }
}

void anim3() {
  // 1. Barrido diagonal de ARRIBA-IZQUIERDA a ABAJO-DERECHA (diagonales 0 a 8)
  for (int d = 0; d < TOTAL_DIAGONALES; d++) {
    dibujarDiagonal(d, true);  // Enciende la diagonal
    delay(VELOCIDAD);
    dibujarDiagonal(d, false); // Apaga la diagonal
  }

  // 2. Barrido de regreso de ABAJO-DERECHA a ARRIBA-IZQUIERDA (diagonales 7 a 1)
  // Evita repetir las esquinas (8 y 0) para mantener un rebote fluido
  for (int d = TOTAL_DIAGONALES - 2; d > 0; d--) {
    dibujarDiagonal(d, true);  // Enciende la diagonal
    delay(VELOCIDAD);
    dibujarDiagonal(d, false); // Apaga la diagonal
  }
}

void dibujarDiagonal(int d, bool estado) {
  for (int x = 0; x < COLUMNAS; x++) {
    for (int y = 0; y < FILAS; y++) {
      if (x + y == d) {
        pantalla.setLed(0, x, y, estado);
      }
    }
  }
}

void anim4() {
  // 1. Expansión: del centro hacia las orillas (pasos 0 a 3)
  for (int paso = 0; paso < 4; paso++) {
    dibujarCirculo(paso, true);  // Enciende el anillo
    delay(VELOCIDAD);
    dibujarCirculo(paso, false); // Apaga el anillo
  }

  // 2. Contracción: de regreso hacia el centro (pasos 2 a 1)
  // Evita repetir los extremos (3 y 0) para mantener un pulso fluido
  for (int paso = 2; paso > 0; paso--) {
    dibujarCirculo(paso, true);  // Enciende el anillo
    delay(VELOCIDAD);
    dibujarCirculo(paso, false); // Apaga el anillo
  }
}

void dibujarCirculo(int paso, bool estado) {
  for (int x = 0; x < COLUMNAS; x++) {
    for (int y = 0; y < FILAS; y++) {
      // Calculamos la distancia al cuadrado desde el centro (dx² + dy²)
      int dx = x - 2;
      int dy = y - 2;
      int distCuadrada = (dx * dx) + (dy * dy);

      // Asignamos cada LED al anillo que le corresponde según su distancia real
      if (paso == 0 && distCuadrada == 0) {
        pantalla.setLed(0, x, y, estado); // Centro exacto
      } 
      else if (paso == 1 && distCuadrada == 1) {
        pantalla.setLed(0, x, y, estado); // Cruz interior (distancia = 1)
      } 
      else if (paso == 2 && (distCuadrada == 2 || distCuadrada == 4)) {
        pantalla.setLed(0, x, y, estado); // Círculo principal (diagonales internas y bordes medios)
      } 
      else if (paso == 3 && (distCuadrada == 5 || distCuadrada == 8)) {
        pantalla.setLed(0, x, y, estado); // Orillas exteriores y esquinas
      }
    }
  }
}

// Función auxiliar para encender o apagar toda una fila (eje X)
void dibujarLineaHorizontal(int y, bool estado) {
  for (int x = 0; x < COLUMNAS; x++) {
    pantalla.setLed(0, x, y, estado);
  }
}

void dibujarLineaVertical(int x, bool estado) {
  for (int y = 0; y < FILAS; y++) {
    pantalla.setLed(0, x, y, estado);
  }
}

void loop() {
  server.handleClient();

  if(id_anim == 1)
    anim1();
  else if(id_anim == 2)
    anim2();
  else if(id_anim == 3)
    anim3();
  else if(id_anim == 4)
    anim4();
}


