#include <avr/eeprom.h>

#include <SoftPWM.h>

#define MAX_PARAM 16

#define PARAM_RED               0
#define PARAM_GREEN             1
#define PARAM_BLUE              2
#define PARAM_PUMP              3
#define PARAM_NB_COLORS         4
#define PARAM_COLOR_SATURATION  5
#define PARAM_COLOR_BRIGHTNESS  6
#define PARAM_FIXED_COLOR       7

#define PARAM_ACTION1        10
#define PARAM_ACTION2        11
#define PARAM_ACTION3        12
#define PARAM_ACTION4        13
#define PARAM_POWER          15 // enabled if 3 

#define BIT_ENABLE_PUMP     0
#define BIT_ENABLE_LIGHT    1





void resetParameter() {

  setAndSaveParameter(PARAM_RED, 255); // first event may not be zero, it would never starts
  setAndSaveParameter(PARAM_GREEN, 255);
  setAndSaveParameter(PARAM_BLUE, 255);
  setAndSaveParameter(PARAM_PUMP, 160);
  setAndSaveParameter(PARAM_NB_COLORS, 2047);
  setAndSaveParameter(PARAM_COLOR_SATURATION, 1000);
  setAndSaveParameter(PARAM_COLOR_BRIGHTNESS, 1000);
  setAndSaveParameter(PARAM_FIXED_COLOR, 0);
  setAndSaveParameter(PARAM_POWER, 3);

}



//When parameters are set (and saved) an event is recorded (256-281 : A-Z + .... (if more parameters than 26))
#define EVENT_SAVE_ALL_PARAMETER     255
#define EVENT_PARAMETER_SET          256


#define EE_START_PARAM           0 // We save the parameter from byte 0 of EEPROM
#define EE_LAST_PARAM            (MAX_PARAM*2-1) // The last parameter is stored at byte 50-51

#define EEPROM_MIN_ADDR            0
#define EEPROM_MAX_ADDR          511


// value that should not be taken into account
// in case of error the parameter is set to this value
#define ERROR_VALUE  -32768

int parameters[MAX_PARAM];

void setupParameters() {
  //We copy all the value in the parameters table
  eeprom_read_block((void*)parameters, (const void*)EE_START_PARAM, MAX_PARAM * 2);
}

int getParameter(byte number) {
  return parameters[number];
}

void setParameterBit(byte number, byte bitToSet) {
  bitSet(parameters[number], bitToSet);
  // parameters[number]=parameters[number] | (1 << bitToSet);
}

void clearParameterBit(byte number, byte bitToClear) {
  bitClear(parameters[number], bitToClear);
  // parameters[number]=parameters[number] & ( ~ (1 << bitToClear));
}

byte getParameterBit(byte number, byte bitToRead) {
  return bitRead(parameters[number], bitToRead);
  // return (parameters[number] >> bitToRead ) & 1;
}

void setParameter(byte number, int value) {
  parameters[number] = value;
}

void incrementParameter(byte number) {
  parameters[number]++;
}

void saveParameters() {
  for (byte i = 0; i < MAX_PARAM; i++) {
    eeprom_write_word((uint16_t*) EE_START_PARAM + i, parameters[i]);
  }
}

/*
  This will take time, around 4 ms
  This will also use the EEPROM that is limited to 100000 writes
*/
void setAndSaveParameter(byte number, int value) {
  parameters[number] = value;
  //The address of the parameter is given by : EE_START_PARAM+number*2
  eeprom_write_word((uint16_t*) EE_START_PARAM + number, value);
}


void incrementAndSaveParameter(byte number, int stepValue, int maxValue) {
  setAndSaveParameter(number, min(parameters[number] + stepValue, maxValue));
}

void decrementAndSaveParameter(byte number, int stepValue, int minValue) {
  setAndSaveParameter(number, max(parameters[number] - stepValue, minValue));
}

void printParameter(Print* output, byte number) {
  output->print(number);
  output->print("-");
  if (number > 25) {
    output->print((char)(floor(number / 26) + 64));
  }
  else {
    output->print(" ");
  }
  output->print((char)(number - floor(number / 26) * 26 + 65));
  output->print(": ");
  output->println(parameters[number]);
}

void printParameters(Print* output) {
  for (int i = 0; i < MAX_PARAM; i++) {
    printParameter(output, i);
  }
}



uint8_t printCompactParameters(Print* output) {
  printCompactParameters(output, MAX_PARAM);
}

uint8_t printCompactParameters(Print* output, byte number) {
  if (number > MAX_PARAM) {
    number = MAX_PARAM;
  }
  byte checkDigit = 0;

  // we first add epoch
  checkDigit ^= toHex(output, (long)now());
  for (int i = 0; i < number; i++) {
    int value = getParameter(i);
    checkDigit ^= toHex(output, value);
  }
  checkDigit ^= toHex(output, (int)getQualifier());
  toHex(output, checkDigit);
  output->println("");
}

/* The qualifier represents the card ID and is stored just after the last parameter */
uint16_t getQualifier() {
  return eeprom_read_word((uint16_t*)(EE_START_PARAM + MAX_PARAM * 2));
}

void setQualifier(uint16_t value) {
  eeprom_write_word((uint16_t*)(EE_START_PARAM + MAX_PARAM * 2), value);
}











