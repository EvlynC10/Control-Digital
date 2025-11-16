clc; 
clear; 
close all;

%% --- 1. CONFIGURACIÓN ---
nombre_archivo = 'practica4.txt';   % Nombre del archivo de datos
rpm_umbral_inicio = 1.0;            % RPM para detectar que el motor comenzó a moverse

%% --- 2. CARGA Y PROCESAMIENTO DE DATOS ---
fprintf('Cargando datos desde %s...\n', nombre_archivo);

% Cargar datos (se asume que hay 2 columnas: tiempo y RPM)
try
    data = readmatrix(nombre_archivo);
catch
    fprintf('Error: No se pudo leer el archivo "%s".\n', nombre_archivo);
    fprintf('Verifica que el archivo esté en la misma carpeta que este script.\n');
    return;
end

tiempo_bruto = data(:, 1);
rpm_bruto = data(:, 2);

% --- Limpieza de datos: eliminar el retraso inicial ---
indice_inicio = find(rpm_bruto > rpm_umbral_inicio, 1, 'first');

if isempty(indice_inicio)
    fprintf('Error: no se detectó movimiento del motor (RPM nunca superó %.1f).\n', rpm_umbral_inicio);
    return;
end

% Recortar los datos para incluir solo la respuesta al escalón
t_datos = tiempo_bruto(indice_inicio:end);
rpm_datos = rpm_bruto(indice_inicio:end);

% Renormalizar tiempo para que t=0 al inicio de la respuesta
t_ajustado = t_datos - t_datos(1); 
fprintf('Datos cargados y retraso inicial eliminado.\n');

%% --- 3. DEFINICIÓN DEL MODELO ---
% Modelo de primer orden: params(1) = K, params(2) = tau
modelo_motor = @(params, t) params(1) * (1 - exp(-t / params(2)));

%% --- 4. AJUSTE DE CURVA ---
K_inicial = max(rpm_datos);  % Suposición inicial para K
tau_inicial = 0.1;           % Suposición inicial para tau (0.1 s)
params_iniciales = [K_inicial, tau_inicial];

fprintf('Ejecutando ajuste de curva (lsqcurvefit)...\n');

options = optimoptions('lsqcurvefit', 'Display', 'off');
parametros_optimos = lsqcurvefit(modelo_motor, params_iniciales, t_ajustado, rpm_datos, [], [], options);

K_real = parametros_optimos(1);
tau_real = parametros_optimos(2);

%% --- 5. RESULTADOS ---
fprintf('\n--- Resultados del Ajuste ---\n');
fprintf('Ganancia Estática (K) = %.2f RPM/Volt\n', K_real);
fprintf('Constante de Tiempo (tau) = %.4f s (%.1f ms)\n', tau_real, tau_real*1000);
fprintf('----------------------------------\n');

%% --- 6. VALIDACIÓN GRÁFICA ---
fprintf('Generando gráfica de validación...\n');

rpm_modelo_ajustado = modelo_motor(parametros_optimos, t_ajustado);

figure;
hold on;
plot(t_ajustado, rpm_datos, 'b.', 'DisplayName', 'Datos Experimentales');
plot(t_ajustado, rpm_modelo_ajustado, 'r-', 'LineWidth', 2, 'DisplayName', 'Modelo Ajustado');
hold off;

title('Validación del Ajuste de Curva del Motor');
xlabel('Tiempo (s)');
ylabel('Velocidad (RPM)');
legend('Location', 'southeast');
grid on;
