
# Reactive LED Matrix
An expandable matrix of addressable WS2812B RGB LEDs, made interactive through an array of high-power infrared LEDs and phototransistors. Panels can be plugged into other panels for large array sizes. Final Project for UMass Amherst CICS 256, Make: Physical Computing.

### Photos
![Board Render](https://github.com/dgorbunov/ReactiveLEDMatrix/blob/main/photos/InteractiveLEDs.png)
![Schematic Image](https://github.com/dgorbunov/ReactiveLEDMatrix/blob/main/photos/schematic.png)
## Hardware
Each panel is controlled by an ATMega32u4 microcontroller (compiling for Arduino Leonardo), along with two MCP3008 8 channel 10-bit ADCs for reading phototransistor voltages. The high-power infrared LEDs can sink up to 100mA, so each pair of IR LEDs is switched on through an N-Channel MOSFET for several microseconds and corresponding phototransistor output read. The edge connectors use UART to send data to up to four neighboring panels and thus propagate matrix effects across multiple panels.

Each tile of the matrix measures 140mm x 140mm with 35mm pitch between the WS2812B LEDs and 130mm between the mounting holes, which are M3.

### Build it Yourself
The BOM, Schematic, and Gerbers can be found [here](https://github.com/dgorbunov/ReactiveLEDMatrix/tree/main/hardware).
