#include "stdafx.h"

// Ensoniq Mirage Sysex ID
unsigned char	MirID[] = {0xF0,
							0x0F,
							0x01};  // Mirage Identifier

// This command instructs the mirage to dump its current
// configuration parameters
unsigned char	ConfigParmsDumpReq[]={5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x00,
									0xF7}; // Configuration parameters dump request


unsigned char	MirageCommandCode[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01}; // Command Code

unsigned char	SelectLowerSample[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x15 // Lower Sample Select
									};

unsigned char	SelectUpperSample[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x14 // Upper Sample Select
									};

unsigned char	ConfigParmsDump[]={MirID[0],
									MirID[1],
									MirID[2],
									0x02}; // Configuration parameters dump 

unsigned char	ProgramDumpReqLower[]={5,
										MirID[0],
										MirID[1],
										MirID[2],
										0x03,
										0xF7}; // Lower Program Dump Request

unsigned char	ProgramDumpReqUpper[]={5, // Length of sysex
										MirID[0],
										MirID[1],
										MirID[2],
										0x13,
										0xF7}; // Upper Program Dump Request

// Used to ask the mirage to dump the current wavesample as selected
// by Wavesample select, parameter [26]
unsigned char	WaveDumpReq[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x04,
									0xF7}; // Wavesample dump request

unsigned char	ProgramDumpLower[] = {MirID[0],
										MirID[1],
										MirID[2],
										0x05};

unsigned char	ProgramDumpUpper[] = {MirID[0],
										MirID[1],
										MirID[2],
										0x15};

unsigned char	WaveDumpData[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x06}; // WaveSample Dump Data

unsigned char	ProgramStatusMessage[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x07}; // Program Dump Data (previous selected sample)

unsigned char	WavesampleStatusMessage[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x08}; // Wavesample Status Message

unsigned char	WavesampleAck[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x09,
									0xF7}; // Sent by the Mirage when the checksum of the received wavesampledump is good. Also sent when a wavesample function is completed

unsigned char	WavesampleNack[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x0A,
									0xF7}; // Sent by the Mirage when the checksum of a received dump is bad

unsigned char ParmChange[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x01,
									0x7F, // End of Commando Code
									0xF7};

unsigned char GetCurrentValue[] = {7,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x7F, // End of Command Code
								0xF7};

unsigned char SampleEnd[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x01,
									0x7F, // End of Commando Code
									0xF7};

unsigned char ValueDown[] = {8,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x0F, // Value Down
								0x7F, // End of Command Code
								0xF7};

unsigned char ValueUp[] = {8,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x0E, // Value Down
								0x7F, // End of Command Code
								0xF7};

unsigned char LoopStart[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x02,
									0x7F, // End of Command Code
									0xF7};

unsigned char LoopEnd[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x03,
									0x7F, // End of Command Code
									0xF7};

unsigned char LoopEndFine[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x04,
									0x7F, // End of Command Code
									0xF7};


unsigned char LoopOn[]={11,
						MirID[0],
						MirID[1],
						MirID[2],
						0x01,
						0x0C,
						0x06,
						0x05,
						0x0D,
						0x0E, // Up Arrow
						0x7F,
						0xF7};

unsigned char LoopOff[]={11,
						MirID[0],
						MirID[1],
						MirID[2],
						0x01,
						0x0C,
						0x06,
						0x05,
						0x0D,
						0x0F, // Down Arrow
						0x7F,
						0xF7};

unsigned char TuningCourse[]={9,
							MirID[0],
							MirID[1],
							MirID[2],
							0x01, // Commando Code
							0x0C, // Parameter 
							0x06, 
							0x07,
							0x7F, // End of Command Code
							0xF7};

unsigned char TuningFine[]={9,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Commando Code
								0x0C, // Parameter 
								0x06, 
								0x08,
								0x7F, // End of Command Code
								0xF7};

unsigned char ParmCurValue[9]={
								MirID[0],
								MirID[1],
								MirID[2],
								0x0E, // Wavesample Parameter Message
								0x00, // ms nybble is bank number, ls nybble is sample number
								0x00, // Parameter Number
								0x00, // Value LS nybble
								0x00, // Value MS nybble
								0xF7};

unsigned char ReceivedParmNumber;
unsigned char ReceivedParmValue[100];
//unsigned char ParmCurValue;