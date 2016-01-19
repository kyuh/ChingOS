#include "video.h"
#include "handlers.h"
#include "boot.h"
#include "assets.h"

/* This is the address of the VGA text-mode video buffer.  Note that this
 * buffer actually holds 8 pages of text, but only the first page (page 0)
 * will be displayed.
 *
 * Individual characters in text-mode VGA are represented as two adjacent
 * bytes:
 *     Byte 0 = the character value
 *     Byte 1 = the color of the character:  the high nibble is the background
 *              color, and the low nibble is the foreground color
 *
 * See http://wiki.osdev.org/Printing_to_Screen for more details.
 *
 * Also, if you decide to use a graphical video mode, the active video buffer
 * may reside at another address, and the data will definitely be in another
 * format.  It's a complicated topic.  If you are really intent on learning
 * more about this topic, go to http://wiki.osdev.org/Main_Page and look at
 * the VGA links in the "Video" section.
 */
#define VIDEO_BUFFER ((char *) 0xA0000)

#define X_RES 320
#define Y_RES 200

/* TODO:  You can create static variables here to hold video display state,
 *        such as the current foreground and background color, a cursor
 *        position, or any other details you might want to keep track of!
 */

void color_screen(unsigned char color)
{
    char * datas = ((char*) PROGRAM_BASE_ADDR);
    datas += 100000;
    datas -= 512;

    Game_Assets * assets = (Game_Assets*)datas;

    char* image = &(assets->screen[0]);
    for(int i = 0; i < X_RES * Y_RES; i++)
    {
        VIDEO_BUFFER[i] = image[i];
    }
}

char string[] = "Nico Nico Ni";
char empty[] = " ";

void write_string_offset(unsigned char color, char* string, int offset) {
	volatile char *video = (volatile char*) VIDEO_BUFFER + offset;
	while(*string != 0) {
		*video++ = *string++;
		*video++ = color;
	}
}

void write_string(unsigned char color, char* string) {
	volatile char *video = (volatile char*) VIDEO_BUFFER;
	while(*string != 0) {
		*video++ = *string++;
		*video++ = color;
	}
}

void color_pixel(unsigned char color, unsigned short offset) {
	volatile char *video = (volatile char*) VIDEO_BUFFER + offset;
	*video++ = 219;
	*video++ = color;
	// (unsigned char *) VIDEO_BUFFER[offset] = color;
}

void init_video(void) {
    color_screen(0x3f);
    /* TODO:  Do any video display initialization you might want to do, such
     *        as clearing the screen, initializing static variable state, etc.
     */
    // Clear Screen - not yet debugged.
    // clearScreen();

    //TEST~nico~~
    write_string_offset(RED, string, 10);

    // TEST 2~~nico~~~~
    color_pixel(RED, 50);
    color_pixel(BLUE, 52);
    color_pixel(YELLOW, 54);
    color_pixel(WHITE, 210);
    color_pixel(GREEN, 212);
    color_pixel(CYAN, 214);

}

