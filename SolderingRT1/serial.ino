void ExecuteSerialCommunication() {
  if(Serial.available()) 
  { 
    char c = (char)Serial.read();
    switch (c){
      case '\n':
        IncomingCmd[CmdIndex] = 0;
        ExecuteSerialCmd();  // execute the command
        CmdIndex = 0; // reset the cmdIndex
        //PrintStatus();
        break;
      default:
            IncomingCmd[CmdIndex] = c;
            if(CmdIndex<99) CmdIndex++;
        break;
      }
  }
}

void ExecuteSerialCmd() {
  if(IncomingCmd[0]=='a' && IncomingCmd[1]==' ') {
    SetTemperature[0] = atoi(IncomingCmd+2);
    Serial.println("SET T1");
  }
  if(IncomingCmd[0]=='b' && IncomingCmd[1]==' ') {
    SetTemperature[1] = atoi(IncomingCmd+2);
    Serial.println("SET T2");
  }
  if(IncomingCmd[0]=='d' && IncomingCmd[1]==' ') {
    DebugMode = atoi(IncomingCmd+2);
    Serial.print("DebugMode ");
    Serial.println(DebugMode);
  } 
  if(IncomingCmd[0]=='w' && IncomingCmd[1]==' ') {
    MaxDutyCycle = atoi(IncomingCmd+2);
    Serial.println("SET Max Duty Cycle");
  }
  if(IncomingCmd[0]=='u' && IncomingCmd[1]==' ') {
    VinUVLO = atoi(IncomingCmd+2);
    Serial.println("SET Vin UVLO");
  }
  if(IncomingCmd[0]=='r' && IncomingCmd[1]==' ') {
    InitializeEEPROM();;
    Serial.println("Reset parameters to default values. Please power cycle.");
  }
  if(IncomingCmd[0]=='P' && IncomingCmd[1]==' ') {
    KP = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET P");
    Serial.println(KP);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='I' && IncomingCmd[1]==' ') {
    KI = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET I");
    Serial.println(KI);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='D' && IncomingCmd[1]==' ') {
    KD = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET D");
    Serial.println(KD);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='h') {
    SerialMenu();
  }
  if(IncomingCmd[0]=='s') {
    SaveParameters();
    Serial.println("Parameters Saved");
	//Ensures parameters are synch
	LoadParameters();
  }  
}

void SerialMenu() {
  char myChar;
  for (unsigned int k = 0; k < strlen_P(HelpMessage); k++) {
    myChar = pgm_read_byte_near(HelpMessage + k);
    Serial.print(myChar);
  }
  Serial.println("Current EEPROM values");
  Serial.print("Preset1: "); Serial.println(PresetTemperature[0]);
  Serial.print("Preset2: "); Serial.println(PresetTemperature[1]);
  Serial.print("Max duty cycle: "); Serial.println(MaxDutyCycle);
  Serial.print("Under Voltage Lock Out UVLO [V]: "); Serial.println(float(VinUVLO)/10,1);
  Serial.println("***************************************************************************");
}

void SerialDebug() {
  Serial.print("Tint(C): ");
  Serial.println(InternalTemperature);
  Serial.print("T1adc:"); Serial.print(MeasuredTemperatureADCvalue[0]);
  Serial.print(" T2adc:"); Serial.println(MeasuredTemperatureADCvalue[1]);
  Serial.print("T1:"); Serial.print(MeasuredTemperature[0]);
  Serial.print(" T2:"); Serial.println(MeasuredTemperature[1]);
  Serial.print("Int. cnt S1: ");
  Serial.println(InterruptCounterS1);
  Serial.print("Int. cnt S2: ");
  Serial.println(InterruptCounterS2);
  Serial.print("Ch1 & Ch2 State: "); Serial.print(ChannelState[0]); Serial.print(" / ");  Serial.println(ChannelState[1]); 
  Serial.print("Duty1/Duty2:  "); Serial.print(OutputVal1); Serial.print(" / "); Serial.println(OutputVal2);
  Serial.print("MAX Duty1/Duty2:  "); Serial.print(MaxDutyCycle); Serial.print(" / "); Serial.println(MaxDutyCycle);
  Serial.print("Vin: ");
  Serial.println(InputVoltage);
  Serial.print("VinUVLO(V): ");
  Serial.println(VinUVLO*10);
  Serial.print("Tint(C): ");
  Serial.println(InternalTemperature);
  Serial.print("InMenuState: ");
  Serial.println(InMenuState);
  Serial.print("RotaryCounter: ");
  Serial.println(RotaryCounter);
  Serial.print("InterruptCounterEncoderSW: ");
  Serial.println(InterruptCounterEncoderSW);
}
