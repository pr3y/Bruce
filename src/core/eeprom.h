#ifndef __BRUCE_EEPROM_H__
#define __BRUCE_EEPROM_H__

#include <EEPROM.h>
#include <Arduino.h>


/*
EEPROM ADDRESSES MAP

0	Rotation	  16		    32	Pass	48	Pass	64	Pass	80	Pass	96		112
1	Dim(N/L)	  17		    33	Pass	49	Pass	65	Pass	81	Pass	97		113
2	Bright	      18		    34	Pass	50	Pass	66	Pass	82	Pass	98		114
3	-	          19		    35	Pass	51	Pass	67	Pass	83	Pass	99		115
4	-	          20	Pass	36	Pass	52	Pass	68	Pass	84	Pass	100		116
5	-	          21	Pass	37	Pass	53	Pass	69	Pass	85		    101		117
6	irTX	      22	Pass	38	Pass	54	Pass	70	Pass	86		    102		118	(L-odd)
7	irRx	      23	Pass	39	Pass	55	Pass	71	Pass	87		    103		119	(L-odd)
8	RfTX	      24	Pass	40	Pass	56	Pass	72	Pass	88		    104		120	(L-even)
9	rfRx	      25	Pass	41	Pass	57	Pass	73	Pass	89		    105		121	(L-even)
10  TimeZone      26	Pass	42	Pass	58	Pass	74	Pass	90		    106		122	(L-BGCOLOR)
11  FGCOLOR       27	Pass	43	Pass	59	Pass	75	Pass	91		    107		123	(L-BGCOLOR)
12  FGCOLOR       28	Pass	44	Pass	60	Pass	76	Pass	92		    108		124	(L-FGCOLOR)
13  rfModule      29	Pass	45	Pass	61	Pass	77	Pass	93		    109		125	(L-FGCOLOR)
14  rfidModule    30	Pass	46	Pass	62	Pass	78	Pass	94		    110		126	(L-AskSpiffs)
15		          31	Pass	47	Pass	63	Pass	79	Pass	95		    111		127	(L-OnlyBins)

From 1 to 5: Nemo shared addresses
(L -*) stands for Launcher addresses
*/


#define EEPROM_ROT            (0)
#define EEPROM_DIMMER         (1)
#define EEPROM_BRIGHT         (2)
#define EEPROM_IR_TX          (6)
#define EEPROM_IR_RX          (7)
#define EEPROM_RF_TX          (8)
#define EEPROM_RF_RX          (9)
#define EEPROM_TMZ            (10)
#define EEPROM_FGCOLOR0       (11)
#define EEPROM_FGCOLOR1       (12)
#define EEPROM_RF_MODULE      (13)
#define EEPROM_RFID_MODULE    (14)
#define EEPROM_PWD            (20)


void load_eeprom(void);

void write_eeprom(int address, uint8_t val);
void write_eeprom_string(int address, String val);

uint8_t read_eeprom(int address);
String read_eeprom_string(int address);

void sync_eeprom_values(void);

#endif