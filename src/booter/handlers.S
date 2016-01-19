# TODO:  Put IRQ handlers in this file.  The handlers can do whatever you
#        want, but the recommendation is to keep them very simple; for
#        example, call a C function that handles the interrupt and then
#        returns.
#
#        Recall that interrupts are automatically disabled when the handler
#        is invoked, so handlers don't need to worry about being interrupted
#        by other handlers.  However, keep in mind that a handler can
#        interrupt any other code!  Therefore, be sure to disable interrupts
#        when working with data structures shared between interrupt handlers
#        and other code.
#
#        Handlers generally must follow this pattern:
#
#        .align 4
#        .globl irqN_handler
#        irqN_handler:
#                pusha   # Save registers from the interrupted code!
#                cld     # If calling C code, clear direction flag
#
#                ...     # Do whatever you want to do here.
#
#                # Acknowledge the interrupt so that the PIC will send more
#                # interrupts!  We do this by sending 0x20 to port 0x20.
#                # See http://wiki.osdev.org/8259_PIC#End_of_Interrupt for
#                # details.
#                mov     $0x20, %al
#                out     %al, $0x20
#
#                popa    # Restore all registers
#                iret    # Go back to interrupted code

.align 4
.globl clearScreen

clearScreen:
	pusha               # pushes all of the registers onto the stack.
	cli                 # disables interrupts
	movl $0xB8000, %edi # Move the beginning of the video buffer into edi.
	movw $2000, %cx     # Initialize the count register to the number of pixels.
	# Store the character attribute in the high bit...
	movb $14, %ah       # White text with a black background.
	# and a blank character (a space) in the low bit.
	movb $' ', %al
	# Repeat for every pixel.
	rep stosw           # We use stosw because we want to copy %ax.
	popa                # Restores the registers.
	ret

# handle timer interrupt
.align 4
.globl irq_timer_handler
irq_timer_handler:
        pusha   # Save registers from the interrupted code!
        cld     # If calling C code, clear direction flag

        call timer_handler
        # Acknowledge the interrupt so that the PIC will send more
        # interrupts!  We do this by sending 0x20 to port 0x20.
        # See http://wiki.osdev.org/8259_PIC#End_of_Interrupt for
        # details.
        mov     $0x20, %al
        out     %al, $0x20

        popa    # Restore all registers
        iret    # Go back to interrupted code
