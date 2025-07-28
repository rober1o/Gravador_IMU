import numpy as np
import matplotlib.pyplot as plt

# Carrega os dados (pula as 2 primeiras linhas: título e cabeçalho)
data = np.loadtxt('dados_IMU.csv', delimiter=';', skiprows=2)

# Separando as colunas
amostras = data[:, 0]
accel_x = data[:, 1]
accel_y = data[:, 2]
accel_z = data[:, 3]
giro_x  = data[:, 4]
giro_y  = data[:, 5]
giro_z  = data[:, 6]

# Primeiro gráfico: Acelerômetro
plt.figure(figsize=(10, 5))
plt.plot(amostras, accel_x, 'r-', label='Aceleração X')
plt.plot(amostras, accel_y, 'g-', label='Aceleração Y')
plt.plot(amostras, accel_z, 'b-', label='Aceleração Z')
plt.title("Dados do Acelerômetro")
plt.xlabel("Número da Amostra")
plt.ylabel("Valor bruto do acelerômetro")
plt.legend()
plt.grid(True)

# Segundo gráfico: Giroscópio
plt.figure(figsize=(10, 5))
plt.plot(amostras, giro_x, 'm-', label='Giro X')
plt.plot(amostras, giro_y, 'c-', label='Giro Y')
plt.plot(amostras, giro_z, 'y-', label='Giro Z')
plt.title("Dados do Giroscópio")
plt.xlabel("Número da Amostra")
plt.ylabel("Valor bruto do giroscópio")
plt.legend()
plt.grid(True)

plt.show()
