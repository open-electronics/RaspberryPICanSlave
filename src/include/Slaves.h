/*
     This file is part of RaspberryPICANSlave
     (C) Riccardo Ventrella 2017
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 3.0 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef __SLAVES_H__
#define __SLAVES_H__

// Uncomment this to DUMP debug stuff on stdout
//#define DUMP
// Verbose deeper DUMP
//#define VERBOSEDUMP
#ifdef VERBOSEDUMP
	#define DUMP
#endif	// VERBOSEDUMP




#define NUM_RELAYS         4

typedef unsigned char   byte;

#ifdef __linux__
	typedef 	long long		__int64;
#else
	typedef 	long			__int64;
#endif
		

// Decoration to let C++ code be used from within plain C modules
#ifdef __cplusplus
extern "C" {
#endif
	
__int64 GetMillis					(void);
void SlavesInit						(void);
void SlavesQuit						(void);
int	 SlavesAreEmpty					(void);
void Slave_AddID					(const int ID);
void Slave_Update_CTRL_ID			(const int CTRL_ID, const int ID);
void Slave_Update_Relays_And_TimeStamp(const byte Relays[], const __int64 TimeStamp, const int ID, const byte bAdd);
void Slave_Update_ExpireTS			(const __int64 ExpireTS, const int ID);
int  SlaveGetFirstID				(void);
int  SlaveUpdateRelay				(int *pCANCTRLId, byte Relays[], const int Id, const int Relay, const int Cmd);
void Slave_DUMPSlavesForDebug		(void);
void GetSlavesXMLSnapShot        	(const char **ppXMLSnapShot);


// Decoration to let C++ code be used from within plain C modules
#ifdef __cplusplus
}
#endif


#endif
