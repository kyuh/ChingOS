
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "handlers.h"

/* This is the entry-point for the game! */
void c_start(void) {
    /* TODO:  You will need to initialize various subsystems here.  This
     *        would include the interrupt handling mechanism, and the various
     *        systems that use interrupts.  Once this is done, you can call
     *        enable_interrupts() to start interrupt handling, and go on to
     *        do whatever else you decide to do!
     */

    // PUT ALL INIT STUFF HERE
    // TEST~nico~~
    init_video();
    init_interrupts();
    init_timer();
    //for(int i = 0; i < 25; i++)
    //{
    //    install_interrupt_handler(i, irq_timer_handler);
    //}

    // Enables interrupts after we disbled them in the bootloader. Remember to
    // disable interrupts when writing an assembly handler.
    enable_interrupts();

     // vvv GAME STUFF GOES HERE

    int x;

    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {
        sleep_until(ticks + 100);
    	x++;
    }
}

