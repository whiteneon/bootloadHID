/* Name: bootloaderconfig.h
 * Project: AVR bootloader HID
 * Author: Christian Starkjohann
 * Creation Date: 2007-03-19
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 * This Revision: $Id$
 */

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__

/*
General Description:
This file (together with some settings in Makefile) configures the boot loader
according to the hardware.

This file contains (besides the hardware configuration normally found in
usbconfig.h) two functions or macros: bootLoaderInit() and
bootLoaderCondition(). Whether you implement them as macros or as static
inline functions is up to you, decide based on code size and convenience.

bootLoaderInit() is called as one of the first actions after reset. It should
be a minimum initialization of the hardware so that the boot loader condition
can be read. This will usually consist of activating a pull-up resistor for an
external jumper which selects boot loader mode. You may call leaveBootloader()
from this function if you know that the main code should run.

bootLoaderCondition() is called immediately after initialization and in each
main loop iteration. If it returns TRUE, the boot loader will be active. If it
returns FALSE, the boot loader jumps to address 0 (the loaded application)
immediately.

For compatibility with Thomas Fischl's avrusbboot, we also support the macro
names BOOTLOADER_INIT and BOOTLOADER_CONDITION for this functionality. If
these macros are defined, the boot loader usees them.
*/

/* ---------------------------- Macro Magic ---------------------------- */
#define		PIN_CONCAT(a,b)			a ## b
#define		PIN_CONCAT3(a,b,c)		a ## b ## c

#define		PIN_PORT(a)			PIN_CONCAT(PORT, a)
#define		PIN_PIN(a)			PIN_CONCAT(PIN, a)
#define		PIN_DDR(a)			PIN_CONCAT(DDR, a)

#define		PIN(a, b)			PIN_CONCAT3(P, a, b)

/* ---------------------------- Hardware Config ---------------------------- */

#define USB_CFG_IOPORTNAME      D
/* This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
#define USB_CFG_DMINUS_BIT      7
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
#define USB_CFG_DPLUS_BIT       2
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0! [You can also use other interrupts, see section
 * "Optional MCU Description" below, or you can connect D- to the interrupt, as
 * it is required if you use the USB_COUNT_SOF feature. If you use D- for the
 * interrupt, the USB interrupt will also be triggered at Start-Of-Frame
 * markers every millisecond.]
 */
 #ifndef JUMPER_PORT
  #define JUMPER_PORT		USB_CFG_IOPORTNAME
#endif
/* 
 * jumper is connected to this port
 */
 
#ifndef LED_PORT
  #define LED_PORT		USB_CFG_IOPORTNAME
#endif
/*
* Bootloader indicator LED port
*/


#ifndef JUMPER_BIT
  /* This is Revision 3 and later (where PD6 and PD7 were swapped */
  #define JUMPER_BIT           6       /* Rev.2 and previous was 7 */
#endif
/* 
 * jumper is connected to this bit in port "JUMPER_PORT", active low
 */


#ifndef LED_BIT
  #define LED_BIT              5
#endif
/*
* Bootloader indicator LED bit
*/
 
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Clock rate of the AVR in MHz. Legal values are 12000, 12800, 15000, 16000,
 * 16500 and 20000. The 12.8 MHz and 16.5 MHz versions of the code require no
 * crystal, they tolerate +/- 1% deviation from the nominal frequency. All
 * other rates require a precision of 2000 ppm and thus a crystal!
 * Default if not specified: 12 MHz
 */

/* ----------------------- Optional Hardware Config ------------------------ */

 #define USB_CFG_PULLUP_IOPORTNAME   B 
/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */
 #define USB_CFG_PULLUP_BIT          0 
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */

/* --------------------------- Functional Range ---------------------------- */

#define BOOTLOADER_CAN_EXIT     1
/* If this macro is defined to 1, the boot loader command line utility can
 * initiate a reboot after uploading the FLASH when the "-r" command line
 * option is given. If you define it to 0 or leave it undefined, the "-r"
 * option won't work and you save a couple of bytes in the boot loader. This
 * may be of advantage if you compile with gcc 4 instead of gcc 3 because it
 * generates slightly larger code.
 * If you need to save even more memory, consider using your own vector table.
 * Since only the reset vector and INT0 (the first two vectors) are used,
 * this saves quite a bit of flash. See Alexander Neumann's boot loader for
 * an example: http://git.lochraster.org:2080/?p=fd0/usbload;a=tree
 */

/* ------------------------------------------------------------------------- */

/* Example configuration: Port D bit 3 is connected to a jumper which ties
 * this pin to GND if the boot loader is requested. Initialization allows
 * several clock cycles for the input voltage to stabilize before
 * bootLoaderCondition() samples the value.
 * We use a function for bootLoaderInit() for convenience and a macro for
 * bootLoaderCondition() for efficiency.
 */

#ifndef __ASSEMBLER__   /* assembler cannot parse function definitions */
#ifndef MCUCSR          /* compatibility between ATMega8 and ATMega88 */
#   define MCUCSR   MCUSR
#endif
#include <util/delay.h>
/* Original func
static inline void  bootLoaderInit(void)
{
    PORTD = 1 << 6; // activate pull-up for key 
    _delay_us(10);  // wait for levels to stabilize
} */

static inline void  bootLoaderInit(void)
{
    /* Deactivated by following 4 lines to activate status LED
	PIN_DDR(JUMPER_PORT)  = 0;
    PIN_PORT(JUMPER_PORT) = (1<< PIN(JUMPER_PORT, JUMPER_BIT)); // activate pull-up 
	*/
	
   PIN_DDR(JUMPER_PORT) &= ~_BV(JUMPER_BIT);
   PIN_DDR(LED_PORT) |= _BV(LED_BIT);
   PIN_PORT(JUMPER_PORT) |= _BV(JUMPER_BIT);
   PIN_PORT(LED_PORT) |= _BV(LED_BIT);
   //PIN_DDR(D) = 0b00100000;
   //PIN_PORT(D) = 0b01100000;
   

//     deactivated by Stephan - reset after each avrdude op is annoing!
//     if(!(MCUCSR & (1 << EXTRF)))    /* If this was not an external reset, ignore */
//         leaveBootloader();

    MCUCSR = 0;                     /* clear all reset flags for next time */
}

static inline void  bootLoaderExit(void)
{
	PIN_PORT(LED_PORT) = 0;
    PIN_PORT(JUMPER_PORT) = 0;
	PIN_DDR(LED_PORT) = 0;
	PIN_DDR(JUMPER_PORT) = 0;		/* undo bootLoaderInit() changes */
	
}


//#define bootLoaderCondition()   ((PIND & (1 << 6)) == 0)   /* True if jumper is set */
#define bootLoaderCondition()		((PIN_PIN(JUMPER_PORT) & (1 << PIN(JUMPER_PORT, JUMPER_BIT))) == 0)

#endif

/* ------------------------------------------------------------------------- */

#endif /* __bootloader_h_included__ */
