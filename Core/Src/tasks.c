/*
 * tasks.c
 *
 *  Created on: Feb 22, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "timer2.h"
#include "uart.h"
#include "button.h"
#include "utils.h"
#include "mpu6050.h"
#include "i2c2.h"
#include "lcd.h"
#include "ds18b20.h"
#include "dht11.h"
#include "sd_data_logger.h"

#define MAX_RETRIES 5

// Global variables to store DHT11 data
volatile uint8_t dht11_humidity = 0;
volatile uint8_t dht11_humidity2 = 0;
volatile uint8_t dht11_temperature = 0;
volatile uint8_t dht11_temperature2 = 0;

static char uart_buf[32];

// Structure for feedback display
typedef struct
{
  char line1[16];
  char line2[16];
  uint32_t end_time;      // System time when feedback should end
  uint8_t active;         // Feedback currently active
} Feedback_t;

static Feedback_t feedback = {0};

// Show a message on LCD for specified duration
static void Feedback_Show(const char *line1, const char *line2, uint16_t duration_ms)
{
  // Copy line1
  int i = 0;
  while(line1[i] && i < 15)
  {
    feedback.line1[i] = line1[i];
    i++;
  }
  feedback.line1[i] = '\0';

  // Copy line2
  i = 0;
  while(line2[i] && i < 15)
  {
    feedback.line2[i] = line2[i];
    i++;
  }
  feedback.line2[i] = '\0';

  // Calculate end time
  feedback.end_time = TIMER2_GetMillis() + duration_ms;
  feedback.active = 1;

  // Show immediately on LCD
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_SendString(feedback.line1);
  LCD_SetCursor(1, 0);
  LCD_SendString(feedback.line2);
}

// Check if feedback time has expired
void Task_Feedback_Update(void)
{
  if(!feedback.active)
    return;

  uint32_t now = TIMER2_GetMillis();

  // Check if we've reached/passed the end time
  if(now >= feedback.end_time)
  {
    feedback.active = 0;
  }
}

void Task_DHT11_Read(void)
{
  uint8_t hum1, hum2, temp1, temp2, checksum;

  // Disable interrupts for critical section
  uint32_t primask = __get_PRIMASK();
  __disable_irq();

  // Try up to MAX_RETRIES times
  for(int retry = 0; retry < MAX_RETRIES; retry++)
  {
    DHT11_Start();

    if(DHT11_Check_Response())
    {
      hum1 = DHT11_Read();
      hum2 = DHT11_Read();
      temp1 = DHT11_Read();
      temp2 = DHT11_Read();
      checksum = DHT11_Read();

      uint8_t calc = hum1 + hum2 + temp1 + temp2;

      if(calc == checksum)
      {
        dht11_humidity = hum1;
        dht11_humidity2 = hum2;
        dht11_temperature = temp1;
        dht11_temperature2 = temp2;
        break;
      }
    }
    TIMER2_Delay_ms(1);
  }

  // Re-enable interrupts
  __set_PRIMASK(primask);
}

// Task to update UART output
void Task_UART_Output(void)
{
  DisplayMode_t mode = Button_GetMode();

  switch(mode)
  {
    case DISPLAY_MODE_TEMP_HUM:
      format_reading_temp(dht11_temperature, dht11_temperature2, dht11_humidity, dht11_humidity2, uart_buf);
      break;

    case DISPLAY_MODE_TEMP:
      format_reading(ds18b20_data.temperature, mpu6050_scaled.temp, uart_buf);
      break;

    case DISPLAY_MODE_ACCEL:
      format_accel_scaled(uart_buf, mpu6050_scaled.accel_x, mpu6050_scaled.accel_y, mpu6050_scaled.accel_z, 2);
      break;

    case DISPLAY_MODE_GYRO:
      format_gyro_scaled(uart_buf, mpu6050_scaled.gyro_x, mpu6050_scaled.gyro_y, mpu6050_scaled.gyro_z, 2);
      break;

    default:
      return;
  }

  USART1_SendString(uart_buf);
}

// Task to read DS18b20 sensor
void Task_DS18B20_Read(void)
{
  // Read the temperature
  float temp = DS18B20_ReadTemperature();

  if(temp > -100.0f)
  {
    ds18b20_data.temperature = temp;
    ds18b20_data.valid = 1;
  }
  else
  {
    ds18b20_data.valid = 0;
  }

  // Immediately start next conversion
  DS18B20_StartConversion();
}

// Task to read MPU6050 sensor
void Task_MPU6050_Read(void)
{
  if(MPU6050_ReadAll() == I2C_OK)
  {
    MPU6050_ScaleAll();
  }
}

// Task to update LCD display
void Task_LCD_Update(void)
{
  if(feedback.active)
    return;
  DisplayMode_t mode = Button_GetMode();

  switch(mode)
  {
    case DISPLAY_MODE_TEMP_HUM:
      LCD_DisplayReading_Temp(dht11_temperature, dht11_temperature2, dht11_humidity, dht11_humidity2);
      break;

    case DISPLAY_MODE_TEMP:
      LCD_DisplayReading(ds18b20_data.temperature, mpu6050_scaled.temp);
      break;

    case DISPLAY_MODE_ACCEL:
      LCD_DisplayAccelScaled(mpu6050_scaled.accel_x, mpu6050_scaled.accel_y, mpu6050_scaled.accel_z);
      break;

    case DISPLAY_MODE_GYRO:
      LCD_DisplayGyroScaled(mpu6050_scaled.gyro_x, mpu6050_scaled.gyro_y, mpu6050_scaled.gyro_z);
      break;

    default:  // Handles DISPLAY_MODE_COUNT and any invalid values
      break;
  }
}

void Task_Button_Status(void)
{
  // Button 2 - SAVE data
  if(g_button2_pressed)
  {
    if(SD_DataLogger_SaveEntry() == SD_LOGGER_OK)
    {
      USART1_SendString("Logged entry #");
      USART1_SendNumber(entry_count);
      USART1_SendString("\r\n");

      char line2[16] = "ENTRY #";
      char num_str[6];
      itoa_32(SD_DataLogger_GetEntryCount(), num_str);

      // Find the end of "ENTRY #" to append number
      uint8_t i = 6; // length of "ENTRY #"
      uint8_t j = 0;
      while(num_str[j] && i < 15)
      {
        line2[i++] = num_str[j++];
      }
      line2[i] = '\0';

      Feedback_Show("SAVED !!!", line2, 1000);

    }
    else
    {
      USART1_SendString("Save FAILED!\r\n");  // DEBUG
      Feedback_Show("ERROR!", "SAVE FAILED", 1000);
    }

    g_button2_pressed = 0;
  }

  // Button 3 - READ data
  if(g_button3_pressed)
  {
    uint32_t count = SD_DataLogger_ReadAll();

    if(count > 0)
    {
      char line2[16] = "ENTRIES #";
      char num_str[6];
      itoa_32(SD_DataLogger_GetEntryCount(), num_str);

      // Find the end of "ENTRY #" to append number
      uint8_t i = 8; // length of "ENTRY #"
      uint8_t j = 0;
      while(num_str[j] && i < 15)
      {
        line2[i++] = num_str[j++];
      }
      line2[i] = '\0';

      Feedback_Show("READ DONE !!!", line2, 1000);
    }
    else
    {
      Feedback_Show("NO DATA !!!", "SAVE DATA FIRST", 1000);
    }

    g_button3_pressed = 0;
  }
}
