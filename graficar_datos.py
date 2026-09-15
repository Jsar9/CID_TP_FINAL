import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

init_time = 0 #secs
sampling_rate = 0.1 # secs (tiempo de muestreo analógico, el tiempo del OB30 en el PLC)

filename = "Mediciones_para_identificar.csv"

df = pd.read_csv(filename)

#Se extraen las columnas como listas y se convierten a Arrays de numpy
position = df.iloc[:, 0].to_numpy()
acceleration = df.iloc[:, 1].to_numpy()
pwm = df.iloc[:, 2].to_numpy()


#Se generan los tiempos
n_samples = len(position)
time = init_time + np.arange(n_samples) * sampling_rate
final_time = time[-1] if n_samples > 0 else 0

print(f"final_time: {final_time:.2f} secs")

fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

ax1.plot(time, position, color='blue')
ax1.set_title('Datos de la planta') 
ax1.set_ylabel('Posición')
ax1.grid(True)

ax2.plot(time, acceleration, color='red')
ax2.set_ylabel('Aceleración')
ax2.grid(True)

ax3.plot(time, pwm, color='green')
ax3.set_ylabel('PWM')
ax3.set_xlabel('Time (secs)') 
ax3.grid(True)

plt.tight_layout()

#Se marca el tiempo del ensayo
start_time = 2.4
end_time = 24

#Se crea la máscara para filtrar los datos del array
mask = (time >= start_time) & (time <= end_time)

#Se recortan los arrays de cada uno de los datos útiles obtenidos
time_cropped = time[mask]
pwm_cropped = pwm[mask]                 
acceleration_cropped = acceleration[mask]

#Se establece en 0 el inicio del array de tiempos
time_cropped = time_cropped - time_cropped[0]

#Se grafican los datos recortados
fig2, (ax4, ax5) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

ax4.plot(time_cropped, acceleration_cropped, color='red')
ax4.set_title('Datos de la planta recortados') 
ax4.set_ylabel('Aceleración')
ax4.grid(True)

ax5.plot(time_cropped, pwm_cropped, color='green')
ax5.set_xlabel('Time (secs)') 
ax5.set_ylabel('PWM')
ax5.grid(True)

plt.tight_layout()
plt.show()

#Guardado de los datos limpios
new_df = pd.DataFrame({
    'Time': time_cropped,
    'Acceleration': acceleration_cropped,
    'PWM': pwm_cropped
})

new_filename = "Mediciones_limpias.csv"

new_df.to_csv(new_filename, index=False)

print(f"Datos guardados exitosamente en: {new_filename}")