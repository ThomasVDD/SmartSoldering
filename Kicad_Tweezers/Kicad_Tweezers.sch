EESchema Schematic File Version 4
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x01_Male S1
U 1 1 5FC3826C
P 5350 3900
F 0 "S1" H 5400 3950 50  0000 R CNN
F 1 "S1" H 5300 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5350 3900 50  0001 C CNN
F 3 "~" H 5350 3900 50  0001 C CNN
	1    5350 3900
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x01_Male S2
U 1 1 5FC39A42
P 5450 3900
F 0 "S2" H 5500 3950 50  0000 R CNN
F 1 "S2" H 5400 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5450 3900 50  0001 C CNN
F 3 "~" H 5450 3900 50  0001 C CNN
	1    5450 3900
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x01_Male R1
U 1 1 5FC39CF0
P 5250 3900
F 0 "R1" H 5300 3950 50  0000 R CNN
F 1 "R1" H 5200 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5250 3900 50  0001 C CNN
F 3 "~" H 5250 3900 50  0001 C CNN
	1    5250 3900
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x01_Male R2
U 1 1 5FC39F20
P 5550 3900
F 0 "R2" H 5600 3950 50  0000 R CNN
F 1 "R2" H 5500 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5550 3900 50  0001 C CNN
F 3 "~" H 5550 3900 50  0001 C CNN
	1    5550 3900
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x01_Male T1
U 1 1 5FC3A0C8
P 5150 3900
F 0 "T1" H 5200 3950 50  0000 R CNN
F 1 "T1" H 5100 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5150 3900 50  0001 C CNN
F 3 "~" H 5150 3900 50  0001 C CNN
	1    5150 3900
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x01_Male T2
U 1 1 5FC3A231
P 5650 3900
F 0 "T2" H 5700 3950 50  0000 R CNN
F 1 "T2" H 5600 3900 50  0000 R CNN
F 2 "SWITCH:Audiojack_pad" H 5650 3900 50  0001 C CNN
F 3 "~" H 5650 3900 50  0001 C CNN
	1    5650 3900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4900 3000 5350 3000
Wire Wire Line
	5350 3700 5350 3000
Connection ~ 5350 3000
Wire Wire Line
	5350 3000 5450 3000
Wire Wire Line
	5450 3700 5450 3000
Connection ~ 5450 3000
Wire Wire Line
	5450 3000 5900 3000
Wire Wire Line
	5550 3700 5550 3100
Wire Wire Line
	5550 3100 5900 3100
Wire Wire Line
	5250 3700 5250 3100
Wire Wire Line
	5250 3100 4900 3100
NoConn ~ 4900 3200
NoConn ~ 5900 3200
Wire Wire Line
	5150 3700 5150 3400
Wire Wire Line
	5150 3300 4900 3300
Wire Wire Line
	5650 3300 5900 3300
$Comp
L Connector:AudioJack3_SwitchTR J2
U 1 1 5FC361D6
P 6100 3100
F 0 "J2" H 6150 3450 50  0000 R CNN
F 1 "AudioJack3_SwitchTR" H 6400 3350 50  0000 R CNN
F 2 "SWITCH:Audiojack3_SwitchTR" H 6100 3100 50  0001 C CNN
F 3 "~" H 6100 3100 50  0001 C CNN
	1    6100 3100
	-1   0    0    -1  
$EndComp
$Comp
L Connector:AudioJack3_SwitchTR J1
U 1 1 5FC3584B
P 4700 3100
F 0 "J1" H 4682 3425 50  0000 C CNN
F 1 "AudioJack3_SwitchTR" H 4682 3334 50  0000 C CNN
F 2 "SWITCH:Audiojack3_SwitchTR" H 4700 3100 50  0001 C CNN
F 3 "~" H 4700 3100 50  0001 C CNN
	1    4700 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3700 5650 3300
Wire Wire Line
	5900 3400 5150 3400
Connection ~ 5150 3400
Wire Wire Line
	5150 3400 5150 3300
NoConn ~ 4900 3400
Text Notes 4050 4850 0    50   ~ 0
J2 not inserted ==> T and TN connected\n                          ==> T2 is shorted to T1 and J1\n                          ==> Both channels heat up through J1\n\nJ2 is inserted    ==> T and TN not connected\n                          ==> T2 is connected to J2, and T1 is connected to J1\n                          ==> Both channels heat up seperately
$EndSCHEMATC
