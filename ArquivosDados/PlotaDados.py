import numpy as np
import matplotlib.pyplot as plt

# Carrega os dados (pula título e cabeçalho)
data = np.loadtxt('dados_IMU.csv', delimiter=';', skiprows=2)

# Colunas
tempo_s = data[:, 7]
accel_x = data[:, 1]
accel_y = data[:, 2]
accel_z = data[:, 3]
giro_x  = data[:, 4]
giro_y  = data[:, 5]
giro_z  = data[:, 6]

# Gráfico do Acelerômetro
plt.figure(figsize=(10, 5))
plt.plot(tempo_s, accel_x, 'r-', label='Aceleração X (m/s²)')
plt.plot(tempo_s, accel_y, 'g-', label='Aceleração Y (m/s²)')
plt.plot(tempo_s, accel_z, 'b-', label='Aceleração Z (m/s²)')
plt.title("Dados do Acelerômetro")
plt.xlabel("Tempo (s)")
plt.ylabel("Aceleração (m/s²)")
plt.legend()
plt.grid(True)

# Gráfico do Giroscópio
plt.figure(figsize=(10, 5))
plt.plot(tempo_s, giro_x, 'm-', label='Giro X (°/s)')
plt.plot(tempo_s, giro_y, 'c-', label='Giro Y (°/s)')
plt.plot(tempo_s, giro_z, 'y-', label='Giro Z (°/s)')
plt.title("Dados do Giroscópio")
plt.xlabel("Tempo (s)")
plt.ylabel("Velocidade Angular (°/s)")
plt.legend()
plt.grid(True)

plt.show()
