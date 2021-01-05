//Software Version
#define SwVersion "Sw.1.8"
#define Author "by ThomasVDD"
#define UserName false
#define User "XX YYY"

//HARDWARE CONFIGURATION
//LCD TFT pins
#define DisplayCSPin   4
#define DisplayDCPin   5
#define DisplayRSTPin  8//(not connected ;) 
//Encoder pins definitions 
#define EncoderAPin 3
#define EncoderBPin 2
#define EncoderSWPin 8
//Heaters pin Definitions
#define Heater1Pin 9
#define Heater2Pin 10
//Temperature Sensor pins Definitions
#define TemperatureSensor1Pin A1
#define TemperatureSensor2Pin A0
//Buttons pins definitions
#define SwitchButton1Pin 0 // THOMASVDD
#define SwitchButton2Pin 1 // THOMASVDD
//Debug pin
#define DebugPin 7
//Others
#define InputVoltagePin A2
#define InternalTemperaturePin A3
//Hardware limits
//Based on measured values
#define DelayMeasurementms 1	//Delay to avoid noise measurement, transistor delay is 200us and 200-300us for opamp steady state delay
//Internal NTC configuration
#define NTCResistance 100000
#define NTCRefTemperature 25
//#define NTCBeta 4485
#define NTCBeta 3950
#define NTCSerieResistor 100000

//Timer1 and PWM frequency definitions
#define Timer1CounterScaling 10

//INPUT VOLTAGE sensing
//OLD HARDWARE v.0.99 (not released only HW testing)
//#define InputVoltageScaling 0.0455728 // 10k and 1.2k 0.0455728, Vref*(Rlow+Rhigh)/Rlow/1024
//NEW HARDWARE v.1.00
// THOMASVDD
#define InputVoltageScaling 0.059; // 10k and 1k 0.0537108, Vref*(Rlow+Rhigh)/Rlow/1024

//Preset MaxDutyCycle depending on the power supply
#define PresetMaxDutyCycle 110 		//Set the default max duty cycle to limit the power from the power supply
//Vin Under Volatge Lock Out (VinUVLO) for battery operation
#define PresetVinUVLO 80 	//80 = 8V, NOT implemented yet

//SOFTWARE CONFIGURATION
//Buttons delays (short VS long press) each count is about 2ms, so  15=30ms
#define ButtonsPressLatch 25 
#define ButtonsPressDebounce 175
#define ButtonsLongPress 800 
#define ButtonsVeryLongPress 1600

// Dispaly Pixels OFFSET
#define XOffset 0
#define YOffset 0
#define DisplayRefreshEveryNLoop 30
#define DelayMenuMs 100

//PID parameters
#define OUTPUT_MIN 0
#define OUTPUT_MAX 220
#define KPconst 5.0 //was 5.0
#define KIconst 0.15 //was 0.15
#define KDconst 3.0 //was 3.0
#define BangBangHysteresisON 100
#define BangBangHysteresisOFF 10
#define PIDCalculationInterval 10 //Has to be higher than one full loop
#define LoopDelay 10 //Has to be higher than the PWM period 127Hz

//ADC to temperature definitions
#define ADCtoTemperatureGain 	0.53 //Original Weller Station
#define ADCtoTemperatureOffset  25.0

//Channel Temperatures Definitions
#define StandbyTemparature	175
#define MinimumTemperature  25
#define MaximumTemperature	455
#define SetTemperatureCh1	275	
#define SetTemperatureCh2	350
#define PresetTemperature1	275
#define PresetTemperature2	350
#define RotaryEncoderTemperatureStep	1
#define TemperatureDisconnectSensing 500
#define LowTemperatureLimit 50

//Tracking Duty Cycle Parameters
#define TrackingDutyCycleMinHysteresysBand 25
#define TrackingDutyCycleTimerStandbyTrigger 1500 //Interrupt timer runs every ~2ms

//Save Parameters Address
#define EEPROM_address 0

//General color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

 
