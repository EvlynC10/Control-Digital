# 🛠️ Control-Digital: Automatización y Sistemas de Control ⚙️

Implementación y control de sistemas de automatización usando PLCs industriales (**Controllino Mega** y **ESP32**) y la interfaz **HMI Stone**. Se exploran el manejo de salidas digitales, control PWM y sistemas de control de velocidad PID.

<img width="1310" height="840" alt="tablero" src="https://github.com/user-attachments/assets/5f872442-eb84-42c8-b2a6-8eb4e683ea5c" />

---

## 🔬 Componentes del Tablero de Control

El proyecto se desarrolla sobre esta plataforma didáctica, que permite la integración de hardware y software de automatización:

* **Controladores:** **Controllino Mega** y **ESP32 PLC 14**.
* **Interfaz HMI:** **HMI Stone** para visualización y control.
* **Alimentación:** Fuente **24V DC**.
* **Conexiones:** Borneras para relés, Entradas/Salidas Digitales y Analógicas.
* **Prototipado:** Protoboards de conexión.

---

## ✨ Tecnologías y Metodología Clave

* **PLCs / Microcontroladores:** **Controllino Mega** y **ESP32 PLC 14** (programados en **Arduino IDE**).
* **Interfaz Humano-Máquina (HMI):** **Stone HMI** (para monitoreo y control en tiempo real a través de comunicación serial).
* **Actuadores/Sensores:** LEDs indicadores, botones físicos y un **Motor DC con encoder**.
* **Metodología de Control:**
    * Temporización **No Bloqueante** (`millis()`).
    * Uso de **Aritmética de Punteros**.
    * Control **PID** (Proporcional-Integral-Derivativo) implementado discretamente.

---

## 🎯 Objetivos Principales del Proyecto

Este trabajo documenta la implementación de los siguientes sistemas de automatización y control:

1.  Implementar secuencias de control para salidas digitales usando temporización no bloqueante y **aritmética de punteros**.
2.  Desarrollar una **Interfaz Gráfica (GUI)** en HMI para el control independiente de dos salidas **PWM**.
3.  Implementar un controlador **PID discreto** para la regulación de velocidad (RPM) de un motor DC, incluyendo adquisición de datos por **interrupciones**.

---

## 🚀 Logros Destacados por Práctica

### Práctica 1: Secuencias de LEDs y Punteros
* Implementación de una secuencia de **9 LEDs en patrón espiral**.
* Uso de **aritmética de punteros** para configurar pines y acceder a la secuencia de forma indirecta.
* Lógica de **temporización no bloqueante** para el ciclo de encendido/apagado.

### Práctica 3: Control PWM e HMI
* Desarrollo de una interfaz con dos **SpinBoxes** en el HMI para ajustar el **Duty Cycle (0-100%)** de dos LEDs de forma independiente.
* La lógica garantiza la **independencia** del control: el **botón físico** actúa como *toggle* (encendido/apagado), anulando el brillo si el LED está deshabilitado.

### Práctica 4: Control de Velocidad PID
* **Modelado del Sistema:** Obtención de la Función de Transferencia del Motor DC: $$G(s)=\frac{4.444}{0.0054s+1}$$.
* **Adquisición de Datos Precisa:** Uso de **interrupciones externas** para el conteo de pulsos del encoder y una **interrupción por temporizador** ($T_s=6.7\text{ ms}$) para la periodicidad del control.
* **Verificación:** El controlador demostró **estabilidad**, **bajo sobrepaso** y un error en estado estacionario **inferior**, verificando la robustez ante perturbaciones.

---

## 📁 Contenido del Repositorio
El código fuente de las prácticas y la documentación se encuentran en las siguientes carpetas:

-   [**Practica 1**](Practicas/practica1_CD/)
-   [**Practica 3**](Practicas/practica3_CD/)
-   [**Practica 4**](Practicas/practica4_CD/)

---

## 📧 Autores
Para cualquier consulta o sugerencia sobre el proyecto, pueden contactar a los autores:

* **Evelyn Nayeli Cabrera Jara:** evelyn.cabrera@ucuenca.edu.ec
* **John Rogelio Molina León:** john.molina@ucuenca.edu.ec
