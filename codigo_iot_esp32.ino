// Definiciones para Blynk 2.0
#define BLYNK_TEMPLATE_ID "TMPL2HW8uMZFF"
#define BLYNK_TEMPLATE_NAME "Tesis"
#define BLYNK_AUTH_TOKEN "SX5_tuMRs4T1xO0FBioWMrmeH4oQ9e27"

// Librerías necesarias
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Credenciales de WiFi
char ssid[] = "YOUR_WIFI_SSID";        // Reemplaza con el nombre de tu red WiFi
char pass[] = "YOUR_WIFI_PASSWORD";    // Reemplaza con la contraseña de tu red WiFi

// Definición de pines
#define RELAY_PUMP 22  // Pin GPIO22 para controlar la bomba (IN2 del relé)
#define RELAY_VALVE 23 // Pin GPIO23 para controlar la válvula (IN1 del relé)

// Variables para tiempos de encendido (en segundos)
int pumpTime = 10;  // Tiempo predeterminado de encendido de la bomba
int valveTime = 10; // Tiempo predeterminado de encendido de la válvula

// Variables para control de estado
bool pumpActive = false;
bool valveActive = false;
unsigned long pumpStartTime = 0;
unsigned long valveStartTime = 0;

// Configuración inicial
void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Configurar los pines como salidas
  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_VALVE, OUTPUT);

  // Inicialmente, apagar ambos relés (relé activo en LOW)
  digitalWrite(RELAY_PUMP, HIGH);
  digitalWrite(RELAY_VALVE, HIGH);

  // Conectar a Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Conectando a Blynk...");
}

// Función para controlar la bomba desde Blynk (Pin Virtual V0)
BLYNK_WRITE(V0) {
  int value = param.asInt(); // Obtener el valor del pin virtual V0
  if (value == 1 && !pumpActive) {
    digitalWrite(RELAY_PUMP, LOW); // Encender bomba (relé activo en LOW)
    pumpStartTime = millis();
    pumpActive = true;
    Serial.println("Bomba encendida");
    Blynk.logEvent("info", "Bomba encendida");
  } else if (value == 0 && pumpActive) {
    digitalWrite(RELAY_PUMP, HIGH); // Apagar bomba
    pumpActive = false;
    Serial.println("Bomba apagada");
    Blynk.logEvent("info", "Bomba apagada");
  }
  // Verificar si la bomba no se encendió correctamente
  if (value == 1 && digitalRead(RELAY_PUMP) != LOW) {
    Blynk.logEvent("error", "¡Error! La bomba no se encendió");
    Serial.println("¡Error! La bomba no se encendió");
  }
}

// Función para controlar la válvula desde Blynk (Pin Virtual V1)
BLYNK_WRITE(V1) {
  int value = param.asInt(); // Obtener el valor del pin virtual V1
  if (value == 1 && !valveActive) {
    digitalWrite(RELAY_VALVE, LOW); // Encender válvula (relé activo en LOW)
    valveStartTime = millis();
    valveActive = true;
    Serial.println("Válvula encendida");
    Blynk.logEvent("info", "Válvula encendida");
  } else if (value == 0 && valveActive) {
    digitalWrite(RELAY_VALVE, HIGH); // Apagar válvula
    valveActive = false;
    Serial.println("Válvula apagada");
    Blynk.logEvent("info", "Válvula apagada");
  }
  // Verificar si la válvula no se encendió correctamente
  if (value == 1 && digitalRead(RELAY_VALVE) != LOW) {
    Blynk.logEvent("error", "¡Error! La válvula no se encendió");
    Serial.println("¡Error! La válvula no se encendió");
  }
}

// Función para ajustar el tiempo de encendido de la bomba (Pin Virtual V2)
BLYNK_WRITE(V2) {
  pumpTime = param.asInt(); // Obtener el valor del slider (en segundos)
  Serial.print("Tiempo de encendido de la bomba ajustado a: ");
  Serial.print(pumpTime);
  Serial.println(" segundos");
}

// Función para ajustar el tiempo de encendido de la válvula (Pin Virtual V3)
BLYNK_WRITE(V3) {
  valveTime = param.asInt(); // Obtener el valor del slider (en segundos)
  Serial.print("Tiempo de encendido de la válvula ajustado a: ");
  Serial.print(valveTime);
  Serial.println(" segundos");
}

// Bucle principal
void loop() {
  Blynk.run(); // Ejecutar Blynk

  // Control de tiempo para la bomba
  if (pumpActive && (millis() - pumpStartTime >= pumpTime * 1000)) {
    digitalWrite(RELAY_PUMP, HIGH); // Apagar bomba
    pumpActive = false;
    Blynk.virtualWrite(V0, 0); // Actualizar estado en Blynk
    Serial.println("Bomba apagada por fin de temporizador");
    Blynk.logEvent("info", "Bomba apagada por fin de temporizador");
  }

  // Control de tiempo para la válvula
  if (valveActive && (millis() - valveStartTime >= valveTime * 1000)) {
    digitalWrite(RELAY_VALVE, HIGH); // Apagar válvula
    valveActive = false;
    Blynk.virtualWrite(V1, 0); // Actualizar estado en Blynk
    Serial.println("Válvula apagada por fin de temporizador");
    Blynk.logEvent("info", "Válvula apagada por fin de temporizador");
  }
}