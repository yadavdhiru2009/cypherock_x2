# cypherock_x2

STM32H7 FreeRTOS Project Setup
------------------------------

Overview :
----------
This setup guide covers everything needed to get started with a FreeRTOS-based project on the STM32H7 MCU. 
We'll configure the Memory Protection Unit (MPU) to define secure regions for our driver modules and integrate
a TFT display (ILI9341) and a touch panel (STMPE811).

Project Initialization : 
------------------------

1. Setting Up STM32CubeIDE: Install STM32CubeIDE, Start a new STM32 project specifically for the STM32H7 microcontroller.

2. Configuring FreeRTOS: Open the .ioc file in STM32CubeIDE. Go to the Middleware section and enable FreeRTOS.
						 Set up tasks and other FreeRTOS configurations needed.

MPU Configuration : 
-------------------

1. Enable the MPU: Go to Configuration > MPU settings. Turn on the MPU and start configuring regions.

2. Defining the Secure Region: Define the secure region for the driver module. Set the base address and size of secure region.
                               Configure the access permissions as required (read/write).

3. Generate Code: Click on Project > Generate Code to apply MPU settings.

Hardware Integration : 
----------------------
1. TFT Display ILI9341

    1.1 SPI Communication Setup:
        Configure the SPI peripheral to communicate with the ILI9341 and SPI pins (MOSI, MISO, SCK, CS, DC, RES) are set up correctly in STM32CubeIDE.

    1.2 Adding ILI9341 Driver:
        Include the ILI9341 driver source files in project. add the necessary header files and initialization code.

    1.3 Display Initialization:
        Initialization function for the ILI9341 display. Implemented the code to send commands and data to the display via SPI.

2. Touch Panel STMPE811

    2.1 I2C Communication Setup:
        Configure the I2C peripheral for the STMPE811 touch panel and I2C pins (SDA, SCL) in STM32CubeIDE.

    2.2 Adding STMPE811 Driver:
        STMPE811 driver source files to project. Include the required header files and initialization code.

    2.3 Touch Panel Initialization:
        Implemented the initialization function for the STMPE811 touch panel.To read touch events and process the touch data.

Integration : 
-------------

    1. Peripheral Initialization:
        Check SPI and I2C peripherals are correctly initialized in main.c.

    2. FreeRTOS Tasks Creation:
        Create tasks for managing TFT display updates and touch panel events. Implement task functions to handle display rendering and touch input.

Final Steps : 
-------------

    1. Build the Project:
        Compile and build your project in STM32CubeIDE.

    2. Deploy to Hardware:
        Flash the firmware onto the STM32H7 MCU using your programmer/debugger.

