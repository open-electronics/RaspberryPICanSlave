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
#include <string.h>
#include <map>
#include <pthread.h>
#include <Slaves.h>

// Mutex which protects SlavesMap access from multiple threads (actually the main one and the CANRx one)
static pthread_mutex_t sMutex;

using namespace std;


// Expire TimeStamp default in msec
#define Expire_TS_Default  3000


class CSlaveValue
{
private:
   int            m_CTRL_ID;             // The Control CAN message ID, i.e. the one to be used to sent relays command to the slave. Read by the CFG.
   byte           m_Relays[NUM_RELAYS];  // Payload of the CAN message: lowest 4 bits represents the 4 relays ON/OFF states
   unsigned long  m_TimeStamp;           // Last message TimeStamp arrival. When 0, the slave is unmapped (i.e. no messages have never arrived)
   int            m_ExpireTimeStamp;     // This is the threshold in msec to be added to the last status message arrival. After it, the Slave is considered "Out of date". Read by the CFG.

public:
   // Ctor
   CSlaveValue(void) : m_CTRL_ID(0), m_TimeStamp(0), m_ExpireTimeStamp(Expire_TS_Default)
   {
	   memset(m_Relays, 0x00, NUM_RELAYS);
   }

   // Setters
   void  SetCTRL_ID     (const int CTRL_ID)              { m_CTRL_ID = CTRL_ID; }
   void  SetRelays      (const byte Relays[])            { memcpy(m_Relays, Relays, NUM_RELAYS); }
   void  SetTS          (const unsigned long TimeStamp)  { m_TimeStamp = TimeStamp; }
   void  SetExpireTS(const int ExpireTimeStamp)          { m_ExpireTimeStamp = ExpireTimeStamp; }

   // Getters
   int   GetCTRL_ID     (void) const                { return m_CTRL_ID; }
   const byte*  GetRelays(void) const                { return m_Relays; }
   unsigned long GetTS   (void) const                 { return m_TimeStamp; }
   int   GetExpireTS    (void) const                { return m_ExpireTimeStamp; }
};


typedef map<int, CSlaveValue> SlavesMapType;
typedef SlavesMapType::iterator SlavesIterator;

// Slaves map repository, indexed by the CAN ID as int
static SlavesMapType SlavesMap;

// Initialization stuff
void SlavesInit(void)
{
   pthread_mutex_init(&sMutex, NULL);
}

void SlavesQuit(void)
{
   pthread_mutex_destroy(&sMutex);
}

int SlavesAreEmpty(void)
{
   pthread_mutex_lock(&sMutex);
   bool rc = SlavesMap.empty();
   pthread_mutex_unlock(&sMutex);
   return rc;
}

void Slave_AddID(const int ID)
{
   pthread_mutex_lock(&sMutex);
   // Emplace it with a defaulted slave (if not present)
   SlavesMap.emplace(ID, CSlaveValue());
  pthread_mutex_unlock(&sMutex); 
}

void Slave_Update_CTRL_ID(const int CTRL_ID, const int ID)
{
   pthread_mutex_lock(&sMutex);
   // The Slave repo is not empty so far. Let's find a given ID match
   SlavesIterator it = SlavesMap.find(ID);
   if (it != SlavesMap.end())
      it->second.SetCTRL_ID(CTRL_ID);
   else
   {
      CSlaveValue Slave;
      Slave.SetCTRL_ID(CTRL_ID);
      SlavesMap.emplace(ID, Slave);
   }
   pthread_mutex_unlock(&sMutex);
}

void Slave_Update_Relays_And_TimeStamp(const byte Relays[], const unsigned long TimeStamp, const int ID)
{
   pthread_mutex_lock(&sMutex);
   // The Slave repo is not empty so far. Let's find a given ID match
   SlavesIterator it = SlavesMap.find(ID);
   if (it != SlavesMap.end())
   {
      it->second.SetRelays(Relays);
      it->second.SetTS(TimeStamp);
   }
   else
   {
      CSlaveValue Slave;
      Slave.SetRelays(Relays);
      Slave.SetTS(TimeStamp);
      SlavesMap.emplace(ID, Slave);
   }
   pthread_mutex_unlock(&sMutex);
}

void Slave_Update_ExpireTS(const int ExpireTS, const int ID)
{
   pthread_mutex_lock(&sMutex);
   // The Slave repo is not empty so far. Let's find a given ID match
   SlavesIterator it = SlavesMap.find(ID);
   if (it != SlavesMap.end())
      it->second.SetExpireTS(ExpireTS);
   else
   {
      CSlaveValue Slave;
      Slave.SetExpireTS(ExpireTS);
      SlavesMap.emplace(ID, Slave);
   }
   pthread_mutex_unlock(&sMutex);
}

void Slave_DUMPSlavesForDebug(void)
{
   pthread_mutex_lock(&sMutex);
    if (SlavesMap.empty())
 	   printf("\nWARNING: No slaves in the cfg!!!\n");
    else
    {
 	   printf("\nnSlaves = %zu\n", SlavesMap.size());
 	   // Loop on slaves and dump them
 	   for (auto it : SlavesMap)
 		   printf("ID = 0x%8x, CTRL_ID = 0x%8x, Expire_TS = %6dms\n", it.first, it.second.GetCTRL_ID(), it.second.GetExpireTS());   	
    }	
    pthread_mutex_unlock(&sMutex);
}
