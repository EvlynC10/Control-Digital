#include <Controllino.h>      // Librería principal del PLC Controllino
#include "Stone_HMI_Define.h" // Librería para la comunicación con el HMI
#include "Procesar_HMI.h"     // Librería auxiliar para la lectura de datos del HMI

// --- Configuración de Pines ---
const int led1 = CONTROLLINO_D0; // Salida PWM para LED 1
const int led2 = CONTROLLINO_D1; // Salida PWM para LED 2
const int button1 = CONTROLLINO_I16; // Entrada digital para Botón 1 (ON/OFF LED 1)
const int button2 = CONTROLLINO_I17; // Entrada digital para Botón 2 (ON/OFF LED 2)

// --- Variables de Estado y Valor ---
bool led1_enabled = false;           // Estado de habilitación del LED 1
bool led2_enabled = false;           // Estado de habilitación del LED 2

int pwmValue1 = 0, pwmValue2 = 0;    // Valor PWM (0 a 255)
float dutyCyclePercent1 = 0, dutyCyclePercent2 = 0; // Duty Cycle leído de HMI (0 a 100)

void setup() {
  Serial.begin(115200);   // Inicializa comunicación Serial para depuración
  Serial2.begin(115200);  // Inicializa comunicación con el HMI
  
  // Configura pines como salida (PWM) y entrada
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  
  HMI_init(); // Inicializa la comunicación HMI
  
  // Inicializa los spin_box de la HMI en 0
  Stone_HMI_Set_Value("spin_box", "spin_box1", NULL, 0); 
  Stone_HMI_Set_Value("spin_box", "spin_box2", NULL, 0); 
}

void loop() {
  // --- Lectura de Duty Cycle desde HMI ---
  dutyCyclePercent1 = HMI_get_value("spin_box", "spin_box1"); 
  dutyCyclePercent2 = HMI_get_value("spin_box", "spin_box2"); 

  // Mapea el porcentaje (0-100) al valor PWM (0-255)
  if (dutyCyclePercent1 >= 0 && dutyCyclePercent1 <= 100)
    pwmValue1 = map(dutyCyclePercent1, 0, 100, 0, 255); 

  if (dutyCyclePercent2 >= 0 && dutyCyclePercent2 <= 100)
    pwmValue2 = map(dutyCyclePercent2, 0, 100, 0, 255); 

  // --- Lógica de Botones (Antirrebote y Toggle) ---

  static bool lastButton1State = LOW; // Estado anterior del botón 1
  static bool lastButton2State = LOW; // Estado anterior del botón 2

  bool currentButton1State = digitalRead(button1);
  bool currentButton2State = digitalRead(button2);

  // Detecta flanco de subida del Botón 1 para alternar el estado (toggle)
  if (currentButton1State == HIGH && lastButton1State == LOW) {
    led1_enabled = !led1_enabled; 
    Serial.print("LED1 estado: ");
    Serial.println(led1_enabled ? "ENCENDIDO" : "APAGADO");
  }

  // Detecta flanco de subida del Botón 2 para alternar el estado (toggle)
  if (currentButton2State == HIGH && lastButton2State == LOW) {
    led2_enabled = !led2_enabled; 
    Serial.print("LED2 estado: ");
    Serial.println(led2_enabled ? "ENCENDIDO" : "APAGADO");
  }

  lastButton1State = currentButton1State; // Actualiza estado anterior de Botones
  lastButton2State = currentButton2State; 

  // --- Control de Salida PWM ---
  
  // Controla LED 1
  if (led1_enabled)
    analogWrite(led1, pwmValue1); // Aplica PWM si está habilitado
  else
    analogWrite(led1, 0);         // Apaga LED si está deshabilitado

  // Controla LED 2
  if (led2_enabled)
    analogWrite(led2, pwmValue2); // Aplica PWM si está habilitado
  else
    analogWrite(led2, 0);         // Apaga LED si está deshabilitado
}