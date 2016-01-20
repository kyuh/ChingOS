
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

int getPixelOffset(int x, int y){
    return y * X_RES + x;
}



// Entity storage
Player player;
GameArray enemy_arr;
GameArray player_bullet_arr;
GameArray enemy_bullet_arr;

// Buffer space for above arrays
char space[200000];

// Capacities
#define MAX_ENEMIES 10
#define MAX_PLAYER_BULLETS 1000
#define MAX_ENEMY_BULLETS 1000

#define ENEMY_ARR_OFFSET 0
#define PLAYER_BULLET_ARR_OFFSET 10000
#define ENEMY_BULLET_ARR_OFFSET 100000

// Positions and other data
#define NUM_ENEMIES 1
#define PLAYER_START_POS_X 160
#define PLAYER_START_POS_Y 150


void init_entities() {
    // Init player
    player.pos_x = PLAYER_START_POS_X;
    player.pos_y = PLAYER_START_POS_Y;

    // Init enemies
    enemy_arr = createGameArray(MAX_ENEMIES, space + ENEMY_ARR_OFFSET);

    Enemy test_enemy;
    test_enemy.pos_x = 160;
    test_enemy.pos_y = 50;

    GameUnion enemy_gu;
    enemy_gu.enemy = test_enemy;
    GameArrayInsert(&enemy_arr, enemy_gu);
}


// Tentative drawing function
void draw_enemies(){
    int i;
    for (i = 0; i < enemy_arr.size; i++){
        GameUnion enemy_gu = GameArrayGet(&enemy_arr, i);
        int pos_x = (int) enemy_gu.enemy.pos_x;
        int pos_y = (int) enemy_gu.enemy.pos_y;
        color_pixel(LIGHT_CYAN, getPixelOffset(pos_x, pos_y));
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
    #if 0
    volatile float a = 3.65;
    volatile float b = 3.22;
    volatile float c = a + b;
    #endif

    init_entities();
    draw_enemies();

    color_pixel(LIGHT_CYAN, 5 * X_RES + 10);

    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {
        int currentTime = ticks;
        
        //game logic goes here
        
        // 2 ticks per game loop
        // so approximately 30fps
        sleep_until(currentTime + 3);
    }
}

