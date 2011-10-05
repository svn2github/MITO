#include "HIDdevice.h"

extern "C" 
{
	#include "hidsdi.h"
	#include "Setupapi.h"
}
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")


HIDdevice::HIDdevice() : mConnected(false), mHandle(NULL), mEvent(NULL)
{
	
}


HIDdevice::~HIDdevice()
{
	if (mConnected)
	{
		Disconnect();
	}
}

bool HIDdevice::Disconnect()
{
	bool retval = false;
	if (mConnected)
	{
		retval = (CloseHandle(mHandle) == TRUE && CloseHandle(mEvent) == TRUE);
	
		mConnected = false;
	}

	return retval;
}


// Ricerca un Wiimote a partire dalla riga StartIndex della tabella degli HidDevice collegati 
// di windows e restituisce l'index di riga in cui lo trova. -1 se non ne trova nessuno.
bool HIDdevice::FindADevice(unsigned short device_id, unsigned short vendor_id, int & FoundIndex, int StartIndex)
{
	if (mConnected)
	{
		if (!Disconnect())
		{
			return false;
		}
	}


	bool retval = false;
	
	//We have to detect and to locate the device, so we have to get the connected device list 

	GUID guid;			// a GUID specifies the class of device. We will set guid to look for HID class
	
	// Use HidD_GetHidGuid to obtain the GUID that Windows uses to represent the HID class.
	HidD_GetHidGuid (&guid);	

	// To get the device list, call SetupDiGetClassDevs, one of the USB API calls in setupapi.dll.
	// The function reserves a block of memory (an InfoSet) that holds an array with one entry per device.
	HDEVINFO devinfo = SetupDiGetClassDevs(&guid,NULL,NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);


	SP_DEVICE_INTERFACE_DATA dev_int_data;			// The SP_DEVICE_INTERFACE_DATA structure defines a device interface.
	dev_int_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);	// Size of the structure.The caller must set this member to sizeof(SP_DEVICE_INTERFACE_DATA).


	// The entries in the InfoSet "devinfo" have to be enumerated with calls to SetupDiEnumDeviceInterfaces in ‘setupapi.dll’. 
	// This function fills a SP_DeviceInterfaceData structure with interface data about the device in the position "index" of the list.
	int index = StartIndex;
	while ( SetupDiEnumDeviceInterfaces(devinfo, NULL, &guid, index, &dev_int_data) )
	{
		DWORD size;

		// We have to call SetupDiGetDeviceInterfaceDetail to fill a PSP_DEVICE_INTERFACE_DETAIL_DATA structure with further details about a device.

		// To get the required size that a device uses to represent its interface detail data, we have to 
		// call SetupDiGetDeviceInterfaceDetail with a NULL DeviceInterfaceDetailData pointer, an DeviceInterfaceDetailDataSize of zero, 
		// and a valid RequiredSize variable. In response to such a call, this function returns the required buffer size at RequiredSize variable.
		SetupDiGetDeviceInterfaceDetail (devinfo, &dev_int_data, NULL, 0, &size,0);	// "size" will contain the required buffer size.
		
		// Now, we can allocate an appropriately sized buffer.
		// The SP_DEVICE_INTERFACE_DETAIL_DATA structure provides detailed information about a particular device interface.
		// PSP_INTERFACE_DEVICE_DETAIL_DATA defines a pointer to SP_INTERFACE_DEVICE_DETAIL_DATA
		PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(size);
		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA); // The caller must set this member to sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA).
		
		// Recalling the function with the above buffer to get further device details. 
		if (SetupDiGetDeviceInterfaceDetail(devinfo, &dev_int_data, detail, size, NULL, NULL))
		{
			// The interface details returned by previous function include a Pointer to a NULL-terminated string that specifies the device path.
			mHandle = CreateFile (detail->DevicePath, 
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING, 
								FILE_FLAG_OVERLAPPED, 
								NULL);

			
			HIDD_ATTRIBUTES attrib;

			// OpenDevice(i) ricerca l'i-simo dispositivo HID connesso, se OpenDevice(i) restituisce true
			// mEvent, mHandle, mOverlapped saranno collegati al disposito i-esimo trovato.
			if ( HidD_GetAttributes (mHandle, &attrib) ) 
			{
                if ( (attrib.ProductID == device_id) &&		//Controlla se il dispositivo HID trovato è un WiiMote
						(attrib.VendorID == vendor_id) )
				{

					mConnected = true;

					// Creates or opens a named or unnamed event object. In this case the initial state of the event object is signaled.
					// If the function succeeds, the return value is a handle to the event object. 
					// If the named event object existed before the function call, the function returns a handle to the existing object.
					mEvent = CreateEvent(NULL, TRUE, TRUE, "");	
			
					mOverlapped.Offset = 0;	
					mOverlapped.OffsetHigh = 0;
					mOverlapped.hEvent = mEvent;	//Handle to an event that will be set to the signaled state when the operation has been completed.
					
					retval = true;
					break;
				}
			}

			free(detail);
		}

		index++;
	}

	SetupDiDestroyDeviceInfoList (devinfo);
	
	if (retval) { FoundIndex=index; }
	else FoundIndex = -1;

	return retval;
}



bool HIDdevice::WriteToDevice(unsigned const char * buffer, int num_bytes)
{
	bool retval = false;
	if (mConnected)
	{
		DWORD bytes_written;
		retval = (WriteFile( mHandle, buffer,num_bytes,&bytes_written,&mOverlapped) == TRUE); 
		retval = retval && bytes_written == num_bytes;
	}
	return retval;

}

bool HIDdevice::ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout)
{
	bool retval = false;
	if (mConnected)
	{
		ReadFile( mHandle, (LPVOID)buffer,max_bytes,(LPDWORD)&bytes_read,&mOverlapped); 
		DWORD Result = WaitForSingleObject(mEvent, timeout);
		if (Result == WAIT_OBJECT_0) 
		{		
			retval = true;
		}
		else 
		{
			CancelIo(mHandle);			
		}
		ResetEvent(mEvent);
	}
	return retval;
}

