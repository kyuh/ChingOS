#include "video.h"
#include "handlers.h"
#include "boot.h"
#include "assets.h"
#include "strings.h"

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


/* TODO:  You can create static variables here to hold video display state,
 *        such as the current foreground and background color, a cursor
 *        position, or any other details you might want to keep track of!
 */

Game_Assets *assets;


void color_screen(unsigned char color)
{
    char* image = &(assets->screen[0]);
    for(int i = 0; i < X_RES * Y_RES; i++)
    {
        VIDEO_BUFFER[i] = image[i];
    }
}

char string[] = "Nico Nico Ni";
char empty[] = " ";

//TODO no good
void write_string_offset(unsigned char color, char* string, int offset) {
	char *video = (char*) VIDEO_BUFFER + offset;
	while(*string != 0) {
		*video++ = *string++;
		*video++ = color;
	}
}

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 8

void write_char(unsigned char color, char* screen_position, char* char_position)
{
    //loop over the font glyph
    //font page width is 64 pixels
    for(int x = 0; x < CHAR_WIDTH; x++)
    {
        for(int y = 0; y < CHAR_HEIGHT; y++)
        {
            if(!char_position[x + y * 64])
                screen_position[x + y * X_RES] =  color;
        }
    }
}

// TODO bounds checking
void write_string_position(unsigned char color, char* string, int x, int y)
{
    char* font_table = &(assets->font[0]);

	char *video = (char*) VIDEO_BUFFER;
    char *start_point = video + x + (y * X_RES);
    
    while(*string != 0)
    {
        unsigned char char_code = *string;
        //get the row and column of the string
        //there are 16 chars per row
        //and lots of rows
        int row = char_code / 16;
        int col = char_code % 16;

        //now find the location of the char start
        char* char_position = font_table + col * CHAR_WIDTH + row * 64 * CHAR_HEIGHT;

        write_char(color, start_point, char_position);

        //advance to the next character
        start_point += CHAR_WIDTH;
        string++;
    }
}

//TODO no good
void write_string(unsigned char color, char* string) {
	char *video = (char*) VIDEO_BUFFER;
	while(*string != 0) {
		*video++ = *string++;
		*video++ = color;
	}
}

void color_pixel(unsigned char color, unsigned short offset) {
	char *video = (char*) VIDEO_BUFFER + offset;
	*video++ = 219;
	*video++ = color;
	// (unsigned char *) VIDEO_BUFFER[offset] = color;
}



void init_video(void) {
    char * datas = ((char*) PROGRAM_BASE_ADDR);
    datas += 100000;
    datas -= 512;

    assets = (Game_Assets*)datas;

    //color_screen(0x3f);
    write_string_position(5, "wowowow", 5, 5);
    write_string_position(5, hu, 5, 15);
    write_string_position(5, katana, 5, 25);
    /* TODO:  Do any video display initialization you might want to do, such
     *        as clearing the screen, initializing static variable state, etc.
     */
    // Clear Screen - not yet debugged.
    // clearScreen();

    #if 0
    //TEST~nico~~
    #if 0
    write_string_offset(RED, string, 10);
    #endif

    // TEST 2~~nico~~~~
    color_pixel(RED, 50);
    color_pixel(BLUE, 52);
    color_pixel(YELLOW, 54);
    color_pixel(WHITE, 210);
    color_pixel(GREEN, 212);
    color_pixel(CYAN, 214);
    #endif

}

