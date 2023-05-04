
# Reactive LED Matrix
An expandable matrix of addressable RGB LEDs, made interactive through an array of high-power infrared LEDs and phototransistors. Panels can be plugged into other panels for infinitely large array sizes. Final Project for CICS 256, Make: Physical Computing. *This project is still in progress as we near the completion date.*
### Photos
![Board Render](https://github.com/dgorbunov/ReactiveLEDMatrix/blob/main/photos/InteractiveLEDs.png)
![Schematic Image](https://github.com/dgorbunov/ReactiveLEDMatrix/blob/main/photos/schematic.png)
## Hardware
Each panel is controlled by an ATMega32u4 microcontroller, along with two MCP3008 8 channel 10-bit ADCs for reading phototransistor outputs. The high-power infrared LEDs can sink up to 100mA, so each pair of IR LEDs is switched through an N-Channel MOSFET. Each MOSFET is pulsed on for ~100µS, so the panel is updated every 1.6ms at 625Hz. The edge connectors use a serial protocol to send data to up to four surrounding panels and thus propagate matrix effects across multiple panels.

Each tile of the matrix measures 140mm x 140mm with 35mm pitch between the WS2812B LEDs and 130mm between the mounting holes (diffuser coming soon).

### Build it Yourself
The BOM, Schematic, and Gerbers can be found [here](https://github.com/dgorbunov/ReactiveLEDMatrix/tree/main/hardware).
