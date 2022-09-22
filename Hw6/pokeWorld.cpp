#include <stdio.h>
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

#include "heap.h"
#include "pokeWorld.h"
#include "parseFiles.h"

/*
    Global Variables
*/
gameWorld world;
character player;
bool isFirstMap = true;
int numTrainers = 0;

int userMove(char userInput);
bool validateWorldMove(int x, int y);
bool isMoveInsideMap(int x, int y);
void moveWorld(char userInput);

int32_t distMap_cmp(const void *key, const void *with)
{
    return ((distanceMapCell *)key)->cost - ((distanceMapCell *)with)->cost;
}

int32_t characterCost_cmp(const void *key, const void *with)
{
    return ((character *)key)->cost - ((character *)with)->cost;
}

std::string checkDirectory()
{
    std::string path = "/share/cs327/pokedex/";
    std::ifstream file(path);

    if (file.good())
    {
        return path;
    }

    path = getenv("HOME");
    path.append("/.poke327/pokedex/");

    std::ifstream file2(path);
    if (file2.good())
    {
        return path;
    }
    return "false";
}

direction randDirection()
{
    return (enum direction)(rand() % 4);
}

bool other_NPC_validTerrain(terrainType ter)
{
    return (ter == ter_grassTall || ter == ter_clearing || ter == ter_road);
}

// Function to determine terrain cell cost
int pathCost(terrainType terrain, charType npcTag)
{
    switch (npcTag)
    {
    case char_hiker:
        switch (terrain)
        {
        case ter_rock:
        case ter_tree:
        case ter_grassTall:
        case ter_forest:
        case ter_mountain:
            return 15;
        case ter_road:
        case ter_clearing:
            return 10;
        default:
            return INT_MAX;
        }

    case char_rival:
        switch (terrain)
        {
        case ter_road:
        case ter_clearing:
            return 10;
        case ter_grassTall:
            return 20;
        default:
            return INT_MAX;
        }
    case char_pc:
        switch (terrain)
        {
        case ter_road:
        case ter_clearing:
        case ter_pokeCenter:
        case ter_pokeMart:
        case ter_exit:
            return 10;
        case ter_grassTall:
            return 20;
        default:
            return INT_MAX;
        }
    case char_wanderer:
    case char_pacer:
    case char_randWalker:
    case char_stationary:
        switch (terrain)
        {
        case ter_road:
        case ter_clearing:
            return 10;
        case ter_grassTall:
            return 20;
        default:
            return INT_MAX;
        }
    default:
        return INT_MAX;
    }
}

void dijkstra(distanceMap *distMap, map *terrainMap)
{

    int x, y;
    int xNeighbor[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int yNeighbor[] = {1, 1, 0, -1, -1, -1, 0, 1};
    heap_t h;
    distanceMapCell *d;

    // Initializes the X and Y positions
    for (y = 0; y < MAP_Y; y++)
    {
        for (x = 0; x < MAP_X; x++)
        {
            distMap->distMap[y][x].yPos = y;
            distMap->distMap[y][x].xPos = x;
        }
    }

    // Initalizes all nodes to cost infinity
    for (y = 0; y < MAP_Y; y++)
    {
        for (x = 0; x < MAP_X; x++)
        {
            distMap->distMap[y][x].cost = INT_MAX;
        }
    }

    // Source node based on player position set to cost 0
    distMap->distMap[player.yPos][player.xPos].cost = 0;

    heap_init(&h, distMap_cmp, NULL);

    // Insert all nodes to a min heap besides borders
    for (y = 1; y < MAP_Y - 1; y++)
    {
        for (x = 1; x < MAP_X - 1; x++)
        {
            distMap->distMap[y][x].hn = heap_insert(&h, &distMap->distMap[y][x]);
        }
    }

    while ((d = (distanceMapCell *)heap_remove_min(&h)))
    {
        d->hn = NULL;

        if (d->cost == INT_MAX)
        {
            continue;
        }

        for (int i = 0; i < 8; i++)
        {
            if (distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].hn)
            {
                if (pathCost(terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]], distMap->npcType) != INT_MAX)
                {
                    if (distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].cost >
                        d->cost + pathCost(terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]], distMap->npcType))
                    {
                        int newCost = d->cost + pathCost((terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]]),
                                                         distMap->npcType);
                        distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].cost = newCost;
                        heap_decrease_key_no_replace(&h, distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].hn);
                    }
                }
            }
        }
    }

    // Print distnce map
    /*for (y = 0; y < MAP_Y; y++)
    {
      for (x = 0; x < MAP_X; x++)
      {
        if (distMap->distMap[y][x].cost != INT_MAX)
        {
          printf("%02d", distMap->distMap[y][x].cost % 100);
        }
        else
        {
          printf("  ");
        }
        printf(" ");
      }
      printf("\n");
    }
    */
    heap_delete(&h);
}

void printMap(map *terMap)
{

    int x, y, i;

    for (y = 0; y < MAP_Y; y++)
    {
        for (x = 0; x < MAP_X; x++)
        {
            switch (terMap->map[y][x])
            {
            case ter_road:
            case ter_exit:
                mvaddch(y + 1, x, '#');
                break;
            case ter_rock:
            case ter_border:
            case ter_mountain:
                mvaddch(y + 1, x, '%');
                break;
            case ter_grassTall:
                mvaddch(y + 1, x, ':');
                break;
            case ter_tree:
            case ter_forest:
                mvaddch(y + 1, x, '\"');
                break;
            case ter_clearing:
                mvaddch(y + 1, x, '.');
                break;
            case ter_pokeCenter:
                mvaddch(y + 1, x, 'C');
                break;
            case ter_pokeMart:
                mvaddch(y + 1, x, 'M');
                break;
            case ter_character:

                for (i = 0; i < numTrainers + 1; i++)
                {
                    // Finds the matching character Type from our character Array that matches with the current map
                    if (terMap->characterArray[i].xPos == x && terMap->characterArray[i].yPos == y)
                    {
                        break;
                    }
                }
                switch (terMap->characterArray[i].characterType)
                {
                case char_pc:
                    mvaddch(y + 1, x, '@');
                    break;
                case char_hiker:
                    mvaddch(y + 1, x, 'h');
                    break;
                case char_rival:
                    mvaddch(y + 1, x, 'r');
                    break;
                case char_pacer:
                    mvaddch(y + 1, x, 'p');
                    break;
                case char_wanderer:
                    mvaddch(y + 1, x, 'w');
                    break;
                case char_stationary:
                    mvaddch(y + 1, x, 's');
                    break;
                case char_randWalker:
                    mvaddch(y + 1, x, 'n');
                    break;
                default:
                    mvaddch(y + 1, x, ' ');
                }
            }
        }
        mvaddch(y + 1, x, '\n');
    }
}

bool isBorder(int x, int y)
{
    return (x == 0 || x == 79 || y == 0 || y == 20);
}

bool validate_PC_Move(terrainType ter, int x, int y)
{
    return ter != ter_border && ter != ter_forest && ter != ter_mountain && ter != ter_rock && ter != ter_tree &&
           x >= 0 && x <= 79 && y >= 0 && y <= 20;
}

bool isMoveInsideMap(int x, int y)
{
    return (x > 0 && x < 79 && y > 0 && y < 20);
}

void generatePath(map *map, int north, int east, int south, int west)
{
    // Generate random exits
    int randX1, randX2, randY1, randY2;
    // These rand values are 1-19 and 1-78 so we don't have exits
    //  on any corner pieces, just looks better to me that way
    randX1 = rand() % 78 + 1;
    randX2 = rand() % 78 + 1;
    randY1 = rand() % 19 + 1;
    randY2 = rand() % 19 + 1;

    // If any of these equals -1, then there is no existing neigbor map. Can generate a random exit
    // If any do not equal -1, match the exits
    if (north != -1)
        randX1 = north;
    if (south != -1)
        randX2 = south;
    if (west != -1)
        randY1 = west;
    if (east != -1)
        randY2 = east;

    // Maps the current exits to the struct to be used later
    map->northExit = randX1;
    map->southExit = randX2;
    map->westExit = randY1;
    map->eastExit = randY2;

    /*
       //Ensures that the two NS exits are one space apart
       if(randY1 == randY2){
         if(randY2++ < 20)
     randY2++;
         else
     randY2--;
       }
       //Ensures that the two WE exits are one space apart
       if(randX1 == randX2){
         if(randX2++ < 79)
     randX2++;
         else
     randX2--;
       }
       */
    map->map[0][randX1] = ter_exit;  // North Exit
    map->map[20][randX2] = ter_exit; // South Exit
    map->map[randY1][0] = ter_exit;  // West Exit
    map->map[randY2][79] = ter_exit; // East Exit

    // Generate paths

    // West -> East Path, start by finding one point's Y value then find
    //  a random X value and make a point. Then connect the two.
    int tempPathX = rand() % 78 + 1;
    map->map[randY1][tempPathX] = ter_road;
    for (int i = 1; i < tempPathX; i++)
    {
        map->map[randY1][i] = ter_road;
    }
    for (int k = 78; k >= tempPathX; k--)
    {
        map->map[randY2][k] = ter_road;
    }

    // Now connect the two horizontal paths vertically. Find out which
    //  path is lower
    int largerYCord;
    int smallerYCord;
    if (randY1 > randY2)
    {
        largerYCord = randY1;
        smallerYCord = randY2;
    }
    else
    {
        largerYCord = randY2;
        smallerYCord = randY1;
    }
    for (int k = smallerYCord; k < largerYCord; k++)
    {
        map->map[k][tempPathX] = ter_road;
    }

    // North -> South Path, similar process
    int tempPathY = rand() % 19 + 1;
    map->map[tempPathY][randX1] = ter_road;
    for (int n = 1; n < tempPathY; n++)
    {
        map->map[n][randX1] = ter_road;
    }
    for (int j = 19; j >= tempPathY; j--)
    {
        map->map[j][randX2] = ter_road;
    }

    // Connect the two vertical paths horizontally
    int largerXCord;
    int smallerXCord;
    if (randX1 > randX2)
    {
        largerXCord = randX1;
        smallerXCord = randX2;
    }
    else
    {
        largerXCord = randX2;
        smallerXCord = randX1;
    }
    for (int m = smallerXCord; m < largerXCord; m++)
    {
        map->map[tempPathY][m] = ter_road;
    }
}

void addCenterAndMart(map *map)
{
    // Add Poke Center and Poke Mart
    bool validPokeCenter = false;
    bool validPokeMart = false;
    int neighborsX[] = {1, 0, -1, 0}; // Allows us to the check the left, right, up, down
    int neighborsY[] = {0, -1, 0, 1}; // neighbors of a path cell

    int manhattanDistance = abs(world.xPos - 199) + abs(world.yPos - 199);
    double probability = (double)(((-45 * manhattanDistance) / 200) + 50) / 100;

    // Flat %5 chance
    if (probability < 0.05)
        probability = 0.05;
    // Distance 0 = center, always spawn
    if (manhattanDistance == 0)
        probability = 1.0;

    // Converts to a whole number
    probability *= 100;

    int spawnChanceCenter = rand() % 100;
    int spawnChanceMart = rand() % 100;

    // If the generator has a number from 0 - the probabiltiy, building will spawn
    if (spawnChanceCenter >= 0 && spawnChanceCenter < probability)
    {
        // Loops through until we find a random path cell, check to see if we can add a
        //  PokeCenter or PokeMart next to the path
        while (!validPokeCenter)
        {
            int randX = rand() % 78 + 1;
            int randY = rand() % 19 + 1;
            if (map->map[randY][randX] == ter_road)
            {
                for (int i = 0; i < 3; i++)
                {
                    // Every spot that is not a border or path is filled with a clearing up until
                    //  this point. So if the current spot we are evaluating is a clearing, we
                    //  can add a Poke Center here.
                    if (map->map[randY + neighborsY[i]][randX + neighborsX[i]] == ter_clearing)
                    {
                        map->map[randY + neighborsY[i]][randX + neighborsX[i]] = ter_pokeCenter;
                        validPokeCenter = true;
                        break;
                    }
                }
            }
        }
    }
    if (spawnChanceMart >= 0 && spawnChanceMart < probability)
    {
        while (!validPokeMart)
        {
            int randX2 = rand() % 78 + 1;
            int randY2 = rand() % 19 + 1;
            if (map->map[randY2][randX2] == ter_road)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (map->map[randY2 + neighborsY[i]][randX2 + neighborsX[i]] == ter_clearing)
                    {
                        map->map[randY2 + neighborsY[i]][randX2 + neighborsX[i]] = ter_pokeMart;
                        validPokeMart = true;
                        break;
                    }
                }
            }
        }
    }
}

void addGrass(map *map)
{

    // First for loop lets us decide how many grass regions we want. I set it to 10 for now to have a balance of clearings and grass
    //  regions.
    // Second and third for loop determine the size of each region randomly
    for (int j = 0; j < NUM_GRASS_REGIONS; j++)
    {
        int xCord = rand() % 78 + 1;
        int yCord = rand() % 19 + 1;

        for (int i = 0; i < rand() % 6 + 3; i++)
        {
            for (int k = 0; k < rand() % 15 + 5; k++)
            {
                if (isMoveInsideMap(xCord + k, yCord + i) && map->map[yCord + i][xCord + k] == ter_clearing)
                {
                    map->map[yCord + i][xCord + k] = ter_grassTall;
                }
            }
        }
    }
}

void addTrees(map *map)
{
    int numTrees = rand() % 21 + 20; // Generates 20-40 trees

    for (int i = 0; i < numTrees; i++)
    {
        int randX = rand() % 78 + 1;
        int randY = rand() % 19 + 1;
        if (map->map[randY][randX] == ter_clearing)
        {
            map->map[randY][randX] = ter_tree;
        }
    }
}

void addRocks(map *map)
{

    int numRocks = rand() % 21 + 20; // Generates 20-40 Rocks

    for (int i = 0; i < numRocks; i++)
    {
        int randX = rand() % 78 + 1;
        int randY = rand() % 19 + 1;
        if (map->map[randY][randX] == ter_clearing)
        {
            map->map[randY][randX] = ter_rock;
        }
    }
}

void addForest(map *map)
{
    int numForestOffset = rand() % 3 - 1; // Generates rand number from -1 - 1. Add to NUM_FOREST to get either 0, 1, or 2 forests per map

    for (int j = 0; j < NUM_FOREST + numForestOffset; j++)
    {
        int xCord = rand() % 78 + 1;
        int yCord = rand() % 19 + 1;

        for (int i = 0; i < rand() % 6 + 3; i++)
        {
            for (int k = 0; k < rand() % 15 + 5; k++)
            {
                if (isMoveInsideMap(xCord + k, yCord + i) && map->map[yCord + i][xCord + k] == ter_clearing)
                {
                    map->map[yCord + i][xCord + k] = ter_forest;
                }
            }
        }
    }
}

void addMountain(map *map)
{
    int numMountainOffset = rand() % 3 - 1; // Generates rand number from -1 - 1. Add to NUM_FOREST to get either 0, 1, or 2 forests per map

    for (int j = 0; j < NUM_FOREST + numMountainOffset; j++)
    {
        int xCord = rand() % 78 + 1;
        int yCord = rand() % 19 + 1;

        for (int i = 0; i < rand() % 6 + 3; i++)
        {
            for (int k = 0; k < rand() % 15 + 5; k++)
            {
                if (isMoveInsideMap(xCord + k, yCord + i) && map->map[yCord + i][xCord + k] == ter_clearing)
                {
                    map->map[yCord + i][xCord + k] = ter_mountain;
                }
            }
        }
    }
}

void generateMap(map *map, int north, int east, int south, int west)
{
    int NS_Border, WE_Border;

    for (NS_Border = 0; NS_Border < MAP_Y; NS_Border++)
    {
        for (WE_Border = 0; WE_Border < MAP_X; WE_Border++)
        {
            if (isBorder(WE_Border, NS_Border))
            {
                map->map[NS_Border][WE_Border] = ter_border;
            }
            else
            {
                // fills in every other cell that is not a border with clearings
                map->map[NS_Border][WE_Border] = ter_clearing;
            }
        }
    }

    generatePath(map, north, east, south, west);
    addCenterAndMart(map);
    addGrass(map);
    addForest(map);
    addMountain(map);
    addTrees(map);
    addRocks(map);

    while (isFirstMap)
    {
        player.xPos = rand() % 78 + 1;
        player.yPos = rand() % 19 + 1;

        if (map->map[player.yPos][player.xPos] == ter_road)
        {
            map->map[player.yPos][player.xPos] = ter_character;
            break;
        }
    }

    isFirstMap = false;

    // dynamically allocate our character array
    // numTrainers + 1 to include the PC
    map->characterArray = (character *)malloc((numTrainers + 1) * sizeof(character));

    // Will always have our PC in the array as the starting index, intializes its variables.
    map->characterArray[0].characterType = char_pc;
    map->characterArray[0].currTerrain = ter_road;
    map->characterArray[0].xPos = player.xPos;
    map->characterArray[0].yPos = player.yPos;

    // Fill our array with the character types
    if (numTrainers >= 1)
    {
        map->characterArray[1].characterType = char_hiker;
    }
    if (numTrainers >= 2)
    {
        map->characterArray[2].characterType = char_rival;
    }
    if (numTrainers > 2)
    {
        for (int i = 3; i < numTrainers + 1; i++)
        {
            charType randCharacter = (enum charType)(rand() % 6 + 1);
            map->characterArray[i].characterType = randCharacter;
        }
    }

    int randX, randY;

    for (int k = 1; k < numTrainers + 1; k++)
    {

        // Finds a valid spot for the NPC to spawn
        while (1)
        {
            randX = rand() % 78 + 1;
            randY = rand() % 19 + 1;
            if (map->map[randY][randX] == ter_clearing || map->map[randY][randX] == ter_grassTall)
            {
                break;
            }
        }
        // currTerrain is used to determine the terrain the character is currently on so that it can be reprinted when the character moves
        map->characterArray[k].currTerrain = map->map[randY][randX];
        map->map[randY][randX] = ter_character;
        map->characterArray[k].xPos = randX, map->characterArray[k].yPos = randY;
    }
}

bool directionalMove(character *c, map *terMap)
{
    if (c->dir == NONE)
    {
        c->dir = randDirection();
    }

    int xChange = 0, yChange = 0;
    int curX = c->xPos, curY = c->yPos;

    switch (c->dir)
    {
    case NORTH:
        yChange--;
        break;
    case SOUTH:
        yChange++;
        break;
    case EAST:
        xChange++;
        break;
    case WEST:
        xChange--;
        break;
    case NONE:
        break;
    }

    // Checks if next move in the same direction is not the border and is a reachable terrain type
    if ((isMoveInsideMap(curX + xChange, curY + yChange)) && other_NPC_validTerrain(terMap->map[curY + yChange][curX + xChange]))
    {
        // This checks for Wanderers. Next move has to be the same terrain type that Wanderer spawned in, otherwise returns false
        if (c->characterType == char_wanderer && (terMap->map[curY + yChange][curX + xChange] != c->currTerrain))
        {
            return false;
        }
        terMap->map[curY][curX] = c->currTerrain;
        c->currTerrain = terMap->map[curY + yChange][curX + xChange];
        terMap->map[curY + yChange][curX + xChange] = ter_character;
        c->yPos = curY + yChange, c->xPos = curX + xChange;

        return true;
    }
    return false;
}

char get_NPC_Char(charType type)
{
    switch (type)
    {
    case char_hiker:
        return 'h';
    case char_rival:
        return 'r';
    case char_pacer:
        return 'p';
    case char_stationary:
        return 's';
    case char_wanderer:
        return 'w';
    case char_randWalker:
        return 'n';
    case char_pc:
        return '@';
    }
    return ' ';
}

bool isTerrain(terrainType ter)
{
    return ter == ter_forest || ter == ter_tree || ter == ter_mountain || ter == ter_rock;
}

void gameLoop(map *terMap)
{

    heap_t h;
    character *c;
    bool quitGame = false;
    int xNeighbor[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int yNeighbor[] = {1, 1, 0, -1, -1, -1, 0, 1};

    world.hikerMap = (distanceMap *)malloc(sizeof(distanceMap));
    world.hikerMap->npcType = char_hiker;

    world.rivalMap = (distanceMap *)malloc(sizeof(distanceMap));
    world.rivalMap->npcType = char_rival;

    dijkstra(world.hikerMap, world.mapArray[world.yPos][world.xPos]);
    dijkstra(world.rivalMap, world.mapArray[world.yPos][world.xPos]);

    // Initialize cost and direction for all characters
    for (int i = 0; i < numTrainers + 1; i++)
    {
        terMap->characterArray[i].cost = 0;
        terMap->characterArray[i].dir = NONE;
        terMap->characterArray[i].beenDefeated = false;
    }

    heap_init(&h, characterCost_cmp, NULL);

    // Add all characters to the queue/heap
    for (int i = 0; i < numTrainers + 1; i++)
    {
        heap_insert(&h, &terMap->characterArray[i]);
    }

    // Use priority queue to determine move order and perform the logic to determine the next move an NPC will make
    //  a) If hiker / rival --> Follow the next shortest path cell per their respective distant maps
    //  b) For any other NPCs, use their movement pattern per the document.
    //  c) PC will reprint the map, gather a user input, and follow the documentation of PC user inputs
    //  Once each character has its move time cost evaluated and moves, readd character back into queue with their new costs
    while ((c = (character *)heap_remove_min(&h)) && !quitGame)
    {

        int newX, newY;
        int minCost;
        int32_t userKey;

        switch (c->characterType)
        {
        case char_pc:
        {
            // Always start by reprinting the map so PC can decide their next move on the most updated map
            clear();
            printMap(terMap);
            refresh();

            int xChange = 0, yChange = 0;
            int curX = c->xPos, curY = c->yPos;

            userKey = getch();

            // User input switch statement
            switch (userKey)
            {
            // Upper-left move
            case '7':
            case 'y':
                if (validate_PC_Move(terMap->map[c->yPos - 1][c->xPos - 1], c->xPos - 1, c->yPos - 1))
                {
                    yChange--, xChange--;
                }
                break;
            // One cell up
            case '8':
            case 'k':
                if (validate_PC_Move(terMap->map[c->yPos - 1][c->xPos], c->xPos, c->yPos - 1))
                {
                    yChange--;
                }
                break;
            // Upper-right move
            case '9':
            case 'u':
                if (validate_PC_Move(terMap->map[c->yPos - 1][c->xPos + 1], c->xPos + 1, c->yPos - 1))
                {
                    yChange--, xChange++;
                }
                break;
            // One cell right
            case '6':
            case 'l':
                if (validate_PC_Move(terMap->map[c->yPos][c->xPos + 1], c->xPos + 1, c->yPos))
                {
                    xChange++;
                }
                break;
            // Lower right move
            case '3':
            case 'n':
                if (validate_PC_Move(terMap->map[c->yPos + 1][c->xPos + 1], c->xPos + 1, c->yPos + 1))
                {
                    yChange++, xChange++;
                }
                break;
            // One cell down
            case '2':
            case 'j':
                if (validate_PC_Move(terMap->map[c->yPos + 1][c->xPos], c->xPos, c->yPos + 1))
                {
                    yChange++;
                }
                break;
            // Lower left move
            case '1':
            case 'b':
                if (validate_PC_Move(terMap->map[c->yPos + 1][c->xPos - 1], c->xPos - 1, c->yPos + 1))
                {
                    yChange++, xChange--;
                }
                break;
            // One cell left
            case '4':
            case 'h':
                if (validate_PC_Move(terMap->map[c->yPos][c->xPos - 1], c->xPos - 1, c->yPos))
                {
                    xChange--;
                }
                break;
            // List of trainers
            case 't':
                for (int i = 1; i < numTrainers + 1; i++)
                {
                    character npc = terMap->characterArray[i];
                    char npcChar = get_NPC_Char(npc.characterType);
                    int verticalPos = c->yPos - npc.yPos;   // Negative Y = South, Positve = North
                    int horizontalPos = npc.xPos - c->xPos; // Positive X = East, Negative = West
                    std::string verticalDirection, horizontalDirection;

                    if (verticalPos >= 0)
                    {
                        verticalDirection = "North";
                    }
                    else
                    {
                        verticalDirection = "South";
                    }
                    if (horizontalPos >= 0)
                    {
                        horizontalDirection = "East";
                    }
                    else
                    {
                        horizontalDirection = "West";
                    }

                    verticalPos = abs(verticalPos), horizontalPos = abs(horizontalPos);
                    mvprintw(22 + i, 0, "%c, %d %s and %d %s", npcChar, verticalPos, verticalDirection.c_str(), horizontalPos, horizontalDirection.c_str());
                }
                refresh();
                while ((userKey = getch()) != 27)
                {
                    if (userKey == KEY_UP)
                    {
                        // Scroll up
                    }
                    if (userKey == KEY_DOWN)
                    {
                        // Scroll down
                    }
                }
                break;
            case 'Q':
            case 'q':
                quitGame = true;
                break;
            default:
                break;
            }

            /*
            Next series of code after the PC userinput switch statement is to check for any entry of a
            - PokeMart / PokeCenter
            - Map Exit
            - Running into a trainer
            Then resinserts the PC into the queue for the next turn. If a map exit is entered, then a new
            map is generated.
            */

            // Enters PokeMart / PokeCenter without having to go onto the C / M terrain and press '>'
            if (terMap->map[curY + yChange][curX + xChange] == ter_pokeMart || terMap->map[curY + yChange][curX + xChange] == ter_pokeCenter)
            {
                std::string type;

                if (c->currTerrain == ter_pokeMart)
                    type = "PokeMart";
                else
                    type = "PokeCenter";

                mvprintw(0, 0, "Entered %s!\n", type.c_str());
                while ((userKey = getch()) != '<')
                {
                    // Placeholder
                }
                mvprintw(0, 0, "Exited %s!\n", type.c_str());
                refresh();
                c->cost += pathCost(c->currTerrain, c->characterType);
                heap_insert(&h, c);
                break;
            }

            // If PC moves to an exit
            if (terMap->map[curY + yChange][curX + xChange] == ter_exit)
            {
                char exitDirection;

                // Figure out which exit PC is taking
                if ((curY + yChange) == 0)
                {
                    exitDirection = 'n';
                }
                else if ((curY + yChange) == 20)
                {
                    exitDirection = 's';
                }
                else if ((curX + xChange) == 0)
                {
                    exitDirection = 'w';
                }
                else
                {
                    exitDirection = 'e';
                }

                // Move to the new world and update the current terrain map we are viewing
                moveWorld(exitDirection);
                terMap = world.mapArray[world.yPos][world.xPos];

                // Make new heap for new map
                heap_delete(&h);

                // Spawn PC on new map on the adjacent exit
                switch (exitDirection)
                {
                // Using North exit, entering new map from South
                case 'n':
                    curY = 19, curX = terMap->southExit;
                    break;
                // Using South exit, entering new map from North
                case 's':
                    curY = 1, curX = terMap->northExit;
                    break;
                // Using East exit, entering new map from West
                case 'e':
                    curY = terMap->westExit, curX = 1;
                    break;
                // Using West exit, entering new map from East
                case 'w':
                    curY = terMap->eastExit, curX = 78;
                    break;
                }

                // Update PCs new location
                player.xPos = curX, player.yPos = curY;
                terMap->characterArray[0].characterType = char_pc;
                terMap->characterArray[0].currTerrain = ter_road;
                terMap->characterArray[0].xPos = player.xPos, terMap->characterArray[0].yPos = player.yPos;
                terMap->map[player.yPos][player.xPos] = ter_character;

                dijkstra(world.hikerMap, terMap);
                dijkstra(world.rivalMap, terMap);

                // Initialize cost and direction for all characters
                for (int i = 0; i < numTrainers + 1; i++)
                {
                    terMap->characterArray[i].cost = 0;
                    terMap->characterArray[i].dir = NONE;
                    terMap->characterArray[i].beenDefeated = false;
                }

                heap_init(&h, characterCost_cmp, NULL);

                // Add all characters to the queue/heap
                for (int i = 0; i < numTrainers + 1; i++)
                {
                    heap_insert(&h, &terMap->characterArray[i]);
                }
                break;
            }

            // Check if PC runs into Trainer
            if (terMap->map[c->yPos + yChange][c->xPos + xChange] == ter_character)
            {
                int tempX, tempY;
                int j;
                tempX = c->xPos + xChange, tempY = c->yPos + yChange;

                // Figure out which NPC the PC ran into
                for (j = 0; j < numTrainers + 1; j++)
                {
                    if (terMap->characterArray[j].xPos == tempX && terMap->characterArray[j].yPos == tempY)
                    {
                        break;
                    }
                }

                if (terMap->characterArray[j].characterType == char_pc)
                {
                    c->cost += pathCost(c->currTerrain, c->characterType);
                    heap_insert(&h, c);
                    break;
                }

                // If this Trainer has not been defeated before, begin battle
                if (!(terMap->characterArray[j].beenDefeated))
                {
                    mvprintw(0, 0, "Entering Pokemon battle!");
                    while ((userKey = getch()) != 27)
                    {
                        // placeholder
                    }
                    terMap->characterArray[j].beenDefeated = true;
                }
                else
                {
                    mvprintw(0, 0, "Already defeated this trainer!");
                    while ((userKey = getch()) != 27)
                    {
                        // placeholder
                    }
                }
                c->cost += pathCost(c->currTerrain, c->characterType);
                heap_insert(&h, c);
                break;
            }

            /*
            If none of the previous three conditions are met
            - Entering Mart / Center
            - Landing on a map exit
            - Running into a trainer
            Then update the PC's move, and readd to the queue
            */

            // Update PC on map
            terMap->map[curY][curX] = c->currTerrain;
            c->currTerrain = terMap->map[curY + yChange][curX + xChange];
            terMap->map[curY + yChange][curX + xChange] = ter_character;
            c->yPos = curY + yChange, c->xPos = curX + xChange;

            // Update PC X & Y Position
            player.xPos = c->xPos, player.yPos = c->xPos;

            c->cost += pathCost(c->currTerrain, c->characterType);
            heap_insert(&h, c);

            // Error here
            // dijkstra(world.hikerMap, world.mapArray[world.yPos][world.xPos]);
            // dijkstra(world.rivalMap, world.mapArray[world.yPos][world.xPos]);
            break;
        }

        case char_hiker:
        {
        case char_rival:
        {

            // Evaluate the cheapest neighboring cell to travel to from the distance map
            minCost = INT_MAX;
            newX = c->xPos, newY = c->yPos;
            distanceMap *tempDistMap = world.rivalMap;

            if (c->characterType == char_hiker)
            {
                tempDistMap = world.hikerMap;
            }

            for (int i = 0; i < 8; i++)
            {
                // If the neighbor is cheaper then the current and not occupied by another character, update that as the best next move
                if (tempDistMap->distMap[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]].cost < minCost &&
                    terMap->map[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]] != ter_character)
                {
                    // Keep track of newX and newY for next move
                    newX = c->xPos + xNeighbor[i];
                    newY = c->yPos + yNeighbor[i];
                    minCost = tempDistMap->distMap[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]].cost;
                }
            }

            // Move the hiker/rival: fill in the old terrain they were on, save the new terrain they are moving to for later, then move them.
            terMap->map[c->yPos][c->xPos] = c->currTerrain;
            c->currTerrain = terMap->map[newY][newX];
            terMap->map[newY][newX] = ter_character;
            c->yPos = newY, c->xPos = newX;

            // Add cost and reinsert back in heap
            c->cost += minCost;
            heap_insert(&h, c);
            break;
        }
        }

        case char_pacer:
        {
            // If directionalMove returns false, means cannot continue in the same direction. Flip direction
            if (!directionalMove(c, terMap))
            {
                switch (c->dir)
                {
                case NORTH:
                    c->dir = SOUTH;
                    break;
                case SOUTH:
                    c->dir = NORTH;
                    break;
                case EAST:
                    c->dir = WEST;
                    break;
                case WEST:
                    c->dir = EAST;
                    break;
                case NONE:
                    break;
                }
            }

            // Update cost, reinsert to heap
            c->cost += pathCost(c->currTerrain, c->characterType);
            heap_insert(&h, c);
            break;
        }

        // Wanderer and RandWalker will have the same case. Wanderer's condition that it can only stay in the same terrain is checked
        //  in the directionalMove function
        case char_wanderer:
        {
        case char_randWalker:
        {
            // If directionalMove returns false, change Wanderer/RandWalker direction randomly with a new direction
            if (!(directionalMove(c, terMap)))
            {
                while (1)
                {
                    direction newDir = (enum direction)(rand() % 4);
                    if (c->dir != newDir)
                    {
                        c->dir = newDir;
                        break;
                    }
                }
            }
            // Update cost, reinsert to heap
            c->cost += pathCost(c->currTerrain, c->characterType);
            heap_insert(&h, c);
            break;
        }
        }
        default:
        {
            break;
        }
        }
    }
    heap_delete(&h);
}

void createWorld(map *startingMap)
{

    for (int i = 0; i < WORLD_DIM; i++)
    {
        for (int j = 0; j < WORLD_DIM; j++)
        {
            world.mapArray[j][i] = NULL;
        }
    }

    world.mapArray[world.yPos][world.xPos] = startingMap;
}

void moveWorld(char moveInput)
{

    // Loops through userInput, calls userMove function
    // 3 Outcomes depending on what userMove returns (set to nextMove)
    //  Return 0 = A NESW move, check bounds and if a map is there or not.
    //    creates one if no map exists
    //  Return 1 = Fly to a new map, create a map if one doesn't exist
    //  Return 2 = Quit

    int north = -1;
    int east = -1;
    int south = -1;
    int west = -1;

    int nextMove = userMove(moveInput);

    if (nextMove == 0)
    {
        // Checks if nextMove does not fall off world
        if (validateWorldMove(world.xPos, world.yPos))
        {

            // Creates new map if we travel to a new cell
            if (world.mapArray[world.yPos][world.xPos] == NULL)
            {

                // These four if statements gets my exits to use as parameters to
                //   generate my next map, checks the four possible maps around it.
                // This current north exit would equal the south exit of the map above it.
                // The south exit would equal the north exit of the map below it.
                //... and so forth
                if (validateWorldMove(world.xPos, world.yPos - 1))
                {
                    if (world.mapArray[world.yPos - 1][world.xPos] != NULL)
                    {
                        north = world.mapArray[world.yPos - 1][world.xPos]->southExit;
                    }
                }
                if (validateWorldMove(world.xPos, world.yPos + 1))
                {
                    if (world.mapArray[world.yPos + 1][world.xPos] != NULL)
                    {
                        south = world.mapArray[world.yPos + 1][world.xPos]->northExit;
                    }
                }
                if (validateWorldMove(world.xPos + 1, world.yPos))
                {
                    if (world.mapArray[world.yPos][world.xPos + 1] != NULL)
                    {
                        east = world.mapArray[world.yPos][world.xPos + 1]->westExit;
                    }
                }
                if (validateWorldMove(world.xPos - 1, world.yPos))
                {
                    if (world.mapArray[world.yPos][world.xPos - 1] != NULL)
                    {
                        west = world.mapArray[world.yPos][world.xPos - 1]->eastExit;
                    }
                }

                // creates the new map with those arguments above
                map *newMap = (map *)malloc(sizeof(map));
                generateMap(newMap, north, east, south, west);

                // Checks world borders, fills edge in with all rock if at world edge
                if (world.xPos == 0)
                    newMap->map[newMap->westExit][0] = ter_border;
                if (world.xPos == 398)
                    newMap->map[newMap->eastExit][79] = ter_border;
                if (world.yPos == 0)
                    newMap->map[0][newMap->northExit] = ter_border;
                if (world.yPos == 398)
                    newMap->map[20][newMap->southExit] = ter_border;

                world.mapArray[world.yPos][world.xPos] = newMap;
            }
        }
        // If user falls off world (out of bounds), reprint previous map
        else
        {
            // Set our position to the previous position
            world.yPos = world.prevYPos;
            world.xPos = world.prevXPos;
        }
    }

    // Fly
    if (nextMove == 1)
    {

        world.prevYPos = world.yPos;
        world.prevXPos = world.xPos;

        printf("Enter X Y: ");
        scanf("%d %d", &world.xPos, &world.yPos);

        if (validateWorldMove(world.xPos, world.yPos))
        {
            if (world.mapArray[world.yPos][world.xPos] == NULL)
            {

                // These four if statements gets my exits to use as parameters to
                //   generate my next map, checks the four possible maps around it.
                // This current north exit would equal the south exit of the map above it.
                // The south exit would equal the north exit of the map below it.
                //... and so forth
                if (validateWorldMove(world.xPos, world.yPos + 1))
                {
                    if (world.mapArray[world.yPos + 1][world.xPos] != NULL)
                    {
                        north = world.mapArray[world.yPos + 1][world.xPos]->southExit;
                    }
                }
                if (validateWorldMove(world.xPos, world.yPos - 1))
                {
                    if (world.mapArray[world.yPos - 1][world.xPos] != NULL)
                    {
                        south = world.mapArray[world.yPos - 1][world.xPos]->northExit;
                    }
                }
                if (validateWorldMove(world.xPos + 1, world.yPos))
                {
                    if (world.mapArray[world.yPos][world.xPos + 1] != NULL)
                    {
                        east = world.mapArray[world.yPos][world.xPos + 1]->westExit;
                    }
                }
                if (validateWorldMove(world.xPos - 1, world.yPos))
                {
                    if (world.mapArray[world.yPos][world.xPos - 1] != NULL)
                    {
                        west = world.mapArray[world.yPos][world.xPos - 1]->eastExit;
                    }
                }

                // Creation of the new map
                map *newMap = (map *)malloc(sizeof(map));
                generateMap(newMap, north, east, south, west);

                // Checks world borders, fills edge with rock if at border
                if (world.xPos == 0)
                    newMap->map[newMap->westExit][0] = ter_border;
                if (world.xPos == 398)
                    newMap->map[newMap->eastExit][79] = ter_border;
                if (world.yPos == 0)
                    newMap->map[0][newMap->northExit] = ter_border;
                if (world.yPos == 398)
                    newMap->map[20][newMap->southExit] = ter_border;

                world.mapArray[world.yPos][world.xPos] = newMap;
                dijkstra(world.hikerMap, newMap);
                dijkstra(world.rivalMap, newMap);
                printMap(world.mapArray[world.yPos][world.xPos]);
                printf("X Y (%d %d)\n", world.xPos, world.yPos);
            }
            else
            {
                dijkstra(world.hikerMap, world.mapArray[world.yPos][world.xPos]);
                dijkstra(world.rivalMap, world.mapArray[world.yPos][world.xPos]);
                printMap(world.mapArray[world.yPos][world.xPos]);
                printf("X Y (%d %d)\n", world.xPos, world.yPos);
            }
        }
        else
        {
            world.yPos = world.prevYPos;
            world.xPos = world.prevXPos;

            dijkstra(world.hikerMap, world.mapArray[world.yPos][world.xPos]);
            dijkstra(world.rivalMap, world.mapArray[world.yPos][world.xPos]);
            printMap(world.mapArray[world.yPos][world.xPos]);
            printf("You fell off the world!\n");
            printf("X Y (%d %d)\n", world.xPos, world.yPos);
        }
    }
}

void deleteWorld()
{
    // Done playing, dealloacate memory
    for (int i = 0; i < WORLD_DIM; i++)
    {
        for (int j = 0; j < WORLD_DIM; j++)
        {
            if (world.mapArray[i][j] != NULL)
            {
                // Not sure if I need to free my character array or if freeing the entire map
                //  will take care of that. But have it added here for now
                free(world.mapArray[i][j]->characterArray);
                free(world.mapArray[i][j]);
            }
        }
    }

    free(world.hikerMap);
    free(world.rivalMap);
}

int userMove(char userInput)
{

    world.prevYPos = world.yPos;
    world.prevXPos = world.xPos;

    switch (userInput)
    {
    case ('n'):
        world.yPos--;
        return 0;
    case ('s'):
        world.yPos++;
        return 0;
    case ('e'):
        world.xPos++;
        return 0;
    case ('w'):
        world.xPos--;
        return 0;
    case ('f'):
        return 1;
    case ('q'):
        return 2;
    default:
        return -1;
    }
}

void init_terminal()
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

// Makes sure no moves go off the world 0-398 since 399x399 2D array
bool validateWorldMove(int x, int y)
{
    return (x >= 0 && x < 399 && y >= 0 && y < 399);
}

int main(int argc, char *argv[])
{

    uint32_t seed;
    std::string directory = checkDirectory();

    if (!directory.compare("false"))
    {
        std::cout << "No database found! Exiting" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "Using database in directory: " << directory << std::endl;
    }

    // Access CSV folder
    directory.append("pokedex/data/csv/");

    // Checks for command lind arguments
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (!strcmp(argv[i], "--numtrainers"))
                numTrainers = atoi(argv[i + 1]);
            if (!strcmp(argv[i], "pokemon"))
                parsePokemon(directory);
            if (!strcmp(argv[i], "moves"))
                parseMoves(directory);
            if (!strcmp(argv[i], "pokemon_moves"))
                parsePokemonMoves(directory);
            if (!strcmp(argv[i], "pokemon_species"))
                parsePokemonSpecies(directory);
            if (!strcmp(argv[i], "experience"))
                parseExperience(directory);
            if (!strcmp(argv[i], "type_names"))
                parseTypeNames(directory);
        }
    }
    return 0;

    world.xPos = 199, world.yPos = 199, world.prevXPos = 199, world.prevYPos = 199;
    map *startingMap = (map *)malloc(sizeof(map));
    seed = time(NULL);
    srand(seed);
    // srand(1646718313);
    printf("Using Seed: %u\n", seed);

    init_terminal();
    generateMap(startingMap, -1, -1, -1, -1);
    createWorld(startingMap);
    gameLoop(startingMap);
    // moveCharacter -> gameLoop
    //  -> This loops and calls moveMap() if needed (handles N, E, S, W map moves, and Quit input)
    // Deinit Screen
    // Delete world
    endwin();
    deleteWorld();

    return 0;
}
