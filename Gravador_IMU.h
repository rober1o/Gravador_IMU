#ifndef GRAVADOR_IMU // Previne múltiplas inclusões do cabeçalho
#define GRAVADOR_IMU

// ============================================================================
// Bibliotecas padrão C
// ============================================================================
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================================
// Bibliotecas Pico SDK
// ============================================================================
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/binary_info.h"

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/rtc.h"
#include "hardware/i2c.h"

// ============================================================================
// Bibliotecas auxiliares externas
// ============================================================================
#include "lib/ssd1306.h"
#include "lib/font.h"

// ============================================================================
// Bibliotecas do sistema de arquivos (FAT)
#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "rtc.h"
#include "sd_card.h"

// Biblioteca de depuração
#include "my_debug.h"

// ============================================================================
// Definições de pinos
// ============================================================================

// Botões
#define BOTAO_A 5
#define BOTAO_B 6

// Buzzer
#define BUZZER_PIN 10

// LEDs
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13

// I2C - MPU6050 (IMU)
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
static int addr = 0x68; // Endereço padrão do MPU6050

// I2C - Display OLED
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C // Endereço I2C do display OLED

// ============================================================================
// Variáveis globais e configurações
// ============================================================================

// Controle de debounce
#define DEBOUNCE_MS 400
absolute_time_t ultimo_acionamento_A = 0;
absolute_time_t ultimo_acionamento_B = 0;

// PWM
uint slice_num;

// Display OLED
ssd1306_t ssd;

// Arquivo de dados
static char filename[20] = "dados_IMU.csv";

// Flags de estado
static bool logger_enabled;
static const uint32_t period = 1000; // Intervalo de log em ms
static absolute_time_t next_log_time;

bool esta_montado =  false;
bool botao_a_pressionado = false;
bool botao_b_pressionado = false;
static volatile bool capturar_dados = false;
bool cor = true; // Usado para alternar estado de cor (LED/display)

// ============================================================================
// Dados da IMU (MPU6050)
// ============================================================================
int16_t aceleracao[3], gyro[3], temp;

char str_tmp[5];    // Temperatura
char str_acel_x[5]; // Aceleração X
char str_acel_y[5]; // Aceleração Y
char str_acel_z[5]; // Aceleração Z
char str_giro_x[5]; // Giroscópio X
char str_giro_y[5]; // Giroscópio Y
char str_giro_z[5]; // Giroscópio Z

// ============================================================================
// Prototipações de funções
// ============================================================================

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
void buzzer_beep(uint duracao_ms);

#endif // GRAVADOR_IMU
