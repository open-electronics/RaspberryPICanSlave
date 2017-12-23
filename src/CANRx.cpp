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
#include <Slaves.h>
#include <CANRx.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string>

#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#endif // __linux__


#define CAN_MSG_LEN					8

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
				 static __int64 LastTS = 0;
                printf("RCVD CAN_frame: ID = 0x%8x DLC = %d TS = %lld\n", rxmsg.can_id, rxmsg.can_dlc, GetMillis()-LastTS);
				LastTS = GetMillis();
 #endif
                // Manage the message in Slaves repo
                ID = rxmsg.can_id;
                // Update the Slave matching this ID by the buffer, which contains mainly the relays status
                // in the first 4 bytes. Actually we are assuming the number of relays (4) is < transferred
                // bytes in the can_frame (should be) without doing further tests.
                // The f() updates the msg arrival TimeStamp using the system one.
                Slave_Update_Relays_And_TimeStamp(rxmsg.data, GetMillis(), ID, 0x00);
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
static int StartCANRxThread(void)
{
	pthread_t Thread;
	
	// Gives current hid_device* as thread cbk parameter
	return pthread_create(&Thread, NULL, CANRxThreadCbk, nullptr);
}

#ifdef __linux__	
static int iSendSocket = 0;
#endif

static bool InitCANTxMainThread(void)
{
#ifdef __linux__	
    sockaddr_can addr;
    ifreq        ifr;

    const char *ifname = "vcan0";

    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr, 0x00, sizeof(addr));

    // Open CAN_RAW socket
    if ((iSendSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
    {
       printf("Error while opening socket!!!\n");
       return false;
    } 

    // Convert the interface string "can0" into interface index
    strcpy(ifr.ifr_name, ifname);
    ioctl(iSendSocket, SIOCGIFINDEX, &ifr);
    // Prepare the address for the binding
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind the socket to the can0 interface
    if (bind(iSendSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
       perror("Error in send socket binding!!!\n");
       return false;
    }	
#endif
	
	return true;
}

int SendCANMsg(const int CANId, const byte PayLoad[], const int PayLoadSize)
{
#ifdef __linux__
	can_frame	msg;
	
	// Reset the data array
	memset(&msg, 0x00, sizeof(msg));
	
	msg.can_id = CANId;
	// Set rightmost bit to tell this is an extended CANFrame
	msg.can_id |= 0x80000000;
	memcpy(&msg.data, PayLoad, CAN_MSG_LEN);
	msg.can_dlc = CAN_MSG_LEN;
	
#ifdef DUMP
	printf("SENT CAN_frame: ID = 0x%8x DLC = %d. Socket = %d: ", msg.can_id, msg.can_dlc, iSendSocket);
	for (int i=0 ; i<msg.can_dlc; i++)
		printf("%d ", msg.data[i]);
	printf("\n");
#endif
	
	int nBytes = write(iSendSocket, &msg, PayLoadSize);
	
#ifdef DUMP
	printf("Number of written bytes: %d\n", CAN_MSG_LEN);
#endif
		
#endif
	return true;
}

int InitCANBus(void)
{
	// Start the CANRxThread
	if (!StartCANRxThread())
		return false;
	// Start and init the CANTx socket, running in the main thread
	return InitCANTxMainThread();
}











