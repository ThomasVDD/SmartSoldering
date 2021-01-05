
void UpdateScreen(){
	
	if (InMenuState==0) {
		//Resfresh screen only if not in menu
			PrintSetTemperature(ChannelXOffset[0],1);
			PrintSetTemperature(ChannelXOffset[1],2);
			PrintMeasuredTemperature(ChannelXOffset[0],1);
			PrintMeasuredTemperature(ChannelXOffset[1],2);
			PrintInputVoltage();

		//Print stars to indicate long press of buttons
		if (SelectedChannel==1){
			if((InterruptCounterS1>ButtonsVeryLongPress)||(InterruptCounterS2>ButtonsVeryLongPress)){
			//Print 2 stars for Channel 1
			PrintStarsChannel(Colour1,ChannelXOffset[0],2);
			} else if((InterruptCounterS1>ButtonsLongPress)||(InterruptCounterS2>ButtonsLongPress)){
				//Print 1 star for Channel 1
				PrintStarsChannel(Colour1,ChannelXOffset[0],1);
			} else {
			PrintStarsChannel(Colour1,ChannelXOffset[0],0);
			}
		} else if (SelectedChannel==2){
			if((InterruptCounterS1>ButtonsVeryLongPress)||(InterruptCounterS2>ButtonsVeryLongPress)){
			//Print 2 stars for Channel 2
			PrintStarsChannel(Colour2,ChannelXOffset[1],2);
			} else if((InterruptCounterS1>ButtonsLongPress)||(InterruptCounterS2>ButtonsLongPress)){
				//Print 1 star for Channel 2
				PrintStarsChannel(Colour2,ChannelXOffset[1],1);
			} else {
				PrintStarsChannel(Colour2,ChannelXOffset[1],0);
			}
		}
		
		//Print channel state
		PrintChannelState(Colour1,ChannelXOffset[0],1);
		PrintChannelState(Colour2,ChannelXOffset[1],2);
	}


	//PrintInputVoltage();
  
}

void UpdateAllScreen(){
	TFTscreen.background(0, 0, 0);
	PrintChannel(Colour1,ChannelXOffset[0],1);
	PrintChannel(Colour2,ChannelXOffset[1],2);
	PrintPresetTemperatures();
	PrintInputVoltage();
}

void PrintChannel(int Colour[3], int Offset, int Ch){ 
  TFTscreen.setTextSize(1);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);//red,green,blue
  TFTscreen.setCursor(5+Offset+XOffset,5+YOffset);
  TFTscreen.print("Status: ");
  PrintChannelState(Colour,Offset,Ch);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(5+Offset+XOffset,29+YOffset);
  TFTscreen.print("Set: ");
  PrintSetTemperature(Offset,Ch);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(5+Offset+XOffset,61+YOffset);
  TFTscreen.print("Measured: ");
  PrintMeasuredTemperature(Offset,Ch);
}

void PrintStarsChannel(int Colour[3], int Offset, int stars){ 
  TFTscreen.setTextSize(1);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setCursor(35+Offset+XOffset,29+YOffset);
  if (stars==1){
    //Print one star
    TFTscreen.print("*");
  } else if (stars==2){
    //Print two stars
    TFTscreen.print("**");
  } else {
    //Clear the stars area
    TFTscreen.noStroke();
    TFTscreen.fill(0,0,0);
    TFTscreen.rect(35+Offset+XOffset,29+YOffset,12,8);
  }
}

void PrintChannelState(int Colour[3],int Offset,int Ch){
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(23+Offset+XOffset,14+YOffset);
  if (Ch==1){
	  TFTscreen.setTextColor(ColourCH1, BLACK);
    if (ChannelState[0]==1)  {
      TFTscreen.print("ON ");
    } else if ((ChannelState[0]==0)&(MeasuredTemperature[0]<TemperatureDisconnectSensing)) {
      TFTscreen.print("OFF");
      } else {
        TFTscreen.print("NC ");
        ChannelState[0]=2;
      }
  }
  if (Ch==2){
	  TFTscreen.setTextColor(ColourCH2, BLACK);
    if (ChannelState[1]==1)  {
      TFTscreen.print("ON ");
    } else if ((ChannelState[1]==0)&(MeasuredTemperature[1]<TemperatureDisconnectSensing)) {
      TFTscreen.print("OFF");
      } else {
        TFTscreen.print("NC ");
        ChannelState[1]=2;
      }
  }
}

void PrintSetTemperature(int Offset,int Ch){
  TFTscreen.setTextSize(2);
  if (SelectedChannel==Ch){
    //Channel is highlited (print inverted)
    if (Ch==1) { 
      TFTscreen.setTextColor(BLACK, ColourCH1);
    } else {
      TFTscreen.setTextColor(BLACK, ColourCH2);
    }
    TFTscreen.setCursor(24+Offset+XOffset,41+YOffset);
    if (SetTemperature[Ch-1]<100) {
      TFTscreen.print(" ");
      TFTscreen.print(SetTemperature[Ch-1]);
    } else {
      TFTscreen.print(SetTemperature[Ch-1]);
    }
  }
  else {
    //Channel is NOT highlited (print normally)
    if (Ch==1) { 
      TFTscreen.setTextColor(ColourCH1, BLACK);
    } else {
      TFTscreen.setTextColor(ColourCH2, BLACK);
    }
    TFTscreen.setCursor(24+Offset+XOffset,41+YOffset);
    if (SetTemperature[Ch-1]<100) {
      TFTscreen.print(" ");
      TFTscreen.print(SetTemperature[Ch-1]);
    } else {
      TFTscreen.print(SetTemperature[Ch-1]);
    }
  }
}

void PrintMeasuredTemperature(int Offset,int Ch){
	TFTscreen.setTextSize(3);
	if (Ch==1) { 
		TFTscreen.setTextColor(ColourCH1, BLACK);
	} else {
		TFTscreen.setTextColor(ColourCH2, BLACK);
	}
	TFTscreen.setCursor(10+Offset+XOffset,73+YOffset);
  // save temp in other variable, to avoid change by ISR
  unsigned int storetemperature = MeasuredTemperature[Ch-1];
	if (storetemperature>=TemperatureDisconnectSensing){
		TFTscreen.print(" - ");
	} else if (storetemperature<=LowTemperatureLimit){
		TFTscreen.print("LOW");
	} else if (storetemperature<100) {
		TFTscreen.print(" ");
		TFTscreen.print(storetemperature);
	} else {
		TFTscreen.print(storetemperature);
	}
}

void PrintPresetTemperatures(){
  TFTscreen.noStroke();
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(0,105,160,20);
  TFTscreen.stroke(Colour3[0], Colour3[1], Colour3[2]);//red,green,blue
  TFTscreen.line(0+XOffset, 105+YOffset, 160+XOffset, 105+YOffset); //Line
  TFTscreen.setTextColor(WHITE, BLACK);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(59+XOffset,109);
  TFTscreen.print("PreSets");
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(5+XOffset,109);
  TFTscreen.print(PresetTemperature[0]);
  TFTscreen.setCursor(120+XOffset,109+YOffset);
  TFTscreen.print(PresetTemperature[1]);
}

void PrintInputVoltage(){
	if ((float(VinUVLO)/10)>InputVoltage) {
		//UVLO Condition
		TFTscreen.setTextColor(BLACK, WHITE);
		TFTscreen.setCursor(59+XOffset,118);
		TFTscreen.setTextSize(1);
		TFTscreen.print("Vin UVLO");
		//Disable BOTH Heaters / Channels
		SwitchChannelOFF(1);
		SwitchChannelOFF(2);
		//Decrease display counter (just to show that UVLO condition on the display for a bit longer about 2.5sec)
		VinUVLO_counter = 10;
	} else if (VinUVLO_counter>0) {
			//We had an UVLO event (now cleared)
			//Let's show it on the display for a bit longer
			VinUVLO_counter = VinUVLO_counter - 1;
			}
			else {
				//Normal operating mode
				VinUVLO_counter = 0;
				TFTscreen.setTextColor(WHITE, BLACK);
				TFTscreen.setCursor(59+XOffset,118);
				TFTscreen.setTextSize(1);
				TFTscreen.print("Vin ");
				TFTscreen.print(InputVoltage,1);
				TFTscreen.print(" ");
			}
}


void PrintMenu(){
	//Print Menu
	//Print black square (menu background)
	TFTscreen.stroke(255,255,255);
	TFTscreen.fill(0,0,0);
	//Print black square with whiteline
	TFTscreen.rect(8,10,140,62);
	TFTscreen.setTextColor(WHITE, BLACK);
	TFTscreen.setTextSize(1);
	TFTscreen.setCursor(20,22);
	TFTscreen.print(" Max Duty: ");
	TFTscreen.setCursor(110,22);
	TFTscreen.print(MaxDutyCycle);
	TFTscreen.setCursor(20,32);
	TFTscreen.print(" UVLO [V]: ");
	TFTscreen.setCursor(110,32);
	TFTscreen.print(float(float(VinUVLO)/10),1);
	TFTscreen.setCursor(20,42);
	TFTscreen.print(" Ch.Tracking: ");
	TFTscreen.setCursor(110,42);
	if (ChannelTrackingState==0) {
		TFTscreen.print("OFF");
	} else {
		TFTscreen.print("ON ");
	}
	TFTscreen.setCursor(20,52);
	TFTscreen.print(" AutoON: ");
	TFTscreen.setCursor(110,52);
	if (AutoOnState==0) {
		TFTscreen.print("OFF");
	} else {
		TFTscreen.print("ON ");
	}
}

void UpdateMenu(){
	if (MenuItem == 1){
		//Change item 1 and Print
		MaxDutyCycle = MaxDutyCycle + RotaryCounter;
		//Check max and min Duty cyckle (no min as unsigned int :D )
		if (MaxDutyCycle>255) {MaxDutyCycle = 255;}
		RotaryCounter = 0;
		TFTscreen.setCursor(20,22);
		TFTscreen.print("*");
		TFTscreen.setCursor(20,32);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,42);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,52);
		TFTscreen.print(" ");
		//Update the value 1st line
		TFTscreen.setCursor(110,22);
		TFTscreen.print(MaxDutyCycle);
		TFTscreen.print("  ");
	}
	if (MenuItem == 2){
		//Change item 2 and Print
		VinUVLO = VinUVLO + RotaryCounter;
		RotaryCounter = 0;
		//Check UVLO level (between 0 and 25V)
		if (VinUVLO>255) {VinUVLO = 255;}
		TFTscreen.setCursor(20,22);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,32);
		TFTscreen.print("*");
		TFTscreen.setCursor(20,42);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,52);
		TFTscreen.print(" ");
		//Update the value 2nd line
		TFTscreen.setCursor(110,32);
		TFTscreen.print(float(float(VinUVLO)/10),1);
		TFTscreen.print("  ");
	}
	if (MenuItem == 3){
		//Change item 3 and Print
		ChannelTrackingState = ChannelTrackingState + RotaryCounter;
		RotaryCounter = 0;
		//Convert ChannelTrackingState to a kind of bool for simplicity ;)
		if (ChannelTrackingState>=1) {ChannelTrackingState = 1;}
		if (ChannelTrackingState<=0) {ChannelTrackingState = 0;}
		TFTscreen.setCursor(20,22);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,32);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,42);
		TFTscreen.print("*");
		TFTscreen.setCursor(20,52);
		TFTscreen.print(" ");
		//Update the value 3rd line
		TFTscreen.setCursor(110,42);
		if (ChannelTrackingState==0) {
			TFTscreen.print("OFF");
		} else {
			TFTscreen.print("ON ");
		}
	}
	if (MenuItem == 4){
		//Change item 4 and Print
		AutoOnState = AutoOnState + RotaryCounter;
		RotaryCounter = 0;
		//Convert ChannelTrackingState to a kind of bool for simplicity ;)
		if (AutoOnState>=1) {AutoOnState = 1;}
		if (AutoOnState<=0) {AutoOnState = 0;}
		TFTscreen.setCursor(20,22);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,32);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,42);
		TFTscreen.print(" ");
		TFTscreen.setCursor(20,52);
		TFTscreen.print("*");
		//Update the value 3rd line
		TFTscreen.setCursor(110,52);
		if (AutoOnState==0) {
			TFTscreen.print("OFF");
		} else {
			TFTscreen.print("ON ");
		}
	}
}
