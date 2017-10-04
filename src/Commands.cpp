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
#include <pthread.h>
#include <Commands.h>
#include <Slaves.h>

// Mutex which protects the commands 
static pthread_mutex_t sCmdMutex;


void CommandInit(void)
{
   pthread_mutex_init(&sCmdMutex, NULL);
}


void CommandQuit(void)
{
   pthread_mutex_destroy(&sCmdMutex);
}


void CommandDispatcher(const char **ppXMLSnapShot, const char Cmd[])
{
   // These mutex could be avoided, since we are using the microwebserver
   // not in multithreaded mode, so each connection is served sequentially by a queue
   // but we leave it here to support the webserver in multithreaded mode
   pthread_mutex_lock(&sCmdMutex);

   // Switch among commands:
   //
   // TODO
   //

   // Always returns a Slaves snapshot
   GetSlavesXMLSnapShot(ppXMLSnapShot);

   pthread_mutex_unlock(&sCmdMutex);
}