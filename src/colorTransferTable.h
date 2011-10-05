/**
 * \file colorTransferTable.h
 * \brief File contenente le tabelle per la gestione del colore relativo alle ricostruzioni 3D
 * \author ICAR-CNR Napoli
 */

#ifndef _colorTransferTable_h_
#define _colorTransferTable_h_


/********************************************** BlackBody **********************************************/

static unsigned char BlackBody_Red[256] = {0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,93,96,99,102,105,108,111,114,117,120,123,126,129,
										132,135,138,141,144,147,150,153,156,159,162,165,168,171,174,177,180,183,186,189,192,195,198,201,204,207,210,213,216,219,222,225,228,231,
										234,237,240,243,246,249,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255};

static unsigned char BlackBody_Green[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,93,96,99,102,
										105,108,111,114,117,120,123,126,129,132,135,138,141,144,147,150,153,156,159,162,165,168,171,174,177,180,183,186,189,192,195,198,201,204,
										207,210,213,216,219,222,225,228,231,234,237,240,243,246,249,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

static unsigned char BlackBody_Blue[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,
										69,72,75,78,81,84,87,90,93,96,99,102,105,108,111,114,117,120,123,126,129,132,135,138,141,144,147,150,153,156,159,162,165,168,171,174,177,
										180,183,186,189,192,195,198,201,204,207,210,213,216,219,222,225,228,231,234,237,240,243,246,249,252,255};

/************************************************************************************************************/



/********************************************** Cardiac **********************************************/

static unsigned char Cardiac_Red[256] = {73,72,70,69,68,66,65,63,62,61,59,58,57,55,54,52,51,48,45,41,38,35,32,29,26,22,19,16,13,10,6,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,17,17,17,17,18,18,18,18,19,19,19,19,20,20,20,21,22,22,23,24,25,25,26,
									27,28,28,29,30,31,31,32,36,39,43,47,50,54,57,61,65,68,72,76,79,83,86,90,95,99,104,109,113,118,123,128,132,137,142,146,151,156,160,165,
									169,172,176,179,183,186,190,194,197,201,204,208,211,215,218,222,224,226,228,230,232,234,236,239,241,243,245,247,249,251,253,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,250,245,240,236,231,226,221,216,211,206,201,197,192,187,182,177,171,164,158,151,145,138,
									132,125,119,112,106,99,93,86,73};

static unsigned char Cardiac_Green[256] = {193,195,197,198,200,202,204,206,208,209,211,213,215,217,218,220,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,216,
									211,205,200,194,188,183,177,171,166,160,155,149,143,138,132,128,125,121,118,114,110,107,103,99,96,92,89,85,81,78,74,70,67,63,60,56,52,49,
									45,41,38,34,31,27,23,20,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,14,17,19,21,23,25,27,29,31,33,37,41,45,50,54,58,62,66,70,74,78,83,
									87,91,95,99,103,107,111,116,120,124,128,132,136,140,144,149,153,157,161,165,170,174,179,184,188,193,197,202,207,211,216,221,225,230,234,
									239,238,237,236,235,234,233,232,231,229,228,227,226,225,224,223,222,220,218,217,215,213,211,209,208,206,204,202,200,198,197,193};

static unsigned char Cardiac_Blue[256] = {3,12,20,29,37,46,54,63,71,80,88,97,105,114,122,131,139,146,154,161,168,175,183,190,197,204,212,219,226,233,241,248,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,253,252,250,249,247,246,244,243,241,239,238,236,235,233,232,230,229,227,226,224,223,221,220,218,
									217,215,214,212,211,209,208,206,203,201,198,196,193,191,188,186,183,180,178,175,173,170,168,165,163,161,159,157,155,153,151,149,146,144,
									142,140,138,136,134,132,127,123,118,113,109,104,99,95,90,85,80,76,71,66,62,57,53,50,46,43,39,36,32,29,25,21,18,14,11,7,4,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,4,5,5,6,7,8,8,9,10,11,11,12,11,11,10,10,9,9,8,
									8,7,6,6,5,5,4,3};

/************************************************************************************************************/



/********************************************** Flow **********************************************/

static unsigned char Flow_Red[256] = {32,32,32,32,31,31,31,31,31,30,30,30,30,30,29,29,29,29,29,28,28,28,28,28,27,27,27,27,27,26,26,26,26,26,25,25,25,25,25,24,24,24,24,24,23,
								23,23,23,23,22,22,22,22,22,21,21,21,21,21,20,20,20,20,19,19,19,19,18,18,18,17,17,17,16,16,16,16,15,15,15,14,14,14,13,13,13,13,12,12,12,
								11,11,11,10,10,10,10,9,9,9,8,8,8,7,7,7,7,6,6,6,5,5,5,4,4,4,4,3,3,3,2,2,2,1,1,1,1,0,0,0,4,8,12,16,20,24,28,32,36,40,45,49,53,57,61,65,69,
								73,77,81,85,89,93,97,101,105,109,113,117,121,125,130,134,138,142,146,150,154,158,162,166,170,174,178,182,186,190,194,198,202,206,210,215,
								219,223,227,231,235,239,243,247,251,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255};

static unsigned char Flow_Green[256] = {200,197,194,191,188,185,182,179,176,173,170,167,164,161,158,155,152,149,146,143,140,137,134,131,128,125,122,119,116,113,110,107,104,101,
								98,95,92,89,86,83,80,77,74,71,68,65,62,59,56,53,50,47,44,41,38,35,32,29,26,23,20,20,20,19,19,19,19,18,18,18,17,17,17,16,16,16,16,15,15,
								15,14,14,14,13,13,13,13,12,12,12,11,11,11,10,10,10,10,9,9,9,8,8,8,7,7,7,7,6,6,6,5,5,5,4,4,4,4,3,3,3,2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,6,10,14,18,22,26,30,34,38,
								42,46,50,54,58,62,66,70,74,78,82,86,90,94,98,102,106,110,114,118,122,126,130,134,138,142,146,150,154,158,162,166,170,174,178,182,186,190,
								194,198,202,206,210,214,218,222,226,230,234,238,242,246,250,255};

static unsigned char Flow_Blue[256] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,247,244,240,236,232,
								228,225,221,217,213,209,206,202,198,194,190,186,183,179,175,171,167,164,160,156,152,148,145,141,137,133,129,126,122,118,114,110,107,103,
								99,95,91,88,84,80,76,72,69,65,61,57,53,49,46,42,38,34,30,27,23,19,15,11,8,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,16,
								24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,255};

/************************************************************************************************************/



/********************************************** GEColor **********************************************/

static unsigned char GEColor_Red[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,5,7,
								9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53,55,57,59,61,63,65,67,69,71,73,75,77,79,81,83,85,86,88,90,92,94,96,98,
								100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,160,162,164,166,
								168,170,171,173,175,177,179,181,183,185,187,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,219,221,223,225,227,229,231,233,
								235,237,239,241,243,245,247,249,251,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255};

static unsigned char GEColor_Green[256] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,65,67,69,71,73,75,77,79,81,83,85,87,89,91,93,
								95,97,99,101,103,105,107,109,111,113,115,117,119,121,123,125,128,126,124,122,120,118,116,114,112,110,108,106,104,102,100,98,96,94,92,90,
								88,86,84,82,80,78,76,74,72,70,68,66,64,63,61,59,57,55,53,51,49,47,45,43,41,39,37,35,33,31,29,27,25,23,21,19,17,15,13,11,9,7,5,3,1,0,2,4,
								6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,
								98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,160,162,164,
								166,168,170,172,174,176,178,180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,232,
								234,236,238,240,242,244,246,248,250,252,255};

static unsigned char GEColor_Blue[256] = {0,1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53,55,57,59,61,63,65,67,69,71,73,75,77,79,81,83,85,87,89,91,
								93,95,97,99,101,103,105,107,109,111,113,115,117,119,121,123,125,127,129,131,133,135,137,139,141,143,145,147,149,151,153,155,157,159,161,
								163,165,167,169,171,173,175,177,179,181,183,185,187,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,219,221,223,225,227,229,
								231,233,235,237,239,241,243,245,247,249,251,253,255,252,248,244,240,236,232,228,224,220,216,212,208,204,200,196,192,188,184,180,176,172,
								168,164,160,156,152,148,144,140,136,132,128,124,120,116,112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,
								12,8,4,0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,85,89,93,97,101,105,109,113,117,121,125,129,133,137,141,145,149,153,
								157,161,165,170,174,178,182,186,190,194,198,202,206,210,214,218,222,226,230,234,238,242,246,250,255};

/************************************************************************************************************/



/********************************************** GrainRainbow **********************************************/

static unsigned char GrainRainbow_Red[256] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,175,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,95,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
										15,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,0,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,0,81,82,83,84,85,86,87,88,89,90,91,92,93,
										94,95,0,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,0,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,0,129,130,
										131,132,133,134,135,136,137,138,139,140,141,142,143,0,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,35,161,162,163,164,165,
										166,167,168,169,170,171,172,173,174,175,115,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,195,193,194,195,196,197,198,199,
										200,201,202,203,204,205,206,207,255,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,255,225,226,227,228,229,230,231,232,233,
										234,235,236,237,238,239,255,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

static unsigned char GrainRainbow_Green[256] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,49,
										50,51,52,53,54,55,56,57,58,59,60,61,62,63,65,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,145,81,82,83,84,85,86,87,88,89,90,91,92,93,94,
										95,225,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,255,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,255,129,
										130,131,132,133,134,135,136,137,138,139,140,141,142,143,255,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,255,161,162,163,
										164,165,166,167,168,169,170,171,172,173,174,175,255,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,255,193,194,195,196,197,
										198,199,200,201,202,203,204,205,206,207,235,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,155,225,226,227,228,229,230,231,
										232,233,234,235,236,237,238,239,75,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

static unsigned char GrainRainbow_Blue[256] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,255,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
										255,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,255,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,255,81,82,83,84,85,86,87,88,89,90,91,
										92,93,94,95,255,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,205,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
										125,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,45,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,0,161,162,
										163,164,165,166,167,168,169,170,171,172,173,174,175,0,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,0,193,194,195,196,197,
										198,199,200,201,202,203,204,205,206,207,0,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,0,225,226,227,228,229,230,231,232,
										233,234,235,236,237,238,239,0,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

/************************************************************************************************************/



/********************************************** HotIron **********************************************/

static unsigned char HotIron_Red[256] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,
									94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,160,162,
									164,166,168,170,172,174,176,178,180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,
									232,234,236,238,240,242,244,246,248,250,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
									255,255,255,255};

static unsigned char HotIron_Green[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,
									14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,
									104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,160,162,164,166,168,170,
									172,174,176,178,180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,232,234,236,238,
									240,242,244,246,248,250,252,255};

static unsigned char HotIron_Blue[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,8,12,16,20,24,28,32,36,
									40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188,
									192,196,200,204,208,212,216,220,224,228,232,236,240,244,248,252,255};

/************************************************************************************************************/



/********************************************** NIH **********************************************/

static unsigned char NIH_Red[256] = {0,5,11,17,22,28,34,39,45,51,56,62,68,73,79,85,79,74,69,63,58,53,47,42,37,31,26,21,15,10,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,5,10,15,21,26,31,37,42,47,53,58,63,69,74,79,85,95,106,116,127,138,148,159,170,180,191,201,212,223,233,244,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,250,248,246,243,241,239,237,234,232,230,228,225,223,
								221,219,216,214,212,210,208,205,203,201,199,196,194,192,190,187,185,183,181,178,176,174,255,255};

static unsigned char NIH_Green[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,15,
								21,26,31,37,42,47,53,58,63,69,74,79,85,90,95,100,106,111,116,122,127,132,138,143,148,154,159,164,170,175,180,185,191,196,201,207,212,217,
								223,228,233,239,244,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,249,244,239,233,228,223,217,212,207,201,196,191,185,
								180,175,170,164,159,154,148,143,138,132,127,122,116,111,106,100,95,90,85,81,78,75,71,68,65,62,58,55,52,49,45,42,39,35,32,29,26,22,19,16,
								13,9,6,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255};

static unsigned char NIH_Blue[256] = {0,11,22,34,45,56,68,79,90,102,113,124,136,147,158,170,164,159,154,148,143,138,132,127,122,116,111,106,100,95,90,85,90,95,100,106,111,116,
								122,127,132,138,143,148,154,159,164,170,175,180,185,191,196,201,207,212,217,223,228,233,239,244,249,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,249,244,239,233,228,223,217,212,207,201,196,191,185,180,175,170,170,170,170,170,170,170,170,170,170,170,
								170,170,170,170,170,170,159,148,138,127,116,106,95,85,74,63,53,42,31,21,10,0,5,10,15,21,26,31,37,42,47,53,58,63,69,74,79,85,79,74,69,63,
								58,53,47,42,37,31,26,21,15,10,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255};

/************************************************************************************************************/



/********************************************** Spectrum **********************************************/

static unsigned char Spectrum_Red[256] = {255,250,245,240,235,230,225,220,215,210,205,200,195,190,185,180,175,170,165,160,155,150,145,140,135,130,125,120,115,110,105,100,95,90,85,
								80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,15,20,25,30,35,
								40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,
								225,230,235,240,245,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

static unsigned char Spectrum_Green[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,15,20,25,30,35,40,45,50,55,
								60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,
								240,245,251,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,250,245,240,235,230,225,220,215,210,205,200,195,190,185,180,175,170,165,160,155,150,145,140,135,130,125,120,115,110,105,
								100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0};

static unsigned char Spectrum_Blue[256] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,250,245,240,235,230,225,220,215,210,205,200,195,190,185,180,175,170,165,160,155,150,145,140,135,130,125,120,115,110,105,100,95,90,85,
								80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/************************************************************************************************************/



/********************************************** VRBones **********************************************/

static unsigned char VRBones_Red[256] = {0,1,3,5,7,8,10,12,14,15,17,19,21,22,24,26,28,29,31,33,35,37,38,40,42,44,45,47,49,51,52,54,56,58,59,61,63,65,67,68,70,72,74,75,77,79,81,
								82,84,86,88,89,91,93,95,96,98,100,102,104,105,107,109,111,112,114,116,118,119,121,123,125,126,128,130,132,134,135,137,139,141,142,144,
								146,148,149,151,153,155,156,158,160,162,164,165,167,169,171,172,174,176,178,179,181,183,185,186,188,190,192,193,195,197,199,201,202,204,
								206,208,209,211,213,215,216,217,217,218,218,219,219,220,220,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,230,
								230,231,231,232,232,233,233,234,234,235,235,236,236,237,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,247,
								247,248,248,249,249,250,250,251,251,252,252,253,253,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
								255,255,255};

static unsigned char VRBones_Green[256] = {0,1,2,3,4,6,7,8,9,11,12,13,14,16,17,18,19,20,22,23,24,25,27,28,29,30,32,33,34,35,37,38,39,40,41,43,44,45,46,48,49,50,51,53,54,55,56,57,
								59,60,61,62,64,65,66,67,69,70,71,72,74,75,76,77,78,80,81,82,83,85,86,87,88,90,91,92,93,94,96,97,98,99,101,102,103,104,106,107,108,109,
								111,112,113,114,115,117,118,119,120,122,123,124,125,127,128,129,130,131,133,134,135,136,138,139,140,141,143,144,145,146,148,149,150,151,152,153,153,154,
								154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,166,167,168,168,169,170,170,171,172,172,173,174,174,175,175,176,177,177,178,179,
								179,180,181,181,182,183,183,184,185,185,186,187,187,188,189,189,190,191,191,192,193,193,194,195,195,196,196,197,198,198,199,200,200,201,
								202,203,204,205,206,207,208,209,210,211,213,214,215,216,217,218,219,220,221,222,223,224,226,227,228,229,230,231,232,233,234,235,236,237,
								239,240,241,242,243,244,245,246,247,248,249,250,251,251,252,252,252,253,253,253,254,254};

static unsigned char VRBones_Blue[256] = {0,0,0,0,1,1,1,2,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,9,10,10,10,11,11,11,11,12,12,12,13,13,13,13,14,14,14,15,15,15,16,16,
								16,16,17,17,17,18,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,23,23,23,23,24,24,24,25,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,
								29,29,30,30,30,31,31,31,32,32,32,32,33,33,33,34,34,34,34,35,35,35,36,36,36,36,36,37,37,37,37,37,38,38,38,38,39,39,39,39,39,40,40,40,40,
								40,41,41,41,41,42,42,42,42,42,43,43,43,43,44,44,44,44,44,45,45,45,45,45,46,46,46,46,47,47,47,47,47,48,48,48,48,48,49,49,49,49,50,50,50,
								50,50,51,51,51,51,51,52,52,52,53,53,53,54,54,54,55,55,55,56,56,56,56,57,57,57,58,58,58,59,59,59,60,60,60,61,61,61,62,62,62,63,63,63,64,
								64,64,65,65,65,66,66,66,67,84,101,118,135,152,169,186,203,220,237};

/************************************************************************************************************/



/********************************************** VRMusclesBones **********************************************/

static unsigned char VRMusclesBones_Red[256] = {0,2,5,8,10,13,16,18,21,24,26,29,32,34,37,40,42,45,48,51,53,56,59,61,64,67,69,72,75,77,80,83,85,88,91,93,96,99,102,104,
										107,110,112,115,118,120,123,126,128,131,134,136,139,142,144,147,150,153,155,158,161,163,166,169,171,174,177,179,182,185,
										187,190,193,195,198,201,204,206,209,212,214,217,220,222,225,228,230,233,236,238,241,244,246,249,252,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255};
					
static unsigned char VRMusclesBones_Green[256] = {0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,9,10,10,10,10,11,11,11,12,12,12,12,
										12,13,13,13,13,14,14,14,14,15,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,21,
										24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,93,96,99,102,105,108,111,114,117,120,123,126,129,131,
										134,137,140,143,146,149,152,155,158,161,164,167,170,173,176,177,179,180,181,182,183,185,186,187,188,189,191,192,193,194,195,
										197,198,199,200,201,203,204,205,206,207,209,210,211,212,213,215,216,217,218,220,221,222,223,224,226,227,228,229,230,232,233,
										234,235,236,238,239,240,241,241,242,242,242,242,243,243,243,243,244,244,244,244,245,245,245,245,246,246,246,246,247,247,247,
										247,248,248,248,248,248,249,249,249,249,250,250,250,250,251,251,251,251,252,252,252,252,253,253,253,253,254,254,254,254};
					
static unsigned char VRMusclesBones_Blue[256] = {0,0,0,0,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,5,6,6,6,7,7,7,7,8,8,8,9,9,9,10,10,10,10,11,11,11,12,12,12,12,13,13,13,14,14,14,15,
										15,15,15,16,16,16,17,17,17,17,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,22,23,23,23,24,24,24,25,25,25,25,26,26,26,27,
										27,27,27,27,27,26,26,26,25,25,25,24,24,24,23,23,22,22,22,21,21,21,20,20,20,19,19,19,18,18,18,17,17,16,16,16,15,15,15,14,14,
										14,13,13,13,12,12,11,11,11,10,10,10,9,9,10,10,11,11,12,12,13,13,14,15,15,16,16,17,17,18,18,19,19,20,21,21,22,22,23,23,24,24,
										25,26,26,27,27,28,28,29,29,30,31,31,32,32,33,33,34,34,35,36,36,37,37,38,38,39,43,47,51,55,58,62,66,70,74,78,82,86,90,84,89,
										102,105,109,113,117,121,125,129,133,137,141,145,149,153,156,160,164,168,172,176,180,184,188,192,196,200,204,207,211,215,219,
										223,227,231,235,239,243,247,251};

/************************************************************************************************************/



/********************************************** VRRedVessels **********************************************/

static unsigned char VRRedVessels_Red[256] = {0,1,2,3,5,6,7,9,10,11,12,14,15,16,18,19,20,21,23,24,25,27,28,29,30,32,33,34,36,37,38,39,41,42,43,45,46,47,49,50,51,52,54,55,56,58,59,60,
										61,63,64,65,67,68,69,70,72,73,74,76,77,78,79,81,82,83,85,86,87,88,90,91,92,94,95,96,98,99,100,101,103,104,105,107,108,109,110,112,113,
										114,116,117,118,119,121,122,123,125,126,127,128,130,131,132,134,135,136,137,139,140,141,143,144,145,147,148,149,150,152,153,154,156,157,
										158,159,161,162,163,165,166,167,168,170,171,172,174,175,176,177,179,180,181,183,184,185,186,188,189,190,192,193,194,196,197,198,199,200,
										201,202,203,204,206,207,208,209,210,211,212,213,214,215,216,217,219,220,221,222,223,224,225,226,227,228,229,231,232,233,234,235,236,237,
										238,239,240,241,243,244,245,246,247,248,249,250,251,252,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
										255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
					
static unsigned char VRRedVessels_Green[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,5,6,8,10,11,13,15,16,18,20,21,23,25,26,28,30,32,33,35,37,38,40,42,43,45,47,48,50,52,53,55,57,59,
										60,62,64,65,67,69,70,72,74,75,77,79,80,82,84,86,87,89,91,93,95,97,99,101,103,105,107,109,111,113,115,117,119,121,124,126,128,130,132,134,
										136,138,140,142,144,146,148,150,152,154,156,158,164,170,176,182,188,194,200,206,212,218,224,230,236,242,248};
					
static unsigned char VRRedVessels_Blue[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
										0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
										37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,55,57,59,61,64,66,68,70,72,75,77,79,81,84,86,88,90,92,95,97,99,101,103,106,108,110,
										112,114,117,119,121,123,126,128,136,144,152,159,167,175,183,191,199,207,215,223,231,239,247};

/************************************************************************************************************/

#endif _colorTransferTable_h_