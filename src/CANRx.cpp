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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string>

#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#endif // __linux__


// CANRx thread callback
static void* CANRxThreadCbk(void *pPtr)
{
#ifdef __linux__

#ifdef DUMP
	printf("CANRx listener thread successfully started!!!\n");
#endif
	
	struct ifreq ifr;
	struct sockaddr_can addr;
	struct can_frame frame; 
	int s;
	
	memset(&ifr, 0x0, sizeof(ifr)); 
	memset(&addr, 0x0, sizeof(addr)); 
	memset(&frame, 0x0, sizeof(frame));
	
	/* open CAN_RAW socket */
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	/* convert interface sting "can0" into interface index */ 
	strcpy(ifr.ifr_name, "can0");
	ioctl(s, SIOCGIFINDEX, &ifr);
	/* setup address for bind */
	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex; 
	/* bind socket to the can0 interface */
	bind(s, (struct sockaddr *)&addr, sizeof(addr));
	
	// Start a loop to listen to incoming CAN messages
	// Loop forever until an error occured.
	while (true)
	{
		
		
		
	}
	
#else
	printf("\nSocketCAN is supported on Linux only!!!\n\n");
#endif	// __linux__

		
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