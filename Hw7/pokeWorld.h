#ifndef POKEWORLD_H
#define POKEWORLD_H

#include "heap.h"
#include "parseFiles.h"
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define WORLD_DIM 399
#define MAP_Y 21
#define MAP_X 80
#define NUM_FOREST 1
#define NUM_MOUNTAIN 1
#define NUM_GRASS_REGIONS 5

enum terrainType
{

    ter_grassTall,
    ter_clearing,
    ter_rock,
    ter_border,
    ter_road,
    ter_exit,
    ter_tree,
    ter_pokeMart,
    ter_pokeCenter,
    ter_character,
    ter_mountain,
    ter_forest,

};

enum direction
{
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NONE,
};

enum charType
{
    char_pc,
    char_hiker,
    char_rival,
    char_pacer,
    char_wanderer,
    char_stationary,
    char_randWalker,
};

// Cell for a distance map
class distanceMapCell
{
public:
    int cost;
    heap_node_t *hn;
    int xPos, yPos;
};

class distanceMap
{
public:
    distanceMapCell distMap[MAP_Y][MAP_X];
    charType npcType;
};

class character
{
public:
    int xPos, yPos;
    int cost;
    heap_node_t *hn;
    charType characterType;
    terrainType currTerrain;
    direction dir;
    bool beenDefeated;
};

// Terrain Map
class map
{
public:
    terrainType map[MAP_Y][MAP_X];
    int northExit;
    int southExit;
    int westExit;
    int eastExit;
    character *characterArray;
};

class gameWorld
{
public:
    int xPos, yPos;
    int prevXPos, prevYPos;
    map *mapArray[WORLD_DIM][WORLD_DIM];
    distanceMap hikerMap;
    distanceMap rivalMap;
};
#endif