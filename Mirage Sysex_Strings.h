extern unsigned char MirID[];  // Mirage Identifier

// This command instructs the mirage to dump its current
// configuration parameters
extern unsigned char ConfigParmsDumpReq[]; // Configuration parameters dump request
extern unsigned char MirageCommandCode[];
extern unsigned char ConfigParmsDump[]; // The Response to ConfigParmsDumpReq;
extern unsigned char ProgramDumpReqLower[]; // Lower Program Dump Request
extern unsigned char ProgramDumpReqUpper[]; // Upper Program Dump Request
extern unsigned char SelectLowerSample[];
extern unsigned char SelectUpperSample[];

// Used to ask the mirage to dump the current wavesample as selected
// by Wavesample select, parameter [26]
extern unsigned char WaveDumpReq[]; // Wavesample dump request
extern unsigned char WaveDumpData[]; // WaveSample Dump Data
extern unsigned char ProgramDumpLower[];
extern unsigned char ProgramDumpUpper[];
extern unsigned char ProgramStatusMessage[];
extern unsigned char WavesampleStatusMessage[];
extern unsigned char WavesampleAck[];
extern unsigned char WavesampleNack[];

// Set front panel parameters
extern unsigned char ValueUp[];
extern unsigned char ValueDown[];
extern unsigned char SampleEnd[];
extern unsigned char LoopStart[];
extern unsigned char LoopEnd[];
extern unsigned char LoopEndFine[];
extern unsigned char LoopOff[];
extern unsigned char LoopOn[];

// Front panel parameters for tuning
extern unsigned char TuningCourse[];
extern unsigned char TuningFine[];

// For parameter changing
extern unsigned char ParmChange[];
extern unsigned char GetCurrentValue[];
extern unsigned char ReceivedParmNumber;
extern unsigned char ReceivedParmValue[];
extern unsigned char ParmCurValue[];
