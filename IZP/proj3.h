/**
 * @file Proj3.h
 * @brief Maze header file
 * @author Lukas Bastyr, xbasty00
 * @date 12.20.2019
 */

#ifndef PROJ3_H
#define PROJ3_H

/**
 * Struct to save the map/maze details:
 *  - Map size
 *  - Individual cells containg its border info
 */
typedef struct {
    int rows; /**<Number of rows*/
    int cols; /**<Number of columms*/
    unsigned char *cells; /**<Array to save the cells*/
} Map;

/** Bit value of idnividual borders */
enum borders { BLEFT/**<Left border, first bit*/=0x1, BRIGHT/**<Right border, second bit*/=0x2,
               BTOP/**<Top border, third bit*/=0x4, BBOTTOM/**<Bottom border, same as BTOP*/=0x4 };

/**
 * @brief Frees the map from memory
 * @pre Loaded map or NULL pointer
 * @param map - Pointer to a map struct
 */
void free_map(Map *map);

/**
 * @brief Loads the map into the memory
 * @pre '--test' or path arguments
 * @post Map is loaded in memory
 * @param filename - Pointer to a name of the map/maze file
 * @param map - Pointer to a map struct
 * @return 0 on success
 */
int load_map(const char *filename, Map *map);

/**
 * @brief Checks whether a cell's border is a wall or not
 * @details Every cell is a triangle. And depending on it's orientation, it has left,right and top or bottom borders.
 * A number represents each individual cell. Depeding on its first 3 bit values we can determinate, which borders are walls.<br>
 * 001 (first bit)  = left border is a wall<br>
 * 010 (second bit) = right border is a wall<br>
 * 100 (third bit)  = top/bottom border is a wall<br>
 * @pre Loaded and checked map
 * @param map - Pointer to a map struct
 * @param r - Current cell row
 * @param c - Current cell columm
 * @param border - which border to check
 * @return true if the border is a wall
 * @return false otherwise
 */
bool isborder(Map *map, int r, int c, int border);

/**
 * @brief Checks if current cell has a top or bottom border
 * @details It has been determinated, that correct mazes start with first cell [1,1] that has top border.
 * We can determinate from that every cell on even position has a top border. If cell position is odd, it has a bottom border.<br>
 * [1,1] -> (1+1) = 0 - even <br>
 * [2,3] -> (2+3) = 5 - odd <br>
 * @pre Loaded map
 * @param r - Current cell row
 * @param c - Current cell columm
 * @return true if the cell has a bottom border
 * @return false otherwise
 */
bool hasbottom(int r, int c);

/**
 * @brief Which border should we start on
 * @details When using right hand rule:
 *  - Right when entering from left on odd row.
 *  - Bottom when entering from left on even row.
 *  - Left when entering from top of the maze.
 *  - Right when entering from bottom of the maze.
 *  - Top when entering from right if cell has a top border.
 *  - Left when entering from right if cell has a bottom border.
 * @pre Loaded and checked map
 * @param map - Pointer to a map struct
 * @param r - enter cell row
 * @param c - enter cell columm
 * @param leftright - path identifier
 * @return int identifying which border we should start on
 */
int start_border(Map *map, int r, int c, int leftright);

/**
 * @brief Checks for map/maze integrity
 * @details Checks if neighboring cells have same border type (wall or empty), if maze is 2D (as it should be),
 * if it has a correct size and if it contains numbers only.
 * @pre Loaded map
 * @post Map is checked
 * @param map - Pointer to a map struct
 * @return 0 on success
 */
int check_map(Map *map);

/**
 * @brief Calls load and check functions
 * @post Map is loaded and checked
 * @param filename - Pointer to a name of the map/maze file 
 * @param map - Pointer to a map struct
 * @return 0 on success
 */
int load_and_check_map(const char *filename, Map *map);

/**
 * @brief Checks if we are out of bounds
 * @pre Loeaded and checked map
 * @param map - Pointer to a map struct
 * @param r - Current cell row
 * @param c - Current cell columm
 * @return true when out of bounds
 * @return false otherwise
 */
bool is_out(Map *map, int r, int c);

/**
 * @brief Prints current location
 * @pre Loaded and checked map
 * @param map - Pointer to a map struct
 * @param r - Current cell row
 * @param c - Current cell columm
 * @param leftright - path identifier
 */
void print_path(Map *map, int r, int c, int leftright);

#endif
