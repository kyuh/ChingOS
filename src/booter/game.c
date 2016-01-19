
#include "video.h"
#include "interrupts.h"

/* This is the entry-point for the game! */
void c_start(void) {
    /* TODO:  You will need to initialize various subsystems here.  This
     *        would include the interrupt handling mechanism, and the various
     *        systems that use interrupts.  Once this is done, you can call
     *        enable_interrupts() to start interrupt handling, and go on to
     *        do whatever else you decide to do!
     */

     // PUT ALL INIT STUFF HERE

     // Enables interrupts after we disbled them in the bootloader. Remember to
     // disable interrupts when writing an assembly handler.
     enable_interrupts();

     // TEST~nico~~
     init_video();

     // vvv GAME STUFF GOES HERE



    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {}
}

