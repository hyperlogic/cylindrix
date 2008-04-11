#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "wiimote.h"

#include "CoreFoundation/CoreFoundation.h"

#define BLUETOOTH_VERSION_USE_CURRENT

#include "IOBluetooth/Bluetooth.h"
#include "IOBluetooth/IOBluetoothUserLib.h"


unsigned char buttonsDown[WiiButton_NumButtons];
unsigned char buttonsPressed[WiiButton_NumButtons];
unsigned char state[WiiButton_NumButtons];

enum WiiButtonFlags
{
	Wii_Two = 0x0001,
	Wii_One = 0x0002,
	Wii_B = 0x0004,
	Wii_A = 0x0008,
	Wii_Minus = 0x0010,
	Wii_Home = 0x0080,
	Wii_Left = 0x0100,
	Wii_Right = 0x0200,
	Wii_Down = 0x0400,
	Wii_Up = 0x0800,
	Wii_Plus = 0x1000
};


static void ConnectToWiiMote();
static void DisconnectFromWiiMote();

void WIIMOTE_Init()
{
	memset( buttonsDown, 0, WiiButton_NumButtons );
	memset( buttonsPressed, 0, WiiButton_NumButtons );
	memset( state, 0, WiiButton_NumButtons );
	
	ConnectToWiiMote();
}

void WIIMOTE_Cleanup()
{
	DisconnectFromWiiMote();
}

void WIIMOTE_Process()
{
	for ( int i = 0; i < WiiButton_NumButtons; ++i )
	{
		if ( state[i] )
		{
			buttonsPressed[i] = buttonsDown[i] ? 0 : 1;
			buttonsDown[i] = 1;
		}
		else
		{
			buttonsPressed[i] = 0;
			buttonsDown[i] = 0;
		}
	}
}

bool WIIMOTE_ButtonDown( WiiButton button )
{
	return buttonsDown[button];
}

bool WIIMOTE_ButtonPressed( WiiButton button )
{
	return buttonsPressed[button];
}

IOBluetoothDeviceRef g_wiiMote = 0;
IOBluetoothL2CAPChannelRef g_wiiMoteChannel = 0;

unsigned char g_lastPacket[1024];

void WiiMoteListener( IOBluetoothL2CAPChannelRef l2capChannel, void *data, UInt16 length, void *refCon )
{
	unsigned char* packet = reinterpret_cast<unsigned char*>(data);
	
	if ( packet[0] == 0xa1 )
	{
		if ( packet[1] == 0x30 )
		{
			unsigned long buttons = ((unsigned long)packet[2] << 8) | packet[3];
			unsigned long prevButtons = ((unsigned long)g_lastPacket[2] << 8) | g_lastPacket[3];
			unsigned long changed = buttons ^ prevButtons;
			
			if ( changed & Wii_Two )
			{
				printf( "Two %s\n", buttons & Wii_Two ? "Pressed" : "Released" );
				state[WiiButton_Two] = buttons & Wii_Two ? 1 : 0;
			}
			if ( changed & Wii_One )
			{
				printf( "One %s\n", buttons & Wii_One ? "Pressed" : "Released" );
				state[WiiButton_One] = buttons & Wii_One ? 1 : 0;
			}
			if ( changed & Wii_B )
			{
				printf( "B %s\n", buttons & Wii_B ? "Pressed" : "Released" );
				state[WiiButton_B] = buttons & Wii_B ? 1 : 0;
			}
			if ( changed & Wii_A )
			{
				printf( "A %s\n", buttons & Wii_A ? "Pressed" : "Released" );
				state[WiiButton_A] = buttons & Wii_A ? 1 : 0;
			}
			if ( changed & Wii_Minus )
			{
				printf( "Minus %s\n", buttons & Wii_Minus ? "Pressed" : "Released" );
				state[WiiButton_Minus] = buttons & Wii_Minus ? 1 : 0;
			}
			if ( changed & Wii_Home )
			{
				printf( "Home %s\n", buttons & Wii_Home ? "Pressed" : "Released" );
				state[WiiButton_Home] = buttons & Wii_Home ? 1 : 0;
			}
			if ( changed & Wii_Left )
			{
				printf( "Left %s\n", buttons & Wii_Left ? "Pressed" : "Released" );
				state[WiiButton_Left] = buttons & Wii_Left ? 1 : 0;
			}
			if ( changed & Wii_Right )
			{
				printf( "Right %s\n", buttons & Wii_Right ? "Pressed" : "Released" );
				state[WiiButton_Right] = buttons & Wii_Right ? 1 : 0;
			}
			if ( changed & Wii_Down )
			{
				printf( "Down %s\n", buttons & Wii_Down ? "Pressed" : "Released" );
				state[WiiButton_Down] = buttons & Wii_Down ? 1 : 0;
			}
			if ( changed & Wii_Up )
			{
				printf( "Up %s\n", buttons & Wii_Up ? "Pressed" : "Released" );
				state[WiiButton_Up] = buttons & Wii_Up ? 1 : 0;
			}
			if ( changed & Wii_Plus )
			{
				printf( "Plus %s\n", buttons & Wii_Plus ? "Pressed" : "Released" );
				state[WiiButton_Plus] = buttons & Wii_Plus ? 1 : 0;
			}
			memcpy( g_lastPacket, packet, length );
		}
	}	
}

static void DeviceFound( void* userRefCon, IOBluetoothDeviceInquiryRef ref, IOBluetoothDeviceRef deviceRef )
{
	CFStringRef name = IOBluetoothDeviceGetName( deviceRef );
	const char* str = CFStringGetCStringPtr( name, 0 );
	printf( "%s\n", str );
	if ( strcmp( str, "Nintendo RVL-CNT-01" ) == 0 )
	{
		printf( "Wii Mote Found\n" );
	
		// Open a l2cap HDI channel
		g_wiiMoteChannel = 0;
		IOBluetoothDeviceOpenL2CAPChannel( deviceRef, kBluetoothL2CAPPSMHIDInterrupt, FALSE, &g_wiiMoteChannel );
		IOBluetoothL2CAPChannelRegisterIncomingDataListener( g_wiiMoteChannel, WiiMoteListener, 0 );
		g_wiiMote = deviceRef;
	}
}

static void ConnectToWiiMote()
{
	IOBluetoothDeviceInquiryRef inquiryRef = 0;
	inquiryRef = IOBluetoothDeviceInquiryCreateWithCallbackRefCon( 0 );
	IOBluetoothDeviceInquirySetDeviceFoundCallback( inquiryRef, DeviceFound );
	IOBluetoothDeviceInquiryStart( inquiryRef );

	printf( "Searching for devices...\n" );
}

static void DisconnectFromWiiMote()
{
	IOBluetoothDeviceCloseConnection( g_wiiMote );
	IOBluetoothL2CAPChannelCloseChannel( g_wiiMoteChannel );
	printf( "disconnecting from wii-mote\n" );
}

