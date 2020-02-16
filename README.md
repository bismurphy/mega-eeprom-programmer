# mega-eeprom-programmer

This is meant to program an EEPROM chip, such as the AT28C256, with an Arduino Mega.

Currently I have core functions built for reading and writing. Next step is integration with a Python script to push a file into the EEPROM.

This is derived from Ben Eater's EEPROM programmer, which uses an Arduino Nano and shift registers, but since I don't have any shift register chips, and also I want to use this with his 6502 Computer, I re-wrote it to run on the Mega.
