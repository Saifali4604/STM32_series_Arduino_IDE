# STM32_series_RTOS_on_Arduino_IDE
# Weather prediction using numerical values
programming any STM32  family board in Arduino IDE 

## Softwares Installation
1. Arduino IDE 2.0 
2. [STM32cube Programmer](https://www.st.com/en/development-tools/stm32cubeprog.html)

## Board package Download
1. Open Arduino IDE -> File -> Preferences -> Additional boards manager URLs - copy paste below lines and click Ok
![image](https://github.com/user-attachments/assets/0521bf00-51d4-40e9-9949-acb07002fc40)

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

2. Go to Board Manager -> Install ``` STM32 MCU based Boards by STMicroelectronics ```
3. After Installation Go to Library Manager Install below Libraries
   ```  STM32duino FreeRTOS by Richard ```

## Code Uploading
1. Make sure you open the STM32cube Programmer, just Open Don't do anything.
2. Plug the Board Through Mini USB (just for Programming)
3. Choose the Below Settings :
   ![image](https://github.com/user-attachments/assets/9fbc890c-c78c-482c-b4d7-72e6538aa591)

## Serial Monitor View
1. Plug the Board Through Mini USB (just for Power Supply).
2. Plug the Micro USB.
3. Open New Sketch in Arduino IDE (Blank Sketck) and Select Port -> COM Port of Micro USB.
4. Open the Serial Monitor.

Note: Micro USB for Serial Monitor and Mini USB for Programming / Power Supply.

## Weather Detection Project
1. Libraries :
   ```
   Adafruit BMP280 Library by Adafruit
   DHT sensor Library by Adafruit
   LiquidCrystal by Arduino, Adafruit
   ```
2. Connections :
   
   BMP280 (I2C1) -> PB6  - SCL | PB7  - SDA

   LCD (I2C1) -> PB6  - SCL | PB7  - SDA

   DHT11 -> Data pin - PB4

   Dust Sensor -> Anolog pin - PA0 | Led pin - PA1

   ![image](https://github.com/Saifali4604/STM32_series_RTOS_on_Arduino_IDE/blob/main/Media/Connection%20diagram.jpg)

   
   

