#ifndef VIDEO_H
#define VIDEO_H


/* Available colors from the 16-color palette used for EGA and VGA, and
 * also for text-mode VGA output.
 */
#define BLACK          0
#define BLUE           1
#define GREEN          2
#define CYAN           3
#define RED            4
#define MAGENTA        5
#define BROWN          6
#define LIGHT_GRAY     7
#define DARK_GRAY      8
#define LIGHT_BLUE     9
#define LIGHT_GREEN   10
#define LIGHT_CYAN    11
#define LIGHT_RED     12
#define LIGHT_MAGENTA 13
#define YELLOW        14
#define WHITE         15


#define X_RES 320
#define Y_RES 200


#define ENEMY_DIM 16
#define ENEMY_WIDTH (ENEMY_DIM)
#define ENEMY_HEIGHT (ENEMY_DIM)
#define PLAYER_HEIGHT 24
#define PLAYER_WIDTH 16
#define BULLET_WIDTH 8
#define BULLET_HEIGHT 8

void init_video(void);
void write_string(unsigned char, char*);
void write_string_position (unsigned char, char*, int, int);

void color_screen(unsigned char color);
void update_screen();
void draw_bg_screen();

void color_rect(unsigned char color, int x, int y, int width, int height);

void draw_player(int x, int y);

void draw_enemy(int type, int x, int y);

void draw_bullet(int color, int type, int x, int y);

#endif /* VIDEO_H */
