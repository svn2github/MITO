#include "wxWiiDriver.h"
#include "wxWiimoteEvent.h"
#include <stdio.h>
#include "math.h"

//report request types
const unsigned char REQUEST_SINGLE_REPORTS			= 0x0;
const unsigned char REQUEST_CONTINUOUS_REPORTS		= 0x4;

// "Output" refers to packets are sent from the host to the Wiimote
const unsigned char OUTPUT_CHANNEL_LED				= 0x11;
const unsigned char OUTPUT_CHANNEL_REPORT			= 0x12;
const unsigned char OUTPUT_CONTROLLER_STATUS		= 0x15;
const unsigned char OUTPUT_READ_MEMORY				= 0x17;
const unsigned char OUTPUT_WRITE_MEMORY				= 0x16;

const unsigned char OUTPUT_ENABLE_IR				= 0x13;
const unsigned char OUTPUT_ENABLE_IR2				= 0x1a;

// "Input" refers to packets that are sent from the Wiimote to the host
const unsigned char INPUT_CHANNEL_BUTTONS_ONLY		= 0x30;
const unsigned char INPUT_CHANNEL_BUTTONS_MOTION	= 0x31;
const unsigned char INPUT_EXPANSION_REPORT			= 0x20;
const unsigned char INPUT_READ_DATA					= 0x21;
const unsigned char INPUT_CHANNEL_WRITE_CONFIRM		= 0x22;
const unsigned char INPUT_CHANNEL_MOTION_IR			= 0x33;

//how to find the calibration data for the wiimote
const unsigned short CALIBRATION_ADDRESS			= 0x16;
const unsigned short CALIBRATION_DATA_LENGTH		= 7;

//IR constants
const unsigned long IR_REG_1 = 0x04b00030;
const unsigned long IR_REG_2 = 0x04b00033;
const unsigned long IR_SENS_ADDR_1 = 0x04b00000;
const unsigned long IR_SENS_ADDR_2 = 0x04b0001a;

const unsigned char IR_SENS_MIDRANGE_PART1[] = {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xaa, 0x00, 0x64};
const unsigned char IR_SENS_MIDRANGE_PART2[] = {0x63, 0x03};

const unsigned char IR_MODE_OFF = 0;
const unsigned char IR_MODE_STD = 1;
const unsigned char IR_MODE_EXP = 3;
const unsigned char IR_MODE_FULL = 5;


//the ID values for a wiimote
const unsigned short mVendorID = 0x057e;
const unsigned short mDeviceID = 0x0306;



void wxWiiDriver::GoWii() {

//		StartReportModeContinuousMotionIR();		

		bool A_button=false; 
		bool B_button=false;

		bool CROSS=false;

		bool MINUS=false;	bool HOME=false;	bool PLUS=false;
		bool ONE=false;		bool TWO=false;	

		float x = 0.f, y = 0.f, z = 0.f;    

		int semTimeout = 300;		
				
		SetLEDs(Led1,Led2,Led3,Led4);

//		Sleep(1000*idWii);
		VibrationCont=0;

		for (;!closingThreadLoop;)
		{			

			if (VibrationCont != 0) 
			{
				if (VibrationCont>0) VibrationCont--;
				SetVibration( (VibrationCont != 0) );				
			}

			if (changeLedsStatus) SetLEDs(Led1,Led2,Led3,Led4);

			//QueryPerformanceCounter((LARGE_INTEGER *)&timeStamp);
			//bool sendReport = ( ( (timeStamp - lastTimeStamp) * 1000 / clockFrequency ) > 1000/100 );

			//__int64 time;
			//QueryPerformanceCounter((LARGE_INTEGER *)&time);
			//if ( (((time-lastWiimoteTime)*1000/clockFrequency)>10) && (!closingThreadLoop) && (SendEvents) && GetInput() )

			if ( (!closingThreadLoop) && (SendEvents) && GetInput() )
			{

				//lastTimeStamp = timeStamp;

				GetCalibratedAcceleration(x,y,z);				
                
				wxWiimoteEvent WiiEvent = wxWiimoteEvent();   			
                
				//SYSTEMTIME st;
				//GetSystemTime(&st);
				//WiiEvent.SetTimestamp( (long)(st.wSecond*1000+st.wMilliseconds) );

				__int64 timeStamp;
				QueryPerformanceCounter((LARGE_INTEGER *)&timeStamp);
				
				WiiEvent.SetTimestamp( (long)(timeStamp*1000/clockFrequency) ); //milliseconds

				WiiEvent.A			= GetLastButtonStatus().mA;
				WiiEvent.B			= GetLastButtonStatus().mB;
                
				WiiEvent.Minus		= GetLastButtonStatus().mMinus;	
				WiiEvent.Home		= GetLastButtonStatus().mHome;
				WiiEvent.Plus		= GetLastButtonStatus().mPlus;
                
				WiiEvent.One		= GetLastButtonStatus().m1;
				WiiEvent.Two		= GetLastButtonStatus().m2;
                
				WiiEvent.LeftCross	= GetLastButtonStatus().mLeft;
				WiiEvent.RightCross	= GetLastButtonStatus().mRight;
				WiiEvent.UpCross	= GetLastButtonStatus().mUp;
				WiiEvent.DownCross	= GetLastButtonStatus().mDown;

				WiiEvent.WiiX		= x;	
				WiiEvent.WiiY		= y;	
				WiiEvent.WiiZ		= z;		

				WiiEvent.IRLight1Found = mLastIRReport.mP1Found;
				if (WiiEvent.IRLight1Found)
				{
					WiiEvent.IRLight1X = mLastIRReport.mP1X;
					WiiEvent.IRLight1Y = mLastIRReport.mP1Y;					

                    lastIRLight1X = mLastIRReport.mP1X;
					lastIRLight1Y = mLastIRReport.mP1Y;
				}
				else
					{
						WiiEvent.IRLight1X = lastIRLight1X;
						WiiEvent.IRLight1Y = lastIRLight1Y;																	
					}
				
				WiiEvent.IRLight2Found = mLastIRReport.mP2Found;
				if (WiiEvent.IRLight2Found)
				{
					WiiEvent.IRLight2X = mLastIRReport.mP2X;
					WiiEvent.IRLight2Y = mLastIRReport.mP2Y;

					lastIRLight2X = mLastIRReport.mP2X;
					lastIRLight2Y = mLastIRReport.mP2Y;
				}
				else
					{
						WiiEvent.IRLight2X = lastIRLight2X;
						WiiEvent.IRLight2Y = lastIRLight2Y;
					}
				
				WiiEvent.Led1	= Led1; 
				WiiEvent.Led2	= Led2; 
				WiiEvent.Led3	= Led3; 
				WiiEvent.Led4	= Led4;

				WiiEvent.idWii	= idWii;

				WiiEvent.SetEventObject(Handler);
			
			
				// Wiimote Button A
				if (GetLastButtonStatus().mA) 
				{
					if (  !A_button  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_A_DOWN);		
						Handler->AddPendingEvent(WiiEvent);	
						A_button=true;					
					}
				}
				else 
				{
					if (   A_button  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_A_UP);										
						Handler->AddPendingEvent(WiiEvent);
						A_button=false;						
					}		
				}
				

				// Wiimote Button B
				if (GetLastButtonStatus().mB)
				{					
					if (  !B_button  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_B_DOWN);																		
						Handler->AddPendingEvent(WiiEvent);						
						B_button=true;																
					}
				}
				else 
				{
					if (   B_button  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_B_UP);										
						Handler->AddPendingEvent(WiiEvent);						
						B_button=false;
					}
				}
				
			
				// Wiimote Button Plus
				if (GetLastButtonStatus().mPlus)    
				{
					if (  !PLUS  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_PLUS_DOWN);										
						Handler->AddPendingEvent(WiiEvent);						
						PLUS=true;
					}
				}
				else 
				{
					if (   PLUS  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_PLUS_UP);										
						Handler->AddPendingEvent(WiiEvent);
						PLUS=false;
					}
				}
				
				
				// Wiimote Button Minus
				if (GetLastButtonStatus().mMinus)    
				{
					if (  !MINUS  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_MINUS_DOWN);															
						Handler->AddPendingEvent(WiiEvent);						
						MINUS=true;
					}	
				}
				else 
				{
					if (   MINUS  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_MINUS_UP);						 						
						Handler->AddPendingEvent(WiiEvent);						
						MINUS=false;
					}
				}
				
				
				// Wiimote Button Home
				if (GetLastButtonStatus().mHome)    
				{				
					if (  !HOME  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_HOME_DOWN);									
						Handler->AddPendingEvent(WiiEvent);
						HOME=true;						
					}
				}
				else 
				{
					if (   HOME  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_HOME_UP);						 					 
						Handler->AddPendingEvent(WiiEvent);						
						HOME=false;						
					}
				}
				
	
				// Wiimote Button One
				if (GetLastButtonStatus().m1)
				{				
					if (  !ONE  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_ONE_DOWN);									
						Handler->AddPendingEvent(WiiEvent);						
						ONE=true;						
					}
				}
				else 
				{
					if (   ONE  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_ONE_UP);						 					 
						Handler->AddPendingEvent(WiiEvent);						
						ONE=false;						
					}
				}
				
					
				// Wiimote Button Two
				if (GetLastButtonStatus().m2)    
				{				
					if (  !TWO  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_TWO_DOWN);									
						Handler->AddPendingEvent(WiiEvent);						
						TWO=true;						
					}
				}
				else 
				{
					if (   TWO  &&  !closingThreadLoop  ) 
					{
						WiiEvent.SetEventType(wxEVT_WII_TWO_UP);						 					 
						Handler->AddPendingEvent(WiiEvent);						
						TWO=false;						
					}
				}
				
	
				// Wiimote Cross
				if (   ( WiiEvent.LeftCross || WiiEvent.RightCross || 
					       WiiEvent.UpCross || WiiEvent.DownCross )		&&   !closingThreadLoop   ) 
				{
					WiiEvent.SetEventType(wxEVT_WII_CROSS_DOWN);													
					Handler->AddPendingEvent(WiiEvent);					
					CROSS=true;											
				}
				else
				{
					if (  CROSS  &&  !closingThreadLoop  ) 
					{					
						WiiEvent.SetEventType(wxEVT_WII_CROSS_UP);						 					
						Handler->AddPendingEvent(WiiEvent);						
						CROSS=false;
						
					}				
				}
				
				
				// Wiimote MOTION_IR
				WiiEvent.SetEventType(wxEVT_WII_MOTION_IR);	
				if ( !closingThreadLoop )
				{
					Handler->AddPendingEvent(WiiEvent);
				}

				//QueryPerformanceCounter((LARGE_INTEGER *)&lastWiimoteTime);
			}			
		}

		
		SetLEDs(false,false,false,false);

		stillLooping = false;

		for (;!closingThread;) {}
}


wxThread::ExitCode wxWiiDriver::Entry() 
{
	bool tryAgain = true;

    for (;!closingThread;) 
    {
		if (tryAgain) 
		{
			//Visto che che come StartIndex utilizziamo l'effettivo HIDTableIndex del Wii a cui vogliamo connetterci, 
			//FoundIndex non ci serve ed utilizziamo un variabile di comodo semplicemente perchè richiesta dalla fn.
			int diComodo;		

			int timeout=50;
			do  { timeout--; }
			while ( (!ConnectToDevice(diComodo,HIDTableIndex)) && (timeout>0) );
            
			if (timeout>0) 
			{ 
				StartDataStream();
				GoWii();					
			}
			else 
			{
				char str[10];		
				wxMessageDialog aboutDialog ( NULL, wxString("Wiimote id = ") + 
													wxString(_itoa(this->idWii,str,10)) +
													wxString(" non reperibile.") 
													, "Wii Thread", wxOK | wxCANCEL);
				if (aboutDialog.ShowModal()== wxID_CANCEL) tryAgain = false;
			}
		}
	}

    return 0;
}


void wxWiiDriver::Init()
{
	mLastButtonStatus.Init();
	mLastExpansionReport.Init();
	mLastMotionReport.Init();
	mOutputControls.Init();
	mReadInfo.Init();
	mAccelCalibrationData.Init();
	mLastIRReport.Init();	
	mIRRunning = false;
	mDataStreamRunning = false;

	lastIRLight1X = lastIRLight2X = 511;
	lastIRLight1Y = lastIRLight2Y = 383;

	//lastWiimoteTime = 0;

	QueryPerformanceFrequency((LARGE_INTEGER *)&clockFrequency);
	//QueryPerformanceCounter((LARGE_INTEGER *)&lastTimeStamp);
}

bool wxWiiDriver::ConnectToDevice(int & FoundIndex, int StartIndex)
{
	Init();	
	const bool retval = HID.FindADevice(mDeviceID,mVendorID,FoundIndex,StartIndex) && 
						StartReportModeButtonPress() && 
						UpdateOutput() &&
						ReadCalibrationData();
	return retval;
}

bool wxWiiDriver::Disconnect()
{
	bool retval = false;
	
	StopDataStream();

	if (HID.IsConnected())
	{
		retval = HID.Disconnect();
	}

	return retval;
}

bool wxWiiDriver::SetVibration(bool vib_on)
{
	bool retval = true;
	if (mOutputControls.mVibration != vib_on)
	{
		mOutputControls.mVibration = vib_on;
		retval = UpdateOutput();
	}
	return retval;
}

void wxWiiDriver::ClearBuffer()
{
	// Sets the first mOutputBufferSize bytes of the block of memory pointed by mOutputBuffer to 0 value (interpreted as an unsigned char).
	memset(mOutputBuffer,0, mOutputBufferSize);	
}

bool wxWiiDriver::SetLEDs(bool led1, bool led2, bool led3, bool led4)
{
	const bool no_change =  mOutputControls.mLED1 == led1 &&
							mOutputControls.mLED2 == led2 &&
							mOutputControls.mLED3 == led3 &&
							mOutputControls.mLED4 == led4;

	if (no_change)
	{
		return true;
	}

	mOutputControls.mLED1 = led1;
	mOutputControls.mLED2 = led2;
	mOutputControls.mLED3 = led3;
	mOutputControls.mLED4 = led4;
	return UpdateOutput();
}

bool wxWiiDriver::UpdateOutput()
{
	ClearBuffer();
	mOutputBuffer[0] = OUTPUT_CHANNEL_LED;
	mOutputBuffer[1] =  (mOutputControls.mVibration ? 0x1 : 0x0) |
						(mOutputControls.mLED1 ? 0x1 : 0x0) << 4 | 
						(mOutputControls.mLED2 ? 0x1 : 0x0) << 5 | 
						(mOutputControls.mLED3 ? 0x1 : 0x0) << 6 | 
						(mOutputControls.mLED4 ? 0x1 : 0x0) << 7; 
	return HID.WriteToDevice(mOutputBuffer, mOutputBufferSize);
}

int  wxWiiDriver::getWiiBatteryLevel()
{
	ClearBuffer();
	mOutputBuffer[0] = OUTPUT_CONTROLLER_STATUS;
	if ( HID.WriteToDevice(mOutputBuffer, mOutputBufferSize) ) 
	{
		do {}
		while ( GetInput() != INPUT_EXPANSION_REPORT );
	}
	else return -1;

	int maxCharge = 200;
	return ( (mLastExpansionReport.mBatteryLevel*100) / maxCharge );
}
int  wxWiiDriver::GetInput(int timeout)
{
	int bytes_read = 0;
	int channel = 0;
	
	if (HID.ReadFromDevice(mInputBuffer, mInputBufferSize, bytes_read))
	{
		channel = mInputBuffer[0];
		switch (channel)
		{
			// 	 Payload Size 6 	Expansion Port
			case INPUT_EXPANSION_REPORT:				
							ParseButtonReport(&mInputBuffer[1]);
							ParseExpansionReport(&mInputBuffer[2]);
							break;

			//   Payload Size 2 	Buttons only
			case INPUT_CHANNEL_BUTTONS_ONLY:			
							ParseButtonReport(&mInputBuffer[1]);
							break;

			// 	 Payload Size 21 	Read data
			case INPUT_READ_DATA:									
							ParseButtonReport(&mInputBuffer[1]);
							ParseReadData(&mInputBuffer[3]);
							break;

			// 	 Payload Size 4 	Write data confirm
			case INPUT_CHANNEL_WRITE_CONFIRM:									
							break;

			// 	 Payload Size 5 	Buttons | Motion Sensing Report
			case INPUT_CHANNEL_BUTTONS_MOTION:			
							ParseButtonReport(&mInputBuffer[1]);
							ParseMotionReport(&mInputBuffer[3]);
							break;

			// 	Payload Size 17 	Buttons | Motion | IR Sensing Report
			case INPUT_CHANNEL_MOTION_IR:	
							ParseButtonReport(&mInputBuffer[1]);
							ParseMotionReport(&mInputBuffer[3]);
							ParseIRReport(&mInputBuffer[6]);
							break;							
		}
		
	}

	return channel;
}

void wxWiiDriver::ParseExpansionReport(const unsigned char *data)
{
	//four bytes long
	mLastExpansionReport.mAttachmentPluggedIn = (data[0] & 0x02) != 0;
	mLastExpansionReport.mIREnabled = (data[0] & 0x08) != 0;
	mLastExpansionReport.mSpeakerEnabled = (data[0] & 0x04) != 0;
	mLastExpansionReport.mLED1On = (data[0] & 0x10) != 0;
	mLastExpansionReport.mLED2On = (data[0] & 0x20) != 0;
	mLastExpansionReport.mLED3On = (data[0] & 0x40) != 0;
	mLastExpansionReport.mLED4On = (data[0] & 0x80) != 0;
	
	mLastExpansionReport.mBatteryLevel = data[4];
}

void wxWiiDriver::ParseButtonReport(const unsigned char * data)
{
	//two bytes long
	mLastButtonStatus.mA = (data[1] & 0x08) != 0;
 	mLastButtonStatus.mB = (data[1] & 0x04) != 0;
 	mLastButtonStatus.m1 = (data[1] & 0x02) != 0;
 	mLastButtonStatus.m2 = (data[1] & 0x01) != 0;
 	mLastButtonStatus.mPlus = (data[0] & 0x10) != 0;
 	mLastButtonStatus.mMinus = (data[1] & 0x10) != 0;
 	mLastButtonStatus.mHome = (data[1] & 0x80) != 0;
 	mLastButtonStatus.mUp = (data[0] & 0x08) != 0;
 	mLastButtonStatus.mDown = (data[0] & 0x04) != 0;
 	mLastButtonStatus.mLeft = (data[0] & 0x01) != 0;
 	mLastButtonStatus.mRight = (data[0] & 0x02) != 0;
}

void wxWiiDriver::ParseMotionReport(const unsigned char * data)
{
	//three bytes long
	mLastMotionReport.mX = data[0];
	mLastMotionReport.mY = data[1];
	mLastMotionReport.mZ = data[2];
}

bool wxWiiDriver::StartReportModeContinuousMotion()
{
	return SelectInputChannel(true,INPUT_CHANNEL_BUTTONS_MOTION);
}
bool wxWiiDriver::StartReportModeContinuousMotionIR()
{
	return SelectInputChannel(true,INPUT_CHANNEL_MOTION_IR);
}

bool wxWiiDriver::StartReportModeButtonPress()
{
	return SelectInputChannel(false,INPUT_CHANNEL_BUTTONS_ONLY);
}

bool wxWiiDriver::SelectInputChannel(bool continuous, unsigned char channel)
{
	ClearBuffer();
	mOutputBuffer[0] = OUTPUT_CHANNEL_REPORT;
	mOutputBuffer[1] = (continuous ? REQUEST_CONTINUOUS_REPORTS : REQUEST_SINGLE_REPORTS) | (mOutputControls.mVibration ? 0x1 : 0x0);
	mOutputBuffer[2] = channel;
	return HID.WriteToDevice(mOutputBuffer, mOutputBufferSize);
}



//this may or may not work to read buffers greater than 16 bytes. . . .
bool wxWiiDriver::SendAReadRequestToWiimote(unsigned int address, unsigned short size, unsigned char * buffer)
{
	bool retval = false;

	if (mReadInfo.mReadStatus != tMemReadInfo::READ_PENDING)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_READ_MEMORY;
		mOutputBuffer[1] = (((address & 0xff000000) >> 24) & 0xFE) | (mOutputControls.mVibration ? 0x1 : 0x0);
		mOutputBuffer[2] = (address & 0x00ff0000) >> 16;
		mOutputBuffer[3] = (address & 0x0000ff00) >> 8;
		mOutputBuffer[4] = (address & 0xff);
		
		mOutputBuffer[5] = (size & 0xff00) >> 8;
		mOutputBuffer[6] = (size & 0xff);
		
		if (HID.WriteToDevice(mOutputBuffer,mOutputBufferSize))
		{
			mReadInfo.mReadStatus = tMemReadInfo::READ_PENDING;
			mReadInfo.mReadBuffer = buffer;
			mReadInfo.mTotalBytesToRead = size;
			mReadInfo.mBytesRead =0;
			mReadInfo.mBaseAddress = (unsigned short)(address & 0xFFFF);
			retval = true;
		}
	}

	return retval;
}

void wxWiiDriver::ParseReadData(const unsigned char * data)
{
	if(mReadInfo.mReadStatus == tMemReadInfo::READ_PENDING)
	{
		const bool error = (data[0] & 0x0F) != 0;
		if (error)
		{
			mReadInfo.mReadStatus = tMemReadInfo::READ_ERROR;
		}
		else
		{
			unsigned char bytes = (data[0] >> 4)+1;
			unsigned short offset = ((unsigned short)data[1] << 8) + data[2];
			unsigned int space_left_in_buffer = mReadInfo.mTotalBytesToRead -  mReadInfo.mBytesRead;
			if (offset == mReadInfo.mBytesRead + mReadInfo.mBaseAddress &&
				space_left_in_buffer >= bytes)
			{
				memcpy(&mReadInfo.mReadBuffer[mReadInfo.mBytesRead],&data[3],bytes);
				
				mReadInfo.mBytesRead+= bytes;
				if (mReadInfo.mBytesRead >= mReadInfo.mTotalBytesToRead)
				{
					mReadInfo.mReadStatus = tMemReadInfo::READ_COMPLETE;
				}
			}
		}
	}

}
bool wxWiiDriver::ReadData(unsigned int address, unsigned short size, unsigned char * buffer)
{
	if (SendAReadRequestToWiimote(address, size,buffer))
	{
		while (mReadInfo.mReadStatus == tMemReadInfo::READ_PENDING)
		{
			if (!GetInput(1000))
			{
				break;
			}
		}
	}
	return mReadInfo.mReadStatus == tMemReadInfo::READ_COMPLETE;
}

bool wxWiiDriver::ReadCalibrationData()
{
	bool retval = false;
	unsigned char buffer[CALIBRATION_DATA_LENGTH];
	if (ReadData(CALIBRATION_ADDRESS, CALIBRATION_DATA_LENGTH,buffer))
	{
		mAccelCalibrationData.mXZero = buffer[0];
		mAccelCalibrationData.mYZero = buffer[1];
		mAccelCalibrationData.mZZero = buffer[2];
		mAccelCalibrationData.mXG = buffer[4];
		mAccelCalibrationData.mYG = buffer[5];
		mAccelCalibrationData.mZG = buffer[6];
		retval = true;
	}
	return retval;
}

void wxWiiDriver::GetCalibratedAcceleration(float & x, float & y, float &z)
{
 	x = (mLastMotionReport.mX - mAccelCalibrationData.mXZero) / (float)(mAccelCalibrationData.mXG- mAccelCalibrationData.mXZero);
	y = (mLastMotionReport.mY - mAccelCalibrationData.mYZero) / (float)(mAccelCalibrationData.mYG- mAccelCalibrationData.mYZero);
	z = (mLastMotionReport.mZ - mAccelCalibrationData.mZZero) / (float)(mAccelCalibrationData.mZG- mAccelCalibrationData.mZZero);
}



bool wxWiiDriver::StartDataStream()
{
	bool retval = false;
	
	StopDataStream();
	
	retval = this->StartReportModeContinuousMotionIR();	
	
	EnableIR();

	mDataStreamRunning = retval;
	return retval;
}


bool wxWiiDriver::StopDataStream()
{
	if (mDataStreamRunning)
	{
		mDataStreamRunning = false;
		DisableIR();
		this->StartReportModeButtonPress();
	}
	return true;;
}


bool wxWiiDriver::EnableIR()
{
	bool retval = false;
	
	DisableIR();

	Sleep(50);

	if (!mIRRunning)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_ENABLE_IR;
		mOutputBuffer[1] = 0x4 | (mOutputControls.mVibration ? 0x1 : 0x0);
		retval = HID.WriteToDevice(mOutputBuffer,mOutputBufferSize);
		Sleep(50);
		
		if (retval)
		{
			mOutputBuffer[0] = OUTPUT_ENABLE_IR2;
			mOutputBuffer[1] = 0x4 | (mOutputControls.mVibration ? 0x1 : 0x0);
			retval = HID.WriteToDevice(mOutputBuffer,mOutputBufferSize);
			Sleep(50);
		}

		if (retval)
		{		
			unsigned char val = 0x08;
			retval = WriteMemory(0x04B00030,1,&val);
			//retval = WriteMemory(IR_REG_1,1,&val);
			Sleep(50);
		}
		
		if (retval)
		{
			unsigned char val = 0x90;
			retval = WriteMemory(0x04B00006,1,&val);
			//retval = WriteMemory(IR_SENS_ADDR_1,9,IR_SENS_MIDRANGE_PART1);
			Sleep(50);
		}

		if (retval)
		{
			unsigned char val = 0xC0;
			retval = WriteMemory(0x04B00008,1,&val);
			//retval = WriteMemory(IR_SENS_ADDR_2,2,IR_SENS_MIDRANGE_PART2);
			Sleep(50);
		}


		if (retval)
		{
			unsigned char val = 0x40;
			retval = WriteMemory(0x04B0001A,1,&val);
			//retval = WriteMemory(IR_REG_2,1,&IR_MODE_EXP);
			Sleep(50);
		}
		
		if (retval)
		{
			unsigned char val = 0x33;			
			retval = WriteMemory(0x04B00033,1,&val);
			//retval = WriteMemory(IR_REG_1,1,&val);
			Sleep(50);
		}

		/*
		if (retval)
		{
			unsigned char val = 0x1;
			//unsigned char val = 0x8;
			retval = WriteMemory(IR_REG_1,1,&val);
		}
		
		if (retval)
		{
			retval = WriteMemory(IR_SENS_ADDR_1,9,IR_SENS_MIDRANGE_PART1);
		}

		if (retval)
		{
			retval = WriteMemory(IR_SENS_ADDR_2,2,IR_SENS_MIDRANGE_PART2);
		}


		if (retval)
		{
			retval = WriteMemory(IR_REG_2,1,&IR_MODE_EXP);
		}
		
		if (retval)
		{
			unsigned char val = 0x8;
			retval = WriteMemory(IR_REG_1,1,&val);
		}
		*/

		mIRRunning = retval;		
	}
	return retval;

}

bool wxWiiDriver::DisableIR()
{
	bool retval = false;

	if (mIRRunning)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_ENABLE_IR;
		mOutputBuffer[1] = (mOutputControls.mVibration ? 0x1 : 0x0);
		retval = HID.WriteToDevice(mOutputBuffer,mOutputBufferSize);
		
		if (retval)
		{
			mOutputBuffer[0] = OUTPUT_ENABLE_IR2;
			mOutputBuffer[1] = (mOutputControls.mVibration ? 0x1 : 0x0);
			retval = HID.WriteToDevice(mOutputBuffer,mOutputBufferSize);
		}

		mIRRunning = false;
	}
	return retval;

}



void wxWiiDriver::ParseIRReport(const unsigned char * data)
{
	/*
	mLastIRReport.mP1X = data[0] << 2 | (data[2] & 0x30) >>4;
	mLastIRReport.mP1Y = data[1] << 2 | (data[2] & 0xc0) >>6;
	mLastIRReport.mP1Size = data[2] & 0xf;

	mLastIRReport.mP2X = data[3] << 2 | (data[5] & 0x30) >>4;
	mLastIRReport.mP2Y = data[4] << 2 | (data[5] & 0xc0) >>6;
	mLastIRReport.mP2Size = data[5] & 0xf;

	mLastIRReport.mP1Found =  !(data[0] == 0xff && data[1] == 0xff && data[2] == 0xff);
	mLastIRReport.mP2Found =  !(data[3] == 0xff && data[4] == 0xff && data[5] == 0xff);
	*/

	//xxxxxxxx yyyyyyyy yyxxssss
    
	mLastIRReport.mP1X = data[0] | ((data[2]>>4)&3) << 8;//data[0] | (data[2] & 0x30) <<4;
	mLastIRReport.mP1Y = data[1] | ((data[2]>>6)&3) << 8;
	mLastIRReport.mP1Size = data[2] & 0xf;
    
	mLastIRReport.mP2X = data[3] | ((data[2]>>0)&3) << 8;
	mLastIRReport.mP2Y = data[4] | ((data[2]>>2)&3) << 8;
	mLastIRReport.mP2Size = data[5] & 0xf;
    
	mLastIRReport.mP1Found =  !(data[0] == 0xff && data[1] == 0xff && data[2] == 0xff);
	mLastIRReport.mP2Found =  !(data[3] == 0xff && data[4] == 0xff && data[5] == 0xff); 

}

bool wxWiiDriver::WriteMemory(unsigned int address, unsigned char size, const unsigned char * buffer)
{
	bool retval = false;
	if (size <= 16)
	{
		ClearBuffer();
				
		mOutputBuffer[0] = OUTPUT_WRITE_MEMORY;		// buf[0] = 0x16
		
		// buf[1] = byte ottenuto a partire dal byte più significativo di address 
		//			sporcato eventualmente si voglia richiedere la vibrazione del wiimote.
		mOutputBuffer[1] = (address & 0xff000000) >> 24 | (mOutputControls.mVibration ? 0x1 : 0x0);
		
		// buf[2] = byte ottenuto estraendo da "address" il suo secondo byte a partire da sinistra.
		mOutputBuffer[2] = (address & 0x00ff0000) >> 16;

		// buf[3] = byte ottenuto estraendo da "address" il suo terzo byte a partire da sinistra.
		mOutputBuffer[3] = (address & 0x0000ff00) >> 8;

		// buf[4] = byte ottenuto estraendo da "address" il suo byte meno significativo		.
		mOutputBuffer[4] = (address & 0xff);

		mOutputBuffer[5] = size;

		memcpy(&mOutputBuffer[6],buffer,size);
		retval = HID.WriteToDevice(mOutputBuffer,mOutputBufferSize);
	}

	return retval;
}



/*
procedure TMainForm.SetWiimoteMode(Mode:Byte);
var
  Buf: array[0..64] of Byte;
begin
  Buf[0] := $12;
  Buf[1] := $00;
  Buf[2] := Mode;

  SendToDevice(Buf, 3);
end;
*/

/*
procedure TMainForm.SendToDevice(Buf: array of Byte; WriteLength: Integer);
var
  numToWrite: Cardinal;
  numWritten: Cardinal;
  numErr: DWORD;
begin
  if not Assigned(CurrentDevice) then
  begin
    MainForm.Caption := 'Device not found';
    Exit;
  end;

  numToWrite := CurrentDevice.Caps.OutputReportByteLength;

  CurrentDevice.OpenFile;
  if not CurrentDevice.WriteFile(Buf, numToWrite, numWritten) then
  begin
    numErr := GetLastError;
    MainForm.Caption := 'Error(' + IntToStr(numErr) + '):' + SysErrorMessage(numErr);
  end
  else
  begin
    CurrentDevice.CloseFile;
  end;
  */

/*
procedure TMainForm.IRSensorStart();
var
  Buf: array[0..64] of Byte;
begin
  SetWiimoteMode(WII_MODE_ACC_IR);

  Buf[0] := $13;
  Buf[1] := $04;
  SendToDevice(Buf, 1);

  Buf[0] := $1A;
  Buf[1] := $04;
  SendToDevice(Buf, 1);

  Buf[0] := $16;
  Buf[1] := $04;
  Buf[2] := $B0;
  Buf[3] := $00;
  Buf[4] := $30;
  Buf[5] := $01;
  Buf[6] := $08;
  SendToDevice(Buf, 6);

  Buf[0] := $16;
  Buf[1] := $04;
  Buf[2] := $B0;
  Buf[3] := $00;
  Buf[4] := $06;
  Buf[5] := $01;
  Buf[6] := $90;
  SendToDevice(Buf, 6);

  Buf[0] := $16;
  Buf[1] := $04;
  Buf[2] := $B0;
  Buf[3] := $00;
  Buf[4] := $08;
  Buf[5] := $01;
  Buf[6] := $C0;
  SendToDevice(Buf, 6);

  Buf[0] := $16;
  Buf[1] := $04;
  Buf[2] := $B0;
  Buf[3] := $00;
  Buf[4] := $1A;
  Buf[5] := $01;
  Buf[6] := $40;
  SendToDevice(Buf, 6);

  Buf[0] := $16;
  Buf[1] := $04;
  Buf[2] := $B0;
  Buf[3] := $00;
  Buf[4] := $33;
  Buf[5] := $01;
  Buf[6] := $33;
  SendToDevice(Buf, 6);
end;
*/

/*
procedure TMainForm.AnalyzePacket(ReportID: Integer; ReportData: PChar; Size: Word);
begin
  case ReportID of
    WII_MODE_ACC:
      PushXYZ(ReportData);
    WII_MODE_ACC_IR:
      begin
        PushXYZ(ReportData);
        AnalyzeIR(ReportData);
      end;
    WII_MODE_ACC_IR_EXT:
      begin
        PushXYZ(ReportData);
        AnalyzeIR(ReportData);
        if Wiimote.isNunchukEnabled then AnalyzeNunchuk(ReportData);
      end;
    WII_MODE_EXPANSION:
    begin
      if(Byte(ReportData[2]) and 2 <> 0)then
      begin
        if Setting.isNunchukEnabled then
          NunchukStart
        else
          IRSensorStart;
        Wiimote.isNunchukEnabled:=true;
      end
      else
      begin
        IRSensorStart;
        Wiimote.isNunchukEnabled:=false;
      end;
      Wiimote.Battery := Integer(ReportData[5]);
    end;
  end;

  AnalyzeButtons((Cardinal(ReportData[0]) shl 8) or Cardinal(ReportData[1]));
end;

*/
