load("solo_datos_identificacion_planta.mat");
data_ang = out.ang_fc(50:end);
data_u = out.u(50:end);
Ts = 1/50;
%%
% 1. Forzamos formato double y armamos el vector de tiempo
data_ang = double(data_ang);
data_u = double(data_u);
t = (0:length(data_ang)-1)' * Ts; 

% --- 2. Estimación Inicial (ARX) para no arrancar a ciegas ---
col1 = data_ang(1:end-2);  
col2 = data_ang(2:end-1);  
col3 = data_u(2:end-1);    
y_target = data_ang(3:end);

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
cost_function = @(x) norm(data_ang - lsim(tf(x(1), [1, abs(x(2))+abs(x(3)), abs(x(2))*abs(x(3))]), data_u, t))^2;

opciones = optimset('Display', 'off', 'MaxFunEvals', 3000, 'MaxIter', 3000);
x_opt = fminsearch(cost_function, x0, opciones);

% --- 4. Construcción de la Transferencia Continua Final ---
K_opt = x_opt(1);
p1_opt = abs(x_opt(2)); % Polo real estable 1
p2_opt = abs(x_opt(3)); % Polo real estable 2

% Construimos la planta expandiendo el denominador
Hs = tf(K_opt, [1, (p1_opt + p2_opt), (p1_opt * p2_opt)]);

% Mostrar los polos obtenidos en consola para verificar
disp('Polos de la planta identificada:');
disp(pole(Hs));

% --- 5. Gráfica de Validación ---
[y_sim, ~] = lsim(Hs, data_u, t);

figure;
plot(t, data_ang, 'b', 'LineWidth', 2); hold on;
plot(t, y_sim, 'r--', 'LineWidth', 1.5);
legend('Datos Reales Medidos', 'Modelo H(s) (2 polos reales, 0 ceros)', 'Location', 'best');
title('Identificación: Planta Continua Sobreamortiguada');
xlabel('Tiempo (s)');
ylabel('Amplitud');
grid on;