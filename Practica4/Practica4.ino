/* ==========================================================
   Control PID de velocidad para motor con visualización HMI STONE
   Descripción: Regula la velocidad del motor según referencia
   establecida desde la HMI.
   ========================================================== */

// === LIBRERÍAS ===
#include <Controllino.h>            // Librería Controllino
#include "Stone_HMI_Define.h"       // Definiciones HMI STONE
#include "Procesar_HMI.h"           // Funciones para comunicación HMI

// === Pines y Variables ===
const int pin_motor = CONTROLLINO_D0;
const int pin_encoder = CONTROLLINO_IN1;

volatile unsigned long conteo_pulsos = 0;
float rpm = 0;
const uint16_t PULSOS_POR_REV = 36;

// === Temporización ===
const float fs = 20.0;     // Hz
const float Ts = 1.0 / fs; // s

// === Variables PID ===
float e[3] = {0, 0, 0};    // Error actual y anteriores
float u = 0;                // Señal de control acumulada
float Kp = 0.3, Ki = 0.075, Kd = 0.3;
float Ti = Kp / Ki;
float Td = 0.0;

float referencia = 0;
float u_k = 0;

// === Variables para HMI ===
char label2_text[10], label4_text[10], label7_text[10];
char label9_text[10], label11_text[10];

// === Temporizadores internos ===
unsigned long t_previo = 0;  // Actualización HMI cada 100 ms
unsigned long t_previo1 = 0; // Lectura referencia cada 10 ms

// === PROTOTIPO ISR ===
void contarPulso();  // Interrupción del encoder

// ==========================================================
// SETUP
// ==========================================================
void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);

    // Inicialización de gráficas en HMI
    STONE_push_series("line_series", "line_series1", 0);
    STONE_push_series("line_series", "line_series2", 0);
    STONE_push_series("line_series", "line_series3", 0);

    // Slider referencia
    Stone_HMI_Set_Value("slider", "slider1", NULL, 0);

    // Pines
    pinMode(pin_encoder, INPUT);
    pinMode(pin_motor, OUTPUT);

    // Interrupción externa para encoder
    attachInterrupt(digitalPinToInterrupt(pin_encoder), contarPulso, FALLING);

    // Configuración Timer1 para interrupción cada Ts
    noInterrupts();
    TCCR1A = 0; TCCR1B = 0;
    TCCR1B |= (1 << WGM12);      // Modo CTC
    TCCR1B |= (1 << CS12);       // Prescaler 256
    OCR1A = 62500 / fs;          // Valor comparador
    TIMSK1 |= (1 << OCIE1A);     // Habilita interrupción
    interrupts();

    HMI_init(); // Inicializar comunicación HMI
}

// ==========================================================
// LOOP PRINCIPAL
// ==========================================================
void loop() {
    // Leer referencia HMI cada 10 ms
    if (millis() - t_previo1 >= 10) {
        referencia = HMI_get_value("slider", "slider1");
        t_previo1 = millis();
    }

    // Actualizar HMI cada 100 ms
    if (millis() - t_previo >= 100) {
        t_previo = millis();
        dtostrf(referencia, 7, 2, label2_text);
        dtostrf(rpm, 7, 2, label4_text);
        dtostrf(Kp, 7, 2, label7_text);
        dtostrf(Ki, 7, 2, label9_text);
        dtostrf(Kd, 7, 2, label11_text);

        Stone_HMI_Set_Text("label", "label2", label2_text);
        Stone_HMI_Set_Text("label", "label4", label4_text);
        Stone_HMI_Set_Text("label", "label7", label7_text);
        Stone_HMI_Set_Text("label", "label9", label9_text);
        Stone_HMI_Set_Text("label", "label11", label11_text);

        STONE_push_series("line_series", "line_series1", referencia);
        STONE_push_series("line_series", "line_series2", rpm);
        STONE_push_series("line_series", "line_series3", u_k);
    }
}

// ==========================================================
// ISR TIMER1: Calcula RPM, actualiza PID y PWM
// ==========================================================
ISR(TIMER1_COMPA_vect) {
    TCNT1 = 0;

    float rpm_actual = (float(conteo_pulsos) * 60.0 * fs) / PULSOS_POR_REV;
    conteo_pulsos = 0;
    rpm = rpm_actual;

    e[2] = e[1]; e[1] = e[0];
    e[0] = referencia - rpm_actual;

    // PID incremental
    float delta_u = Kp*(e[0]-e[1]) + (Kp*Ts/Ti)*e[0];
    u += delta_u;

    u_k = constrain(u, 0, 255);
    analogWrite(pin_motor, int(u_k));

    Serial.print("\nRPM: "); Serial.println(rpm);
    Serial.print("Señal de control: "); Serial.println(u_k);
}

// ==========================================================
// ISR ENCODER
// ==========================================================
void contarPulso() {
    conteo_pulsos++;
}
