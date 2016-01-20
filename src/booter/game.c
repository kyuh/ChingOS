
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

void handle_keyboard(){
    if (keys_pressed[KEY_SPACE]){
        // player shoots bullet
        Bullet b;
        b.pos_x = 160;
        b.pos_y = 170;
        b.vel_x = 0;
        b.vel_y = -2;

        GameUnion b_gu;
        b_gu.bullet = b;
        GameArrayInsert(&player_bullet_arr, b_gu);
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


void update_bullet_positions(GameArray *bullet_arr){

    // Init temporary storage space for filtered bullets
    //temp_arr = createGameArray(MAX_TEMP, space + TEMP_ARR_OFFSET);

    // We'll basically overwrite any bullet that needs
    // to be removed
    int offset = 0;

    int i;
    for (int i = 0; i < bullet_arr->size; i++){
        GameUnion b_gu = GameArrayGet(bullet_arr, i);
        b_gu.bullet.pos_x += b_gu.bullet.vel_x;
        b_gu.bullet.pos_y += b_gu.bullet.vel_y;

        if (!bound_check_x(b_gu.bullet.pos_x) || !bound_check_y(b_gu.bullet.pos_y)){
            offset++;
        } else {
            GameArraySet(bullet_arr, i - offset, b_gu);
        }
    }

    bullet_arr->size -= offset;
}


void update_player_bullets(){
    update_bullet_positions(&player_bullet_arr);
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
    GameArrayInsert(&player_bullet_arr, pb_gu);

    GameUnion eb_gu;
    eb_gu.bullet.pos_x = 200;
    eb_gu.bullet.pos_y = 120;
    GameArrayInsert(&enemy_bullet_arr, eb_gu);


    draw_entities();

    // TEST
    color_pixel(LIGHT_CYAN, 5 * X_RES + 10);

    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {
        int currentTime = ticks;
        
        handle_keyboard();

        update_player_bullets();

        draw_entities();

        
        // 2 ticks per game loop
        // so approximately 30fps
        sleep_until(currentTime + 20);
    }
}

