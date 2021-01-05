// Inclusions of Logo and Definitions
#include "SolderingRT1_Logo.h"
#include "SolderingRT1_Parameters.h"

// Include Libraries
#include <TFT.h>
//#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <FlexiTimer2.h>
#include <AutoPID.h>
#include "EEPROM.h"
#include <avr/pgmspace.h>

//Include some strings in program memory to save RAM
const char HelpMessage[] PROGMEM = {"***************************************************************************\n"
	"Soldering Station serial interface:\n"
	"serial speed 9600baud\n"
	"by Riccardo Pittini\n"
	"***************************************************************************\n"
	"Menu: \n"
	"h: Print help menu\n"
	"a XXX: Set ch.1 temperature, XXX temperature in degC\n"
	"b XXX: Set ch.2 temperature, XXX temperature in degC\n"
	"d X: Disable/Enable debug serial mode, X 0 or 1\n"
	"w XXX: Set Maximum duty cycle /Input Power (read below, default: 100)\n"
	"u XXX: Set Vin UVLO protection (87 = 8.7V, default: 80)\n"
	"s: Save parameters\n"
	"r: Reset parameters to default values\n"
	"***************************************************************************\n"
	"NOTE on input power limitation: \n"
	"Feauture is implemented as duty cycle limitation\n"
	"Calculate the desired duty cyckle as: Iavgmax = Vin/2*(duty/255), Considers ONLY one channel active\n"
	"duty = Iavgmax*510/Vin\n"
	"Duty value is from 0 to 255, Example: w 181\n"
	"***************************************************************************\n"};

// Create instance for the library
TFT TFTscreen = TFT(DisplayCSPin, DisplayDCPin, DisplayRSTPin);

//Varibles for PID
double TempMeas1, setPoint1, OutputVal1=0;
double TempMeas2, setPoint2, OutputVal2=0;
double KP = KPconst, KI = KIconst, KD = KDconst;
//Create two PID instances
AutoPID PIDch1(&TempMeas1, &setPoint1, &OutputVal1, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPID PIDch2(&TempMeas2, &setPoint2, &OutputVal2, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

//LCD printing offset and colours
const int Colour1[]={255,255,0}; //RGB
const int Colour2[]={0,255,0}; //RGB
const int Colour3[]={255,255,255}; //RGB
const long unsigned ColourCH1=YELLOW;
const long unsigned ColourCH2=GREEN;
const int ChannelXOffset[]={5,85};

//Variables
int RotaryCounter = 0; 
int InterruptCounterEncoderSW = 0; 
int InterruptCounterS1 = 0;
int InterruptCounterS2 = 0;
long unsigned int TimeInterruptCounter = 0;
unsigned int InMenuState = 0;
unsigned int MenuItem = 1;
unsigned int MenuItemState = 0;
unsigned int ChannelState[] = {1,1};
int aState;
int aLastState;
int aStateSW;
int aLastStateSW;
int EncoderButtonState;
int Button1State = 1;
int Button2State = 1;
unsigned int SetTemperature[] = {SetTemperatureCh1,SetTemperatureCh2};
unsigned int PresetTemperature[] = {PresetTemperature1,PresetTemperature2};
unsigned int MeasuredTemperature[] = {0,0};
unsigned int MeasuredTemperatureADCvalue[] = {0,0};
unsigned int MaxDutyCycle = PresetMaxDutyCycle;

#define AVGSamplesLength 16
#define TrackingAVGLength 128
long unsigned int DutyCycleAVGtemp = 0;	
unsigned int DutyCycleAVG1Samples[AVGSamplesLength] = {};
unsigned int DutyCycleAVG1SamplesIndex = 0;
unsigned int TrackingDutyCycleAVG1[TrackingAVGLength] = {};
unsigned int TrackingDutyCycleAVG1Index = 0;

unsigned int DutyCycleAVG1 = 0;
unsigned int DutyCycleAVG2 = 0;
unsigned int OldDutyCycleAVG1 = 0;
unsigned int OldDutyCycleAVG2 = 0;
long unsigned int TimeIntervalStandbyMonitoring = 10000; //10k = 20sec
long unsigned int TrackingDutyCycleTimerStandby [] = {0,0};
unsigned int index = 0;
unsigned int VinUVLO = PresetVinUVLO;
int ChannelTrackingState = 0;
int AutoOnState = 1;
//int InSetMode = 0;
int SelectedChannel = 0;
int RefreshScreen = 0;
int RefreshScreenTemperature = 0;
//int PreviousChannel = 0;
unsigned int InChannelSettings = 0;
unsigned int FirstBoot = 1;
unsigned int VinUVLO_counter = 0;
unsigned int DisplayRefreshCounter = 0;
float InputVoltage = 12.0;
int IncomingChar = 0;
int InternalTemperatureADC = 0;
float InternalTemperature = 0.0;
float Value = 0.0;
//Variables for serial communication
bool DebugMode = 1;
char IncomingCmd[100];
int CmdIndex;

 
void setup() {

	pinMode (EncoderAPin,INPUT_PULLUP);
	pinMode (EncoderBPin,INPUT_PULLUP);
	pinMode (EncoderSWPin,INPUT_PULLUP);
	pinMode (TemperatureSensor1Pin,INPUT);
	pinMode (TemperatureSensor2Pin,INPUT);
	pinMode (InputVoltagePin,INPUT);
	pinMode (InternalTemperaturePin,INPUT);
	pinMode (SwitchButton1Pin,INPUT_PULLUP);
	pinMode (SwitchButton2Pin,INPUT_PULLUP);
	pinMode (Heater1Pin, OUTPUT);
	pinMode (Heater2Pin, OUTPUT);
	pinMode (DebugPin,OUTPUT);

	Serial.begin (115200);    

	//initialize the library
	TFTscreen.begin();

	// clear the screen with a black background
	TFTscreen.background(0, 0, 0);
	TFTscreen.setTextSize(1);
	TFTscreen.stroke(255,255,0);//red,green,blue
	TFTscreen.setCursor(10+XOffset,20+YOffset);
	TFTscreen.println("Soldering RT1");
	TFTscreen.println(" ");
	TFTscreen.setTextSize(3);
	TFTscreen.setCursor(10+XOffset,35+YOffset);
	TFTscreen.println(SwVersion);
	TFTscreen.setTextSize(1);
	TFTscreen.setCursor(30+XOffset,80+YOffset);
	TFTscreen.println(Author);
	if (UserName){TFTscreen.setCursor(30+XOffset,95+YOffset); TFTscreen.print("Property of"); TFTscreen.setCursor(30+XOffset,105+YOffset);TFTscreen.println(User);}
	//delay(1500);
	//TFTscreen.background(0, 0, 0);
	//TFTscreen.drawBitmap(5,5,stationLOGO1,124,47,23432);
	delay(500);
	TFTscreen.background(0, 0, 0);

	FirstBoot = EEPROM.read(EEPROM_address);
	if (FirstBoot==0){
		//NOT the first boot, Load STORED parameters
		LoadParameters();
		//Check that data are not corrupted
		if ((PresetTemperature[0]<MinimumTemperature)|(PresetTemperature[0]>MaximumTemperature)){
			InitializeEEPROM();
		}
		if ((PresetTemperature[1]<MinimumTemperature)|(PresetTemperature[1]>MaximumTemperature)){
			InitializeEEPROM();
		}
		if (MaxDutyCycle>OUTPUT_MAX){
		  InitializeEEPROM();
		}
		if (VinUVLO>255){
		  InitializeEEPROM();
		}
		//Now everything good, we can set the set-points
		//Load the channels to de default parameters
		SetTemperature[0] = PresetTemperature[0];
		SetTemperature[1] = PresetTemperature[1];
	} else {
		//Write the memory cells, as content is unknown
		InitializeEEPROM();
	}

	//Print Soldering Iron Channels T1 and T2
	PrintChannel(Colour1,ChannelXOffset[0],1);
	PrintChannel(Colour2,ChannelXOffset[1],2);
	PrintPresetTemperatures();

	// Reads the initial state of the EncoderA
	aLastState = digitalRead(EncoderAPin);

	//*************************************************
	// Timer 1, USED for PWM
	//*************************************************
	// Pin Setup
	// Output Direction
	DDRB |= (1 << 7) | (1 << 6) | (1 << 5);
	// Counter max 15000 
	ICR1 = 0x0A00;  // 256*10 about 98Hz with /64
	/* 
	[COM1A1,COM1A0,COM1B1,COM1B0,COM1C1,COM1C0,WGM11.WGM10]
	bit 7:2 Compare output mode 3 channels.  
	Setting OCnA/OCnB/OCnC on comptare match, set output to LOW
	bit 1:0  Fast PWM, TOP: ICRn  Modality
	*/
	TCCR1A = 0b10101010;
	/*
	[ICNC1,ICES1,–,WGM13,WGM12,CS12,CS11,CS10]
	bit 7:5 Don't Care
	bit 4:3  Fast PWM
	bit 2:0 Prescaler: clock/8
	001=/1,010=/8,011=/64,100=/256,101=/1024
	*/
	TCCR1B = 0b00011011;

	TIMSK1 |= (1<<OCIE1A)|(1<<OCIE1B); //enable output compare interrupt
	// Initializing output variables
	OCR1A = 0;
	OCR1B = 0;

	//Setup interrupt, running every 2ms for encoder and buttons triggers
	FlexiTimer2::set(2, InterruptMain); // Good was : 2 = interrupt runs every 2ms
	FlexiTimer2::start();

	//Configure PIDs
	//If temperature is more than 50 degrees below or above setpoint, OUTPUT will be set to min or max respectively
	PIDch1.setBangBang(BangBangHysteresisON,BangBangHysteresisOFF);
	PIDch2.setBangBang(BangBangHysteresisON,BangBangHysteresisOFF);
	//set PID update interval to XXX ms
	PIDch1.setTimeStep(PIDCalculationInterval);
	PIDch2.setTimeStep(PIDCalculationInterval);
	//Load Maximum duty cycles
	PIDch1.setOutputRange(0,MaxDutyCycle);
	PIDch2.setOutputRange(0,MaxDutyCycle);

	sei();
	//Just a delay to let things settle...
	delay(100);
}

void loop() {
	//Encure that the loop is longer than 1 PWM cycle.
	delay(LoopDelay);
	//RUN the main loop
	MonitorChannels();
	ExecuteControl();
	ExecuteStateMachine();
	//Update display
	if (DisplayRefreshCounter >= DisplayRefreshEveryNLoop) {
		UpdateScreen();
		DisplayRefreshCounter = 0;
	} else { 
		DisplayRefreshCounter++;
	}
	//Communication at last
	ExecuteSerialCommunication();
	if (DebugMode) {
		SerialDebug();
	}
}


void ExecuteControl(){
	//Monitor Duty Cycle for Standby Function
	MonitorChannels();
	//Update PID setpoints
	setPoint1 = SetTemperature[0];
	setPoint2 = SetTemperature[1];
	//Compute both PIDs and Write Outputs
	UpdatePWM();
}

void ReadInternalTemperature(){
	InternalTemperature = 0.0;
	InternalTemperatureADC = analogRead(InternalTemperaturePin);
	Value = InternalTemperatureADC;
	Value = 1023 / Value - 1;
	Value = NTCSerieResistor / Value;
	InternalTemperature = Value / NTCResistance;     // (R/Ro)
	InternalTemperature = log(InternalTemperature); // ln(R/Ro)
	InternalTemperature /= NTCBeta;                   // 1/B * ln(R/Ro)
	InternalTemperature += 1.0 / (NTCRefTemperature + 273.15); // + (1/To)
	InternalTemperature = 1.0 / InternalTemperature;                 // Invert the value
	InternalTemperature -= 273.15;                         // Convert it to Celsius
}

void StopPWM(){
	OCR1A = 0;
	OCR1B = 0;
}

void UpdatePWM(){
	
	//Enable the PWM only if the channel is active
	if (ChannelState[0]==1)  //Checks that a soldering probe is connected to the station, if not T>500 (ADC saturated)
	{	if (MeasuredTemperature[0]<TemperatureDisconnectSensing) {
			//Measured temperature is in the regular range, regular PID operation 
			PIDch1.run();
			OCR1A = OutputVal1*Timer1CounterScaling;
		} else {
			ToggleChannelONOFF(1);
			PIDch1.stop();
			PIDch1.reset();
		}
	} else if ((ChannelState[0]==2)&(MeasuredTemperature[0]<TemperatureDisconnectSensing)) {
			//Channel was disconnected but now is connected
			if (AutoOnState == 1) {
				//change state back to ON (Auto ON function, Default)
				SwitchChannelON(1);
				PIDch1.run();
			} else if (AutoOnState == 0) {
				//change state back to OFF
				SwitchChannelOFF(1);
				PIDch1.stop();
				PIDch1.reset();
			}
		} else if((ChannelState[0]==0)&(MeasuredTemperature[0]>=TemperatureDisconnectSensing)) 
			{ //The heater has been disconnected
				SwitchChannelNC(1);
				PIDch1.stop();
				PIDch1.reset();
			} else {
				SwitchChannelOFF(1);
				PIDch1.stop();
				PIDch1.reset();
			}


	//Enable the PWM only if the channel is active
	if (ChannelState[1]==1)  //Checks that a solering probe is connected to the station, if not T>500 (ADC saturated)
	{	if (MeasuredTemperature[1]<TemperatureDisconnectSensing) {
			//Measured temperature is in the regular range, regular PID operation 
			PIDch2.run();
			OCR1B = OutputVal2*Timer1CounterScaling;
		} else {
			ToggleChannelONOFF(2);
			PIDch2.stop();
			PIDch2.reset();
		}
	} else if ((ChannelState[1]==2)&(MeasuredTemperature[1]<TemperatureDisconnectSensing)) {
			//Channel was disconnected but now is connected
			if (AutoOnState == 1) {
				//change state back to ON (Auto ON function, Default)
				SwitchChannelON(2);
				PIDch2.run();
			} else if (AutoOnState == 0) {
				//change state back to OFF
				SwitchChannelOFF(2);
				PIDch2.stop();
				PIDch2.reset();
			}
		} else if((ChannelState[1]==0)&(MeasuredTemperature[1]>=TemperatureDisconnectSensing))
			{ //The heater has been disconnected
				SwitchChannelNC(2);
				PIDch2.stop();
				PIDch2.reset();
			} else {
				SwitchChannelOFF(2);
				PIDch2.stop();
				PIDch2.reset();
			}
	
}

void ReadTemperatures(){
  MeasuredTemperatureADCvalue[0] = analogRead(TemperatureSensor1Pin);
  MeasuredTemperatureADCvalue[1] = analogRead(TemperatureSensor2Pin);
  MeasuredTemperature[0] = MeasuredTemperatureADCvalue[0]*ADCtoTemperatureGain+ADCtoTemperatureOffset;
  MeasuredTemperature[1] = MeasuredTemperatureADCvalue[1]*ADCtoTemperatureGain+ADCtoTemperatureOffset;
	TempMeas1 = MeasuredTemperature[0];
	TempMeas2 = MeasuredTemperature[1];
}

void SetChannelTemperature(){
    if ((SelectedChannel>0)&&(InMenuState==0)){
		if (SelectedChannel==1){
			SetTemperature[0] = SetTemperature[0] + RotaryCounter;
			//Check temperature ranges
			if (SetTemperature[0] > MaximumTemperature){
				SetTemperature[0] = MinimumTemperature;
			}
			else if (SetTemperature[0] < MinimumTemperature){
				SetTemperature[0] = MaximumTemperature;
			}
     if (ChannelTrackingState==1) {
        //Set temperature for both channels
        SetTemperature[1] = SetTemperature[0];
     }
		}
		else {
			SetTemperature[1] = SetTemperature[1] + RotaryCounter;
			//Check temperature ranges
			if (SetTemperature[1] > MaximumTemperature){
				SetTemperature[1] = MinimumTemperature;
			}
			else if (SetTemperature[1] < MinimumTemperature){
				SetTemperature[1] = MaximumTemperature;
			}
     if (ChannelTrackingState==1) {
        //Set temperature for both channels
        SetTemperature[0] = SetTemperature[1];
     }
		}
		if (RotaryCounter!=0) {
			// Update screen only if knob is rotated
			PrintSetTemperature(ChannelXOffset[0],1);
			PrintSetTemperature(ChannelXOffset[1],2);
			RotaryCounter = 0;
		}
    }
}

void SwitchChannelOFF(int Ch){
  if (Ch==1){
    ChannelState[0] = 0;
	  OCR1A = 0;
  } else if (Ch==2){
    ChannelState[1] = 0;
	  OCR1B = 0;
  }
}

void SwitchChannelNC(int Ch){
  if (Ch==1){
    ChannelState[0] = 2;
  } else if (Ch==2){
    ChannelState[1] = 2;
  }
}

void SwitchChannelON(int Ch){
  if (Ch==1){
    ChannelState[0] = 1;
  } else if (Ch==2){
    ChannelState[1] = 1;
  }
}

void ToggleChannelONOFF(int Ch){
  if (Ch==1){
	  if (ChannelState[0]==1){
		  ChannelState[0] = 0;
	  } else if (ChannelState[0]==0) {
		  ChannelState[0] = 1;
	  }
  } else if (Ch==2){
	  if (ChannelState[1]==1){
		  ChannelState[1] = 0;
	  } else if (ChannelState[1]==0) {
		  ChannelState[1] = 1;
	  }
  }
}

void MonitorChannels(){
 //1 cycle 10ms, after 8 cycles i refreshed also the screen total 220ms

 
	//The following part monitors the duty cycle, averages it, and stores it into a vector of averages
	//Store current duty cycle, and increase index
	DutyCycleAVG1Samples[DutyCycleAVG1SamplesIndex] = OutputVal1;
	DutyCycleAVG1SamplesIndex++;
	//Check index, if index=max then calculate average
	if (DutyCycleAVG1SamplesIndex>=AVGSamplesLength) { 
  //Reset index first
  //DutyCycleAVG1SamplesIndex = 0;
		//Just stored the last sample, store AVG value, clean the vector and reset the index
		DutyCycleAVGtemp = 0;
		for (int i=0; i<=AVGSamplesLength-1; i++) {
			DutyCycleAVGtemp = DutyCycleAVGtemp + DutyCycleAVG1Samples[i];
		}
		//Calculate the average over AVGSamplesLength samples, and store in the AVGvector
		DutyCycleAVGtemp = DutyCycleAVGtemp/AVGSamplesLength;
		TrackingDutyCycleAVG1[TrackingDutyCycleAVG1Index] = DutyCycleAVGtemp;
		TrackingDutyCycleAVG1Index++;
		if (TrackingDutyCycleAVG1Index>=TrackingAVGLength){
			//Just roll over index and write over the 
			TrackingDutyCycleAVG1Index = 0;
		}
		//Reset the index (no need to clean the vector as we overwrite it in the next loop...)
		DutyCycleAVG1SamplesIndex = 0;
	}
	
	
	/*
#define AVGSamplesLength 16
#define TrackingAVGLength 64
long unsigned int DutyCycleAVGtemp = 0;	
unsigned int DutyCycleAVG1Samples[AVGSamplesLength] = {};
unsigned int DutyCycleAVG1SamplesIndex = 0;
unsigned int TrackingDutyCycleAVG1Samples[TrackingAVGLength] = {};
unsigned int TrackingDutyCycleAVG1Index = 0;

*/
	

}

void ExecuteStateMachine(){
	if ((SelectedChannel>0)&&(InMenuState==0)){
		//Set temperature via the rotary encoder
		SetChannelTemperature();
		//If rotary knob is pushed and channel is selected and NOT in MENU, toggle channel state (ON/OFF)
		if((SelectedChannel==1)&&(InterruptCounterEncoderSW>ButtonsPressLatch)){
			if (TimeInterruptCounter>0) {
				// Button was released, toggle channel
				ToggleChannelONOFF(1);
				InterruptCounterEncoderSW = 0;
			} //Else: do nothing 
		}
		if((SelectedChannel==2)&&(InterruptCounterEncoderSW>ButtonsPressLatch)){
			if (TimeInterruptCounter>0) {
				// Button was released, toggle channel
				ToggleChannelONOFF(2);
				InterruptCounterEncoderSW = 0;
			} //Else: do nothing /wait
		}
	} if ((InMenuState==0)&&(InterruptCounterEncoderSW>ButtonsPressLatch)) {
		//In main screen with no selected channels
		//A short press for the encoder button should not do anything, only LONG press will open the menu
		if (TimeInterruptCounter>0) {
			// Button was released, do nothign but reset counter
			InterruptCounterEncoderSW = 0;
		}
	}
	
	//Handling menu for additional functions
	//NOTE: the soldering station controller keeps running 
	//Check for entering menu
	if ((InterruptCounterEncoderSW>ButtonsLongPress)&&(InMenuState==0)){
		//Enter MENU and print menu for the first time
		InterruptCounterEncoderSW = 0;
		InMenuState = 1;
		PrintMenu();
	}
	//Check if INMENU and handle it
	if (InMenuState == 1){
		//we are in the MENU
		//Delay every menu cycle by 50ms just for visual
		delay(DelayMenuMs);
		//Print Menu
		TFTscreen.setTextColor(WHITE, BLACK);
		TFTscreen.setTextSize(1);
		UpdateMenu();
		//Check for exiting menu
		if (InterruptCounterEncoderSW>ButtonsLongPress){
			//Exit menu, save parameters, update all screen
			InterruptCounterEncoderSW = 0;
			InMenuState = 0;
			//Save parameters
			SaveParameters();
			//Load parameters, ensures all parameters are synch.
			LoadParameters();
			//Refresh screen
			UpdateAllScreen();
			//Exit menu
		}
		else if (InterruptCounterEncoderSW>ButtonsPressLatch){
			if (TimeInterruptCounter>0) {
				// Button was released
				//Go to next item in the menu
				MenuItem++;
				InterruptCounterEncoderSW = 0;
				if (MenuItem>4){
					//Reached last menu item, loop to first
					MenuItem = 1;
				}
			} //Else: do nothing /wait
		}
	}
	
	//Button 1 (LEFT)
	if((Button1State)&&(InterruptCounterS1>0)){
		if(SelectedChannel==1){
		  //VeryLongPress /LongPress /ShortPress
		  if (InterruptCounterS1>ButtonsVeryLongPress){
				//VeryLongPress: Program the Preset Temperature
				PresetTemperature[0] = SetTemperature[0];
				SaveParameters();
				PrintPresetTemperatures();
			} else if (InterruptCounterS1>ButtonsLongPress){
				//LongPress: Load Preset1 on Channel 1
				SetTemperature[0] = PresetTemperature[0];
				//RefreshScreen = 1;
			} else {
				//ShortPress
				//Exit channel 1
				SelectedChannel = 0;
				//RefreshScreen = 1;
			}
		} else if(SelectedChannel==2) { //No Channel 1 selected
		  //VeryLongPress /LongPress /ShortPress
		  if (InterruptCounterS1>ButtonsVeryLongPress){
				//VeryLongPress: Program the Preset Temperature
				PresetTemperature[0] = SetTemperature[1];
				SaveParameters();
				PrintPresetTemperatures();
			} else if (InterruptCounterS1>ButtonsLongPress){
				//LongPress: Load Preset1 on Channel 1
				SetTemperature[1] = PresetTemperature[0];
				RefreshScreen = 1;
			} else {
				//ShortPress
				//Select channel 1
				SelectedChannel = 1;
				//RefreshScreen = 1;
			}
		} else {
			SelectedChannel = 1;
			//RefreshScreen = 1;
		}
		InterruptCounterS1 = 0;
	}
  
	//Button 2 (RIGHT)
	if((Button2State)&&(InterruptCounterS2>0)){
		if(SelectedChannel==2){
		  //VeryLongPress /LongPress /ShortPress
		  if (InterruptCounterS2>ButtonsVeryLongPress){
				//VeryLongPress: Program the Preset Temperature
				PresetTemperature[1] = SetTemperature[1];
				SaveParameters();
				PrintPresetTemperatures();
			} else if (InterruptCounterS2>ButtonsLongPress){
				//LongPress: Load Preset2 on Channel 2
				SetTemperature[1] = PresetTemperature[1];
				//RefreshScreen = 1;
			} else {
				//ShortPress
				//Exit channel 2
				SelectedChannel = 0;
				//RefreshScreen = 1;
			}
		} else if(SelectedChannel==1) { //No Channel 2 selected
		  //VeryLongPress /LongPress /ShortPress
		  if (InterruptCounterS2>ButtonsVeryLongPress){
				//VeryLongPress: Program the Preset Temperature
				PresetTemperature[1] = SetTemperature[0];
				SaveParameters();
				PrintPresetTemperatures();
			} else if (InterruptCounterS2>ButtonsLongPress){
				//LongPress: Load Preset2 on Channel 1
				SetTemperature[0] = PresetTemperature[1];
				//RefreshScreen = 1;
			} else {
				//ShortPress
				//Select channel 1
				SelectedChannel = 2;
				//RefreshScreen = 1;
			}
		} else {
			SelectedChannel = 2;
			//RefreshScreen = 1;
		}
		InterruptCounterS2 = 0;
	}
}


//Interrupt fur buttons etc... 
void InterruptMain(void){
	aState = digitalRead(EncoderAPin); // Reads the "current" state of the EncoderA
	// If the previous and the current state of the EncoderA are different, that means a Pulse has occured
	if (aState != aLastState){     
	// If the EncoderB state is different to the EncoderA state, that means the encoder is rotating clockwise
	if (digitalRead(EncoderBPin) != aState) { 
		//RotaryCounter --;
		RotaryCounter = RotaryCounter -RotaryEncoderTemperatureStep;
	} else {
		//RotaryCounter ++;
		RotaryCounter = RotaryCounter +RotaryEncoderTemperatureStep;
		}
	} 
	aLastState = aState; // Updates the previous state of the EncoderA with the current state
	
	//Read buttons
	Button1State = digitalRead(SwitchButton1Pin);
	Button2State = digitalRead(SwitchButton2Pin);
	EncoderButtonState = digitalRead(EncoderSWPin);
	if(!Button1State){
		InterruptCounterS1++;
	}
	if(!Button2State){
		InterruptCounterS2++;
	}
	if (!EncoderButtonState){
		InterruptCounterEncoderSW++;
	}

	//Buttons are active low (they are in pullup)
	if ((Button1State+Button2State+EncoderButtonState)==3){
		//No buttons pressed
		TimeInterruptCounter++;
	} else {
		//If a button is pressed, reset the counter
		TimeInterruptCounter = 0;
	}

if (TimeInterruptCounter>55000) {TimeInterruptCounter=0;}
  
  /*if (digitalRead(DebugPin)){
    digitalWrite(DebugPin,0);
  } else {
    digitalWrite(DebugPin,1);
  }*/
  
	//Tracking Duty Cycle timer
	//TrackingDutyCycleTimerStandby[0]++;
	//TrackingDutyCycleTimerStandby[1]++;
}



//Interrupt from PWM compare registers
//pin 9 : OC1A, heater 1
//pin 10: OC1B, heater 2
ISR(TIMER1_COMPA_vect)
{
	if (OCR1A > OCR1B) {
  //if (OutputVal1 > OutputVal2) {
		//Then Duty1 is greater than Duty2
		// TIMER1_COMPA_vect interrupt already occourred, time to measure the temperatures
		delay(DelayMeasurementms);
		//Read input voltage
		InputVoltage = analogRead(InputVoltagePin)*InputVoltageScaling;
		//Read Temperatures
		ReadTemperatures();
	} else {
		//Then Duty2 is greater than Duty1
		//Wait for other interrupt to occour 
	}
}

ISR(TIMER1_COMPB_vect)
{
	if (OCR1A <= OCR1B) {
		//Then Duty2 is greater than Duty1
		// TIMER1_COMPA_vect interrupt already occourred, time to measure the temperatures
		delay(DelayMeasurementms);
		//Read input voltage
		InputVoltage = analogRead(InputVoltagePin)*InputVoltageScaling;
		//Read Temperatures
		ReadTemperatures();		
	} else {
		//Then Duty1 is greater than Duty2
		//Wait for other interrupt to occour 
	}
}
