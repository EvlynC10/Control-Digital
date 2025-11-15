#include <Controllino.h> 
#include "Stone_HMI_Define.h" // HMI STONE: Interfaz gráfica.
#include "Procesar_HMI.h" // Funciones para HMI.

// Pines:
const int pin_motor = CONTROLLINO_D0; // Salida PWM (Motor).
const int pin_encoder = CONTROLLINO_IN1; // Entrada de pulsos (Encoder).

// Variables de velocidad:
volatile unsigned long conteo_pulsos = 0; // Pulsos contados en Ts.
float rpm = 0; // Velocidad actual (RPM).
const uint16_t PULSOS_POR_REV = 36; // Pulsos por vuelta del encoder.

// Parámetros de tiempo:
const float fs = 20.0; // Frecuencia de muestreo (20 Hz).
const float Ts = 1.0 / fs; // Período de muestreo (0.05 s).

// Controlador PI:
float e[3] = {0, 0, 0}; // Almacena errores: e[0] (actual), e[1] (anterior), e[2] (ante-anterior).
float u = 0; // Acumulador de señal de control (integral).
float Kp = 0.3, Ki = 0.075, Kd = 0.3; // Ganancias PID.
float Ti = Kp / Ki; // Constante integral.
float Td = 0.0; 

float referencia = 0; // Setpoint de RPM.
float u_k = 0; // Señal de control final (PWM).

// Buffers de texto para la HMI:
char label2_text[10], label4_text[10], label7_text[10];
char label9_text[10], label11_text[10];

// Tiempos para control de ciclos:
unsigned long t_previo = 0; // Controla la actualización de la HMI (100 ms).
unsigned long t_previo1 = 0; // Controla la lectura de la referencia (10 ms).

void contarPulso(); // Función de interrupción del encoder.

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200); // Comunicación con la HMI.

    // Inicializa gráficos y slider en la HMI a 0.
    STONE_push_series("line_series", "line_series1", 0);
    STONE_push_series("line_series", "line_series2", 0);
    STONE_push_series("line_series", "line_series3", 0);
    Stone_HMI_Set_Value("slider", "slider1", NULL, 0);

    // Configuración de pines.
    pinMode(pin_encoder, INPUT);
    pinMode(pin_motor, OUTPUT);

    // Configura la interrupción: Llama a contarPulso() en cada pulso del encoder.
    attachInterrupt(digitalPinToInterrupt(pin_encoder), contarPulso, FALLING);

    // Configuración del Timer1 para interrupción periódica (Controlador).
    noInterrupts();
    TCCR1A = 0; TCCR1B = 0;
    TCCR1B |= (1 << WGM12); // Modo CTC.
    TCCR1B |= (1 << CS12); // Preescalador 256.
    OCR1A = 62500 / fs; // Establece el período Ts (20 Hz).
    TIMSK1 |= (1 << OCIE1A); // Habilita la interrupción.
    interrupts();

    HMI_init(); // Inicializa el procesamiento de comandos de la HMI.
}

void loop() {
    // Lee la referencia (setpoint) del slider de la HMI cada 10 ms.
    if (millis() - t_previo1 >= 10) { 
        referencia = HMI_get_value("slider", "slider1");
        t_previo1 = millis();
    }

    // Actualiza la HMI (textos y gráficos) cada 100 ms.
    if (millis() - t_previo >= 100) {
        t_previo = millis();
        
        // Conversión de valores a texto (float a string).
        dtostrf(referencia, 7, 2, label2_text);
        dtostrf(rpm, 7, 2, label4_text);
        // ... (resto de conversiones para ganancias Kp, Ki, Kd)

        // Envío de textos y datos de gráficos a la HMI.
        Stone_HMI_Set_Text("label", "label2", label2_text);
        // ... (resto de envíos de texto)
        STONE_push_series("line_series", "line_series1", referencia);
        STONE_push_series("line_series", "line_series2", rpm);
        STONE_push_series("line_series", "line_series3", u_k);
    }
}

// ISR del Timer1: Se ejecuta a la frecuencia de muestreo (20 Hz) para el control.
ISR(TIMER1_COMPA_vect) {
    TCNT1 = 0; // Reinicia el contador.

    // Cálculo de RPM: basado en los pulsos contados en el período Ts.
    float rpm_actual = (float(conteo_pulsos) * 60.0 * fs) / PULSOS_POR_REV;
    conteo_pulsos = 0; // Reinicia el contador para el siguiente ciclo.
    rpm = rpm_actual; 

    // Cálculo del error: Error actual = Referencia - RPM.
    e[2] = e[1]; e[1] = e[0];
    e[0] = referencia - rpm_actual;

    // Cálculo del incremento de la señal de control (delta_u) del PI.
    float delta_u = Kp*(e[0]-e[1]) + (Kp*Ts/Ti)*e[0];
    u += delta_u; // Acumula el incremento (Integral).

    // Satura la señal de control (u) entre 0 y 255.
    u_k = constrain(u, 0, 255);
    analogWrite(pin_motor, int(u_k)); // Aplica el PWM al motor.

    // Salida de debug por Serial.
    Serial.print("\nRPM: "); Serial.println(rpm);
    Serial.print("Señal de control: "); Serial.println(u_k);
}

// ISR de Interrupción Externa: Se ejecuta con cada pulso del encoder.
void contarPulso() {
    conteo_pulsos++; // Simplemente incrementa el contador.
}