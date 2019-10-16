RIOLib - Remote/Radio Input-Output Library for wired and wireless communication.

This is a testing project.

This program demonstrates some of the functionality of the library.

To build the project, type:
   $ make

To program the binary into the microcontroller, type:
   $ make program

Alternatively, to program the binary, type:
	$ avrdude -c usbasp -p t85 -U flash:w:"main.hex":a

To clean up files left out from previous builds, type:
   $ make clean


