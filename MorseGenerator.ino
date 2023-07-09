/* ------------------------------------------------------------------------- *
 * Name   : MorseGenerator
 * Author : Gerard Wassink
 * Date   : July 4, 2023
 * Purpose: Generate morse code from serial text input
 * Versions:
 *   0.1  : Initial code base
 *   0.2  : Added
 *            #commands
 *            tx pin (for future use to turn transmitter on)
 *   0.3  : Small correction of last character disappearing
 *   0.4  : Optimized string handling to free SRAM
 *          Optimized text handling and help info
 *          Some code cleanup & comment improvements
 *   1.0  : First release, based on version 0.4
 *   1.1  : Code cleanup, comment improvements
 * ------------------------------------------------------------------------- */
#define progVersion "1.1"                   // Program version definition
/* ------------------------------------------------------------------------- *
 *             GNU LICENSE CONDITIONS
 * ------------------------------------------------------------------------- *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * ------------------------------------------------------------------------- *
 *       Copyright (C) July 2023 Gerard Wassink
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
 *       Functional defines
 * ------------------------------------------------------------------------- */
#define audioPin  8                     // pin to hook up buzzer
#define txPin     9                     // pin to hook up relay for tx
#define callSign  "PD1GAW"              // personal callsign

/* ------------------------------------------------------------------------- *
 *       Function declarations
 * ------------------------------------------------------------------------- */
void initTable();                       // initialize interpretation table
void commandRoutine();                  // interpret and execute # commands
void outputMorse();                     // convert input to morse code
void morseChar();                       // ... same for every chaacter
void txOn();                            // start transmitting
void txOff();                           // stop transmitting
void showVars();                        // show current parameter values
void showHelp();                        // show help information

/* ------------------------------------------------------------------------- *
 *       Global variable declarations
 * ------------------------------------------------------------------------- */
unsigned int frequency  = 1200;         // start at 1200 Hz
unsigned long dotLen    = 75;           // start value for dots
unsigned long dashLen   = 3 * dotLen;   // calculate dash lenght
unsigned long spaceLen  = 14 * dotLen;  // calculate space lenght

String morseTable['Z'-' '+1];           // declare morse code table


/* ------------------------------------------------------------------------- *
 *       Program initialization                                      setup()
 * ------------------------------------------------------------------------- */
void setup() {  
  Serial.begin(9600);

  initTable();                          // Initialize morse code table

                                        // Initial show settings
  Serial.print( F("PD1GAW - MorseGenerator version ") );
  Serial.println(progVersion);

  showVars();

  Serial.println( F("Use # for help info") );

  outputMorse(callSign);
}


/* ------------------------------------------------------------------------- *
 *       Program repetition                                           loop()
 * ------------------------------------------------------------------------- */
void loop()
{ 
  char inputBuffer[100];
  int pointer = 0;
  String inputText = "";
  
  Serial.println(" ");                  // Prompt
  Serial.print("> ");

  while (Serial.available() == 0) {}    // Wait untill serial input available
  inputText = Serial.readString();      // Read string from serial input
  inputText.toUpperCase();              // Convert to uppercase
  inputText.trim();                     // Strip leading & trailing whitespace

  Serial.println(inputText);            // Echo text to serial

  if (inputText[0] == '#') {            // Command input?
    commandRoutine(inputText);
  } else {
    outputMorse(inputText);             // else generate morse code
  }

}


/* ------------------------------------------------------------------------- *
 *       Handle commands                                    commandRoutine()
 * ------------------------------------------------------------------------- *
 * We come here knowing that the first character is a "#" sign
 * Determine valid commands and execute them
 * ------------------------------------------------------------------------- */
void commandRoutine(String inputText) {
  int len = inputText.length();
  int val = 0;

  if (len == 1) {                       // just the # ? then  Help info
    showHelp();

  } else {                              // isolate command and operand
    String command = String(inputText.substring(1,4) );
    String operand = String(inputText.substring(5,len) );

                                        // handle DOT command
    if (command.compareTo("DOT") == 0) {
      val = operand.toInt();

      if (val > 0) {
        Serial.println();
        Serial.print( F("DOT value is ") );
        Serial.println(val);
        dotLen   = val;                 // Set dot length
        dashLen  = 3 * dotLen;          // Calculate new dash length
        spaceLen = 14 * dotLen;         // Calculate new space length
      } else {
        Serial.println();
        Serial.print( F("operand not valid: ") );
        Serial.println(operand);
      }
                                        // handle FRQ command
    } else if (command.compareTo("FRQ") == 0) {
      val = operand.toInt();
      if (val > 0) {
        Serial.println();
        Serial.print( F("FRQ value is ") );
        Serial.println(val);
        frequency = val;                // Set tone frequency

      } else {
        Serial.println();
        Serial.print( F("operand not valid: ") );
        Serial.println(operand);
      }

                                        // handle VAL command
    } else if (command.compareTo("VAL") == 0) {
      showVars();

                                        // handle HLP command
    } else if (command.compareTo("HLP") == 0) {
      showHelp();

    } else {
      Serial.print( F("Invalid command : ") );
      Serial.println(command);
      showHelp();
    }
  }
}


/* ------------------------------------------------------------------------- *
 *       Show valid commands                                 validCommands()
 * ------------------------------------------------------------------------- */
void showHelp() {
  Serial.println( F("PD1GAW - MorseGenerator - help info") );
  Serial.println(" ");
  Serial.println( F("Type anything, the valid morse code characters will") );
  Serial.println( F("be converted to morse code, on the screen as well as in sound.") );
  Serial.println( F("Non valid morse code characters will be discarded.") );
  Serial.println(" ");
  Serial.println( F("Commands (always start with a '#'):") );
  Serial.println( F("  #dot value - Change the morse speed") );
  Serial.println( F("  #frq value - Change the morse frequency") );
  Serial.println( F("  #val       - Show all current values") );
  Serial.println( F("  #          - Show this help info") );
}


/* ------------------------------------------------------------------------- *
 *       Show variables                                           showVars()
 * ------------------------------------------------------------------------- */
void showVars() {
  Serial.println( F("Current parameter values") );
  Serial.print( F("Frequency   : ") );
  Serial.println(frequency);
  Serial.print( F("Dot length  : ") );
  Serial.println(dotLen);
  Serial.print( F("Dash length : ") );
  Serial.println(dashLen);
  Serial.print( F("Space length: ") );
  Serial.println(spaceLen);
}


/* ------------------------------------------------------------------------- *
 *       Handle every non-command text                         outputMorse()
 * ------------------------------------------------------------------------- */
void outputMorse(String inputText) {
  txOn();                               // Transmitter on

  int limit = inputText.length();       // Determine length of input text
  for (int p = 0; p < limit; p++) { 
    char c = inputText[p];              // Look at every character in the input
    if (c >= ' ' && c <= 'Z') {         //   is it in our range?
      morseChar(c);                     //     then output as morse beeps
    }
  }

  txOff();                              // Transmitter off
}


/* ------------------------------------------------------------------------- *
 *       Handle every character                                  morseChar()
 * ------------------------------------------------------------------------- *
 * 
 * All characters in the range from SPACE to capital 'Z' are converted to  
 * an offset relative to 32 (x'20' or SPACE).
 * 
 * Characters below SPACE and above capital 'Z' are discarded.
 * 
 * The position of an input character c in the morseTable() is determined as:
 *   position = [c-' ']
 * 
 * So in the table, the space character is the first item with index 0,
 * hence the 'Z' character (with ascii value 90) is the 58th character.
 * 
 * This will be the ascii-value of the character minus the ascii-value of SPACE
 * that we can use as an index in the morseTable()
 * 
 * ------------------------------------------------------------------------- */
void morseChar(char c)
{
  String morseCode = morseTable[c-' ']; // Index in two-dimensional morseTable

  switch (morseCode[0]) {

    case '0':                           // value not valid for morse code
      break;

    case 'S':                           // Space in input, don't print
      Serial.print("  ");
      break;
      
    default:
      Serial.print(morseCode);          // Print morse code (dots and dashes)
      Serial.print("  ");               // Print seperator
      break;
  }

  // -------------------------------------------------------------------------
  // Generate the morse code for the character based on the value from the
  // morseTable
  // -------------------------------------------------------------------------
  int len = morseCode.length();
  
  for (int i=0; i<len; i++) {
    char x = morseCode[i];

    switch (x) {

      case '.':
        tone(audioPin, frequency, dotLen);  // start playing a dot tone
        delay(dotLen + dotLen);
        break;

      case '-':
        tone(audioPin, frequency, dashLen); // start playing a dash tone
        delay(dashLen + dotLen);
        break;

      case 'S':                         // Space between words, wait a bit
        delay(5 * dotLen);
        break;

      default:
        break;
    }
  }
  delay(dotLen);                        // Space between characters
}


/* ------------------------------------------------------------------------- *
 *       Transmit on                                                  txOn()
 * ------------------------------------------------------------------------- *
 * For now, turn TX LED on, later: switch TX relay
 * ------------------------------------------------------------------------- */
void txOn() 
{
  digitalWrite(txPin, HIGH);
}


/* ------------------------------------------------------------------------- *
 *       Transmit off                                                txOff()
 * ------------------------------------------------------------------------- *
 * For now, turn TX LED off, later: switch TX relay
 * ------------------------------------------------------------------------- */
void txOff()
{
  digitalWrite(txPin, LOW);
}


/* ------------------------------------------------------------------------- *
 *       Initialize translation table                            initTable()
 * ------------------------------------------------------------------------- *
 * A single "0" in the table value denotes a non-valid morse code character
 * ------------------------------------------------------------------------- */
void initTable()
{
  morseTable[' '-' ']  = "S";            // S means SPACE
  morseTable['!'-' ']  = "-.-.--";
  morseTable['"'-' ']  = ".-..-.";
  morseTable['#'-' ']  = "0";
  morseTable['$'-' ']  = "0";
  morseTable['%'-' ']  = "0";
  morseTable['&'-' ']  = "0";
  morseTable['\''-' '] = ".----.";
  morseTable['('-' ']  = "-.--.";
  morseTable[')'-' ']  = "-.--.-";
  morseTable['*'-' ']  = "0";
  morseTable['+'-' ']  = ".-.-.";
  morseTable[','-' ']  = "--..--";
  morseTable['-'-' ']  = "-....-";
  morseTable['.'-' ']  = ".-.-.-";
  morseTable['/'-' ']  = "-..-.";
  morseTable['0'-' ']  = "-----";
  morseTable['1'-' ']  = ".----";
  morseTable['2'-' ']  = "..---";
  morseTable['3'-' ']  = "...--";
  morseTable['4'-' ']  = "....-";
  morseTable['5'-' ']  = ".....";
  morseTable['6'-' ']  = "-....";
  morseTable['7'-' ']  = "--...";
  morseTable['8'-' ']  = "---..";
  morseTable['9'-' ']  = "----.";
  morseTable[':'-' ']  = "---...";
  morseTable[';'-' ']  = "-.-.-.";
  morseTable['<'-' ']  = "0";
  morseTable['='-' ']  = "-...-";
  morseTable['>'-' ']  = "0";
  morseTable['?'-' ']  = "..--..";
  morseTable['@'-' ']  = "0";
  morseTable['A'-' ']  = ".-";
  morseTable['B'-' ']  = "-...";
  morseTable['C'-' ']  = "-.-.";
  morseTable['D'-' ']  = "-..";
  morseTable['E'-' ']  = ".";
  morseTable['F'-' ']  = "..-.";
  morseTable['G'-' ']  = "--.";
  morseTable['H'-' ']  = "....";
  morseTable['I'-' ']  = "..";
  morseTable['J'-' ']  = ".---";
  morseTable['K'-' ']  = "-.-";
  morseTable['L'-' ']  = ".-..";
  morseTable['M'-' ']  = "--";
  morseTable['N'-' ']  = "-.";
  morseTable['O'-' ']  = "---";
  morseTable['P'-' ']  = ".--.";
  morseTable['Q'-' ']  = "--.-";
  morseTable['R'-' ']  = ".-.";
  morseTable['S'-' ']  = "...";
  morseTable['T'-' ']  = "-";
  morseTable['U'-' ']  = "..-";
  morseTable['V'-' ']  = "...-";
  morseTable['W'-' ']  = ".--";
  morseTable['X'-' ']  = "-..-";
  morseTable['Y'-' ']  = "-.--";
  morseTable['Z'-' ']  = "--..";
}
