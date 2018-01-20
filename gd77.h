
//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------


#pragma pack(push, 1)

typedef	unsigned char		BYTE;	//8
typedef unsigned short		DBYTE;	//16
typedef unsigned int		WORD;	//32
typedef unsigned long long	DWORD;	//64


//---------------------------------------------------------------------------------------------------------------------------------------------------
//  GD77 Channel functional bits mapping
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct AnalogBitsDef
{
	BYTE	Pad1;
	BYTE	TxDTMF : 1;
	BYTE	Pad2 : 7;
	BYTE	Pad3;
	BYTE	RxDTMF : 1;
	BYTE	Pad4 : 7;
};

union	AnalogBitsUnionDef					// Struct of 64bits used to pack analog tone and signalling detail
{
	WORD Bits;
	AnalogBitsDef	AnalogBits;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  GD77 Channel functional bits mapping
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct FuncBitsDef
{
	BYTE	Pad1;

	BYTE	PrivateCallConfirmed : 1;		//LSB
	BYTE	NoIdea : 1;
	BYTE	TimeSlot2 : 1;					//	high = On timeslot 2
	BYTE	NoIdea2 : 1;
	BYTE	Pad4 : 4;


	BYTE	DualCapacity : 1;
	BYTE	Pad2 : 2;						//  More analog stuff
	BYTE	PTTID : 2;
	BYTE	Pad3 : 1;
	BYTE	STE : 2;						// 00 = Freq, 01 = 120, 02 = 180, 03 = 240

	BYTE	NormalSquelch : 1;				//LSB, high for Normal, 0 for tight
	BYTE	Wide : 1;						// high 25 khz, else 12.5
	BYTE	RXOnly : 1;						// High = RXOnly
	BYTE	TalkAround : 1;					// High = Talk Around
	BYTE	LoneWorker : 1;					// High = Lone Worker
	BYTE	AutoScan : 1;					// High = AutoScan
	BYTE	Vox : 1;						// High = VOX Enabled
	BYTE	PowerHigh : 1;					// High = Power High

};

union FuncBitsUnionDef
{
	WORD	Bits;
	FuncBitsDef	FuncBits;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77 Channels - - Offset 14224
//
//
//
//
//
//
//
//
//
#define	CHANNEL_START	14224
#define CHANNEL_MAX		16000
#define	CHANNEL_SIZE	56
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77Channel
{

	BYTE	m_szChannelName[15];
	BYTE	cChannelNameTerminator;
	BYTE	m_szRXFreq[4];
	BYTE	m_szTXFreq[4];
	BYTE	Mode;									// 1 = Dig, 0 = Analog

	BYTE	Unknown[2];
	BYTE	TOT;									// 34 States
													// Infinity, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210. 225, 240, 255, 270, 285, 300, 315, 330, 345, 360, 375, 390, 405, 420, 435, 450, 465, 480, 495
	BYTE	TOTResetDelay;							// 0 to 255 Seconds
	BYTE	Admit;									// 0 = Always, 1 = Free, 2 = Color Code

	BYTE	Unknown2;								// 
	BYTE	ScanList;

	BYTE	cbRXTone[2];							// a Tone of 151.4 is stored in two bytes 14 15 
	BYTE	cbTXTone[2];							//
	
	AnalogBitsUnionDef	AnalogBitsUnion;

	BYTE	Unknown3;								// Always 0x16
	BYTE	Unknown4;								// Always 0x01

	BYTE	rxColorCode;							// 0 - 15
	BYTE	RXGroupList;							// None to 16
	BYTE	txColorCode;							// 0 - 15
	BYTE	EmergencySystem;						// 0 -254 
	
	BYTE	Contact;								// Per the manual only supports 256 contacts?
	BYTE	UnKnown5;

	FuncBitsUnionDef	FuncBitsUnion;


};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  GD77 Zone 
//
//
//
#define ZONE_START		32816
#define ZONE_MAX		ZONE_START + (ZONE_SLOTLIMIT * ZONE_SIZE)
#define ZONE_SIZE		48
#define ZONE_SLOTLIMIT	250			// Per the manual
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77Zone
{

	BYTE	m_szZoneName[15];			// End of valid zone is marked by a leading NULL
	BYTE	cZoneNameTerminator;		// Always 0xff
	DBYTE	Channel[16];				// A zone at max can contain 16 channels, 1 based, 0 = empty
};



//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77Contact
//
//
//
//
#define CONTACT_START	95776
#define CONTACT_MAX		CONTACT_START + (CONTACT_SLOTLIMIT * CONTACT_SIZE)
#define	CONTACT_SIZE	24
#define CONTACT_SLOTLIMIT	1000		// Per v3.0.06 Release notes support for 1000, previous versions supported 256, file layout the same.
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77Contact
{
	BYTE	m_szContactName[16];
	BYTE	CallID[4];					// Four bytes 
	BYTE	CallType;					// 0 = Group, 1 = Private, 2 = All Call
	BYTE	CallReceiveTone;			// 01 = Yes
	BYTE	RingStyle;					// 0(None) - 10 CPS options
	BYTE	Terminator;					// 0xff
	
};
//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77 GroupList
// Per firmware v 3.0.6 support for 128
// This is the last struture identified in the data file. There is space after the end of this area yet to be identified.
// Size is 48 bytes
//
#define	GROUPLIST_START	120480
#define GROUPLIST_MAX	GROUPLIST_START + (GROUPLIST_SIZE * GROUPLIST_SLOTLIMIT)							// End of file is 131071
#define GROUPLIST_SIZE	48
#define GROUPLIST_SLOTLIMIT	128			// Per v3.0.6 Release notes

//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77GroupList
{
	BYTE	m_szGroupName[15];
	BYTE	cGroupNameTerminator;		// Always 0xff
	DBYTE	GroupList[16];				//
};
//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77ModelName
//
//
//
#define		MODELNAME_START	224
#define		MODELNAME_SIZE	20
#define		MODELNAME_NAMELEN	8
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77ModelName
{
	BYTE	m_szModelName[MODELNAME_NAMELEN];			// String 0xff terminated "GD-77"
	DWORD	UnKnown1;
	DWORD	UnKnown2;
	DWORD	FirmwareVersion;
};
#pragma pack(pop)


