//---------------------------------------------------------------------------------------------------------------------------------------------------
// GD77
//
// Desc:	This application is a test mule used to develop and validate the radioddity GD-77 file layout.
//			Due to the test nature of this app it is brute force and has very few safety nets.
//
//			Radio firmware 3.0.6 was used in the development of parsers
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "malloc.h"
#include <string.h>
#include <process.h>
#include <stdio.h>

using namespace std;

int		DumpChannels(FILE *CPSFile);
int		DumpContacts(FILE *CPSFile);
int		DumpZone(FILE *CPSFile);
int		DumpRXGroups(FILE *CPSFile);
int		DumpModelName(FILE *CPSFile);
int		DumpScanList(FILE *CPSFile);

void	Usage();

inline int BCDToString(unsigned char *sInput, char *sOutput);
inline int WordToString(unsigned char *sInput, char *sOutput);
inline int CallIDToString(unsigned char *sInput, char *sOutput);

FILE	*CPSFile = NULL;


int main(int argc,char *argv[],char *envp[])
{
	errno_t errorCode;
	char	czFileName[FILENAME_MAX];
	BYTE	CmdLineOption = 'a';			// default is dump channels if only the filename is provided
	BYTE	nNumFlag = 0;
	BYTE	FlagsRemain = 1;

	if (argc > 1)
	{
		strcpy_s(czFileName, argv[1]);
		printf("Dumping %s\r\n", czFileName);

		if ((errorCode = fopen_s(&CPSFile, czFileName, "rb")) != NULL)
		{
			printf("Unable to open file %s, errorCode %d\r\n", czFileName, errorCode);
			fclose(CPSFile);
			return 0;
		}
		else
		{
			if (!DumpModelName(CPSFile))
			{
				fclose(CPSFile);
				return 0;
			}
		}

		if (argc > 2)
		{
			nNumFlag = 1;
			FlagsRemain = argc - 2;
		}

		do
		{
			if (nNumFlag)
			{
				if (*argv[nNumFlag + 1] == '-')
					CmdLineOption = *(argv[nNumFlag + 1] + 1);
				++nNumFlag;
			}
		
			switch (CmdLineOption)
			{
			case	'c':
			case	'C':
				DumpContacts(CPSFile);
				break;
			case	'z':
			case	'Z':
				DumpZone(CPSFile);
				break;
			case	'a':
			case	'A':
				DumpChannels(CPSFile);
				break;
			case	'g':
			case	'G':
				DumpRXGroups(CPSFile);
				break;
			case	's':
			case	'S':
				DumpScanList(CPSFile);
				break;
			default:
				Usage();
				break;
			}

		} while (--FlagsRemain);
		fclose(CPSFile);
	}
	else
		Usage();

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// voide Usage()
//
// Desc: Make the user aware of the options. 
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void Usage()
{
	printf("\tUsage GD77 [filename] <options>\r\n");
	printf("\r\n");
	printf("\t[filename] The mandatory file with extensio to open  \r\n");
	printf("\tOptions  \r\n");
	printf("\t-c Contacts\r\n");
	printf("\t-z Zones\r\n");
	printf("\t-a Channels(Default)\r\n");
	printf("\t-g RX Groups\r\n");
	printf("\t-s Scanlists\r\n");
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//  int DumpChannels(FILE *CPSFile)
//
//	Pass in sn openfile and the routine will dump the channels which of all of the data types is the most complex with the most amount of bit fields.
//  The real work was done in the development of the structure and unions which allow easy access to the stream contents.
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpChannels(FILE *CPSFile)
{
	int	long	lOffset = CHANNEL_START;
	char		czReadBuffer[CHANNEL_SIZE * 2];
	char		czRXBuff[20];
	char		czTXBuff[20];
	char		szRXToneBuff[20];
	char		szTXToneBuff[20];
	char		szAdmit[20];
	size_t		ReadReturn;

	GD77Channel	*pGD77Channel;

	fseek(CPSFile, CHANNEL_START, SEEK_SET);
		
	do 
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), CHANNEL_SIZE, CPSFile))
		{
			pGD77Channel = (GD77Channel *)&czReadBuffer[0];

			if (*pGD77Channel->m_szChannelName != 0)
			{
				BCDToString(&pGD77Channel->m_szRXFreq[0], czRXBuff);
				BCDToString(&pGD77Channel->m_szTXFreq[0], czTXBuff);

				WordToString(&pGD77Channel->cbRXTone[0], szRXToneBuff);
				WordToString(&pGD77Channel->cbTXTone[0], szTXToneBuff);

				switch (pGD77Channel->Admit)
				{
				case 0:
					strcpy(szAdmit, "Always");
					break;
				case 1:
					strcpy(szAdmit, "Free");
					break;
				case 2:
					strcpy(szAdmit, "Color");
					break;
				default:
					strcpy(szAdmit, "Error");
					break;
				}

				printf("Channel Name [%15.15s]  RX:[%8.8s] TX:[%8.8s] Mode:%s RTone:[%4.4s] TTone[%4.4s] Wide:%s VOX:%s Scan:%s Lone:%s Talk:%s RX:%s\r\n\t\t\t\tTS2[%s] Admit:[%6.6s] Emerg:[#%d] Color:[%d] Contact:[%d] ScanList:[%d] RXList[%d] Privacy[%s] Privacy Group[%u] Private Call Confirmed[%s] Emergency Ack[%s]\r\n",
					pGD77Channel->m_szChannelName,
					czRXBuff,
					czTXBuff,
					pGD77Channel->Mode ? "Dig" : "Ana",
					szRXToneBuff,
					szTXToneBuff,
					pGD77Channel->FuncBitsUnion.FuncBits.Wide ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.Vox ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.AutoScan ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.LoneWorker ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.TalkAround ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.RXOnly ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.TimeSlot2 ? "Y" : "N",
					szAdmit,
					pGD77Channel->EmergencySystem,
					pGD77Channel->rxColorCode,
					pGD77Channel->Contact,
					pGD77Channel->ScanList,
					pGD77Channel->RXGroupList,
					pGD77Channel->FuncBitsUnion.FuncBits.Privacy ? "Y" : "N",
					pGD77Channel->PrivacyGroup,
					pGD77Channel->FuncBitsUnion.FuncBits.PrivateCallConfirmed ? "Y" : "N",
					pGD77Channel->FuncBitsUnion.FuncBits.EmergencyAlarmAck ? "Y" : "N");
			}
		}
		else
			lOffset = CHANNEL_MAX;

		lOffset += CHANNEL_SIZE;
	} while (lOffset < CHANNEL_MAX);
	
	return(1);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// int DumpZone(FILE *CPSFile)
//
// Dump the contents of the zone section. Each zone can contain up to 16 Channels
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpZone(FILE *CPSFile)
{
	int	long	lOffset = ZONE_START;
	char		czReadBuffer[ZONE_SIZE * 2];
	BYTE		bOffset = 0;
	size_t		ReadReturn;

	fseek(CPSFile, ZONE_START, SEEK_SET);
	GD77Zone	*pGD77Zone;

	do
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), ZONE_SIZE, CPSFile))
		{
			if (ReadReturn != NULL)
				pGD77Zone = (GD77Zone *)&czReadBuffer[0];
			else
				return(0);

			if (*pGD77Zone->m_szZoneName != 0)
			{
				bOffset = 0;
				printf("Zone  Name [%15.15s] ", pGD77Zone->m_szZoneName);
				while (bOffset < ZONE_MAXCHANNELS)
				{
					if (*(pGD77Zone->Channel + bOffset) != 0)
					{
						printf("%02d,", *(pGD77Zone->Channel + bOffset));
						++bOffset;
					}
					else
						bOffset = ZONE_MAXCHANNELS;
				}
				printf("\r\n");
			}
		}
		else
			lOffset = ZONE_MAX;

		lOffset += ZONE_SIZE;
	} while (lOffset < ZONE_MAX);

	return(1);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// int	DumpScanList(FILE *CPSFile)
//
// The scanlist contains a fair number of options affecting individual scan list behavior. And is weird in that the config file contains unused ScanList text
// entries on initialization. The lower nibble of a bit field byte is used to indicate valid entries. See struct def for details. 
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int	DumpScanList(FILE *CPSFile)
{
	int	long	lOffset = SCANLIST_START;
	char		czReadBuffer[SCANLIST_SIZE * 2];
	BYTE		bOffset = 0;
	size_t		ReadReturn;

	fseek(CPSFile, SCANLIST_START, SEEK_SET);
	GD77Scanlist *pGD77ScanList;

	do
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), SCANLIST_SIZE, CPSFile))
		{
			pGD77ScanList = (GD77Scanlist *)&czReadBuffer[0];

			if (*pGD77ScanList->m_szListName != 0xff && pGD77ScanList->GD77ScanBits.GD77ScanBits.Valid == 0)
			{
				bOffset = 0;
				printf("ScanList Name [%15.15s] PLType:%u] Hold:[%u] SampleTime[%u] Channel1[%u] Channel2[%u]\r\n",
																													pGD77ScanList->m_szListName,
																													pGD77ScanList->GD77ScanBits.GD77ScanBits.PLType,
																													pGD77ScanList->HoldTime,
																													pGD77ScanList->SampleTime,
																													pGD77ScanList->PriorityChannel1,
																													pGD77ScanList->PriorityChannel2);
				printf("\t\t\t\tTalkback[%s] Channel Mark[%s]\r\n\t\t\t\t", pGD77ScanList->GD77ScanBits.GD77ScanBits.TalkBack ? "Y" : "N",
																			pGD77ScanList->GD77ScanBits.GD77ScanBits.ChannelMark ? "Y" : "N");
				while (bOffset < SCANLIST_CHANNELLIMIT)
				{
					if (*(pGD77ScanList->Member + bOffset) != 0)
					{
						printf("%02d,", *(pGD77ScanList->Member + bOffset));
						++bOffset;
					}
					else
						bOffset = SCANLIST_CHANNELLIMIT;
				}
				printf("\r\n");
			}
			else
				lOffset = SCANLIST_MAX;
		}
		else
			lOffset = SCANLIST_MAX;

		lOffset += SCANLIST_SIZE;
	} while (lOffset < SCANLIST_MAX);

	return(1);

}//---------------------------------------------------------------------------------------------------------------------------------------------------
//  int DumpRXGroups(FILE *CPSFile)
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpRXGroups(FILE *CPSFile)
{
	int	long	lOffset = GROUPLIST_START;
	char		czReadBuffer[GROUPLIST_SIZE * 2];
	BYTE		bOffset = 0;
	size_t		ReadReturn;

	GD77GroupList *pGD77GroupList;

	fseek(CPSFile, GROUPLIST_START, SEEK_SET);

	do
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), GROUPLIST_SIZE, CPSFile))
		{
			pGD77GroupList = (GD77GroupList *)&czReadBuffer[0];

			if (*pGD77GroupList->m_szGroupName != 0xff)
			{
				bOffset = 0;
				printf("RXGroupList Name [%15.15s] ", pGD77GroupList->m_szGroupName);
				while (bOffset < GROUPLIST_MAXCONTACTS)
				{
					if (*(pGD77GroupList->GroupList + bOffset) != 0)
					{
						printf("%02d,", *(pGD77GroupList->GroupList + bOffset));
						++bOffset;
					}
					else
						bOffset = GROUPLIST_MAXCONTACTS;
				}
				printf("\r\n");
			}
		}
		else
			lOffset = GROUPLIST_MAX;

		lOffset += GROUPLIST_SIZE;
	} while (lOffset < GROUPLIST_MAX);

	return(1);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// int DumpContacts(FILE *CPSFile)
//
// Contacts are simple with only 5 elements not very efficently stored
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpContacts(FILE *CPSFile)
{
	int	long	lOffset = CONTACT_START;
	char		czReadBuffer[CONTACT_SIZE * 2];
	char		czCallIDBuff[20];

	size_t		ReadReturn;
	GD77Contact	*pGD77Contact;

	fseek(CPSFile, CONTACT_START, SEEK_SET);

	do
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), CONTACT_SIZE, CPSFile))
		{
			if (ReadReturn != NULL)
				pGD77Contact = (GD77Contact *)&czReadBuffer[0];
			if (*pGD77Contact->m_szContactName != 0xff)
			{
				CallIDToString(pGD77Contact->CallID, czCallIDBuff);

				printf("Contact[%15.15s] CallID:[%08.8s] CallType:[%d] RingStyle[%02d] Call Receive Tone[%s]\r\n",
					pGD77Contact->m_szContactName,czCallIDBuff,
					pGD77Contact->CallType,
					pGD77Contact->RingStyle,
					pGD77Contact->CallReceiveTone ? "Y" : "N");
			}
		}
		else
			lOffset = CONTACT_MAX;

		lOffset += CONTACT_SIZE;
	} while (lOffset < CONTACT_MAX);

	return(1);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// int		DumpModelName(FILE *CPSFile)
//
// I was curious if I could find the firmware rev in the file as it may be nessasary to detect if the mapping changes over the revisions 
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int		DumpModelName(FILE *CPSFile)
{
	int	long	lSize;
	char		czReadBuffer[MODELNAME_SIZE * 2];
	size_t		ReadReturn;

	fseek(CPSFile, 0, SEEK_END);
	lSize = ftell(CPSFile);
	printf("File is %d bytes\n\r", lSize);
	rewind(CPSFile);
	fseek(CPSFile, MODELNAME_START, SEEK_SET);
	GD77ModelName *pGD77ModelName;


	if (ReadReturn = fread(czReadBuffer, sizeof(char), MODELNAME_SIZE, CPSFile))
	{
		pGD77ModelName = ( GD77ModelName *)&czReadBuffer[0];
		printf("ModelName [%8.8s] Firmware:%02u.%02u.%02u \r\n",	pGD77ModelName->m_szModelName, 
															pGD77ModelName->FirmwareVersion[3], 
															pGD77ModelName->FirmwareVersion[2],
															pGD77ModelName->FirmwareVersion[1]);
				
	}
	else
	{
		printf("Unable to read Model name and firmware version -- Exiting\r\n");
		return(0);
	}

	rewind(CPSFile);			// Other functions assume file pointer at the beginning
	return(1);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------
// Misc conversion routines
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
inline int BCDToString(unsigned char *sInput, char *sOutput)
{
	sprintf(sOutput, "%x%x%x%x", *(sInput + 3), *(sInput + 2), *(sInput + 1), *(sInput));
	return(1);
}
inline int CallIDToString(unsigned char *sInput, char *sOutput)
{
	sprintf(sOutput, "%x%x%x%x", *(sInput), *(sInput + 1), *(sInput + 2),*(sInput + 3));
	return(1);
}
inline int WordToString(unsigned char *sInput, char *sOutput)
{
	if (*sInput == 0xff && *(sInput+1) == 0xff)
		sprintf(sOutput, "   ");
	else
		sprintf(sOutput, "%x%x",  *(sInput + 1), *(sInput));
	return(1);
}

