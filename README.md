This is a Half-Life project inspired by the startup project Starbie. I modified the original display principle by replacing the direct screen display with an ST7735 TFT display, which is used as a projection light to create a larger projected image. The core concept remains the same as mentioned above. While the original Starbie idea requires looking directly at a small screen, this project uses the ST7735 screen as a projection light instead so the image can be projected onto a larger surface. 
The screen in the code is a 1.8-inch, 128x160 SPI TFT display, and the code currently handles drawing the pet on that screen. There are no lenses, housing, or projection optics parts included in this repository yet, so that part of the setup is still to be worked out. 
The project also features a DHT11 temperature and humidity sensor. It reads data every 2.5 seconds, and the readings are printed to the Serial Monitor at 115200 baud. The code includes a Sensor Monitor screen that displays the temperature, humidity, and the same three status bars; it shows dashes when no reading is available yet. 
For the microcontroller, the project uses the Seeed Studio XIAO ESP32, which is already capable of providing enough power. The MPU6050 is the other sensor component on this board, and it is fully functional. On the board, it connects via J1, an 8-pin connector with a footprint specifically designed for an MPU6050 module. The 3D Models folder includes a GY-521 MPU6050 accelerometer model, and the footprint folder contains an MPU6050 module footprint. The connector includes SCL, SDA, 3.3V, and GND lines in the schematic, making it ready to plug a module straight in. Tilting the device triggers special sound effects.
<img width="1621" height="1027" alt="Screenshot 2026-10-11 175944" src="https://github.com/user-attachments/assets/595523da-9b77-453a-ac3e-7de6b46782b2" />
<img width="686" height="455" alt="image" src="https://github.com/user-attachments/assets/d8f6dfdf-d7ba-4802-9ab6-6b709c31afa7" />



## Bill of Materials (BOM)

| Item / Component | Qty | Description / Specs | Part Number / Link |
| :--- | :---: | :--- | :--- |
| **ST7735 TFT Display** | 1 | Main display, 1.8" 128x160 SPI TFT | ST7735 |
| **Seeed Studio XIAO ESP32-C3** | 1 | Microcontroller, Wi-Fi & Bluetooth MCU | XIAO ESP32-C3 |
| **DHT11** | 1 | Environmental sensor, temperature and humidity | DHT11 |
| **PCB** | 1 | Custom printed circuit board, glass esp32-2 | glass esp32-2 |
| **MX Switches** | 1 | Mechanical switches, Cherry MX 1.00u PCB keyswitches (SW1, SW2) | SW_Push |
| **GY-521 MPU6050** | 1 | 6-axis motion sensor, 3-axis accelerometer + 3-axis gyroscope, I2C | GY-521 |
