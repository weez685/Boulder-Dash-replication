
// cs_caverun.c
// Written by Jonathan Sun (z5689744) on 11/03/2025
//
// Description: <INSERT DESCRIPTION OF PROGRAM>

// Provided Libraries
#include <stdio.h>

// Add your own #include statements below this line
#include <math.h>

// Provided constants
#define COLS 10
#define ROWS 10
#define INVALID_ROW -1
#define INVALID_COL -1
#define INITIAL_LIVES 3

// Add your own #defines constants below this line
#define ZERO_LIVES 0

// Provided Enums
// Enum for features on the game board
enum entity {
    EMPTY,
    DIRT,
    WALL,
    BOULDER,
    GEM,
    EXIT_LOCKED,
    EXIT_UNLOCKED,
    HIDDEN,
    LAVA
};

// Add your own enums below this line

// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
};

// Add your own structs below this line
struct player {
    int row;
    int col;
    int points;
    int game_running;
    int dash_cooldown;
    int lives;
    int respawn_row;
    int respawn_col;
    int respawn_blocked;
    int illumination_on;
    int illumination_radius;
    char gravity_direction;
};
struct boulder {
    int row;
    int col;
};
// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col,
    int lives_remaining
);
void print_board_line(void);
void print_board_header(int lives);
void print_map_statistics(
    int number_of_dirt_tiles,
    int number_of_gem_tiles,
    int number_of_boulder_tiles,
    double completion_percentage,
    int maximum_points_remaining
);

// Add your function prototypes below this line
void print_welcome();
int invalid_spawn(int row, int col);
void map_features(
    char entity, 
    int feat_row, 
    int feat_col,
    struct player player,
    struct tile board[ROWS][COLS]
);
void print_map_features(
    int entity,
    int feat_row,
    int feat_col,
    struct tile board[ROWS][COLS]
);
void multiple_walls(
    int feat_row,
    int feat_col,
    int end_row,
    int end_col,
    struct player player,
    struct tile board[ROWS][COLS]
);
int wall_check(
    int start_row,
    int start_col,
    int end_row,
    int end_col,
    struct tile board[ROWS][COLS]
);
void print_multiple_walls(
    int start_row,
    int start_col,
    int end_row,
    int end_col,
    struct tile board[ROWS][COLS]
);
void gameplay(struct player player, struct tile board[ROWS][COLS]);

struct player movement(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]
);
struct player single_move(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]
);
struct player dash_move(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]
);
int dig_tile(
    struct player,
    struct tile board[ROWS][COLS]
);
void count_map_features(struct tile board[ROWS][COLS]);

struct player boulder_turn(
    struct tile board[ROWS][COLS],
    struct player player
);
struct player check_collisions(
    struct tile board[ROWS][COLS],
    struct player player
);

void map_statistics(
    int player_row, 
    int player_col, 
    int intial_collectible, 
    struct tile board[ROWS][COLS]
);
int gems_remaining(struct tile board[ROWS][COLS]);

void unlock_exits(struct tile board[ROWS][COLS]);

void check_exits(struct tile board[ROWS][COLS]);

struct player check_win(struct player player, struct tile board[ROWS][COLS]);

char lowercase(char command);

void initialise_illumination_board(struct tile hidden_board[ROWS][COLS]);

struct player illumination_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]
);
void update_hidden_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]
);
struct player gravity_shift(struct player player);

struct player scan_down_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player
);
struct player down_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder
);

struct player scan_up_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player
);
struct player up_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder
);

struct player scan_left_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player
);
struct player left_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder
);

struct player scan_right_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player
);
struct player right_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder
);
void print_correct_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]
);

// Provided sample main() function (you will need to modify this)
int main(void) {
    print_welcome();
    struct tile board[ROWS][COLS];
    initialise_board(board);
    //Create loop to validate players spawn
    struct player player; 
    int i = 1;
    while (i == 1) {
        printf("Enter the player's starting position: ");
        scanf("%d %d", &player.row , &player.col);
        if (invalid_spawn(player.row, player.col)) {
            printf("Position %d %d is invalid!\n", player.row , player.col);
        } else {
            i = 0;
        }
    }
    print_board(board, player.row , player.col , INITIAL_LIVES);
    //Create loop to scan map features and validate
    printf("Enter map features:\n");
    int feat_row, feat_col, end_row, end_col; 
    char entity;
    while (scanf(" %c", &entity) == 1 && entity != 's') {
        if (entity == 'W') {
            scanf("%d %d %d %d", &feat_row, &feat_col, &end_row, &end_col);
            multiple_walls(
            feat_row, feat_col, end_row, end_col, player, board
            );
        } else {
            scanf ("%d %d", &feat_row, &feat_col);  
            map_features(
            entity, feat_row, feat_col, player, board
            );     
        }
    }
    check_exits(board);
    print_board(board, player.row, player.col, INITIAL_LIVES);
    board[player.row][player.col].entity = EMPTY;
    printf("--- Gameplay Phase ---\n");
    gameplay(player, board);
 
    return 0;
}

// Add your function definitions below this line
// Function to validate player spawn
void print_welcome() {
    printf("Welcome to CS Caverun!\n\n");
    printf("--- Game Setup Phase ---\n");
}
int invalid_spawn(int row, int col) {
    return (row < 0 || row >= ROWS || col < 0 || col >= COLS);
}
// Validate map features
void map_features(
    char entity, 
    int feat_row, 
    int feat_col,
    struct player player,
    struct tile board[ROWS][COLS]) {
    if (invalid_spawn(feat_row, feat_col)) {
        printf("Invalid location: position is not on map!\n");
    } else if (
    (feat_row == player.row && feat_col == player.col) || 
    (board[feat_row][feat_col].entity != DIRT)) {
        printf("Invalid location: tile is occupied!\n");
    } else {
        print_map_features(entity, feat_row, feat_col, board);
    }
}
void print_map_features(
    int entity,
    int feat_row,
    int feat_col,
    struct tile board[ROWS][COLS]) {
    // Scan characters to print map features
    if (entity == 'w') {
        board[feat_row][feat_col].entity = WALL;
    } else if (entity == 'b') {
        board[feat_row][feat_col].entity = BOULDER;
    } else if (entity == 'g') {
        board[feat_row][feat_col].entity = GEM;
    } else if (entity == 'e') {
        board[feat_row][feat_col].entity = EXIT_LOCKED;
    }
} 
//Function for when multiple walls want to be printed
void multiple_walls(
    int feat_row,
    int feat_col,
    int end_row,
    int end_col,
    struct player player,
    struct tile board[ROWS][COLS]) {

    // Check if walls are out of bounds
    if (invalid_spawn(feat_row, feat_col) || invalid_spawn(end_row, end_col)) {
        printf("Invalid location: feature cannot be placed here!\n");
        return;
    }
    // Check if wall overlaps with player
    if (player.row >= feat_row && player.row <= end_row &&
        player.col >= feat_col && player.col <= end_col) {
        printf("Invalid location: feature cannot be placed here!\n");
        return;
    }
    // Check that all tiles within the range are dirt
    if (wall_check(feat_row, feat_col, end_row, end_col, board) == 0) {
        printf("Invalid location: feature cannot be placed here!\n");
        return;
    }
    // Place walls
    print_multiple_walls(feat_row, feat_col, end_row, end_col, board);
}

// Ensure that the walls can be placed only in tiles that are dirt
int wall_check(
    int start_row,
    int start_col,
    int end_row,
    int end_col,
    struct tile board[ROWS][COLS]) {

    for (int row = start_row; row <= end_row; row++) {
        for (int col = start_col; col <= end_col; col++) {
            if (board[row][col].entity != DIRT) {
                return 0;
            } 
        }
    } 
    return 1;
}
void print_multiple_walls(
    int start_row,
    int start_col,
    int end_row,
    int end_col,
    struct tile board[ROWS][COLS]) {

    for (int row = start_row; row <= end_row; row++) {
        for (int col = start_col; col <= end_col; col++) {
            board[row][col].entity = WALL;
        }
    }
}
// Function to create gameplay loop
void gameplay(struct player player, struct tile board[ROWS][COLS]) {
    char command;
    player.points = 0;
    player.game_running = 1;
    int old_row = player.row;
    int old_col = player.col;
    player.dash_cooldown = 0;
    player.respawn_row = player.row;
    player.respawn_col = player.col;
    player.lives = INITIAL_LIVES;
    player.respawn_blocked = 0;
    player.illumination_on = 0;
    player.illumination_radius = 0;
    player.gravity_direction = 's';
    struct tile hidden_board[ROWS][COLS];

    initialise_illumination_board(hidden_board);
    // Handle non-player-movement commands
    while (player.game_running == 1) {
        scanf(" %c", &command);
        if (command == 'g') {
            player = gravity_shift(player);
            player = boulder_turn(board, player);
            print_correct_board(player, board, hidden_board);
        } else if (command == 'i') {
            player = illumination_board(player, board, hidden_board);
        } else if (command == 'q') {
            printf("--- Quitting Game ---\n");
            player.game_running = 0;
        } else if (command == 'p') {
            printf("You have %d point(s)!\n", player.points);
        } else if (command == 'm') {
            count_map_features(board);
        } else if (command == 'r') {
            player.dash_cooldown = 0;
            player = boulder_turn(board, player);
            print_correct_board(player, board, hidden_board); 
        } else {
            // Scan movement 
            player = movement(command, player, board);
            // If position has changed: update points, check gems, reprint board
            if (player.row != old_row || player.col != old_col) {  
                player = boulder_turn(board, player);
                print_correct_board(player, board, hidden_board);
            } else {
                print_board(board, player.row, player.col, player.lives);
            }
        }  
        player = check_win(player, board);
    }
} 
// Check for illumination and print correct board
void print_correct_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]) {
    if (player.illumination_on == 1) {
        update_hidden_board (player, board, hidden_board);
        print_board(hidden_board, player.row, player.col, player.lives);
    } else {
        print_board(board, player.row, player.col, player.lives);
    } 
}
// Determine shift direction and pass into play struct
struct player gravity_shift(struct player player) {
    char shift_direction;
    scanf(" %c", &shift_direction);
    if (shift_direction == 'w') {
        printf("Gravity now pulls UP!\n");
    } else if (shift_direction == 'a') {
        printf("Gravity now pulls LEFT!\n");
    } else if (shift_direction == 's') {
        printf("Gravity now pulls DOWN!\n");
    } else if (shift_direction == 'd') {
        printf("Gravity now pulls RIGHT!\n");
    }
    player.gravity_direction = shift_direction;
    return player;
}
// Initialise illumination board
void initialise_illumination_board(struct tile illumination_board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            illumination_board[row][col].entity = HIDDEN;
        }
    }
}   
// update board when illumination is activated/deactivated
struct player illumination_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]) {
    int radius;
    scanf("%d", &radius);
    if (radius > 0) {
        printf("Illumination Mode: Activated\n");
        player.illumination_on = 1;
        player.illumination_radius = radius;
        update_hidden_board(player, board, hidden_board);
        print_board(hidden_board, player.row, player.col, player.lives);
    } else {
        printf("Illumination Mode: Deactivated\n");
        print_board(board, player.row, player.col, player.lives);
        player.illumination_on = 0;
    }
    return player;
}
// create illumination effect
void update_hidden_board(
    struct player player,
    struct tile board[ROWS][COLS],
    struct tile hidden_board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // caculate distance
            double x = row - player.row;
            double y = col - player.col;
            double distance = sqrt(x*x + y*y);
            // Ensure that only radius around player illuminates
            if (distance <= player.illumination_radius) {
                // Show illuminated parts of board
                hidden_board[row][col].entity = board[row][col].entity;
            } else {
                hidden_board[row][col].entity = HIDDEN;
            }
        }
    }
}
void count_map_features(struct tile board[ROWS][COLS]) {
    // declare and initialise variables for map stats
    int collectible_dirt = 0;
    int collectible_gem = 0; 
    int num_boulders = 0;
    int maximum_points_remaining = 0;
    int num_empty = 0;
    double num_collectible = 0.0;
    double completion_percentage = 0.0;
    // for loop to scan the entire board for collectible features and boulders 
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == DIRT) {
                collectible_dirt++;
            } else if (board[row][col].entity == GEM) {
                collectible_gem++;
            } else if (board[row][col].entity == BOULDER) {
                num_boulders++;
            } else if (board[row][col].entity == EMPTY) {
                num_empty++;
            }
        }
    }
    // Calculate number of things initially collectible exluding player spawn
    num_collectible = collectible_dirt + collectible_gem + num_empty - 1;
    // Calculate percentage 
    completion_percentage = (num_empty - 1) * 100 / num_collectible;
    // Calculate max points remaining
    maximum_points_remaining = collectible_dirt + collectible_gem * 20;
    print_map_statistics(
        collectible_dirt,
        collectible_gem,
        num_boulders,
        completion_percentage,
        maximum_points_remaining);
}
// Scan movement for single/dash
struct player movement(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]) {
    if (command == 'W' ||
        command == 'A' ||
        command == 'S' ||
        command == 'D') {
        player = dash_move(command, player, board);
    } else {
        player = single_move(command, player, board);
        player.dash_cooldown = 0;
    }
    return player;
}
// Handle dash movements
struct player dash_move(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]) {
    // Scan second command for dash move and set cooldown
    char command2;
    scanf(" %c", &command2);
    if (player.dash_cooldown == 0) {
        command = lowercase(command);
        int old_row = player.row;
        int old_col = player.col;
        player = single_move (command, player, board);
        if (player.row == old_row && player.col == old_col) {
            player.dash_cooldown = 1;
            return player;
        }
        if (board[player.row][player.col].entity == EXIT_UNLOCKED) {
            return player;

        } else {
            command2 = lowercase(command2);
            player = single_move (command2, player, board);
            player.dash_cooldown = 1;
            return player;
        }
    } else {
        // If dash was on cooldown, reset it
        printf("You're out of breath! Skipping dash move...\n");
        player.dash_cooldown = 0;
    }
    return player;
}
// Handle single movements
struct player single_move(
    char command,
    struct player player,
    struct tile board[ROWS][COLS]) {
    int new_row = player.row;
    int new_col = player.col;
    if (command == 'w') {
        new_row--;
    } else if (command == 'a') {
        new_col--;
    } else if (command == 's') {
        new_row++;
    } else if (command == 'd') {
        new_col++;
    } 
    // Check if new position is valid if not reprint board
    if (invalid_spawn(new_row, new_col) ||
        board[new_row][new_col].entity == WALL ||
        board[new_row][new_col].entity == BOULDER || 
        board[new_row][new_col].entity == EXIT_LOCKED) {
        return player;
    } else {
        player.row = new_row;
        player.col = new_col;
        player.points = dig_tile(player, board);
        return player;
    }
}
// Function to make capital letters lowercase for dashing
char lowercase(char command){
    command = command + 'a' - 'A';
    return command;
}
int dig_tile(
    struct player player,
    struct tile board[ROWS][COLS]) {
    // Change dug out tiles to empty and calculate points earned
    if (board[player.row][player.col].entity == DIRT) {
        board[player.row][player.col].entity = EMPTY;
        player.points = player.points + 1;
    } else if (board[player.row][player.col].entity == GEM) {
        board[player.row][player.col].entity = EMPTY;
        player.points = player.points + 20;
    }
    // Check to see if there are gems remaining
    if (gems_remaining(board) == 0) {
        // Add function to unlock locked exits
        unlock_exits(board);
    }
    return player.points;
}
struct player boulder_turn(
    struct tile board[ROWS][COLS],
    struct player player) {
    //Scan board depending on direction of gravity
    if (player.gravity_direction == 's') {
        player = scan_down_gravity_board(board, player);
    } else if (player.gravity_direction == 'w') { 
        player = scan_up_gravity_board(board, player);    
    } else if (player.gravity_direction == 'a') { 
        player = scan_left_gravity_board(board, player);     
    } else if (player.gravity_direction == 'd') { 
        player = scan_right_gravity_board(board, player);  
    }
    return player;
}
struct player scan_down_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player) {
    struct boulder boulder;
    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == BOULDER) {
                boulder.row = row;
                boulder.col = col;
                player = down_gravity(board, player, boulder);
            }
        }
    }
    return player;
}
struct player down_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder) {
    if (boulder.row < ROWS - 1 &&
        board[boulder.row + 1][boulder.col].entity == EMPTY) {
        board[boulder.row][boulder.col].entity = EMPTY;
        board[boulder.row + 1][boulder.col].entity = BOULDER;
    } 
    if (boulder.row + 1 == player.row && boulder.col == player.col
        &&  board[boulder.row + 1][boulder.col].entity != EXIT_UNLOCKED) {
        player.lives--;
        player = check_collisions(board, player);
    }
    return player;
}

struct player scan_up_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player) {
    struct boulder boulder;     
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == BOULDER) {
                boulder.row = row;
                boulder.col = col;
                player = up_gravity(board, player, boulder);
            }
        }
    }
    return player;
}
struct player up_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder) { 
    if (boulder.row > 0 &&
        board[boulder.row - 1][boulder.col].entity == EMPTY) {
        board[boulder.row][boulder.col].entity = EMPTY;
        board[boulder.row - 1][boulder.col].entity = BOULDER;
    }    
    if (boulder.row - 1 == player.row && boulder.col == player.col
        && board[boulder.row - 1][boulder.col].entity != EXIT_UNLOCKED) {
        player.lives--;
        player = check_collisions(board, player);
    }
    return player;
}

struct player scan_left_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player) {
    struct boulder boulder;
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            if (board[row][col].entity == BOULDER) {
                boulder.row = row;
                boulder.col = col;
                player = left_gravity(board, player, boulder);
            }
        }
    }
    return player;
}
struct player left_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder) { 
    if (boulder.col > 0 &&
        board[boulder.row][boulder.col - 1].entity == EMPTY) {
        board[boulder.row][boulder.col].entity = EMPTY;
        board[boulder.row][boulder.col - 1].entity = BOULDER;
    }    
    if (boulder.row == player.row && boulder.col - 1 == player.col
        && board[boulder.row][boulder.col - 1].entity != EXIT_UNLOCKED) {
        player.lives--;
        player = check_collisions(board, player);
    }
    return player;
}

struct player scan_right_gravity_board(
    struct tile board[ROWS][COLS],
    struct player player) {
    struct boulder boulder;
    for (int col = COLS - 1; col >= 0; col--) {
        for (int row = 0; row < ROWS; row++) {
            if (board[row][col].entity == BOULDER) {
                boulder.row = row;
                boulder.col = col;
                player = right_gravity(board, player, boulder);
            }
        }
    }
    return player;
}
struct player right_gravity(
    struct tile board[ROWS][COLS],
    struct player player,
    struct boulder boulder) {    
    if (boulder.col < COLS - 1 &&
        board[boulder.row][boulder.col + 1].entity == EMPTY) {
        board[boulder.row][boulder.col].entity = EMPTY;
        board[boulder.row][boulder.col + 1].entity = BOULDER;
    }    
    if (boulder.row == player.row && boulder.col + 1 == player.col
        && board[boulder.row][boulder.col + 1].entity != EXIT_UNLOCKED) {
        player.lives--;
        player = check_collisions(board, player);
    }
    return player;
}
// Check conditions of boulders with players
struct player check_collisions(
    struct tile board[ROWS][COLS],
    struct player player) {
    if (board[player.respawn_row][player.respawn_col].entity == EMPTY) {
        if (player.lives > ZERO_LIVES) {
            player.row = player.respawn_row;
            player.col = player.respawn_col; 
            player.respawn_blocked = 0;   
            printf("Respawning!\n");
            return player;
        }
        if (player.lives == ZERO_LIVES) {
            printf("Game Lost! You scored %d points!\n", player.points);
            player.game_running = 0;
            player.respawn_blocked = 1;
            return player;
        }
    }
    if (board[player.respawn_row][player.respawn_col].entity == BOULDER) {
        printf("Respawn blocked! Game over. Final score: %d points.\n", 
                player.points);
        player.game_running = 0;
        player.respawn_blocked = 1;
        return player;
    } 
    return player;
}
// If no more gems, unlock exits
void check_exits(struct tile board[ROWS][COLS]) {
    int gems = 0;
    // Check for gems
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == GEM) {
                gems = 1;
            }
        }
    }
    // If no gems were found, unlock all locked exits
    if (gems == 0) {
        unlock_exits(board);
    }
    return;
}
// Check if there are gems remaining
int gems_remaining (struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == GEM) {
                return 1;
            }
        }
    }
    return 0;
}
void unlock_exits(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == EXIT_LOCKED) {
                board[row][col].entity = EXIT_UNLOCKED;
            } 
        }
    }
}
// Check for win
struct player check_win(struct player player, struct tile board[ROWS][COLS]) {
    if (player.lives > ZERO_LIVES) {
        if (player.respawn_blocked == 0) {
            if (board[player.row][player.col].entity == EXIT_UNLOCKED) {
                printf("You Win! Final Score: %d point(s)!\n", player.points);
                player.game_running = 0;
                return player;
            }
        }
    }
    return player;
}
// =============================================================================
// Definitions of Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tile entities to DIRT.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = DIRT;
        }
    }
}

// Prints the game board, showing the player's position and lives remaining
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col,
    int lives_remaining

    
) {
    print_board_line();
    print_board_header(lives_remaining);
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            printf("|");
            if (row == player_row && col == player_col) {
                printf("^_^");
            } else if (board[row][col].entity == EMPTY) {
                printf("   ");
            } else if (board[row][col].entity == DIRT) {
                printf(" . ");
            } else if (board[row][col].entity == WALL) {
                printf("|||");
            } else if (board[row][col].entity == BOULDER) {
                printf("(O)");
            } else if (board[row][col].entity == GEM) {
                printf("*^*");
            } else if (board[row][col].entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (board[row][col].entity == EXIT_UNLOCKED) {
                printf("[ ]");
            } else if (board[row][col].entity == HIDDEN) {
                printf(" X ");
            } else if (board[row][col].entity == LAVA) {
                printf("^^^");
            } else {
                printf("   ");
            }
        }
        printf("|\n");
        print_board_line();
    }
    printf("\n");
    return;
}

// Helper function for print_board(). You will not need to call this.
void print_board_header(int lives) {
    printf("| Lives: %d    C A V E R U N             |\n", lives);
}

// Helper function for print_board(). You will not need to call this.
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Prints game statistics: tile types, completion %, and points remaining.
void print_map_statistics(
    int number_of_dirt_tiles,
    int number_of_gem_tiles,
    int number_of_boulder_tiles,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("========= Map Statistics =========\n");
    printf("Tiles Remaining by Type:\n");
    printf("  - DIRT:      %d\n", number_of_dirt_tiles);
    printf("  - GEMS:      %d\n", number_of_gem_tiles);
    printf("  - BOULDERS:  %d\n", number_of_boulder_tiles);
    printf("Completion Status:\n");
    printf("  - Collectible Completion: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("==================================\n");
}