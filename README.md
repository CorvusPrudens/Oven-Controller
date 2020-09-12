# Oven-Controller
 Hardware, firmware, and support software for an ATMEGA 328-P based reflow oven controller.

![Oven Controller](/images/controller_oven.jpg)

## Hardware
<img align="right" src="/images/controller_naked.jpg">
 The board is controlled by an ATMEGA328-P microcontroller running at 20 MHz. USB communication and flashing is handled with an FT232R USB to UART IC. The board uses a USB-C connector because every modern device should.

 The controller includes a MAX31865 PTD temperature sensor IC with headers for a three-wire PTD probe, a solid state relay, and two additional gpio. The board also include a 7-12 volt power input header and a standard Arduino ICSP header for bootloader flashing.

 The most prominent feature is, of course, the 128x64 pixel OLED display. The on-screen UI is navigated with the 6 pushbuttons arranged in a gamepad-like fashion.

 The board was designed in kiCad, but can be easily ported to Eagle.
<br /><br />
## Firmware
<img align="left" src="/images/controller_ui.jpg">
 The firmware provides an intuitive, menu-based UI for the user. Without any programming or reflashing, the user can define up to eight temperature profiles that persist even after power cycling. These profiles can then be called up to control a modified toaster oven via a solid state relay.

 The firmware provides out of the box support for a piezo buzzer on the available gpio terminal, which provides feedback for UI navigation and an open-door notification.
<br /><br /><br /><br /><br /><br /><br /><br /><br /><br />
## Software
 I have also written a few support programs with Processing and Python. They perform various functions, including generating timer settings for different clock frequencies, generating playable note sequences given a semi-intuitive note table entry, generating bitmaps from images (ideally monochromatic or dichromatic images), and a program that allows a connected computer to log and display the live temperature profile as recorded by the controller.
