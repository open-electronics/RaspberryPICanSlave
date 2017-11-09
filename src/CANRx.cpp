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
#include <string.h>
#include <unistd.h>
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
	
    sockaddr_can addr;
    can_frame    rxmsg;
    fd_set       readset;
    ifreq        ifr;
    int          sd, ID;

    const char *ifname = "vcan0";

    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr, 0x00, sizeof(addr));

    // Open CAN_RAW socket
    if ((sd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
    {
       printf("Error while opening socket!!!\n");
       return nullptr;
    } 

    // Convert the interface string "can0" into interface index
    strcpy(ifr.ifr_name, ifname);
    ioctl(sd, SIOCGIFINDEX, &ifr);
    // Prepare the address for the binding
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind the socket to the can0 interface
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
       perror("Error in socket binding!!!\n");
       return nullptr;
    }

    // Here we are ready to enter the listening loop
    while (true)
    {
       // Use a blocking select without any timeout. This will ensure this
       // thread will be put to sleep until a valid CAN message will arrive
       FD_ZERO(&readset);
       FD_SET(sd, &readset);
       // Blocking select on the unique socket descriptor
       if (select(sd + 1, &readset, nullptr, nullptr, nullptr) >= 0)
       {
          // A message is ready: check for the right socket descriptor
          if (FD_ISSET(sd, &readset))
          {
             // Use a simple read on the descriptor
             if (read(sd, &rxmsg, sizeof(rxmsg)))
             {
				 rxmsg.can_id &= 0x7FFFFFFF;
 #ifdef DUMP
                printf("CAN_frame: ID = 0x%8x DLC = %d\n", rxmsg.can_id, rxmsg.can_dlc);
 #endif
                // Manage the message in Slaves repo
                ID = rxmsg.can_id;
                // Update the Slave matching this ID by the buffer, which contains mainly the relays status
                // in the first 4 bytes. Actually we are assuming the number of relays (4) is < transferred
                // bytes in the can_frame (should be) without doing further tests.
                // The f() updates the msg arrival TimeStamp using the system one.
                Slave_Update_Relays_And_TimeStamp(rxmsg.data, GetMillis(), ID);
             }
          }
       }
 #ifdef DUMP
       else
          printf("Socket Select error!!!");
 #endif
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
