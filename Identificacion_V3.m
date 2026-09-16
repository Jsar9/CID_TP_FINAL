filename = 'Mediciones_limpias.csv'; 

% readtable lee el CSV incluyendo los nombres de las columnas
datos_csv = readtable(filename);

t = datos_csv{:, 1};        % Columna 1: Tiempo
data_ang = datos_csv{:, 2}; % Columna 2: Salida (Aceleración)
data_u = datos_csv{:, 3};   % Columna 3: Entrada (PWM)

Ts = 0.1;
%%
% 1. Forzamos formato double y armamos el vector de tiempo
data_ang = double(data_ang);
data_u = double(data_u);

u_eq = 13824; y_eq = mean(data_ang(data_u==u_eq)); % o el reposo que corresponda
delta_u = data_u - u_eq;
delta_y = data_ang - y_eq;

%%
% Hace lo mismo que el ARX + optimización pero maneja mejor el offset  
data_id = iddata(delta_y, delta_u, Ts);
modelo1 = tfest(data_id, 1, 0);          % 1 polo, sin ceros
modelo2 = tfest(data_id, 2, 0);          % 2 polos, sin ceros
modelo2_delay = tfest(data_id, 2, 0, 'InputDelay', 1);
compare(data_id, modelo1, modelo2, modelo2_delay)

disp('Polos modelo1 (1 polo):');
disp(pole(modelo1));

disp('Polos modelo2 (2 polos):');
disp(pole(modelo2));

disp('Polos modelo2_delay (2 polos + delay):');
disp(pole(modelo2_delay));
%%
% --- 2. Estimación Inicial (ARX) para no arrancar a ciegas ---
col1 = delta_y(1:end-2);  
col2 = delta_y(2:end-1);  
col3 = delta_u(2:end-1);    
y_target = delta_y(3:end);

matriz_datos = [col1, col2, col3];
Const_ARX = pinv(matriz_datos) * y_target;   % [c1; c2; alfa]

% Pasamos este primer intento a continuo para extraer parámetros semilla
Gz_arx = tf([Const_ARX(3) 0], [1, -Const_ARX(2), -Const_ARX(1)], Ts);
Hs_arx = d2c(Gz_arx, "tustin");
[num_arx, den_arx] = tfdata(Hs_arx, 'v');

% Extraemos las raíces del ARX para usarlas como semillas
K_ini = num_arx(end); 
polos_arx = roots(den_arx);

% Obligamos a que las semillas iniciales sean reales y positivas (estables)
p1_ini = abs(real(polos_arx(1)));
p2_ini = abs(real(polos_arx(2)));

% Si el ARX dio polos complejos conjugados, p1_ini y p2_ini serán iguales.
% Le damos una minúscula separación para ayudar al gradiente del optimizador.
if p1_ini == p2_ini
    p2_ini = p2_ini * 1.05; 
end

% Vector inicial: [Ganancia, Polo_Real_1, Polo_Real_2]
x0 = double([K_ini, p1_ini, p2_ini]); 

% --- 3. Optimización Directa de H(s) Minimizando Error ---
% x(1) = K,  x(2) = p1,  x(3) = p2
% Denominador: (s + |p1|)*(s + |p2|) = s^2 + (|p1|+|p2|)*s + (|p1|*|p2|)
cost_function = @(x) norm(delta_y - lsim(tf(x(1), [1, abs(x(2))+abs(x(3)), abs(x(2))*abs(x(3))]), delta_u, t))^2;

opciones = optimset('Display', 'off', 'MaxFunEvals', 3000, 'MaxIter', 3000);
x_opt = fminsearch(cost_function, x0, opciones);

% --- 4. Construcción de la Transferencia Continua Final ---
K_opt = x_opt(1);
p1_opt = x_opt(2); % Polo real estable 1
p2_opt = x_opt(3); % Polo real estable 2

% Construimos la planta expandiendo el denominador
Hs = tf(K_opt, [1, (p1_opt + p2_opt), (p1_opt * p2_opt)]);

% Mostrar los polos obtenidos en consola para verificar
disp('Polos de la planta identificada:');
disp(pole(Hs));
%%
% --- 5. Gráfica de Validación ---
[delta_y_sim, ~] = lsim(Hs, delta_u, t);   % simular con delta_u, no data_u
y_sim = delta_y_sim + y_eq;                % devolver el offset para comparar en escala real

figure;
plot(t, data_ang, 'b', 'LineWidth', 2); hold on;
plot(t, y_sim, 'r--', 'LineWidth', 1.5);
legend('Datos Reales Medidos', 'Modelo H(s) (2 polos reales, 0 ceros)', 'Location', 'best');
title('Identificación: Planta Continua Sobreamortiguada');
xlabel('Tiempo (s)'); ylabel('Amplitud'); grid on;