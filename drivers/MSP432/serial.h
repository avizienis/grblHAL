/*

  serial.h - low level functions for transmitting bytes via the serial port

  Part of Grbl

  Copyright (c) 2017-2018 Terje Io

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdint.h>
#include <stdbool.h>

#define BACKCHANNEL // comment out to use Launchpad Backchannel UART

#define eusci(p) eusciM(p)
#define eusciM(p) EUSCI_ ## p
#define eusciINT(p) eusciI(p)
#define eusciI(p) EUSCI ## p ## _IRQn

#define BS   0x08
#define LF   0x0A
#define CR   0x0D
#define XON  0x11
#define XOFF 0x13
#define CAN  0x18
#define EOF  0x1A
#define DEL  0x7F
#define EOL  "\r\n"

#define XONOK (XON|0x80)
#define XOFFOK (XOFF|0x80)
#define TX_BUFFER_SIZE 128      // must be a power of 2
#define RX_BUFFER_SIZE 1024     // must be a power of 2
#define RX_BUFFER_HWM 900
#define RX_BUFFER_LWM 300
#define RTS_PORT P1
#define RTS_PIN  4
#define RTS_BIT (1<<RTS_PIN)
//#define LINE_BUFFER_SIZE 20

// Define serial port pins and module

#define SERIAL_MOD A0
#define SERIAL_MODULE eusci(SERIAL_MOD)
#define SERIAL_MODULE_INT eusciINT(SERIAL_MOD)
#define SERIAL_PORT P1
#define SERIAL_RX BIT2
#define SERIAL_TX BIT3
#define SERIAL_RTS_PORT P1
#define SERIAL_RTS_PIN 4
#define SERIAL_RTS_BIT (1<<SERIAL_RTS_PIN)

#define SERIAL2_MOD A2
#define SERIAL2_MODULE eusci(SERIAL2_MOD)
#define SERIAL2_MODULE_INT eusciINT(SERIAL2_MOD)
#define SERIAL2_PORT P3
#define SERIAL2_RX BIT2
#define SERIAL2_TX BIT3

//

void serialInit(void);
int16_t serialGetC(void);
bool serialPutC(const char c);
void serialWriteS(const char *s);
void serialWriteLn(const char *s);
void serialWrite(const char *s, uint16_t length);

#ifdef LINE_BUFFER_SIZE
char *serialReadLn (void);
#endif

#ifdef RX_BUFFER_SIZE
uint16_t serialTxCount(void);
uint16_t serialRxCount(void);
uint16_t serialRxFree(void);
void serialRxFlush(void);
void serialRxCancel(void);
void setSerialReceiveCallback (bool (*fn)(char));
void setSerialBlockingCallback (bool (*fn)(void));
#endif

#ifdef SERIAL2_MOD
uint16_t serial2RxFree (void);
void serial2RxFlush (void);
void serial2RxCancel (void);
int16_t serial2GetC (void);
void serialSelect (bool mpg);
#endif
