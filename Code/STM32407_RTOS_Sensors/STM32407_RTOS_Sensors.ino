// I2C1 - PB6  - SCL | PB7  - SDA
// I2C2 - PB10 - SCL | PB11 - SDA
// I2C3 - PA8  - SCL | PC9  - SDA

// SPI1 - PA5  - SCK | PA6  - MISO | PA7  - MOSI | PA4  - NSS
// SPI2 - PB10 - SCK | PC2  - MISO | PC3  - MOSI | PB12 - NSS
// SPI2 - PC10 - SCK | PC11 - MISO | PC12 - MOSI | PA15 - NSS

// LED3 - ORANGE- PD13
// LED4 - GREEN - PD12
// LED5 - RED   - PD14
// LED6 - BLUE  - PD15

// BUTTON1 - PA0 - USER

//////////////////////////////////////////////////////////////////////////////////////////////

// RTOS 
#include <STM32FreeRTOS.h>
void Task_DHT_Read( void *pvParameters );
void Task_Dust_read( void *pvParameters );
void Task_BMP_Read( void *pvParameters );
void Task_LCD( void *pvParameters );
void Task_Serial_write( void *pvParameters );
SemaphoreHandle_t tftMutex ;

void setup() {
  Serial.begin(9600);
  tftMutex = xSemaphoreCreateMutex();
  xTaskCreate(Task_DHT_Read, "DHT11 Read",  256, NULL, 1, NULL);
  xTaskCreate(Task_Dust_read, "Dust Read",  256, NULL, 1, NULL);
  xTaskCreate(Task_BMP_Read, "BMP Read",  256, NULL, 1, NULL);
  xTaskCreate(Task_LCD, "LCD",  256, NULL, 1, NULL);
  xTaskCreate(Task_Serial_write, "Serial write",  256, NULL, 1, NULL);
  vTaskStartScheduler();
  while(1);
}

/////////////////////////////////////////// DHT 11 ///////////////////////////////////////////
#include "DHT.h"
#define DHTPIN  PB4
DHT dht(DHTPIN, DHT11);
int h; 
float temp;

void Task_DHT_Read(void *pvParameters){
  dht.begin();
  for(;;){
    h = dht.readHumidity();
    temp = dht.readTemperature();
    vTaskDelay(10);
  }
}

/////////////////////////////////////////// BMP 280 ///////////////////////////////////////////
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp; // I2C1 - PB6  - SCL | PB7  - SDA
float BMP_temp, BMP_pressure, BMP_altitude;
unsigned status;

void Task_BMP_Read(void *pvParameters){
  status = bmp.begin(BMP280_ADDRESS);
  if (!status) {
    Serial.println("Could not find BMP280 sensor!");
    vTaskDelete(NULL);
  }
  
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  
  for(;;){
    if (xSemaphoreTake(tftMutex, portMAX_DELAY) == pdTRUE){
      bmp.begin(BMP280_ADDRESS);
      BMP_temp = bmp.readTemperature();
      BMP_pressure = bmp.readPressure();
      BMP_altitude = bmp.readAltitude(1013.25);
      xSemaphoreGive(tftMutex);
    }
    vTaskDelay(100); // Read every 1 sec
  }
}

/////////////////////////////////////////// Dust ///////////////////////////////////////////
const int sensorPin = PA0;
const int ledPin = PA1;
const int adcMax = 4095;
const float Vcc = 3.3;
int dust;

void Task_Dust_read(void *pvParameters){
  pinMode(PA1, OUTPUT);
  for(;;){
    digitalWrite(ledPin, LOW);
    delayMicroseconds(280);
    int adc = analogRead(PA0);  
    float v0 = Vcc * adc / adcMax;
    float density = ((0.170 * v0 - 0.1) *1000) + 30;  // mg/m^3
    digitalWrite(ledPin, HIGH);
    dust = abs(map(density, 0, 71, 0, 100));
    vTaskDelay(10);
  }
}

/////////////////////////////////////////// LCD ///////////////////////////////////////////
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 3); // I2C address 0x27, 16 column and 2 rows
// I2C1 - PB6  - SCL | PB7  - SDA

void Task_LCD(void *pvParameters){
  if (xSemaphoreTake(tftMutex, portMAX_DELAY) == pdTRUE){
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.clear();
    xSemaphoreGive(tftMutex);
  }
  for(;;){
    if (xSemaphoreTake(tftMutex, portMAX_DELAY) == pdTRUE){

      lcd.init(); // initialize the lcd

      lcd.setCursor(0, 0);
      lcd.print("Hum:");
      lcd.print(h);
      lcd.print("%|");

      lcd.setCursor(0, 1);
      lcd.print("Air:");
      lcd.print(dust);
      lcd.print("%");
      lcd.setCursor(7, 1);
      lcd.print("|");
      

      lcd.setCursor(9, 0);
      lcd.print("Pressure:"); 
      lcd.setCursor(10, 1);
      lcd.print(BMP_pressure);
      lcd.print("Pa");

      lcd.setCursor(0, 2);
      lcd.print("Temp:");
      lcd.print("  |");
      lcd.setCursor(0, 3);
      lcd.print(temp);
      lcd.print("C");
      lcd.print(" |");
      
      lcd.setCursor(9, 2);
      lcd.print("Altitude:");
      lcd.setCursor(10, 3);
      lcd.print(BMP_altitude);
      lcd.print(" m");

      xSemaphoreGive(tftMutex);
    }
    vTaskDelay(50);
  }
}


/////////////////////////////////////////// Serial ///////////////////////////////////////////

void Task_Serial_write(void *pvParameters){
  while ( !Serial ) vTaskDelay(100); // wait for native usb
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
  }
  for(;;){
    // Serial.print("Temp_DHT = ");
    // Serial.print(temp);
    // Serial.print("C");
    // Serial.print('\t');
    // Serial.print("Hum_DHT = ");
    // Serial.print(h);
    // Serial.print(" %");
    // Serial.print('\t');
    // Serial.print("Air = ");
    // Serial.print(dust);
    // Serial.print("%");
    // Serial.print('\t');
    // Serial.print("Pressure = ");
    // Serial.print(BMP_pressure);
    // Serial.print(" Pa");
    // Serial.print('\t');
    // Serial.print("Approx altitude = ");
    // Serial.print(BMP_altitude);
    // Serial.println(" m");
    // vTaskDelay(10);

    Serial.print(temp);
    Serial.print('\t');
    Serial.print(h);
    Serial.print('\t');
    Serial.print(dust);
    Serial.print('\t');
    Serial.print(BMP_pressure);
    Serial.print('\t');
    Serial.println(BMP_altitude);

  }
}


void loop() {}
