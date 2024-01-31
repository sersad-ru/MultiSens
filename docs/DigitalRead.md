# DigitalRead Plugin

The DigitalRead plugin reads digital pin `P0` and `P1`.
`P0` is configured as `INPUT`. `P1` is configured as `INPUT_PULLUP`.
Results are displayed on the device screen and sends to the serial in human-readable and 
Arduino `SerialPlotter` compartible format.

* Use the `UP` and `DOWN` buttons to change poll delay. 
  Avaible values are 10, 50, 100, 250, 500, 1000, 1500 and 2000 ms

* Additional mode is `INTERRUPT` mode. Interrupts are configured as **CHANGE** on both pins. 
  In interrupt mode values are printed only if an interrupt occurs.

* Not only values are printed to serial but time (in ms) since last changes of values.

* Current delay is displayed after `P1` value (use the `LEFT` and `RIGTH` buttons to scrool the screen)

* Press and hold the `SELECT` button to store current delay in the `EEPROM`.

* Hold the `DOWN` button and press `RESET` to clear stored value and return to defaults. 
  (**ATTENTION!** All stored data will be cleared. **FOR ALL** plugins!)

[Back to Home](/#supported-devices)

