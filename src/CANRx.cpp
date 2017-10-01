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
#include <stdio.h>
#include <pthread.h>
#include <CANRx.h>
#include <Slaves.h>


// CANRx thread callback
static void* CANRxThreadCbk(void *pPtr)
{
#ifdef DUMP
	printf("CANRx listener thread successfully started!!!\n");
#endif
	
	
	
	return nullptr;
}

// Start a thread which listen to CAN Rx frames. All rx reads are blocking
// in order to keep thread in sleep mode until a CAN message is reiceived.
int StartCANRxThread(void)
{
	pthread_t Thread;
	
	// Gives current hid_device* as thread cbk parameter
	return pthread_create(&Thread, NULL, CANRxThreadCbk, nullptr);
}