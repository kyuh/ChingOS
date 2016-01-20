
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "handlers.h"
#include "keyboard.h"



char space[100000];


typedef char* ST_ARR_TYPE;


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


typedef union {
    Bullet b;
    Enemy e;
    int val;
} GameUnionType;

// Sized array of char arrays
typedef struct {
    GameUnionType *data;
    int size;
    int capacity;
} GameArray;

#if 0

StringArray createStringArray(int starting_cap){
    StringArray stArr;
    stArr.data = malloc(starting_cap * sizeof(ST_ARR_TYPE));
    stArr.size = 0;
    stArr.capacity = starting_cap;
    return stArr;
}

void stringArrayDeleteLast(StringArray *stArr){
    stArr->data[ stArr->size ] = NULL;      // optional
    stArr->size -= 1;
}

void stringArrayResize(StringArray *stArr){
    ST_ARR_TYPE *new_buffer = malloc(2 * stArr->capacity * sizeof(ST_ARR_TYPE));
    memcpy(new_buffer, stArr->data, stArr->capacity * sizeof(ST_ARR_TYPE));

    free(stArr->data);
    stArr->data = new_buffer;
    stArr->capacity *= 2;
}

void stringArrayInsert(StringArray *stArr, ST_ARR_TYPE elt){
    if (stArr->size == stArr->capacity){
        stringArrayResize(stArr);
    }
    stArr->data[stArr->size] = elt;
    stArr->size += 1;
}

ST_ARR_TYPE stringArrayGet(StringArray *stArr, size_t idx){
    return stArr->data[idx];
}

char **stringArrayToNormalPlusNull(StringArray *stArr){
    int n = stArr->size;
    char **arr = malloc((n+1) * sizeof(char*));
    for (int i = 0; i < n; i++){
        arr[i] = stringArrayGet(stArr, i);
    }
    arr[n] = NULL;
    return arr;
}

#endif





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

