#include "kero_math.h"
#include "kero_platform.h"
#include "kero_sprite.h"
#include <time.h>

ksprite_t frame_buffer;

int main(int argc, char* argv[]) {
    srand(time(0));
    
    KPInit(1280, 720, "Mazes Article");
    
    frame_buffer.pixels = kp_frame_buffer.pixels;
    frame_buffer.w = kp_frame_buffer.w;
    frame_buffer.h = kp_frame_buffer.h;
    
    // Start of article code
    
    typedef struct {
        int width, height;
        uint8_t* cells;
    } maze_t;
    
# define MAZE_UP 1
# define MAZE_RIGHT 2
# define MAZE_DOWN 4
# define MAZE_LEFT 8
# define NUM_DIRECTIONS 4
#define CELL_VISITED 128
    
    // Recursive Backtracker
#if 0
    // Preparation
    maze_t maze = {0};
    maze.width = maze.height = 10;
    maze.cells = (uint8_t*)calloc(sizeof(uint8_t), maze.width * maze.height);
    int* cell_stack = (int*)malloc(maze.width * maze.height * sizeof(int));
    int cell_stack_top = 0;
    
    // Step 1: Select a random point
    cell_stack[0] = rand() % maze.width*maze.height;
    maze.cells[cell_stack[0]] |= CELL_VISITED;
    
    // Step 2: Move and connect to a random unvisited neighbouring cell. Repeat until the current cell has no unvisited neighbours.
    do {
        int x = cell_stack[cell_stack_top]%maze.width;
        int y = cell_stack[cell_stack_top]/maze.width;
        // Draw the maze
        KSSetAllPixels(&frame_buffer, 0xffffffff);
        int size = 10;
        KSDrawRectFilled(&frame_buffer, x*size, y*size, (x+1)*size, (y+1)*size, 0xff00ff00);
        for(int y = 0; y < maze.height; ++y) {
            for(int x = 0; x < maze.width; ++x) {
                if( !(maze.cells[x + y*maze.width] & MAZE_UP) ) {
                    KSDrawLine(&frame_buffer, x*size, (y+1)*size, (x+1)*size, (y+1)*size, 0xff000000);
                }
                if( !(maze.cells[x + y*maze.width] & MAZE_RIGHT) ) {
                    KSDrawLine(&frame_buffer, (x+1)*size, y*size, (x+1)*size, (y+1)*size, 0xff000000);
                }
            }
        }
        KPFlip();
        
        while( (y < maze.height-1 && !(maze.cells[x + (y+1)*maze.width] & CELL_VISITED) ) || (y > 0 && !(maze.cells[x + (y-1)*maze.width] & CELL_VISITED) ) || (x < maze.width-1 && !(maze.cells[x+1 + y*maze.width] & CELL_VISITED) ) || (x > 0 && !(maze.cells[x-1 + y*maze.width] & CELL_VISITED) ) ) {
            int direction = rand();
            bool connected = false;
            for(int neighbour_checks = 0; neighbour_checks < NUM_DIRECTIONS && !connected; ++neighbour_checks) {
                direction = (direction+1)%4;
                switch(direction) {
                    
                    case 0: { // Up
                        if( y < maze.height-1 && !(maze.cells[x + (y+1)*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_UP;
                            ++y;
                            maze.cells[x + y*maze.width] |= MAZE_DOWN;
                            connected = true;
                        }
                    }break;
                    
                    case 1: { // Down
                        if( y > 0 && !(maze.cells[x + (y-1)*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_DOWN;
                            --y;
                            maze.cells[x + y*maze.width] |= MAZE_UP;
                            connected = true;
                        }
                    }break;
                    
                    case 2: { // Right
                        if( x < maze.width-1 && !(maze.cells[x+1 + y*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_RIGHT;
                            ++x;
                            maze.cells[x + y*maze.width] |= MAZE_LEFT;
                            connected = true;
                        }
                    }break;
                    
                    case 3: { // Left
                        if( x > 0 && !(maze.cells[x-1 + y*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_LEFT;
                            --x;
                            maze.cells[x + y*maze.width] |= MAZE_RIGHT;
                            connected = true;
                        }
                    }break;
                }
            }
            if(connected) {
                cell_stack[++cell_stack_top] = x + y*maze.width;
                maze.cells[x + y*maze.width] |= CELL_VISITED;
                // Draw the maze
                KSSetAllPixels(&frame_buffer, 0xffffffff);
                int size = 10;
                KSDrawRectFilled(&frame_buffer, x*size, y*size, (x+1)*size, (y+1)*size, 0xff00ff00);
                for(int y = 0; y < maze.height; ++y) {
                    for(int x = 0; x < maze.width; ++x) {
                        if( !(maze.cells[x + y*maze.width] & MAZE_UP) ) {
                            KSDrawLine(&frame_buffer, x*size, (y+1)*size, (x+1)*size, (y+1)*size, 0xff000000);
                        }
                        if( !(maze.cells[x + y*maze.width] & MAZE_RIGHT) ) {
                            KSDrawLine(&frame_buffer, (x+1)*size, y*size, (x+1)*size, (y+1)*size, 0xff000000);
                        }
                    }
                }
                KPFlip();
            }
        }
        
        // Step 3: Backtrack to each previously visited cell in order until one with an unvisited neighbour is found. Go to step 2.
        // Step 4: When you return to the first cell, every cell has been visited. End.
    } while(--cell_stack_top > 0);
    
    free(cell_stack);
    // End of Recursive Backtracker
    
#else
    
    // Prim's Algorithm
    
    // Preparation
    maze_t maze = {0};
    maze.width = maze.height = 10;
    maze.cells = (uint8_t*)calloc(sizeof(uint8_t), maze.width * maze.height);
    int* visited_cells = (int*)malloc(maze.width * maze.height * sizeof(int));
    int num_visited_cells = 1;
    
    // Step 1: Select a random point, mark as visited and add it to the list of visited cells.
    visited_cells[0] = rand() % (maze.width*maze.height);
    maze.cells[visited_cells[0]] |= CELL_VISITED;
    
    // Step 2: Until the list of visited cells is empty. . .
    while(num_visited_cells > 0) {
        
        // Step 3: Select a random cell from the list of visited cells.
        int selected = rand()%num_visited_cells;
        int cell = visited_cells[selected];
        int x = cell%maze.width;
        int y = cell/maze.width;
        
        // Step 4: If the current cell has no unvisited neighbours, remove it from the list. Go to (2)
        if(!(y < maze.height-1 && !(maze.cells[x + (y+1)*maze.width] & CELL_VISITED) ) && !(y > 0 && !(maze.cells[x + (y-1)*maze.width] & CELL_VISITED) ) && !(x < maze.width-1 && !(maze.cells[x+1 + y*maze.width] & CELL_VISITED) ) && !(x > 0 && !(maze.cells[x-1 + y*maze.width] & CELL_VISITED) )) {
            --num_visited_cells;
            for(int i = selected; i < num_visited_cells; ++i) {
                visited_cells[i] = visited_cells[i+1];
            }
        }
        // Step 5: Connect to a random unvisited neighbour of the current cell, mark that neighbour as visited and add it to the list. Go to (2)
        else {
            int direction = rand();
            bool connected = false;
            for(int neighbour_checks = 0; neighbour_checks < NUM_DIRECTIONS && !connected; ++neighbour_checks) {
                direction = (direction+1)%4;
                switch(direction) {
                    
                    case 0: { // Up
                        if( y < maze.height-1 && !(maze.cells[x + (y+1)*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_UP;
                            ++y;
                            maze.cells[x + y*maze.width] |= MAZE_DOWN;
                            connected = true;
                        }
                    }break;
                    
                    case 1: { // Down
                        if( y > 0 && !(maze.cells[x + (y-1)*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_DOWN;
                            --y;
                            maze.cells[x + y*maze.width] |= MAZE_UP;
                            connected = true;
                        }
                    }break;
                    
                    case 2: { // Right
                        if( x < maze.width-1 && !(maze.cells[x+1 + y*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_RIGHT;
                            ++x;
                            maze.cells[x + y*maze.width] |= MAZE_LEFT;
                            connected = true;
                        }
                    }break;
                    
                    case 3: { // Left
                        if( x > 0 && !(maze.cells[x-1 + y*maze.width] & CELL_VISITED) ) {
                            maze.cells[x + y*maze.width] |= MAZE_LEFT;
                            --x;
                            maze.cells[x + y*maze.width] |= MAZE_RIGHT;
                            connected = true;
                        }
                    }break;
                }
            }
            visited_cells[num_visited_cells++] = x + y*maze.width;
            maze.cells[x + y*maze.width] |= CELL_VISITED;
            
            // Draw the maze
            KSSetAllPixels(&frame_buffer, 0xffffffff);
            int size = 10;
            for(int i = 0; i < num_visited_cells; ++i) {
                int x = visited_cells[i]%maze.width;
                int y = visited_cells[i]/maze.width;
                KSDrawRectFilled(&frame_buffer, x*size, y*size, (x+1)*size, (y+1)*size, 0xff888888);
            }
            for(int y = 0; y < maze.height; ++y) {
                for(int x = 0; x < maze.width; ++x) {
                    if( !(maze.cells[x + y*maze.width] & MAZE_UP) ) {
                        KSDrawLine(&frame_buffer, x*size, (y+1)*size, (x+1)*size, (y+1)*size, 0xff000000);
                    }
                    if( !(maze.cells[x + y*maze.width] & MAZE_RIGHT) ) {
                        KSDrawLine(&frame_buffer, (x+1)*size, y*size, (x+1)*size, (y+1)*size, 0xff000000);
                    }
                }
            }
            KPFlip();
        }
    }
    
    free(visited_cells);
    // End of Prim's algorithm
    
#endif
    
    // Draw the maze
    KSSetAllPixels(&frame_buffer, 0xffffffff);
    int size = 10;
    for(int y = 0; y < maze.height; ++y) {
        for(int x = 0; x < maze.width; ++x) {
            if( !(maze.cells[x + y*maze.width] & MAZE_UP) ) {
                KSDrawLine(&frame_buffer, x*size, (y+1)*size, (x+1)*size, (y+1)*size, 0xff000000);
            }
            if( !(maze.cells[x + y*maze.width] & MAZE_RIGHT) ) {
                KSDrawLine(&frame_buffer, (x+1)*size, y*size, (x+1)*size, (y+1)*size, 0xff000000);
            }
        }
    }
    KPFlip();
    
    // End of article code
    
    bool game_running = true;
    while(game_running) {
        while(KPEventsQueued()) {
            kp_event_t* e = KPNextEvent();
            switch(e->type) {
                case KPEVENT_KEY_PRESS:{
                    switch(e->key) {
                        case KEY_ESCAPE:{
                            exit(0);
                        }break;
                    }
                }break;
                case KPEVENT_RESIZE:{
                    frame_buffer.pixels = kp_frame_buffer.pixels;
                    frame_buffer.w = kp_frame_buffer.w;
                    frame_buffer.h = kp_frame_buffer.h;
                }break;
                case KPEVENT_QUIT:{
                    exit(0);
                }break;
            }
            KPFreeEvent(e);
        }
        
        KPFlip();
    }
    
    return 0;
}