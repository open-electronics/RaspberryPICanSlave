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
#ifndef __CANRX_H__
#define __CANRX_H__

// Decoration to let C++ code be used from within plain C modules
#ifdef __cplusplus
extern "C" {
#endif
	

int InitCANBus(void);
int SendCANMsg(const int CANId, const byte PayLoad[], const int PayLoadSize);

	
// Decoration to let C++ code be used from within plain C modules
#ifdef __cplusplus
}
#endif


#endif