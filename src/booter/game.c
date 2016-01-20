
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "handlers.h"
#include "keyboard.h"




/* Wrapper for all game types,
used so that we can have a common vector type */

typedef struct {
    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
} Bullet;


typedef struct {
    float pos_x;
    float pos_y;
} Enemy;


typedef struct {
    float pos_x;
    float pos_y;
} Player;


typedef union {
    Bullet bullet;
    Enemy enemy;
    int val;
} GameUnion;

// Sized array of char arrays
typedef struct {
    GameUnion *data;
    int size;
    int capacity;
} GameArray;

GameArray createGameArray(int cap, char *memory_start_pos){
    GameArray gArr;
    gArr.data = (GameUnion*)memory_start_pos;
    gArr.size = 0;
    gArr.capacity = cap;
    return gArr;
}

void GameArrayDeleteLast(GameArray *gArr){
    gArr->size -= 1;
}

// Return if successfully inserted
int GameArrayInsert(GameArray *gArr, GameUnion elt){
    if (gArr->size == gArr->capacity){
        return 0;
    }
    gArr->data[gArr->size] = elt;
    gArr->size += 1;
    return 1;
}

GameUnion GameArrayGet(GameArray *gArr, unsigned int idx){
    return gArr->data[idx];
}

// TODO: Probably should put some safety check
GameUnion GameArraySet(GameArray *gArr, unsigned int idx, GameUnion thing){
    gArr->data[idx] = thing;
}

int getPixelOffset(int x, int y){
    return y * X_RES + x;
}



// Entity storage
Player player;
GameArray enemy_arr;
GameArray player_bullet_arr;
GameArray enemy_bullet_arr;

GameArray temp_arr;

// Buffer space for above arrays
char *space = 0x100000;

// Capacities
#define MAX_ENEMIES 10
#define MAX_PLAYER_BULLETS 500
#define MAX_ENEMY_BULLETS 1000
#define MAX_TEMP 1000

#define ENEMY_ARR_OFFSET 0
#define PLAYER_BULLET_ARR_OFFSET 10000
#define ENEMY_BULLET_ARR_OFFSET 100000
#define TEMP_ARR_OFFSET 200000

// Positions and other data
#define NUM_ENEMIES 1
#define PLAYER_START_POS_X 160
#define PLAYER_START_POS_Y 150

#define PLAYER_MOVE_INCREMENT 3


void init_entities() {
    // Init player
    player.pos_x = PLAYER_START_POS_X;
    player.pos_y = PLAYER_START_POS_Y;

    // Init enemies
    enemy_arr = createGameArray(MAX_ENEMIES, space + ENEMY_ARR_OFFSET);

    // (Just create one for now and put it at a set location)
    Enemy test_enemy;
    test_enemy.pos_x = 160;
    test_enemy.pos_y = 50;

    GameUnion enemy_gu;
    enemy_gu.enemy = test_enemy;
    GameArrayInsert(&enemy_arr, enemy_gu);


    // Init player bullets
    player_bullet_arr = createGameArray(MAX_PLAYER_BULLETS, space + PLAYER_BULLET_ARR_OFFSET);

    // Init player bullets
    enemy_bullet_arr = createGameArray(MAX_ENEMY_BULLETS, space + ENEMY_BULLET_ARR_OFFSET);

}


// Tentative drawing function
void draw_entities(){
    int i;

    // clear
    color_screen(BLACK);

    // Draw player
    draw_player(player.pos_x, player.pos_y);

    // Draw enemies
    for (i = 0; i < enemy_arr.size; i++){
        GameUnion enemy_gu = GameArrayGet(&enemy_arr, i);
        int pos_x = (int) enemy_gu.enemy.pos_x;
        int pos_y = (int) enemy_gu.enemy.pos_y;
        draw_enemy(0, pos_x, pos_y);
    }

    // Draw player bullets
    for (i = 0; i < player_bullet_arr.size; i++){
        GameUnion player_bullet_gu = GameArrayGet(&player_bullet_arr, i);
        int pos_x = (int) player_bullet_gu.bullet.pos_x;
        int pos_y = (int) player_bullet_gu.bullet.pos_y;
        draw_bullet(WHITE, 0, pos_x, pos_y);
    }

    // Draw enemy bullets
    for (i = 0; i < enemy_bullet_arr.size; i++){
        GameUnion enemy_bullet_gu = GameArrayGet(&enemy_bullet_arr, i);
        int pos_x = (int) enemy_bullet_gu.bullet.pos_x;
        int pos_y = (int) enemy_bullet_gu.bullet.pos_y;
        draw_bullet(WHITE, 0, pos_x, pos_y);
    }
}


#define KEY_UP    (0x11)
#define KEY_DOWN  (0x1F)
#define KEY_LEFT  (0x1E)
#define KEY_RIGHT (0x20)
#define KEY_SPACE (0x39)


#define BULLET_WAIT 30
int last_bullet_tick = -1;

void handle_keyboard(){
    if (keys_pressed[KEY_SPACE] && (last_bullet_tick + BULLET_WAIT < ticks)){
        // player shoots bullet
        Bullet b;
        b.pos_x = 160;
        b.pos_y = 170;
        b.vel_x = 0;
        b.vel_y = -2;

        GameUnion b_gu;
        b_gu.bullet = b;
        GameArrayInsert(&player_bullet_arr, b_gu);

        last_bullet_tick = ticks;
    }

    if (keys_pressed[KEY_LEFT]){
        player.pos_x -= PLAYER_MOVE_INCREMENT;
    }

    if (keys_pressed[KEY_RIGHT]){
        player.pos_x += PLAYER_MOVE_INCREMENT;
    }

    if (keys_pressed[KEY_DOWN]){
        player.pos_y += PLAYER_MOVE_INCREMENT;
    }

    if (keys_pressed[KEY_UP]){
        player.pos_y -= PLAYER_MOVE_INCREMENT;
    }

}

int bound_check_x(float x){
    return (x >= 0 && x < X_RES);
}

int bound_check_y(float y){
    return (y >= 0 && y < Y_RES);
}


float kabs(float x){
    if (x >= 0){
        return x;
    } else {
        return -x;
    }
}

int pb_enemy_intersect(Bullet b, Enemy e){
    //return (kabs(b.pos_y - e.pos_y) < 10);

    int y_in_range = ((e.pos_y - ENEMY_HEIGHT < b.pos_y + BULLET_HEIGHT)
                   || (e.pos_y + ENEMY_HEIGHT > b.pos_y - BULLET_HEIGHT));

    int x_in_range = ((e.pos_x - ENEMY_WIDTH < b.pos_x + BULLET_WIDTH)
                   || (e.pos_x + ENEMY_WIDTH > b.pos_x - BULLET_WIDTH));

    //return x_in_range;
    return (x_in_range && y_in_range);

}

int eb_player_intersect(Bullet b, Player p){

    int y_in_range = ((p.pos_y - PLAYER_HEIGHT < b.pos_y + BULLET_HEIGHT)
                   || (p.pos_y + PLAYER_HEIGHT > b.pos_y - BULLET_HEIGHT));

    int x_in_range = ((p.pos_x - PLAYER_WIDTH < b.pos_x + BULLET_WIDTH)
                   || (p.pos_x + PLAYER_WIDTH > b.pos_x - BULLET_WIDTH));

    return (x_in_range && y_in_range);

}


void update_player_bullets() {
 
    // Init temporary storage space for filtered bullets
    //temp_arr = createGameArray(MAX_TEMP, space + TEMP_ARR_OFFSET);


    //draw_bullet(WHITE, 0, 50, 20);
    //return 0;


    // We'll basically overwrite any bullet that needs
    // to be removed
    int pb_offset = 0;


    int i;
    for (int i = 0; i < player_bullet_arr.size; i++){

        // TEST
        //draw_bullet(WHITE, 0, 50, 180);

        GameUnion b_gu = GameArrayGet(&player_bullet_arr, i);
        b_gu.bullet.pos_x += b_gu.bullet.vel_x;
        b_gu.bullet.pos_y += b_gu.bullet.vel_y;

        if (!bound_check_x(b_gu.bullet.pos_x) || !bound_check_y(b_gu.bullet.pos_y)){
            pb_offset++;
        } else {

            int enemy_offset = 0;
            int an_enemy_hit = 0;
            for (int j = 0; j < enemy_arr.size; j++){
                GameUnion e_gu = GameArrayGet(&enemy_arr, j);

                if (pb_enemy_intersect(b_gu.bullet, e_gu.enemy)){
                    enemy_offset++;
                    an_enemy_hit = 1;
                } else {
                    GameArraySet(&enemy_arr, i - enemy_offset, e_gu);
                }
            }

            enemy_arr.size -= enemy_offset;

            if (an_enemy_hit) {
                pb_offset++;
            } else {
                GameArraySet(&player_bullet_arr, i - pb_offset, b_gu);   
            }
        }


    }

    player_bullet_arr.size -= pb_offset;
}

// A bit of ugly repeated code
void update_enemy_bullets() {
    // Init temporary storage space for filtered bullets
    //temp_arr = createGameArray(MAX_TEMP, space + TEMP_ARR_OFFSET);

    // We'll basically overwrite any bullet that needs
    // to be removed
    int offset = 0;

    int i;
    for (int i = 0; i < enemy_bullet_arr.size; i++){
        GameUnion b_gu = GameArrayGet(&enemy_bullet_arr, i);
        b_gu.bullet.pos_x += b_gu.bullet.vel_x;
        b_gu.bullet.pos_y += b_gu.bullet.vel_y;

        if (!bound_check_x(b_gu.bullet.pos_x) || !bound_check_y(b_gu.bullet.pos_y)){
            offset++;
        } else if (eb_player_intersect(b_gu.bullet, player)){
            color_screen(BLACK);
            // For now, just loop, game over
            while(1){}
        } else {
            GameArraySet(&enemy_bullet_arr, i - offset, b_gu);
        }

    }

    enemy_bullet_arr.size -= offset;
}

// See the Wikipedia article on
// linear congruential generators
unsigned int prev_internal_val;
unsigned int a = 1664525;
unsigned int c = 1013904223;
unsigned int gen_rand(int range) {
    prev_internal_val = a * prev_internal_val + c;
    return (prev_internal_val % range);
}


#define INVERSE_BULLET_CHANCE 1000

void add_enemy_bullets(){
    int i;
    for (i = 0; i < enemy_arr.size; i++){

        GameUnion gu_cur_enemy = GameArrayGet(&enemy_arr, i);

        // Don't add at all ticks - randomly select
        unsigned int testval = gen_rand(INVERSE_BULLET_CHANCE);
        if (!testval){
            Bullet b;
            b.pos_x = gu_cur_enemy.enemy.pos_x;
            b.pos_y = gu_cur_enemy.enemy.pos_y;

            // boring velocity for now
            b.vel_x = 0;
            b.vel_y = 2;

            GameUnion gu_cur_bullet;
            gu_cur_bullet.bullet = b;

            GameArrayInsert(&enemy_bullet_arr, gu_cur_bullet);
        }
    }
}


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

    // init the interrupt table
    init_interrupts();

    //init the timer and keyboard state and 
    // add the interrupt handlers
    init_timer();
    init_keyboard();

    // Enables interrupts after we disbled them in the bootloader. Remember to
    // disable interrupts when writing an assembly handler.
    enable_interrupts();

     // vvv GAME STUFF GOES HERE


    init_entities();

    // TEST
    // Add some random stray bullets for testing
    GameUnion pb_gu;
    pb_gu.bullet.pos_x = 100;
    pb_gu.bullet.pos_y = 100;
    pb_gu.bullet.vel_x = 0;
    pb_gu.bullet.vel_y = 0;
    GameArrayInsert(&player_bullet_arr, pb_gu);

    GameUnion eb_gu;
    eb_gu.bullet.pos_x = 200;
    eb_gu.bullet.pos_y = 120;
    pb_gu.bullet.vel_x = 0;
    pb_gu.bullet.vel_y = 0;
    GameArrayInsert(&enemy_bullet_arr, eb_gu);


    draw_entities();

    // TEST
    color_pixel(LIGHT_CYAN, 5 * X_RES + 10);

    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {
        int currentTime = ticks;
        
        handle_keyboard();

        update_player_bullets();

        add_enemy_bullets();

        update_enemy_bullets();

        draw_entities();

        
        // 2 ticks per game loop
        // so approximately 30fps
        sleep_until(currentTime + 10);
    }
}

