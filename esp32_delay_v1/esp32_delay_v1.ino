//esp32_delay_v1
//author:Andre Simao

// Inclui as bibliotecas necessárias
#include <Arduino.h>
#include <driver/dac.h>

// Define os pinos utilizados para entrada e saída
#define INPUT_PIN 34
#define LEFT_OUTPUT_PIN 25
#define RIGHT_OUTPUT_PIN 26

// Define as constantes utilizadas para o efeito de delay
#define BUFFER_SIZE 2048
#define DELAY_TIME 2000
#define DELAY_FEEDBACK 0.5
#define DELAY_MIX 0.3

// Declara as variáveis utilizadas pelo efeito de delay
int16_t buffer[BUFFER_SIZE];
int bufferIndex = 0;

// Inicializa o DAC para a saída de áudio
void setupAudio() {
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);
}

// Função para aplicar o efeito de delay ao sinal de entrada
void applyDelay(int16_t *input, int16_t *output, int numSamples) {
  for (int i = 0; i < numSamples; i++) {
    // Salva o sinal de entrada no buffer
    buffer[bufferIndex] = input[i];

    // Aplica o atraso com feedback ao sinal
    int16_t delayed = buffer[(bufferIndex + BUFFER_SIZE - DELAY_TIME) % BUFFER_SIZE];
    buffer[bufferIndex] = input[i] + delayed * DELAY_FEEDBACK;

    // Mistura o sinal atrasado com o sinal original
    int16_t mixed = input[i] * (1 - DELAY_MIX) + delayed * DELAY_MIX;

    // Salva o sinal processado no buffer de saída
    output[i] = mixed;

    // Incrementa o índice do buffer circular
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  }
}

// Configuração inicial do programa
void setup() {
  // Inicializa a comunicação serial para depuração
  Serial.begin(115200);

  // Inicializa o DAC para a saída de áudio
  setupAudio();
}

// Loop principal do programa
void loop() {
  // Lê o valor atual da entrada analógica
  int16_t inputSample = analogRead(INPUT_PIN);

  // Cria um buffer de entrada e de saída para o efeito de delay
  int16_t inputBuffer[1] = { inputSample };
  int16_t outputBuffer[1];

  // Aplica o efeito de delay ao buffer de entrada
  applyDelay(inputBuffer, outputBuffer, 1);

  // Escreve o valor processado nos canais de saída
  dac_output_voltage(DAC_CHANNEL_1, outputBuffer[0]);
  dac_output_voltage(DAC_CHANNEL_2, outputBuffer[0]);

  // Espera um curto período antes de ler o próximo valor da entrada
  delayMicroseconds(100);
}
