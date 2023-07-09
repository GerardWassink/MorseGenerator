# PD1GAW - MorseGenerator
Generate morse code beeps from text on Serial port

## Help info
Type anything with valid morse code characters and it will be converted to morse code, on the screen as well as in sound.
Non valid morse code characters will be discarded.

Commands start with a '#':
  #dot value - Changing the morse speed
  #frq value - Changing the morse frequency
  #val       - Show all current values
  #          - Show this help info

## Initial values for parameters:
Frequency   : 1200
Dot length  : 75
Dash length : 225
Space length: 1050
