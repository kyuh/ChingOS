#include "ports.h"
#include "keyboard.h"
#include "handlers.h"
#include "interrupts.h"

/* This is the IO port of the PS/2 controller, where the keyboard's scan
 * codes are made available.  Scan codes can be read as follows:
 *
 *     unsigned char scan_code = inb(KEYBOARD_PORT);
 *
 * Most keys generate a scan-code when they are pressed, and a second scan-
 * code when the same key is released.  For such keys, the only difference
 * between the "pressed" and "released" scan-codes is that the top bit is
 * cleared in the "pressed" scan-code, and it is set in the "released" scan-
 * code.
 *
 * A few keys generate two scan-codes when they are pressed, and then two
 * more scan-codes when they are released.  For example, the arrow keys (the
 * ones that aren't part of the numeric keypad) will usually generate two
 * scan-codes for press or release.  In these cases, the keyboard controller
 * fires two interrupts, so you don't have to do anything special - the
 * interrupt handler will receive each byte in a separate invocation of the
 * handler.
 *
 * See http://wiki.osdev.org/PS/2_Keyboard for details.
 */
#define KEYBOARD_PORT 0x60


/* TODO:  You can create static variables here to hold keyboard state.
 *        Note that if you create some kind of circular queue (a very good
 *        idea, you should declare it "volatile" so that the compiler knows
 *        that it can be changed by exceptional control flow.
 *
 *        Also, don't forget that interrupts can interrupt *any* code,
 *        including code that fetches key data!  If you are manipulating a
 *        shared data structure that is also manipulated from an interrupt
 *        handler, you might want to disable interrupts while you access it,
 *        so that nothing gets mangled...
 */

// so we're gonna go simple here 
// we just have an item for each key, and it's 1 if pressed
// and 0 if released
// we can easily poll this in the other code
// This goes in the header
unsigned char keys_pressed[255];


void init_keyboard(void) {
    /* TODO:  Initialize any state required by the keyboard handler. */

    install_interrupt_handler(KEYBOARD_INTERRUPT, irq_keyboard_handler);
}

//for now don't worry about multibyte scancodes
//that means that if you press a multibyte key
//the keys array might get a bit trashed
void keyboard_handler(void)
{
    unsigned char scan_code = inb(KEYBOARD_PORT);
    
    //mask off the top bit to see if it's pressed
    char pressed = !(0x80 & scan_code);
    
    //mask off the remainder for the code
    scan_code &= 0x7F;

    keys_pressed[scan_code] = pressed;
}
