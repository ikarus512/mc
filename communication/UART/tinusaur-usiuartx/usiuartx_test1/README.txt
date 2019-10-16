USIUARTX - Tinusaur USIUARTX library for USI as UART in half-duplex mode

This is a testing project.

This program outputs some text.

To build the project, type:
   $ make

To program the binary into the microcontroller, type:
   $ make program

Alternatively, to program the binary, type:
	$ avrdude -c usbasp -p t85 -U flash:w:"main.hex":a

To clean up files left out from previous builds, type:
   $ make clean


