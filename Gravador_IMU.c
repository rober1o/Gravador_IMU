#include "Gravador_IMU.h"

// ============================================================================
// Função: sd_get_by_name
// Descrição: Retorna o ponteiro para o cartão SD com o nome especificado.
// ============================================================================
static sd_card_t *sd_get_by_name(const char *const name)
{
    for (size_t i = 0; i < sd_get_num(); ++i)
        if (0 == strcmp(sd_get_by_num(i)->pcName, name))
            return sd_get_by_num(i);

    // Caso não encontre, imprime debug e retorna NULL
    DBG_PRINTF("%s: unknown name %s\n", __func__, name);
    return NULL;
}

// ============================================================================
// Função: sd_get_fs_by_name
// Descrição: Retorna o ponteiro para o sistema de arquivos FATFS
//            associado ao cartão SD com o nome especificado.
// ============================================================================
static FATFS *sd_get_fs_by_name(const char *name)
{
    for (size_t i = 0; i < sd_get_num(); ++i)
        if (0 == strcmp(sd_get_by_num(i)->pcName, name))
            return &sd_get_by_num(i)->fatfs;

    // Caso não encontre, imprime debug e retorna NULL
    DBG_PRINTF("%s: unknown name %s\n", __func__, name);
    return NULL;
}

// ============================================================================
// Função: run_mount
// Descrição: Monta o cartão SD e exibe mensagens de erro/sucesso no display.
//            Também controla os LEDs indicadores.
// ============================================================================
static void run_mount()
{
    const char *arg1 = strtok(NULL, " ");
    if (!arg1)
        arg1 = sd_get_by_num(0)->pcName;

    FATFS *p_fs = sd_get_fs_by_name(arg1);
    if (!p_fs)
    {
        // Exibe mensagem de erro no display
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO AO MONTAR", 8, 20);
        ssd1306_draw_string(&ssd, " O CARTAO", 23, 35);
        ssd1306_send_data(&ssd);

        // Pisca roxo (vermelho + azul) por 3 segundos
        for (int i = 0; i < 10; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }
        return;
    }

    // Tenta montar o sistema de arquivos
    FRESULT fr = f_mount(p_fs, arg1, 1);
    if (FR_OK != fr)
    {
        // Exibe mensagem de erro no display
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO AO MONTAR", 8, 20);
        ssd1306_draw_string(&ssd, " O CARTAO", 23, 35);
        ssd1306_send_data(&ssd);
        gpio_put(LED_VERDE, 0);

        // Pisca roxo (vermelho + azul) por 3 segundos
        for (int i = 0; i < 10; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }
        return;
    }

    // Montagem bem-sucedida
    sd_card_t *pSD = sd_get_by_name(arg1);
    myASSERT(pSD);
    pSD->mounted = true;

    // Exibe mensagem de sucesso no display
    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
    ssd1306_draw_string(&ssd, "CARTAO MONTADO", 8, 20);
    ssd1306_draw_string(&ssd, " COM SUCESSO", 15, 30);
    ssd1306_send_data(&ssd);

    // Ativa LED verde e desliga os outros
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 1);

    // Aguarda para exibir mensagem
    sleep_ms(3000);
    esta_montado = true;
}

// ============================================================================
// Função: run_unmount
// Descrição: Desmonta o cartão SD e exibe mensagens no display.
//            Controla também os LEDs indicativos de status.
// ============================================================================
static void run_unmount()
{
    // Estado inicial: LED amarelo (verde + vermelho)
    gpio_put(LED_VERDE, 1);
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AZUL, 0);

    // Obtém o nome do cartão, ou usa o padrão (primeiro cartão)
    const char *arg1 = strtok(NULL, " ");
    if (!arg1)
        arg1 = sd_get_by_num(0)->pcName;

    // Tenta obter o sistema de arquivos
    FATFS *p_fs = sd_get_fs_by_name(arg1);
    if (!p_fs)
    {
        // Erro ao localizar FS - exibe mensagem no display
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO AO DESMONTAR", 8, 20);
        ssd1306_draw_string(&ssd, " O CARTAO", 23, 35);
        ssd1306_send_data(&ssd);

        // Pisca roxo (vermelho + azul)
        for (int i = 0; i < 6; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }
        return;
    }

    // Tenta desmontar o sistema de arquivos
    FRESULT fr = f_unmount(arg1);
    if (FR_OK != fr)
    {
        // Erro ao desmontar - exibe mensagem
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO AO DESMONTAR", 8, 20);
        ssd1306_draw_string(&ssd, " O CARTAO", 23, 35);
        ssd1306_send_data(&ssd);

        // Pisca roxo (vermelho + azul)
        for (int i = 0; i < 6; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }
        return;
    }

    // Desmontagem bem-sucedida
    sd_card_t *pSD = sd_get_by_name(arg1);
    myASSERT(pSD);
    pSD->mounted = false;
    pSD->m_Status |= STA_NOINIT;
    esta_montado = false;

    // Exibe mensagem de sucesso no display
    ssd1306_fill(&ssd, !cor);
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
    ssd1306_draw_string(&ssd, "SD DESMONTADO", 12, 20);
    ssd1306_draw_string(&ssd, " COM SUCESSO", 15, 35);
    ssd1306_send_data(&ssd);

    // Aguarda tempo para leitura da mensagem
    sleep_ms(3000);
}

// ============================================================================
// Função: capture_imu_data_and_save
// Descrição: Captura dados da IMU e salva em um arquivo CSV no cartão SD.
//            Mostra mensagens no display e usa LEDs/buzzer para feedback.
// ============================================================================
void capture_imu_data_and_save()
{
    FIL file;
    FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);

    // Verifica se o cartão está montado
    if (!esta_montado)
    {
        // Exibe mensagem de erro no display
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO! MONTE O", 12, 20);
        ssd1306_draw_string(&ssd, " CARTAO PARA ", 12, 35);
        ssd1306_draw_string(&ssd, " CAPTURAR ", 28, 50);
        ssd1306_send_data(&ssd);
        capturar_dados = false;
        gpio_put(LED_VERDE, 0);

        // Pisca roxo (vermelho + azul)
        for (int i = 0; i < 10; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }

        // Estado final: LED amarelo indica erro persistente
        gpio_put(LED_VERDE, 1);
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_AZUL, 0);
        return;
    }

    // Verifica se houve erro ao abrir arquivo
    if (res != FR_OK)
    {
        // Exibe mesma mensagem de erro no display
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "ERRO! MONTE O", 12, 20);
        ssd1306_draw_string(&ssd, " CARTAO PARA ", 12, 35);
        ssd1306_draw_string(&ssd, " CAPTURAR ", 28, 50);
        ssd1306_send_data(&ssd);
        capturar_dados = false;
        gpio_put(LED_VERDE, 0);

        // Pisca roxo (vermelho + azul)
        for (int i = 0; i < 10; i++)
        {
            gpio_put(LED_VERMELHO, 1);
            gpio_put(LED_AZUL, 1);
            sleep_ms(150);
            gpio_put(LED_VERMELHO, 0);
            gpio_put(LED_AZUL, 0);
            sleep_ms(150);
        }

        // Estado final: LED amarelo
        gpio_put(LED_VERDE, 1);
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_AZUL, 0);
        return;
    }

    // =====================================================================
    // Cabeçalho CSV com BOM UTF-8 + campo de tempo em segundos
    // =====================================================================
    UINT bw;
    const unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
    f_write(&file, bom, sizeof(bom), &bw);

    const char *header = "numero_amostra;accel_x;accel_y;accel_z;giro_x;giro_y;giro_z;tempo_s\n";
    f_write(&file, header, strlen(header), &bw);

    // =====================================================================
    // Início da captura de dados
    // =====================================================================
    int16_t accel[3], gyro[3], temp;
    int amostra = 0;

    // Tempo de referência inicial
    absolute_time_t tempo_inicial = get_absolute_time();

    // LED vermelho aceso indica início da captura
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AZUL, 0);
    buzzer_beep(500);

    while (capturar_dados && amostra < 128)
    {
        // Lê dados brutos da IMU
        mpu6050_read_raw(accel, gyro, &temp);

        // Captura tempo atual e calcula tempo relativo em segundos
        absolute_time_t tempo_atual = get_absolute_time();
        int64_t tempo_decorrido_us = absolute_time_diff_us(tempo_inicial, tempo_atual);
        double tempo_s = tempo_decorrido_us / 1e6;

        // Prepara linha CSV com tempo
        char line[160];
        sprintf(line, "%d;%d;%d;%d;%d;%d;%d;%.3f\n",
                amostra + 1, accel[0], accel[1], accel[2],
                gyro[0], gyro[1], gyro[2], tempo_s);

        // Pisca LED azul durante gravação
        gpio_put(LED_AZUL, 1);
        f_write(&file, line, strlen(line), &bw);
        gpio_put(LED_AZUL, 0);

        // Atualiza display com progresso
        char linha1[] = "AMOSTRAS";
        char linha2[] = "CAPTURADAS";
        char linha3[16];
        sprintf(linha3, "%d/128", amostra + 1);

        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, linha1, 35, 15);
        ssd1306_draw_string(&ssd, linha2, 28, 30);
        ssd1306_draw_string(&ssd, linha3, 44, 47);
        ssd1306_send_data(&ssd);

        amostra++;
        sleep_ms(150); // Tempo entre amostras (ajustável)
    }

    // =====================================================================
    // Finaliza captura: fecha arquivo e sinaliza sucesso
    // =====================================================================
    f_close(&file);
    gpio_put(LED_VERMELHO, 0);
    buzzer_beep(200);
    sleep_ms(100);
    buzzer_beep(200);
    pwm_set_enabled(slice_num, false);

    // Feedback visual: pisca LED azul e exibe mensagem
    for (int i = 0; i < 6; i++)
    {
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_draw_string(&ssd, "DADOS SALVOS", 20, 25);
        ssd1306_draw_string(&ssd, "NO CARTAO", 35, 40);
        ssd1306_send_data(&ssd);

        gpio_put(LED_AZUL, 1);
        sleep_ms(250);
        gpio_put(LED_AZUL, 0);
        sleep_ms(250);
    }

    // Estado final: sistema pronto (verde aceso)
    gpio_put(LED_VERDE, 1);
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_AZUL, 0);
    capturar_dados = false;
}

// ============================================================================
// INTERRUPÇÃO DOS BOTÕES (BOTAO_A e BOTAO_B)
// ============================================================================
void gpio_irq_handler(uint gpio, uint32_t events)
{
    absolute_time_t agora = get_absolute_time();

    // --- BOTÃO A ---
    if (gpio == BOTAO_A && (events & GPIO_IRQ_EDGE_FALL))
    {
        if (absolute_time_diff_us(ultimo_acionamento_A, agora) >= DEBOUNCE_MS * 1000)
        {
            ultimo_acionamento_A = agora;
            botao_a_pressionado = true;
        }
    }

    // --- BOTÃO B ---
    else if (gpio == BOTAO_B && (events & GPIO_IRQ_EDGE_FALL))
    {
        if (absolute_time_diff_us(ultimo_acionamento_B, agora) >= DEBOUNCE_MS * 1000)
        {
            ultimo_acionamento_B = agora;
            botao_b_pressionado = true;

            // Alterna captura de dados
            capturar_dados = !capturar_dados;

            // (Espaço para lógica adicional, se necessário)
        }
    }
}

// ============================================================================
// MPU6050 - Reset
// ============================================================================
static void mpu6050_reset()
{
    // Envia comando de reset: registrador 0x6B com valor 0x80
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
    sleep_ms(100); // Aguarda estabilização

    // Sai do modo sleep: registrador 0x6B com valor 0x00
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
    sleep_ms(10); // Aguarda estabilização
}

// ============================================================================
// MPU6050 - Leitura de dados crus: aceleração, giroscópio e temperatura
// ============================================================================
static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp)
{
    uint8_t buffer[6];
    uint8_t val;

    // --- Leitura dos 3 eixos do acelerômetro (0x3B - 0x40) ---
    val = 0x3B;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
        accel[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];

    // --- Leitura dos 3 eixos do giroscópio (0x43 - 0x48) ---
    val = 0x43;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
        gyro[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];

    // --- Leitura da temperatura interna (0x41 - 0x42) ---
    val = 0x41;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, buffer, 2, false);

    *temp = (buffer[0] << 8) | buffer[1];
}

// ============================================================================
// PWM - Inicialização do buzzer
// ============================================================================
void inicializar_pwm_buzzer()
{
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    pwm_set_clkdiv(slice_num, 10.0f);                                     // Clock de 12.5 MHz
    pwm_set_wrap(slice_num, 6250);                                        // 2 kHz (12.5 MHz / 6250)
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 3125); // Duty cycle 50%
    pwm_set_enabled(slice_num, false);
}

// ============================================================================
// PWM - Emite um beep no buzzer por uma duração específica
// ============================================================================
void buzzer_beep(uint duracao_ms)
{
    pwm_set_enabled(slice_num, true);
    sleep_ms(duracao_ms);
    pwm_set_enabled(slice_num, false);
}

// ============================================================================
// Função Principal - Inicialização geral do sistema
// ============================================================================
int main()
{
    stdio_init_all(); // Inicializa UART padrão para printf()

    // ============================================================================
    // Configuração dos Botões com Pull-up e Interrupções
    // ============================================================================
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A); // Habilita pull-up interno no BOTAO_A

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B); // Habilita pull-up interno no BOTAO_B

    // Configura interrupção na borda de descida para os dois botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true); // Usa o mesmo callback já definido

    // ============================================================================
    // Inicialização dos Sistemas de Tempo e ADC
    // ============================================================================
    time_init(); // Inicializa sistema de tempo/RTC
    adc_init();  // Inicializa o ADC para futuras leituras analógicas

    // ============================================================================
    // Inicialização da UART e Terminal
    // ============================================================================
    printf("FatFS SPI example\n");
    printf("\033[2J\033[H"); // Limpa a tela do terminal e posiciona cursor no início
    printf("\n> ");          // Exibe prompt inicial
    stdio_flush();           // Garante envio dos dados via UART

    // ============================================================================
    // Inicialização da I2C do Display OLED
    // ============================================================================
    i2c_init(I2C_PORT_DISP, 400 * 1000); // Inicializa I2C a 400 kHz

    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C); // Configura SDA como I2C
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C); // Configura SCL como I2C
    gpio_pull_up(I2C_SDA_DISP);                     // Habilita pull-up no SDA
    gpio_pull_up(I2C_SCL_DISP);                     // Habilita pull-up no SCL

    // Inicializa e configura o display OLED
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT_DISP);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd); // Envia configurações iniciais

    // Limpa o display (inicialmente com todos os pixels apagados)
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // ============================================================================
    // Inicialização da I2C do Acelerômetro MPU6050
    // ============================================================================
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa nova I2C a 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // ============================================================================
    // Inicialização do PWM do Buzzer
    // ============================================================================
    inicializar_pwm_buzzer(); // Configura o PWM para o buzzer (não ativa ainda)

    // Adiciona pinos I2C ao Binary Info para ferramentas de depuração
    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

    // Reinicia o MPU6050 (função externa)
    mpu6050_reset();

    // ============================================================================
    // Inicialização dos LEDs
    // ============================================================================
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0); // Apaga LED verde

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0); // Apaga LED azul

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0); // Apaga LED vermelho

    // ============================================================================
    // Atualização Inicial do Display OLED com Interface Gráfica
    // ============================================================================
    ssd1306_fill(&ssd, !cor);                          // Preenche display com base na variável 'cor'
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo de borda
    ssd1306_draw_string(&ssd, "INICIANDO...", 20, 30); // Escreve texto centralizado
    ssd1306_send_data(&ssd);                           // Atualiza display

    // ============================================================================
    // Sinalização Inicial com LEDs
    // ============================================================================
    gpio_put(LED_VERDE, 1);    // Acende LED verde
    gpio_put(LED_VERMELHO, 1); // Acende LED vermelho
    sleep_ms(2000);            // Aguarda 2 segundos
    gpio_put(LED_VERDE, 0);    // Apaga LED verde
    gpio_put(LED_VERMELHO, 0); // Apaga LED vermelho

    // ============================================================================
    // Loop Principal - Atualização contínua do sistema
    // ============================================================================
    while (true)
    {
        // ============================================================================
        // Verifica se o cartão SD está montado
        // ============================================================================
        if (!esta_montado)
        {
            // Atualiza o conteúdo do display com instruções para montagem
            ssd1306_fill(&ssd, !cor);                     // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha moldura
            ssd1306_draw_string(&ssd, "APERTE BOTAO A", 9, 15);
            ssd1306_draw_string(&ssd, "P/ MONTAR OU", 15, 30);
            ssd1306_draw_string(&ssd, "DESMONTAR SD", 20, 45);
            ssd1306_send_data(&ssd); // Envia para o display

            gpio_put(LED_VERMELHO, 1); // Acende LED vermelho (erro/desmontado)
            gpio_put(LED_VERDE, 1);    // Acende LED verde
            gpio_put(LED_AZUL, 0);     // Apaga LED azul
        }
        else
        {
            // Atualiza o conteúdo do display com instruções para captura de dados
            ssd1306_fill(&ssd, !cor);                     // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha moldura
            ssd1306_draw_string(&ssd, "BOTAO B:", 35, 10);
            ssd1306_draw_string(&ssd, "INICIAR/PARAR", 12, 25);
            ssd1306_draw_string(&ssd, "CAPTURA DE", 25, 40);
            ssd1306_draw_string(&ssd, "DADOS", 45, 50);
            ssd1306_send_data(&ssd); // Envia para o display

            gpio_put(LED_VERMELHO, 0); // Apaga LED vermelho
            gpio_put(LED_AZUL, 0);     // Apaga LED azul
            gpio_put(LED_VERDE, 1);    // Acende LED verde (pronto)
        }

        // ============================================================================
        // Verifica se o botão A foi pressionado (Montar ou Desmontar SD)
        // ============================================================================
        if (botao_a_pressionado)
        {
            // Exibe mensagem de espera no display
            ssd1306_fill(&ssd, !cor);                        // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);    // Desenha moldura
            ssd1306_draw_string(&ssd, "AGUARDE...", 30, 30); // Mensagem de espera
            ssd1306_send_data(&ssd);                         // Envia para o display

            if (!esta_montado)
            {
                run_mount(); // Monta o cartão SD
            }
            else
            {
                run_unmount(); // Desmonta o cartão SD
            }

            botao_a_pressionado = false; // Reseta flag do botão A
        }

        // ============================================================================
        // Verifica se o botão B foi pressionado (Captura de dados)
        // ============================================================================
        if (botao_b_pressionado)
        {
            // Exibe mensagem de espera no display
            ssd1306_fill(&ssd, !cor);                        // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);    // Desenha moldura
            ssd1306_draw_string(&ssd, "AGUARDE...", 30, 30); // Mensagem de espera
            ssd1306_send_data(&ssd);                         // Envia para o display

            capture_imu_data_and_save(); // Captura dados do IMU e salva no SD
            botao_b_pressionado = false; // Reseta flag do botão B
        }

        // ============================================================================
        // Aguarda antes da próxima iteração (reduz uso de CPU)
        // ============================================================================
        sleep_ms(100); // Pequeno atraso para suavizar o loop
    }

    return 0;
}
