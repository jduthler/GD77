
//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77 File stream parsers
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)							//BYTE alignment is critical

typedef	unsigned char		BYTE;	//8			// My types
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
	BYTE	Pad1 :6;						// Assumed spare as all dialog fields have been tracked down.
	BYTE	EmergencyAlarmAck : 1;			// This started to work with CPS 2.0.5
	BYTE	DataCallConfirmed : 1;			// This started to work with CPS 2.0.5

	BYTE	PrivateCallConfirmed : 1;		//LSB
	BYTE	NoIdea : 1;
	BYTE	TimeSlot2 : 1;					//	high = On timeslot 2
	BYTE	NoIdea2 : 1;
	BYTE	Privacy : 1;
	BYTE	Pad4 : 3;


	BYTE	DualCapacity : 1;
	BYTE	Pad2 : 2;						//  More analog stuff
	BYTE	PTTID : 2;						// Analog option
	BYTE	Pad3 : 1;
	BYTE	STE : 2;						// Analog option, 00 = Freq, 01 = 120, 02 = 180, 03 = 240

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
#define	CHANNEL_START	14224
#define CHANNEL_MAX		CHANNEL_START + (CHANNEL_SIZE * CHANNEL_SLOTS)
#define	CHANNEL_SIZE	56
#define CHANNEL_SLOTS	128	
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77Channel
{

	BYTE	m_szChannelName[15];					// unused positions are 0xff filled
	BYTE	cChannelNameTerminator;					// always 0xff
	BYTE	m_szRXFreq[4];
	BYTE	m_szTXFreq[4];
	BYTE	Mode;									// 1 = Dig, 0 = Analog

	BYTE	Unknown[2];								// 
	BYTE	TOT;									// 34 States
													// Infinity, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210. 225, 240, 255, 270, 285, 300, 315, 330, 345, 360, 375, 390, 405, 420, 435, 450, 465, 480, 495
	BYTE	TOTResetDelay;							// 0 to 255 Seconds
	BYTE	Admit;									// 0 = Always, 1 = Free, 2 = Color Code

	BYTE	Unknown2;								// 
	BYTE	ScanList;								// Index into the scan list, 0 = disable

	BYTE	cbRXTone[2];							// a Tone of 151.4 is stored in two bytes 14 15 
	BYTE	cbTXTone[2];							//
	
	AnalogBitsUnionDef	AnalogBitsUnion;			// See union def

	BYTE	Unknown3;								// Always 0x16
	BYTE	PrivacyGroup;							// Privacy Group - System supports 16 64 length keys

	BYTE	rxColorCode;							// 0 - 15
	BYTE	RXGroupList;							// None to 16
	BYTE	txColorCode;							// 0 - 15
	BYTE	EmergencySystem;						// 0 -254 
	
	BYTE	Contact;								// Per the manual only supports 256 contacts?
	BYTE	UnKnown5;

	FuncBitsUnionDef	FuncBitsUnion;				// see union def
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  GD77 Zone 
//
//
//
#define ZONE_START			32816
#define ZONE_MAX			ZONE_START + (ZONE_SLOTLIMIT * ZONE_SIZE)
#define ZONE_SIZE			48
#define ZONE_SLOTLIMIT		250			// Per the manual
#define	ZONE_MAXCHANNELS	16
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
#define	GROUPLIST_START			120480
#define GROUPLIST_MAX			GROUPLIST_START + (GROUPLIST_SIZE * GROUPLIST_SLOTLIMIT)							// End of file is 131071
#define GROUPLIST_SIZE			48
#define GROUPLIST_SLOTLIMIT		128			// Per v3.0.6 Release notes
#define GROUPLIST_MAXCONTACTS	15			// Not published anywhere, v2.0.5 CPS only allows 15 to be added
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct GD77GroupList
{
	BYTE	m_szGroupName[15];
	BYTE	cGroupNameTerminator;		// Always 0xff
	DBYTE	GroupList[16];				//
};
//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77 ScanList
//
//
//
//
#define	SCANLIST_START			6096
#define	SCANLIST_MAX			SCANLIST_START + (SCANLIST_SIZE * SCANLIST_SLOTLIMIT)
#define SCANLIST_SIZE			88
#define	SCANLIST_SLOTLIMIT		64			// Increased in FW v3.0.6 to 64 with 31 channels
#define	SCANLIST_CHANNELLIMIT	31
#define SCANLIST_CHANNELARRAYSIZE	32
#define SCANLIST_DESCLENGTH		15
//
struct GD77ScanBitsDef
{
	BYTE	Valid	: 4;					// 0 = Valid entry, 0xff not valid
	BYTE	ChannelMark : 1;				// 1 if Checked
	BYTE	PLType : 2;						// 00 = Non Priority, 01 = Disable, 02 = Priority, 3 Priority and Non Priority
	BYTE	TalkBack : 1;					// 1 if checked
};

union GD77ScanBitsUnionDef
{
	BYTE				Bits;
	GD77ScanBitsDef		GD77ScanBits;
};

struct GD77Scanlist
{
	BYTE	m_szListName[SCANLIST_DESCLENGTH];
	GD77ScanBitsUnionDef GD77ScanBits;	// Misc flags and other enumerated values
	DBYTE	Member[SCANLIST_CHANNELARRAYSIZE];
	DBYTE	PriorityChannel1;			// 
	DBYTE	PriorityChannel2;			//
	DBYTE	DesignatedTX;				// 0 = Last Active, 1 = Selected (First in the CPS list), the channel slot - 1 , note this can exceed the channel member list which can contain 32 total, Selected + 31 Slots.
	BYTE	HoldTime;					// 0x02 = Min 50ms, 0xff = Max 6375ms, 25ms step
	BYTE	SampleTime;					// 0x03 = Min 750ms, 0x1f = Max 7750ms, 250ms Step
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
	WORD	UnKnown1;
	WORD	UnKnown2;
	BYTE	FirmwareVersion[4];
};
#pragma pack(pop)


