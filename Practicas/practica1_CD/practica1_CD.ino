#include <Controllino.h> // Librería del Controllino

// --- Variables de Temporización (No Bloqueante) ---
unsigned long t_previo = 0;   // Último tiempo de cambio
unsigned long intervalo = 500; // Retardo (ms)

// --- Arrays y Punteros ---
int leds[9] = { // Pines digitales de los LEDs
  CONTROLLINO_D0,
  CONTROLLINO_D1,
  CONTROLLINO_D2,
  CONTROLLINO_D6,
  CONTROLLINO_D7,
  CONTROLLINO_D8,
  CONTROLLINO_D12,
  CONTROLLINO_D13,
  CONTROLLINO_D14
};

int secuencia[9] = {0, 3, 6, 7, 8, 5, 2, 1, 4}; // Define el orden de encendido (índices)

int* ptrLeds = leds;         // Puntero a la lista de pines
int* ptrSecuencia = secuencia; // Puntero a la secuencia de índices

int indice = 0; // Índice actual de la secuencia

void setup() {
  // Inicialización: Configura pines como OUTPUT y apaga todos los LEDs
  for (int i = 0; i < 9; i++) {
    pinMode(*(ptrLeds + i), OUTPUT);
    digitalWrite(*(ptrLeds + i), LOW);
  }
  Serial.begin(9600);
}

void loop() {
  unsigned long t_actual = millis(); // Obtiene el tiempo actual

  // --- Lógica de Temporización ---
  // Comprueba si ya pasó el tiempo de intervalo
  if (t_actual - t_previo >= intervalo) {
    t_previo = t_actual; // Actualiza el tiempo

    // --- Control de Secuencia (Punteros) ---

    // Apaga el LED que estaba encendido anteriormente
    int pinAnterior = *(ptrLeds + *(ptrSecuencia + ((indice + 8) % 9))); 
    digitalWrite(pinAnterior, LOW);

    // Enciende el LED actual según la secuencia
    int pinActual = *(ptrLeds + *(ptrSecuencia + indice));
    digitalWrite(pinActual, HIGH);
    
    // Avanza al siguiente paso de la secuencia (circular)
    indice = (indice + 1) % 9;
  }
}