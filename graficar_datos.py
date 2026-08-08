import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

init_time = 0 #secs
sampling_rate = 0.1 # secs (tiempo de muestreo analógico, el tiempo del OB30 en el PLC)

filename = "primera_lectura.csv"

df = pd.read_csv(filename)

#Se extrae la primer columna como una lista y se convierte a Array de Numpy
amplitude = df.iloc[:, 0].to_numpy()

#Se imprimen los tamaños
print(df.shape)
print(amplitude.shape)

#Se genera un array de Numpy para el array de tiempos
n_samples = len(amplitude)
final_time = init_time + n_samples * sampling_rate
time = np.arange(init_time, final_time, sampling_rate)

print(final_time)

plt.plot(time,amplitude)
plt.xlabel('Time (secs)')
plt.ylabel('Amplitude')
plt.show()