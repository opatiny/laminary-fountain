// The maximal length of a parameter value. It is a int so the value must be between -32768 to 32767

#define SERIAL_BUFFER_LENGTH 32
char serialBuffer[SERIAL_BUFFER_LENGTH];
byte serialBufferPosition=0;

NIL_WORKING_AREA(waThreadSerial, 128); // minimum 128
NIL_THREAD(ThreadSerial, arg) {

  Serial.begin(9600);
  while(true) {
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read(); 

      if (inChar==13 || inChar==10) {
        // this is a carriage return;
        if (serialBufferPosition>0) {
          printResult(serialBuffer, &Serial);
        } 
        serialBufferPosition=0;
        serialBuffer[0]='\0';
      } 
      else {
        if (serialBufferPosition<SERIAL_BUFFER_LENGTH) {
          serialBuffer[serialBufferPosition]=inChar;
          serialBufferPosition++;
          if (serialBufferPosition<SERIAL_BUFFER_LENGTH) {
            serialBuffer[serialBufferPosition]='\0';
          }
        }
      }  
    }
    nilThdSleepMilliseconds(1);
  }
}



void printResult(char* data, Print* output) {
  boolean theEnd=false;
  byte paramCurrent=0; // Which parameter are we defining
  // The maximal length of a parameter value. It is a int so the value must be between -32768 to 32767
#define MAX_PARAM_VALUE_LENGTH 12
  char paramValue[MAX_PARAM_VALUE_LENGTH];
  byte paramValuePosition=0;
  byte i=0;

  while (!theEnd) {
    byte inChar=data[i];
    i++;
    if (inChar=='\0' || i==SERIAL_BUFFER_LENGTH) theEnd=true;
    if (inChar=='f') { // show settings
      printFreeMemory(output);
    } 
    else if (inChar=='h') {
      serialPrintHelp(output);
    } 
    else if (inChar=='s') { // show settings
      printParameters(output);
    }
    else if (inChar==',') { // store value and increment
      if (paramCurrent>0) {
        if (paramValuePosition>0) {
          setAndSaveParameter(paramCurrent-1,atoi(paramValue));
        } 
        else {
          output->println(parameters[paramCurrent-1]);
        }
        if (paramCurrent<=MAX_PARAM) {
          paramCurrent++;
          paramValuePosition=0;
          paramValue[0]='\0';
        } 
      }
    }
    else if (theEnd) {
      if (data[0]=='c') {
        if (paramValuePosition>0) {
          printCompactParameters(output, atoi(paramValue));
        } 
        else {
          printCompactParameters(output);
        } 
      }
      else if (data[0]=='e') {
        if (paramValuePosition>0) {
          setTime(atol(paramValue));
        } 
        else {
          output->println(now());
        }
      }
      else if (data[0]=='r') {
        if (paramValuePosition>0) {
          if (atol(paramValue)==1234) {
            resetParameter();
            output->println(F("Done"));
          }
        } 
        else {
          output->println(F("To reset enter r1234"));
        }
      }
      else if (data[0]=='q') {
        if (paramValuePosition>0) {
          setQualifier(atoi(paramValue));
        } 
        else {
          uint16_t a=getQualifier();
          output->println(a);
        }
      }
      // this is a carriage return;
      else if (paramCurrent>0) {
        if (paramValuePosition>0) {
          setAndSaveParameter(paramCurrent-1,atoi(paramValue));
        } 
        else {
          output->println(parameters[paramCurrent-1]);
        }
      }      
    }
    else if ((inChar>47 && inChar<58) || inChar=='-') { // a number (could be negative)
      if (paramValuePosition<MAX_PARAM_VALUE_LENGTH) {
        paramValue[paramValuePosition]=inChar;
        paramValuePosition++;
        if (paramValuePosition<MAX_PARAM_VALUE_LENGTH) {
          paramValue[paramValuePosition]='\0';
        }
      }
    } 
    else if (inChar>64 && inChar<92) { // a character so we define the field
      // we extend however the code to allow 2 letters fields !!!
      // we extend however the code to allow 2 letters fields !!
      if (paramCurrent>0) {
        paramCurrent*=26;
      }
      paramCurrent+=inChar-64;
      if (paramCurrent>MAX_PARAM) {
        paramCurrent=0; 
      }
    } 

  }
}


void serialPrintHelp(Print* output) {
  output->println(F("(c)ompact settings"));
  output->println(F("(e)poch"));
  output->println(F("(f)ree mem"));
  output->println(F("(h)elp"));
  output->println(F("(i)2c"));
  output->println(F("(q)ualifier"));
  output->println(F("(r)eset"));
  output->println(F("(s)ettings"));

  output->println(F("(z) eeprom"));
}

















