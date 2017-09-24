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
#include <vector>
#include <Slaves.h>

using namespace std;


// Expire TimeStamp default in msec
#define Expire_TS_Default  3000


class CSlave
{
private:
   int      m_ID;                 // The State CAN message ID, served by the slave periodically. Read by the CFG (section) or inited as unmapped slave, eventually.
   int      m_CTRL_ID;            // The Control CAN message ID, i.e. the one to be used to sent relays command to the slave. Read by the CFG.
   int      m_Relays;             // Payload of the CAN message: lowest 4 bits represents the 4 relays ON/OFF states
   int      m_TimeStamp;          // Last message TimeStamp arrival
   int      m_ExpireTimeStamp;    // This is the threshold in msec to be added to the last status message arrival. After it, the Slave is considered "Out of date". Read by the CFG.

public:
   // Ctor
   CSlave (void) : m_ID(0), m_CTRL_ID(0), m_Relays(0), m_TimeStamp(0), m_ExpireTimeStamp(Expire_TS_Default) {}

   // Setters
   void  SetID          (const int ID)              { m_ID = ID; };
   void  SetCTRL_ID     (const int CTRL_ID)         { m_CTRL_ID = CTRL_ID; }
   void  SetRelays      (const int Relays)          { m_Relays = Relays; }
   void  SetTS          (const int TimeStamp)       { m_TimeStamp = TimeStamp; }
   void  SetExpireTS(const int ExpireTimeStamp)     { m_ExpireTimeStamp = ExpireTimeStamp; }

   // Getters
   int   GetID          (void) const                { return m_ID; }
   int   GetCTRL_ID     (void) const                { return m_CTRL_ID; }
   int   GetRelays      (void) const                { return m_Relays; }
   int   GetTTS         (void) const                { return m_TimeStamp; }
   int   GetExpireTS    (void) const                { return m_ExpireTimeStamp; }
};

// This represents the current CAN Slave repository (TODO: 2B moved to a separate cpp)
static vector<CSlave> sSlaves;


bool SlavesAreEmpty(void)
{
	return sSlaves.empty();
}

void Slave_AddID(const int ID)
{
    // Append a new empty Slave entry and modify it below from its back reference.
    // This approach saves memory and speed, since avoids to allocate a dedicated element
    // on the stack, doing a useless copy operator onto it.
    sSlaves.emplace_back();
    CSlave& Slave = sSlaves.back();
    Slave.SetID(ID);
}

void Slave_Add_CTRL_ID_ToLastID(const int CTRL_ID)
{
	// Nothing to do if there are no slaves
	if(sSlaves.empty())
		return;
    // Get the current Slave as the last one
    CSlave& Slave = sSlaves.back();
	Slave.SetCTRL_ID(CTRL_ID);
}

void Slave_Add_ExpireTS_ToLastID(const int ExpireTS)
{
	// Nothing to do if there are no slaves
	if(sSlaves.empty())
		return;
    // Get the current Slave as the last one
    CSlave& Slave = sSlaves.back();
	Slave.SetExpireTS(ExpireTS);
}

void Slave_DUMPSlavesForDebug(void)
{
    if (sSlaves.empty())
 	   printf("\nWARNING: No slaves in the cfg!!!\n");
    else
    {
 	   printf("\nnSlaves = %lu\n", sSlaves.size());
 	   // Loop on slaves and dump them
 	   for (auto it : sSlaves)
 		   printf("ID = 0x%8x, CTRL_ID = 0x%8x, Expire_TS = %6dms\n", it.GetID(), it.GetCTRL_ID(), it.GetExpireTS());   	
    }	
}









