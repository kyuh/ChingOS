#include "video.h"
#include "handlers.h"
#include "boot.h"
#include "assets.h"
#include "timer.h"

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
#define VGA_BUFFER ((char *) 0xA0000)
// we need to single buffer our video or else
// we'll get flickering on slower devices
// this is a nonissue with h/w virt (like on vbox), but for software qemu
// this will help
#define VIDEO_BUFFER ((char *) 0xA00000)


/* TODO:  You can create static variables here to hold video display state,
 *        such as the current foreground and background color, a cursor
 *        position, or any other details you might want to keep track of!
 */

Game_Assets *assets;

//copies temporary screen buffer to vga screen buffer
void update_screen()
{
    for(int i = 0; i < X_RES * Y_RES; i++)
    {
        VGA_BUFFER[i] = VIDEO_BUFFER[i];
    }
}

void color_screen(unsigned char color)
{
    for(int i = 0; i < X_RES * Y_RES; i++)
    {
        VIDEO_BUFFER[i] = color;
    }
}

//no bounds checking
void color_rect(unsigned char color, int x, int y, int width, int height)
{
    for(int i = x; i < x + width; i++)
    {
        for(int j = y; j < y + height; j++)
        {
            VIDEO_BUFFER[i + X_RES * j] = color;
        }
    }
}


void draw_bg_screen()
{
    char* image = &(assets->screen[0]);
    for(int i = 0; i < X_RES * Y_RES; i++)
    {
        VIDEO_BUFFER[i] = image[i];
    }
}

char string[] = "Nico Nico Ni";
char empty[] = " ";


////////////////
// code to draw sprites


void draw_player(int x, int y)
{
    //offset it because we want to draw centered
    x -= PLAYER_WIDTH / 2;
    y -= PLAYER_HEIGHT / 2;

	char *video = (char*) VIDEO_BUFFER;
    //player is offset 48 px on sprite sheet
    //sprite sheet is 128 px wide

    char *player_start = &(assets->girls[48 * 128]);
    
    //now offset it for the different idle animation
    int frame = (ticks >> 5) % 4;

    player_start += PLAYER_WIDTH * frame;

    for(int xi = 0; xi < PLAYER_WIDTH; xi++)
    {
        for(int yi = 0; yi < PLAYER_HEIGHT; yi++)
        {
            int xpos = x + xi;
            int ypos = y + yi;
            //bounds check and transparency check
            if(xpos > 0 && xpos < X_RES &&
               ypos > 0 && ypos < Y_RES &&
               player_start[xi + yi * 128])
            {
                video[xpos + X_RES * ypos] = player_start[xi + 128 * yi];
            }
        }
    }
}

void draw_enemy(int type, int x, int y)
{
    //offset it because we want to draw centered
    x -= ENEMY_DIM / 2;
    y -= ENEMY_DIM / 2;

	char *video = (char*) VIDEO_BUFFER;
    //player is offset 48 px on sprite sheet
    //sprite sheet is 128 px wide

    char *enemy_start = &(assets->girls[ENEMY_DIM * 128 * type]);
    
    //now offset it for the different idle animation
    int frame = (ticks >> 5) % 8;

    enemy_start += ENEMY_DIM * frame;

    for(int xi = 0; xi < ENEMY_DIM; xi++)
    {
        for(int yi = 0; yi < ENEMY_DIM; yi++)
        {
            int xpos = x + xi;
            int ypos = y + yi;
            //bounds check and transparency check
            if(xpos > 0 && xpos < X_RES &&
               ypos > 0 && ypos < Y_RES &&
               enemy_start[xi + yi * 128])
            {
                video[xpos + X_RES * ypos] = enemy_start[xi + 128 * yi];
            }
        }
    }
}

// color is NOT a vga, color, it's from 1-16 in the bullet strip
void draw_bullet(int color, int type, int x, int y)
{
    //offset it because we want to draw centered
    x -= BULLET_WIDTH / 2;
    y -= BULLET_HEIGHT / 2;

    //we are willing to draw bullets partially clipping off the screen
	char *video = (char*) VIDEO_BUFFER;

    //bullet sheet is 128 px wide
    char *bullet_sheet = &(assets->bullets[0]);
    char *this_bullet = bullet_sheet + BULLET_WIDTH * color + BULLET_HEIGHT * 128 * type;

    for(int xi = 0; xi < BULLET_WIDTH; xi++)
    {
        for(int yi = 0; yi < BULLET_HEIGHT; yi++)
        {
            int xpos = x + xi;
            int ypos = y + yi;
            //bounds check and transparency check
            if(xpos > 0 && xpos < X_RES &&
               ypos > 0 && ypos < Y_RES &&
               this_bullet[xi + yi * 128])
            {
                video[xpos + X_RES * ypos] = this_bullet[xi + 128 * yi];
            }
        }
    }
}

////////////////
// code to write characters and strings


//
// color is a VGA color, look at the vga color palette
//

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

// Warning, does not check bounds
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

void write_string(unsigned char color, char* string) {
    write_string_position(color, string, 0, 0);
}

//DED method, do not call
void color_pixel(unsigned char color, unsigned short offset) {
}
//also DED
void write_string_offset(unsigned char color, char* string, int offset) {
}


void init_video(void) {
    char * datas = ((char*) PROGRAM_BASE_ADDR);
    datas += 100000;
    datas -= 512;

    assets = (Game_Assets*)datas;

    //color_screen(0x3f);
//    write_string_position(5, "wowowow", 5, 5);
//    write_string_position(5, hu, 5, 15);
//    write_string_position(5, katana, 5, 25);

    draw_bullet(6, 11, 15, 5);
    draw_player(15, 35);
    draw_enemy(0, 15, 55);
    draw_enemy(1, 15, 70);
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

