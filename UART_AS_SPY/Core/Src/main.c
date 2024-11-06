#include "main.h"
#include <stdio.h>
#include <string.h>

UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);

// Function prototypes for new features
uint8_t calculate_crc8(const uint8_t *data, size_t length);
void self_diagnostics(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();

    char msg[50];
    uint32_t adcValue;
    float temperature;
    uint8_t rx_data;
    int display_flag = 0;  // 0 = stop, 1 = start
    int encrypted_mode = 0;  // 0 = unencrypted, 1 = encrypted

    HAL_UART_Transmit(&huart2, (uint8_t*)"System Initialized\r\n", 20, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)"Commands:\r\n'1' = Start\r\n'0' = Stop\r\n'2' = Status\r\n'3' = Encrypted Mode\r\n'4' = Self-Diagnostics\r\n", 95, HAL_MAX_DELAY);

    while (1)
    {
        // Check if there is any data received over UART
        if (HAL_UART_Receive(&huart2, &rx_data, 1, 100) == HAL_OK)
        {
            HAL_UART_Transmit(&huart2, &rx_data, 1, HAL_MAX_DELAY);

            if (rx_data == '1')
            {
                display_flag = 1;
                HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nDisplaying data\r\n", 19, HAL_MAX_DELAY);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);  // Green LED ON for 'Start'
                HAL_Delay(500);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
            }
            else if (rx_data == '0')
            {
                display_flag = 0;
                HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nStopped displaying data\r\n", 28, HAL_MAX_DELAY);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);  // Red LED ON for 'Stop'
                HAL_Delay(500);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
            }
            else if (rx_data == '2')
            {
                HAL_ADC_Start(&hadc1);
                if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
                {
                    adcValue = HAL_ADC_GetValue(&hadc1);
                    temperature = ((adcValue / 4095.0) * 5) * 100;

                    // Combine temperature and CRC in one line
                    sprintf(msg, "\r\nTemperature: %.2f degC, CRC: %02X\r\n", temperature, calculate_crc8((uint8_t*)msg, strlen(msg)));

                    if (encrypted_mode)
                    {
                        for (int i = 0; i < strlen(msg); i++)
                            msg[i] ^= 0xAA;
											     strcat(msg, "\r\n");  // Add newline after encryption to ensure each message is on a new line
                    }

                    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


                    // Toggle GPIO pin for 2 seconds
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
                    HAL_Delay(2000);
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
                }
            }
            else if (rx_data == '3')
            {
                encrypted_mode = !encrypted_mode;
                if (encrypted_mode)
                    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nEncrypted Mode ON\r\n", 21, HAL_MAX_DELAY);
                else
                    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nEncrypted Mode OFF\r\n", 22, HAL_MAX_DELAY);
            }
            else if (rx_data == '4')
            {
                HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nStarting Self-Diagnostics...\r\n", 32, HAL_MAX_DELAY);
                self_diagnostics();
            }
            else
            {
                HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nInvalid Command\r\n", 20, HAL_MAX_DELAY);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
                HAL_Delay(500);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
            }
        }

        // Continuous data display when display_flag is 1
        if (display_flag == 1)
        {
            HAL_ADC_Start(&hadc1);
            if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
            {
                adcValue = HAL_ADC_GetValue(&hadc1);
                temperature = ((adcValue / 4095.0) * 5) * 100;

                // Combine temperature and CRC in one line
                sprintf(msg, "Temperature: %.2f degC, CRC: %02X\r\n", temperature, calculate_crc8((uint8_t*)msg, strlen(msg)));

                if (encrypted_mode)
                {
                    for (int i = 0; i < strlen(msg); i++)
                        msg[i] ^= 0xAA;
		                    strcat(msg, "\r\n");  // Add newline after encryption to ensure each message is on a new line
                }

                HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


                // Toggle GPIO pin for 1 second for visual indication (reduce delay)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
                HAL_Delay(2000);  // Reduced delay for quicker display update
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
            }
        }
    }
}

// CRC Calculation function for CRC-8
uint8_t calculate_crc8(const uint8_t *data, size_t length)
{
    uint8_t crc = 0x00;
    const uint8_t polynomial = 0x07;

    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Self-Diagnostics function to check UART, ADC, and GPIO
void self_diagnostics(void)
{
    char msg[100];
    uint32_t adcValue;
    HAL_StatusTypeDef uart_status, adc_status, gpio_status;

    uart_status = HAL_UART_Transmit(&huart2, (uint8_t*)"UART Test\r\n", 11, HAL_MAX_DELAY);

    HAL_ADC_Start(&hadc1);
    adc_status = HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    if (adc_status == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc1);
    }

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    gpio_status = HAL_OK;

    snprintf(msg, sizeof(msg),
             "Diagnostics Report:\r\n"
             "UART: %s\r\n"
             "ADC: %s (Sample Value: %u)\r\n"
             "GPIO: %s\r\n",
             (uart_status == HAL_OK) ? "OK" : "ERROR",
             (adc_status == HAL_OK) ? "OK" : "ERROR", adcValue,
             (gpio_status == HAL_OK) ? "OK" : "ERROR");

    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}


// UART, ADC, GPIO initialization functions remain the same

// UART2 initialization function
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

// ADC1 initialization function
void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

// GPIO initialization function
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure GPIO pin Output Level
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_13, GPIO_PIN_RESET);

    // Configure GPIO pin : PA4 (Green LED for 'Start', 'Status')
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure GPIO pin : PA13 (Red LED for 'Stop' and Error)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
    // Clock configuration code as generated by CubeMX
}

// Error handler function
void Error_Handler(void)
{
    while (1) 
    {
        // Optionally blink the error LED or implement other error handling
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
        HAL_Delay(250);
    }
}

