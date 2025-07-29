# 📦 GRAVADOR_IMU

Este projeto visa desenvolver um **datalogger interativo de movimento**, que registra dados de **aceleração** e **giroscópio** em tempo real. Utilizando o sensor **MPU6050** e a plataforma **BitDo-gLab**, os dados são armazenados em um **cartão microSD**, com exibição simultânea de informações referente ao sistema em um **display OLED**.

O sistema é controlado por **botões físicos** e fornece **feedback visual e sonoro**, oferecendo ao usuário uma experiência **intuitiva e segura** durante a coleta de dados. Os dados podem ser analisados posteriormente em **planilhas** ou através de um **script Python gráfico**.

---

## 🧰 Componentes Utilizados

1. 🔘 **Botão Pushbutton**  
2. 🖥️ **Display OLED 1306**  
3. 🔊 **Buzzer**  
4. 🧭 **Sensor IMU MPU6050**  
5. 🌈 **LED RGB**  
6. 💾 **Módulo de cartão microSD**  

---

## ⚙️ Funcionalidades

- ✅ Exibição em tempo real do estado do sistema:
  - **Necessita de montagem**
  - **Pronto para captura**
  - **Capturando dados**

- 📈 Captura de **dados inerciais** (aceleração e giroscópio) e armazenamento em **formato CSV**

- 📊 Visualização dos dados:
  - Em **planilhas** (como Excel, LibreOffice)
  - Ou em **gráficos interativos** via o script `PlotaDados.py`

---

## 🚀 Como Usar

1. Clone este repositório:

   ```bash
   git clone https://github.com/rober1o/Gravador_IMU.git
   ```

2. Importe o projeto na extensão **Raspberry Pi Pico** no VS Code ou editor compatível.

3. Compile o projeto.

4. Conecte a **BitDo-gLab** via cabo apropriado (USB).

---

## 🎥 Demonstração

Vídeo demonstrando as funcionalidades da solução implementada:  
[🔗 Assista no YouTube](https://youtu.be/q46sUjKzfUE)
