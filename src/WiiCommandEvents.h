/*
	Wii Command Events definition

*/

#ifndef _WiiCommandEvents_h_
#define _WiiCommandEvents_h_

//vtkWiimoteEvents
//#include <vtkCommand.h>

enum vtkWiimoteEvents
{
    vtkWiiMotion = /*vtkCommand::UserEvent*/  3742,    
	vtkWiiADown,
	vtkWiiAUp,
	vtkWiiBDown,
	vtkWiiBUp,
	vtkWiiMinusDown,
	vtkWiiMinusUp,
	vtkWiiHomeDown,
	vtkWiiHomeUp,
	vtkWiiPlusDown,
	vtkWiiPlusUp,
	vtkWiiOneDown,
	vtkWiiOneUp,
	vtkWiiTwoDown,
	vtkWiiTwoUp,
	vtkWiiCrossDown,
	vtkWiiCrossUp
};

#endif // _WiiCommandEvents_h_