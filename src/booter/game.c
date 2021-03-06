
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "handlers.h"
#include "keyboard.h"

#include "strings.h"



/* Wrapper for all game types,
used so that we can have a common vector type */

#define BULLET_T 0
#define POWER_T 1
#define NIL 0
#define PRED 1
#define PBLUE 2

typedef struct {
    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
    int type;    // Holds bullet vs. powerup
    int state;   // Holds powerup type.
} Bullet;


typedef struct {
    float pos_x;
    float pos_y;
    int type;
    float state;
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

void GameArrayDelete(GameArray *gArr, unsigned int idx){
    gArr->data[idx] = gArr->data[gArr->size - 1];
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
char *space = (char *)0x100000;

// Capacities
#define MAX_ENEMIES 50
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

int dead;
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
    test_enemy.type = 0;
    test_enemy.state = 0;

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
        draw_enemy(enemy_gu.enemy.type, pos_x, pos_y);
    }

    // Draw player bullets
    for (i = 0; i < player_bullet_arr.size; i++){
        GameUnion player_bullet_gu = GameArrayGet(&player_bullet_arr, i);
        int pos_x = (int) player_bullet_gu.bullet.pos_x;
        int pos_y = (int) player_bullet_gu.bullet.pos_y;
        draw_bullet(1, 6, pos_x, pos_y);
    }

    // Draw enemy bullets
    for (i = 0; i < enemy_bullet_arr.size; i++){
        GameUnion enemy_bullet_gu = GameArrayGet(&enemy_bullet_arr, i);
        int pos_x = (int) enemy_bullet_gu.bullet.pos_x;
        int pos_y = (int) enemy_bullet_gu.bullet.pos_y;
        if(enemy_bullet_gu.bullet.type == BULLET_T)
        {
            if(enemy_bullet_gu.bullet.state == PRED)
            {
                draw_bullet(2, 3, pos_x, pos_y);
            }
            else
            {
                draw_bullet(6, 2, pos_x, pos_y);
            }
        }
        else
        {
            if(enemy_bullet_gu.bullet.state == PRED)
            {
                draw_bullet(2, 7, pos_x, pos_y);
            }
            else
            {
                draw_bullet(5, 7, pos_x, pos_y);
            }
        }
    }
}


#define KEY_UP    (0x48)
#define KEY_DOWN  (0x50)
#define KEY_LEFT  (0x4B)
#define KEY_RIGHT (0x4D)
#define KEY_SPACE (0x2C)
#define KEY_SHIFT (0x2A)


#define BULLET_WAIT 3
int last_bullet_tick = -1;

void handle_keyboard(){
    if (keys_pressed[KEY_SPACE] && (last_bullet_tick + BULLET_WAIT < ticks)){
        // player shoots bullet
        Bullet b;
        b.pos_x = player.pos_x;
        b.pos_y = player.pos_y - 5;
        b.vel_x = 0;
        b.vel_y = -16;
        b.type = BULLET_T;

        GameUnion b_gu;
        b_gu.bullet = b;
        GameArrayInsert(&player_bullet_arr, b_gu);

        last_bullet_tick = ticks;
    }

    int increment = PLAYER_MOVE_INCREMENT;
    if (!keys_pressed[KEY_SHIFT])
    {
        increment *= 2;
    }

    if (keys_pressed[KEY_LEFT]){
        player.pos_x -= increment;
    }

    if (keys_pressed[KEY_RIGHT]){
        player.pos_x += increment;
    }

    if (keys_pressed[KEY_DOWN]){
        player.pos_y += increment;
    }

    if (keys_pressed[KEY_UP]){
        player.pos_y -= increment;
    }
}

int bound_check_x(float x){
    return (x >= 0 && x < X_RES);
}

int bound_check_y(float y){
    return (y >= 0 && y < Y_RES);
}

//fast inverse square root
float Q_rsqrt( float number )
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can b
        return y;
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

    int y_in_range = kabs(b.pos_y - e.pos_y) < ENEMY_HEIGHT / 2.0 + BULLET_HEIGHT / 2.0;

    int x_in_range = kabs(b.pos_x - e.pos_x) < ENEMY_WIDTH / 2.0 + BULLET_WIDTH / 2.0;

    //return x_in_range;
    return (x_in_range && y_in_range);

}

int eb_player_intersect(Bullet b, Player p){

    int y_in_range = kabs(b.pos_y - p.pos_y) < 4 / 2.0 + BULLET_HEIGHT / 2.0;

    int x_in_range = kabs(b.pos_x - p.pos_x) < 4 / 2.0 + BULLET_WIDTH / 2.0;

    return (x_in_range && y_in_range);

}


void add_powerup(int);
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
                    add_powerup(j);
                } else {
                    GameArraySet(&enemy_arr, j - enemy_offset, e_gu);
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
        
        //powerup bullet movement
        if(b_gu.bullet.type == POWER_T && b_gu.bullet.vel_y < 2)
        {
            b_gu.bullet.vel_y += .01;
        }

        if (b_gu.bullet.type != POWER_T && 
            (!bound_check_x(b_gu.bullet.pos_x) || !bound_check_y(b_gu.bullet.pos_y))){
            offset++;
        } else if (eb_player_intersect(b_gu.bullet, player)){
            color_screen(BLACK);
            // If the bullet is a power up, the player gets a powerup
            if (b_gu.bullet.type == POWER_T){
                //pflag = b_gu.bullet.status;
                offset++;
            }
            else{
                // For now, just loop, game over
                dead = 1;
            }
        } else {
            GameArraySet(&enemy_bullet_arr, i - offset, b_gu);
        }

    }

    enemy_bullet_arr.size -= offset;
}

// See the Wikipedia article on
// linear congruential generators
unsigned int prev_internal_val;
unsigned int a = 1103515245;
unsigned int c = 12345;
unsigned int gen_rand(int range) {
    prev_internal_val = (a * prev_internal_val + c) % 0x80000000;
    return (prev_internal_val % range);
}


#define INVERSE_BULLET_CHANCE 73
#define INVERSE_TYPE_CHANCE 2

void add_powerup(int offset){
    GameUnion gu_cur_enemy = GameArrayGet(&enemy_arr, offset);
    // Don't add at all ticks - randomly select
    Bullet b;
    b.pos_x = gu_cur_enemy.enemy.pos_x;
    b.pos_y = gu_cur_enemy.enemy.pos_y;
    b.type = POWER_T;
    
    // See if we get red or blue flavor.
    unsigned int status = gen_rand(INVERSE_TYPE_CHANCE);
    if (!status){
        b.state = PRED;
    }
    else{
        b.state = PBLUE;
    }
    // Powerups just fall down for now.
    b.vel_x = 0.0;
    b.vel_y = -1.0;
    
    GameUnion gu_cur_bullet;
    gu_cur_bullet.bullet = b;
    
    GameArrayInsert(&enemy_bullet_arr, gu_cur_bullet);
}


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
            b.type = BULLET_T;

            b.state = 0;
            // boring velocity for now
            // find the direction of the player
            float dir_x = player.pos_x - b.pos_x;
            float dir_y = player.pos_y - b.pos_y;

            float normalization_factor = Q_rsqrt(dir_x * dir_x + dir_y * dir_y);
            b.vel_x = dir_x * normalization_factor;
            b.vel_y = dir_y * normalization_factor;
            
            GameUnion gu_cur_bullet;
            gu_cur_bullet.bullet = b;
            GameArrayInsert(&enemy_bullet_arr, gu_cur_bullet);
            //burst fire from shooter dudes
            if(gu_cur_enemy.enemy.type == 1)
            {
                b.vel_x -= 1.75;
                b.state = PRED;
                for(int j = 0; j < 6; j ++)
                {
                    gu_cur_bullet.bullet = b;
                    GameArrayInsert(&enemy_bullet_arr, gu_cur_bullet);
                    b.vel_x += .5;
                }
            }
        }
    }
}

void spawn_enemies()
{
    unsigned int testval = gen_rand(19);
    if(!testval)
    {
        Enemy test_enemy;
        test_enemy.pos_x = gen_rand(X_RES);
        test_enemy.pos_y = 0;
        test_enemy.type = gen_rand(111) > 90 ? 1 : 0;
        test_enemy.state = ((float)gen_rand(100) / 50.) - 1.0;

        GameUnion enemy_gu;
        enemy_gu.enemy = test_enemy;
        GameArrayInsert(&enemy_arr, enemy_gu);
    }
}

void update_enemy_positions()
{
    int i = 0;
    while (i < enemy_arr.size) {
        GameUnion enemy = GameArrayGet(&enemy_arr, i);
        //rear shooter enemies
        if(enemy.enemy.type == 1)
        {
            enemy.enemy.state++;
            if(enemy.enemy.pos_y < 50)
            {
                enemy.enemy.pos_y += 1;
            }
            
            if(enemy.enemy.state > 100)
            {
                enemy.enemy.pos_x += enemy.enemy.pos_x > 160 ? 1 : -1;
            }
        }
        else
        {
            enemy.enemy.pos_y += 0.8;
            enemy.enemy.state += ((float)gen_rand(100) / 200.) - 0.25;
            //now attract it to the ends
            if(enemy.enemy.pos_x > 160)
            {
                enemy.enemy.state += .01;
                enemy.enemy.state = enemy.enemy.state < 1.5 ? enemy.enemy.state : 1.5;
            }
            else
            {
                enemy.enemy.state -= .01;
                enemy.enemy.state = enemy.enemy.state > -1.5 ? enemy.enemy.state : -1.5;
            }
            enemy.enemy.pos_x += enemy.enemy.state;
        }
        GameArraySet(&enemy_arr, i, enemy);
        //remove oob enemies
        if(enemy.enemy.pos_x < 0 || enemy.enemy.pos_x > X_RES)
        {
            GameArrayDelete(&enemy_arr, i);
        }
        else
        {
            i++;
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
    
    //title screen goes here
    while (1) {
        int currentTime = ticks;
        
        if(keys_pressed[KEY_SPACE])
        {
            break;
        }

        //draw bg 
        draw_bg_screen();

        color_rect(BLUE, 110, 40, 100, 55);

        write_string_position(WHITE, hu, 150, 45);
        write_string_position(RED, "Embodiment of the OS", 120, 55);
        write_string_position(RED, "Press z to play", 125, 65);
        write_string_position(WHITE, "arrows move, z shoots", 120, 75);
        //update the screen
        update_screen();

        // 2 ticks per game loop
        // so approximately 30fps
        if(!sleep_until(currentTime + 3))
        {
            write_string(3, "you're lagging go faster");
            //this will make you lag even more but whatev
            update_screen();
        }
    }



    //now for the game

    init_entities();
    draw_entities();

    /* Loop forever, so that we don't fall back into the bootloader code. */
    while (1) {
        int currentTime = ticks;
        
        handle_keyboard();
        
        spawn_enemies();

        update_player_bullets();

        update_enemy_positions();

        add_enemy_bullets();

        update_enemy_bullets();

        draw_entities();

        //update the screen
        update_screen();

        // 2 ticks per game loop
        // so approximately 30fps
        if(!sleep_until(currentTime + 3))
        {
            write_string(3, "you're lagging go faster");
            //this will make you lag even more but whatev
            update_screen();
        }

        //game over code
        if(dead)
        {
            color_rect(BLUE, 110, 40, 100, 55);

            write_string_position(WHITE, "Game Over", 150, 45);
            write_string_position(RED, "Press z to restart", 125, 65);
            while(1)
            {
                if(keys_pressed[KEY_SPACE])
                {
                    break;
                }
                update_screen();
                sleep_until(currentTime + 3);
            }
            dead = 0;
            init_entities();
            draw_entities();
        }
    }
}

