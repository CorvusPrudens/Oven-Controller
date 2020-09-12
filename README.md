# Oven-Controller
 Software and Firmware for a custom-made ATMEGA 328-P based reflow oven controller
![Oven Controller](/images/controller_oven.jpg)

## Hardware
 The board is controlled by an ATMEGA328-P microcontroller running at 20 MHz. USB communication and flashing is handled with an FT232R USB to UART IC. The board uses a USB-C connector because every modern device should.

 ![Oven Controller](/images/controller_naked.jpg)

 The controller includes a MAX31865 PTD temperature sensor IC with headers for a three-wire PTD probe, a solid state relay, and two additional gpio. The board also include a 7-12 volt power input header and a standard Arduino ICSP header for bootloader flashing.

 The most prominent feature is, of course, the 128x64 pixel OLED display. The on-screen UI is navigated with the 6 pushbuttons arranged in a gamepad-like fashion.

 The board was designed in kiCad, but can be easily ported to Eagle.

## Firmware
 The firmware provides an intuitive, menu-based UI for the user. Without any programming or reflashing, the user can define up to eight temperature profiles that persist even after power cycling. These profiles can then be called up to control a modified toaster oven via a solid state relay.

 ![Oven Controller](/images/controller_ui.jpg)

 The firmware provides out of the box support for a piezo buzzer on the available gpio terminal, which provides feedback for UI navigation and an open-door notification.
