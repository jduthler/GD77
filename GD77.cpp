// GD77.cpp : Defines the entry point for the console application.
//



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

void	Usage();

inline int BCDToString(unsigned char *sInput, char *sOutput);
inline int WordToString(unsigned char *sInput, char *sOutput);
inline int CallIDToString(unsigned char *sInput, char *sOutput);


void Bcd_To_Hex(unsigned char *Input_Buff, unsigned char *Output_Buff, unsigned char Len);



FILE	*CPSFile = NULL;


int main(int argc,char *argv[],char *envp[])
{
	errno_t errorCode;
	char	czFileName[FILENAME_MAX];
	BYTE	CmdLineOption = 'a';			// default is channels
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

			default:
				Usage();
				break;
			}

		} while (--FlagsRemain);

		fclose(CPSFile);
		
	}
	else
	{
		Usage();
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void Usage()
{
	printf("Usage ......\r\n");


}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpChannels(FILE *CPSFile)
{
	int	long	lSize;
	int	long	lOffset = CHANNEL_START;
	char		czReadBuffer[CHANNEL_SIZE * 2];
	char czRXBuff[20];
	char czTXBuff[20];
	char szRXToneBuff[20];
	char szTXToneBuff[20];
	char szAdmit[20];
	GD77Channel	*pGD77Channel;



	fseek(CPSFile,0, SEEK_END);
	lSize = ftell(CPSFile);
	printf("File is %d bytes\n\r", lSize);
	rewind(CPSFile);
	fseek(CPSFile, CHANNEL_START, SEEK_SET);
		
	do 
	{
		fread(czReadBuffer, sizeof(char), CHANNEL_SIZE, CPSFile);

		pGD77Channel = (GD77Channel *)&czReadBuffer[0];
		
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
		
		printf("Channel Name [%15.15s] RX:[%8.8s] TX:[%8.8s] Mode:%s RTone:[%4.4s] TTone[%4.4s] Wide:%s VOX:%s Scan:%s Lone:%s Talk:%s RX:%s\r\n       TS2[%s] Admit:[%6.6s] Emerg:[#%d] Color:[%d] Contact:[%d] ScanList:[%d] RXList[%d]\r\n", 
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
			pGD77Channel->RXGroupList);
		

		lOffset += CHANNEL_SIZE;
		
		
	} while (lOffset < CHANNEL_MAX);
	
	return(1);
	
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpZone(FILE *CPSFile)
{
	int	long	lSize;
	int	long	lOffset = ZONE_START;
	char		czReadBuffer[ZONE_SIZE * 2];
	BYTE		bOffset = 0;
	size_t		ReadReturn;

	fseek(CPSFile, 0, SEEK_END);
	lSize = ftell(CPSFile);
	printf("File is %d bytes\n\r", lSize);
	rewind(CPSFile);
	fseek(CPSFile, ZONE_START, SEEK_SET);
	GD77Zone	*pGD77Zone;


	do
	{
		if (ReadReturn = fread(czReadBuffer, sizeof(char), ZONE_SIZE, CPSFile))
		{
			if (ReadReturn != NULL)
				pGD77Zone = (GD77Zone *)&czReadBuffer[0];
			if (*pGD77Zone->m_szZoneName != 0)
			{
				bOffset = 0;
				printf("Zone  Name [%15.15s] ", pGD77Zone->m_szZoneName);
				while (bOffset < 16)
				{
					if (*(pGD77Zone->Channel + bOffset) != 0)
					{
						printf("%02d,", *(pGD77Zone->Channel + bOffset));
						++bOffset;
					}
					else
						bOffset = 16;
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
//
//
//
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
int DumpContacts(FILE *CPSFile)
{
	int	long	lSize;
	int	long	lOffset = CONTACT_START;
	char		czReadBuffer[CONTACT_SIZE * 2];
	char		czCallIDBuff[20];

	size_t		ReadReturn;
	GD77Contact	*pGD77Contact;


	fseek(CPSFile, 0, SEEK_END);
	lSize = ftell(CPSFile);
	printf("File is %d bytes\n\r", lSize);
	rewind(CPSFile);
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
//
//
//
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
	GD77ModelName *pGDModelName;


	if (ReadReturn = fread(czReadBuffer, sizeof(char), MODELNAME_SIZE, CPSFile))
	{
		if (ReadReturn != NULL)
		{
			pGDModelName = (*pGDModelName)&czReadBuffer[0];
			printf("ModelName [%8.8s] Firmware:%d \r\n", pGDModelName->m_szModelName, pGDModelName->FirmwareVersion);
			return(1);
		}
		else
		{
			printf("Unable to read Model name and firmware version\r\n");
			return(0);
		}
		
	}
}



	
	

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





// pass input 8 bit BCD format and conversion length, and get back hex values
// in output buffer.
void Bcd_To_Hex(unsigned char *Input_Buff, unsigned char *Output_Buff, unsigned char Len)
{ // by Neeraj Verma : neeraj.niet@gmail.com
	unsigned char i, j;
	for (i = 0, j = 0; i<Len; i++, j++)
	{
		if ((i + 1) >= Len)
		{
			if (*(Input_Buff + Len - 1 - i) >0x40)
				*(Output_Buff + j) = (*(Input_Buff + Len - 1 - i) - 0x37);
			else
				*(Output_Buff + j) = (*(Input_Buff + Len - 1 - i) - 0x30);
		}
		else
		{
			if (*(Input_Buff + Len - 1 - i) >0x40)
			{
				if (*(Input_Buff + Len - 2 - i) >0x40)
				{
					*(Output_Buff + j) = ((*(Input_Buff + Len - i - 2) - 0x37) << 4) | (*(Input_Buff + Len - 1 - i) - 0x37);
				}
				else
				{
					*(Output_Buff + j) = ((*(Input_Buff + Len - i - 2) - 0x30) << 4) | (*(Input_Buff + Len - 1 - i) - 0x37);
				}
			}
			else
			{
				if (*(Input_Buff + Len - 2 - i) >0x40)
				{
					*(Output_Buff + j) = ((*(Input_Buff + Len - i - 2) - 0x37) << 4) | (*(Input_Buff + Len - 1 - i) - 0x30);
				}
				else
				{
					*(Output_Buff + j) = ((*(Input_Buff + Len - i - 2) - 0x30) << 4) | (*(Input_Buff + Len - 1 - i) - 0x30);
				}
			}
		}
		i++;
	}
}