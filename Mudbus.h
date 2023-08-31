/*
    Mudbus.h - an Arduino library for a Modbus TCP slave.
    Copyright (C) 2011  Dee Wykoff

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
		
		2021 Nestor Levi Palomeque
*/

//#define MbDebug

// For Arduino 0022
// #include "WProgram.h"
// For Arduino 1.0
#include "Arduino.h"

#include <SPI.h>
#include <Ethernet.h>

#ifndef Mudbus_h
#define Mudbus_h

#define MB_N_R 125 //Max 16 bit registers for Modbus is 125

#define MB_N_C 128 //Max coils for Modbus is 2000 - dont need that many so here is a multiple of 8

#ifndef MB_PORT
#define MB_PORT 502
#endif

enum MB_FC {
  MB_FC_NONE           = 0,
  MB_FC_READ_COILS     = 1,
  MB_FC_READ_REGISTERS = 3,
  MB_FC_WRITE_COIL     = 5,
  MB_FC_WRITE_REGISTER = 6,
  //Function codes 15 & 16 by Martin Pettersson http://siamect.com
  MB_FC_WRITE_MULTIPLE_COILS = 15,
  MB_FC_WRITE_MULTIPLE_REGISTERS = 16
};

class Mudbus
{
public:
  //Mudbus(size_t R = MB_N_R, size_t C = MB_N_C);
	Mudbus();
	Mudbus(int *MW, bool *MB, uint8_t *BArray) : R(MW), C(MB), ByteArray(BArray){}
	void Run();  
	int *R;
	bool *C;
  bool Active;    
  unsigned long PreviousActivityTime;
  int Runs, Reads, Writes;
private: 
  uint8_t *ByteArray;			//Vector auxiliar donde se guardan los datos recibidos desde el cliente
  MB_FC FC;
  void SetFC(int fc);
};

template<size_t n_r = MB_N_R, size_t n_c = MB_N_C>	//Template, para editar el tamaño de los vectores desde la instancia
class Modbus : public Mudbus{
	public:
		Modbus() : Mudbus(MW, MB, Array.MW){}	//Envío los punteros de los array creados acá (mediante 'template')
		int MW[n_r];										//Vector de MW, es el mismo que R en la clase Mudbus
		bool MB[n_c]; 									//Vector de MB, es el mismo que C en la clase Mudbus
	private:
		union array{
			uint8_t MW[n_r * 2 + 10];
			uint8_t MB[n_c / 16 + 10];
		}Array;													//Vector auxiliar, es el mismo que ByteArray en la clase Mudbus
};

#endif

/* Speculations on Modbus message structure:
**********************************************
**********Master(PC) request frames***********
00 ID high              0
01 ID low               1
02 Protocol high        0
03 Protocol low         0
04 Message length high  0
05 Message length low   6 (6 bytes after this)
06 Slave number         1
07 Function code
08 Start address high   maybe 0
09 Start address low    maybe 0
10 Length high          maybe 125 or Data high if write
11 Length low           maybe 125 or Data low if write
**********************************************
**********Slave(Arduino) response frames******
00 ID high              echo  /           0
01 ID low               echo  /  slave ID 1
02 Protocol high        echo
03 Protocol low         echo
04 Message length high  echo
05 Message length low   num bytes after this
06 Slave number         echo
07 Function code        echo
08 Start address high   num bytes of data
09 Data high
10 Data low
**********************************************
*/