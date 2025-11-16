clc; 
clear; 
close all;

%% --- FASE 1: Definición del Modelo G(s) Basado en Análisis ---
K_real = 1133.2;    % Ganancia estática (RPM/Volt)
tau_real = 0.0054;  % Constante de tiempo (s)
t0_real = 6.331;    % Retardo experimental (s)

fprintf('--- Modelo de Planta G(s) Definido ---\n');
fprintf('Ganancia Estática (K) = %.2f RPM/Volt\n', K_real);
fprintf('Constante de Tiempo (tau) = %.4f s\n', tau_real);
fprintf('Retardo Experimental (t0) = %.3f s\n', t0_real);

% Crear la Función de Transferencia
Gs = tf(K_real, [tau_real 1]);

% NOTA: No se incluye el retraso experimental t0 en pidtune
% porque es un artefacto del experimento y no una propiedad real.
% Gs.InputDelay = t0_real;  <-- COMENTADO

%% --- FASE 2: Justificación del Tiempo de Muestreo (Ts) ---
fprintf('\n--- Justificación del Tiempo de Muestreo (Ts) ---\n');
fprintf('tau = %.4f s (%.1f ms)\n', tau_real, tau_real*1000);
fprintf('Ts requerido <= %.4f s (%.1f ms)\n', tau_real/10, (tau_real/10)*1000);
fprintf('Ts seleccionado = 0.0067 s (6.67 ms) -> fs = 150 Hz\n');

%% --- FASE 3: Sintonización del Controlador C(s) ---
% Sintonización de un controlador PID para el modelo G(s) sin retraso
C = pidtune(Gs, 'PID');

% Extraer y mostrar las ganancias
Kp = C.Kp;
Ki = C.Ki;
Kd = C.Kd;
Ti = Kp / Ki;

fprintf('\n--- Ganancias del Controlador ---\n');
fprintf('Usar estos valores en tu código .ino\n');
fprintf('Kp = %.6f\n', Kp);
fprintf('Ki = %.6f\n', Ki);
fprintf('Kd = %.6f\n', Kd);
fprintf('Ti (Tiempo Integral) = %.4f s\n', Ti);

%% --- FASE 4: Simulación del Lazo Cerrado ---
H = feedback(C*Gs, 1);  % Simulación sin el retraso artificial
figure;
step(H);
title('Respuesta Simulada (Modelo sin retraso artificial)');
xlabel('Tiempo (s)');
ylabel('Velocidad (RPM)');
grid on;
