/* ------------------------------------------------------------------------- *
 * Name   : MorseGenerator
 * Author : Gerard Wassink
 * Date   : July 4, 2023
 * Purpose: Generate morse code from serial text input
 * Versions:
 *   0.1  : Initial code base
  * ------------------------------------------------------------------------- */
#define progVersion "0.1"                   // Program version definition
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
#define audioPin  8  
#define frequency 1200
#define dotLen    100
#define dashLen   300
#define spaceLen  1400

/* ------------------------------------------------------------------------- *
 *       Function declarations
 * ------------------------------------------------------------------------- */
void morseChar();
void initTable();
void outputMorse();

/* ------------------------------------------------------------------------- *
 *       Global variable declarations
 * ------------------------------------------------------------------------- */
String morseTable['Z'-' '+1];


/* ------------------------------------------------------------------------- *
 *       Program initialization                                      setup()
 * ------------------------------------------------------------------------- */
void setup() {  
  Serial.begin(9600);

  initTable();                          // Initialize morse code table

                                        // Initial prompt
  Serial.print("PD1GAW - MorseGenerator version ");
  Serial.println(progVersion);

  outputMorse("PD1GAW");

  Serial.println(" ");
  Serial.print("> ");
}


/* ------------------------------------------------------------------------- *
 *       Program repetition                                           loop()
 * ------------------------------------------------------------------------- */
void loop()
{ 
  char inputBuffer[100];
  int pointer = 0;
  String inputText = "";
  
  while (Serial.available() == 0) {}    // Wait untill serial input waiting
  inputText = Serial.readString();      // Read string from serial input
  inputText.toUpperCase();              // Convert to uppercase

  Serial.print(inputText);            // Echo text to serial

  outputMorse(inputText);

  Serial.println(" ");                  // Next prompt
  Serial.print("> ");

}


/* ------------------------------------------------------------------------- *
 *       Handle every text                                     outputMorse()
 * ------------------------------------------------------------------------- */
void outputMorse(String inputText) {
  int limit = inputText.length();       // Determine length
  for (int p = 0; p < limit - 1; p++) { 
    char c = inputText[p];              // Look at every character in the input
    if (c >= ' ' && c <= 'Z') {         //   is it in our range?
      morseChar(c);                     //     then output as morse beeps
    }
  }
}


/* ------------------------------------------------------------------------- *
 *       Handle every character                                  morseChar()
 * ------------------------------------------------------------------------- *
 * 
 * All characters in the range from SPACE to capital 'Z' are converted to  
 * an offset relative to 32 (x'20' or SPACE).
 * 
 * Characters below SPACE and above 'Z' are discarded.
 * 
 * So in the table, the space character is the first item with index 0,
 * hence the 'Z' character (with ascii value 90) is the 58th character.
 * 
 * The position of an input character c in the morseTable() is determined as:
 *   position = [c-' ']
 * 
 * This will be the ascii-value of the character minus the ascii-value of SPACE
 * that we can use as an index in the morseTable()
 * 
 * ------------------------------------------------------------------------- */
void morseChar(char c)
{
  String morseCode = morseTable[c-' ']; // Index in morseTable

  switch (morseCode[0]) {

    case '0':                           // value not valid for morse code
      break;

    case 'S':                           // Space in input, don't print
      Serial.print("  ");
      break;
      
    default:
      Serial.print(morseCode);          // Print morse code
      Serial.print("  ");              // Print seperator
      break;
  }

/* ------------------------------------------------------------------------- *
 * Generate the morse code for the character based on the value from the
 * morseTable
 * ------------------------------------------------------------------------- */
  int len = morseCode.length();
  for (int i=0; i<len; i++) {
    char x = morseCode[i];

    switch (x) {

      case '.':
        tone(audioPin, frequency, dotLen);  // start playing a dot tone
        delay(dotLen+100);
        break;

      case '-':
        tone(audioPin, frequency, dashLen); // start playing a dash tone
        delay(dashLen+100);
        break;

      case 'S':                         // Space between words, wait a bit
        delay(dotLen*5);
        break;

      default:
        break;
    }
  }
  delay(dotLen);                        // Space between characters
}


/* ------------------------------------------------------------------------- *
 *       Initialize translation table                            initTable()
 * ------------------------------------------------------------------------- *
 * A single "0" in the table value denotes a non-valid morse code character
 * ------------------------------------------------------------------------- */
void initTable()
{
  morseTable[' '-' ']  = "S";            // SPACE
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
