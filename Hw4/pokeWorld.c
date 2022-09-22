#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "heap.h"

#define WORLD_DIM 399
#define MAP_Y 21
#define MAP_X 80
#define NUM_FOREST 1
#define NUM_MOUNTAIN 1
#define NUM_GRASS_REGIONS 5

int numTrainers = 0;

typedef enum terrainType
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
  ter_mixed,
  ter_character,
  ter_mountain,
  ter_forest,

} terType_t;

typedef enum direction
{
  NORTH,
  SOUTH,
  EAST,
  WEST,
} direction_t;

typedef enum characterType
{
  char_pc,
  char_hiker,
  char_rival,
  char_pacer,
  char_wanderer,
  char_stationary,
  char_randWalker,
} charType_t;

typedef struct distanceMapCell
{
  int cost;
  heap_node_t *hn;
  int xPos, yPos;
} distMapCell_t;

typedef struct distanceMap
{
  distMapCell_t distMap[MAP_Y][MAP_X];
  charType_t npcType;

} distMap_t;

typedef struct gameCharacter
{
  int xPos, yPos;
  int cost;
  heap_node_t *hn;
  charType_t characterType;
  terType_t currTerrain;
  direction_t direction;

} character_t;

typedef struct map
{

  terType_t map[MAP_Y][MAP_X];
  int northExit;
  int southExit;
  int westExit;
  int eastExit;
  character_t *characterArray;

} map_t;

//Global player variable
character_t player;

int userMove(char userInput);
bool validateWorldMove(int x, int y);
bool isMoveInsideMap(int x, int y);

//Global variabels for X and Y positions
int worldXPos = 199;
int worldYPos = 199;

//Global variables to track previous X & Y position.
//Useful for cases where player leaves world boundaries
int prevXPos = 199;
int prevYPos = 199;

//Creates a 2D World array of 399x399. Initalizes all to NULL
map_t *mapArray[WORLD_DIM][WORLD_DIM];

int32_t distMap_cmp(const void *key, const void *with)
{
  return ((distMapCell_t *)key)->cost - ((distMapCell_t *)with)->cost;
}

int32_t characterCost_cmp(const void *key, const void *with)
{
  return ((character_t *)key)->cost - ((character_t *)with)->cost;
}

direction_t randDirection()
{
  return (enum direction)(rand() % 4);
}

bool other_NPC_validTerrain(terType_t ter)
{
  return (ter == ter_grassTall || ter == ter_clearing || ter == ter_road);
}

//Function to determine terrain cell cost
int pathCost(terType_t terrain, charType_t npcTag)
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

void dijkstra(distMap_t *distMap, map_t *terrainMap)
{

  int x, y;
  int xNeighbor[] = {0, 1, 1, 1, 0, -1, -1, -1};
  int yNeighbor[] = {1, 1, 0, -1, -1, -1, 0, 1};
  heap_t h;
  distMapCell_t *d;

  //Initializes the X and Y positions
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      distMap->distMap[y][x].yPos = y;
      distMap->distMap[y][x].xPos = x;
    }
  }

  //Initalizes all nodes to cost infinity
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      distMap->distMap[y][x].cost = INT_MAX;
    }
  }

  //Source node based on player position set to cost 0
  distMap->distMap[player.yPos][player.xPos].cost = 0;

  heap_init(&h, distMap_cmp, NULL);

  //Insert all nodes to a min heap besides borders
  for (y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      distMap->distMap[y][x].hn = heap_insert(&h, &distMap->distMap[y][x]);
    }
  }

  //Add the exits as nodes to the min heap
  /*
  /Actually, not sure I need this
  /
  distMap->distMap[20][terrainMap->southExit].hn = heap_insert(&h, &distMap->distMap[20][terrainMap->southExit]);
  distMap->distMap[0][terrainMap->northExit].hn = heap_insert(&h, &distMap->distMap[0][terrainMap->northExit]);
  distMap->distMap[terrainMap->westExit][0].hn = heap_insert(&h, &distMap->distMap[terrainMap->westExit][0]);
  distMap->distMap[terrainMap->eastExit][79].hn = heap_insert(&h, &distMap->distMap[terrainMap->eastExit][79]);
  */
  while ((d = heap_remove_min(&h)))
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

  //Print distnce map
  for (y = 0; y < MAP_Y; y++)
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

  heap_delete(&h);
}

void printMap(map_t *map)
{

  int x, y, i;

  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      switch (map->map[y][x])
      {
      case ter_road:
      case ter_exit:
        printf("#");
        break;
      case ter_rock:
      case ter_border:
      case ter_mountain:
        printf("%%");
        break;
      case ter_grassTall:
        printf(":");
        break;
      case ter_tree:
      case ter_forest:
        printf("\"");
        break;
      case ter_clearing:
        printf(".");
        break;
      case ter_mixed:
        //not sure what to put here yet
        break;
      case ter_pokeCenter:
        printf("C");
        break;
      case ter_pokeMart:
        printf("M");
        break;
      case ter_character:

        for (i = 0; i < numTrainers + 1; i++)
        {
          //Finds the matching character Type from our character Array that matches with the current map
          if (map->characterArray[i].xPos == x && map->characterArray[i].yPos == y)
          {
            break;
          }
        }
        switch (map->characterArray[i].characterType)
        {
        case char_pc:
          printf("@");
          break;
        case char_hiker:
          printf("h");
          break;
        case char_rival:
          printf("r");
          break;
        case char_pacer:
          printf("p");
          break;
        case char_wanderer:
          printf("w");
          break;
        case char_stationary:
          printf("s");
          break;
        case char_randWalker:
          printf("n");
          break;
        default:
          printf(" ");
        }
      }
    }
    printf("\n");
  }
}

bool isBorder(int x, int y)
{
  return (x == 0 || x == 79 || y == 0 || y == 20);
}

bool isMoveInsideMap(int x, int y)
{
  return (x > 0 && x < 79 && y > 0 && y < 20);
}

void generatePath(map_t *map, int north, int east, int south, int west)
{
  //Generate random exits
  int randX1, randX2, randY1, randY2;
  //These rand values are 1-19 and 1-78 so we don't have exits
  // on any corner pieces, just looks better to me that way
  randX1 = rand() % 78 + 1;
  randX2 = rand() % 78 + 1;
  randY1 = rand() % 19 + 1;
  randY2 = rand() % 19 + 1;

  //If any of these equals -1, then there is no existing neigbor map. Can generate a random exit
  //If any do not equal -1, match the exits
  if (north != -1)
    randX1 = north;
  if (south != -1)
    randX2 = south;
  if (west != -1)
    randY1 = west;
  if (east != -1)
    randY2 = east;

  //Maps the current exits to the struct to be used later
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
  map->map[0][randX1] = ter_exit;  //North Exit
  map->map[20][randX2] = ter_exit; //South Exit
  map->map[randY1][0] = ter_exit;  //West Exit
  map->map[randY2][79] = ter_exit; //East Exit

  //Generate paths

  //West -> East Path, start by finding one point's Y value then find
  // a random X value and make a point. Then connect the two.
  int tempPathX = rand() % 78 + 1;
  map->map[randY1][tempPathX] = ter_road;
  for (int i = 0; i < tempPathX; i++)
  {
    map->map[randY1][i] = ter_road;
  }
  for (int k = 78; k >= tempPathX; k--)
  {
    map->map[randY2][k] = ter_road;
  }

  //Now connect the two horizontal paths vertically. Find out which
  // path is lower
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

  //North -> South Path, similar process
  int tempPathY = rand() % 19 + 1;
  map->map[tempPathY][randX1] = ter_road;
  for (int n = 0; n < tempPathY; n++)
  {
    map->map[n][randX1] = ter_road;
  }
  for (int j = 19; j >= tempPathY; j--)
  {
    map->map[j][randX2] = ter_road;
  }

  //Connect the two vertical paths horizontally
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

void addCenterAndMart(map_t *map)
{
  //Add Poke Center and Poke Mart
  bool validPokeCenter = false;
  bool validPokeMart = false;
  int neighborsX[] = {1, 0, -1, 0}; //Allows us to the check the left, right, up, down
  int neighborsY[] = {0, -1, 0, 1}; // neighbors of a path cell

  int manhattanDistance = abs(worldXPos - 199) + abs(worldYPos - 199);
  double probability = (double)(((-45 * manhattanDistance) / 200) + 50) / 100;

  //Flat %5 chance
  if (probability < 0.05)
    probability = 0.05;
  //Distance 0 = center, always spawn
  if (manhattanDistance == 0)
    probability = 1.0;

  //Converts to a whole number
  probability *= 100;

  int spawnChanceCenter = rand() % 100;
  int spawnChanceMart = rand() % 100;

  //If the generator has a number from 0 - the probabiltiy, building will spawn
  if (spawnChanceCenter >= 0 && spawnChanceCenter < probability)
  {
    //Loops through until we find a random path cell, check to see if we can add a
    // PokeCenter or PokeMart next to the path
    while (!validPokeCenter)
    {
      int randX = rand() % 78 + 1;
      int randY = rand() % 19 + 1;
      if (map->map[randY][randX] == ter_road)
      {
        for (int i = 0; i < 3; i++)
        {
          //Every spot that is not a border or path is filled with a clearing up until
          // this point. So if the current spot we are evaluating is a clearing, we
          // can add a Poke Center here.
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

void addGrass(map_t *map)
{

  //First for loop lets us decide how many grass regions we want.
  //Second and third for loop determine the size of each region randomly
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

void addTrees(map_t *map)
{
  int numTrees = rand() % 21 + 20; //Generates 20-40 trees

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

void addRocks(map_t *map)
{

  int numRocks = rand() % 21 + 20; //Generates 20-40 Rocks

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

void addForest(map_t *map)
{
  int numForestOffset = rand() % 3 - 1; //Generates rand number from -1 - 1. Add to NUM_FOREST to get either 0, 1, or 2 forests per map

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

void addMountain(map_t *map)
{
  int numMountainOffset = rand() % 3 - 1; //Generates rand number from -1 - 1. Add to NUM_FOREST to get either 0, 1, or 2 forests per map

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

void generateMap(map_t *map, int north, int east, int south, int west)
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
        //fills in every other cell that is not a border with clearings
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

  while (1)
  {
    player.xPos = rand() % 78 + 1;
    player.yPos = rand() % 19 + 1;

    if (map->map[player.yPos][player.xPos] == ter_road)
    {
      map->map[player.yPos][player.xPos] = ter_character;
      break;
    }
  }

  //dynamically allocate our character array
  //numTrainers + 1 to include the PC
  map->characterArray = (character_t *)malloc((numTrainers + 1) * sizeof(character_t));

  //Will always have our PC in the array as the starting index, intializes its variables.
  map->characterArray[0].characterType = char_pc;
  map->characterArray[0].currTerrain = ter_road;
  map->characterArray[0].xPos = player.xPos;
  map->characterArray[0].yPos = player.yPos;

  //Fill our array with the character types
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
      charType_t randCharacter = (enum characterType)(rand() % 6 + 1);
      map->characterArray[i].characterType = randCharacter;
    }
  }

  int randX, randY;

  for (int k = 1; k < numTrainers + 1; k++)
  {
    charType_t type = map->characterArray[k].characterType;
    //Finds a valid spot for the NPC to spawn
    while (1)
    {
      randX = rand() % 78 + 1;
      randY = rand() % 19 + 1;

      //Special case for hiker, can spawn almost anywhere (mountains, forest, rocks, boulders, grass, etc)
      if (type == char_hiker && map->map[randY][randX] != ter_border && map->map[randY][randX] != ter_road)
      {
        break;
      }
      if (map->map[randY][randX] == ter_clearing || map->map[randY][randX] == ter_grassTall)
      {
        break;
      }
    }
    //currTerrain is used to determine the terrain the character is currently on so that it can be reprinted when the character moves
    map->characterArray[k].currTerrain = map->map[randY][randX];
    map->map[randY][randX] = ter_character;
    map->characterArray[k].xPos = randX, map->characterArray[k].yPos = randY;
  }
}

//Made this as a test to see if my cost was matching with the current terain the charactr is on
/*void test(character_t c)
  {
    switch (c.currTerrain)
    {
    case ter_road:
      printf("Path\n");
      break;
    case ter_clearing:
      printf("Clearing\n");
      break;
    case ter_grassTall:
      printf("Grass\n");
      break;
    default:
      printf("NA\n");
      break;
    }
  }
  */

bool directionalMove(character_t *c, map_t *terMap)
{
  if (c->direction == -1)
  {
    c->direction = randDirection();
  }

  int xChange = 0, yChange = 0;
  int curX = c->xPos, curY = c->yPos;

  switch (c->direction)
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
  }

  //Checks if next move in the same direction is not the border and is a reachable terrain type
  if ((isMoveInsideMap(curX + xChange, curY + yChange)) && other_NPC_validTerrain(terMap->map[curY + yChange][curX + xChange]))
  {
    //This checks for Wanderers. Next move has to be the same terrain type that Wanderer spawned in, otherwise returns false
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

void moveCharacters(distMap_t *hikerMap, distMap_t *rivalMap, map_t *terMap)
{

  heap_t h;
  character_t *c;
  int xNeighbor[] = {0, 1, 1, 1, 0, -1, -1, -1};
  int yNeighbor[] = {1, 1, 0, -1, -1, -1, 0, 1};
  //int x, y;

  //Initialize cost and direction for all characters
  for (int i = 0; i < numTrainers + 1; i++)
  {
    terMap->characterArray[i].cost = 0;
    terMap->characterArray[i].direction = -1;
  }

  heap_init(&h, characterCost_cmp, NULL);

  //Add all characters to the queue/heap
  for (int i = 0; i < numTrainers + 1; i++)
  {
    heap_insert(&h, &terMap->characterArray[i]);
  }

  //Use priority queue to determine move order and perform the logic to determine the next move an NPC will make
  // a) If hiker / rival --> Follow the next shortest path cell per their respective distant maps
  // b) For any other NPCs, use their movement pattern per the document.
  // c) PC stationary for now, will add movement later
  // Once each character has its move time cost evaluated and moves, readd character back into queue with their new costs
  while ((c = heap_remove_min(&h)))
  {

    int newX, newY;
    int minCost;

    switch (c->characterType)
    {
    case char_pc:
      //For now, PC stays still, adds the road cost and reinserts
      c->cost += pathCost(c->currTerrain, c->characterType);
      heap_insert(&h, c);
      printMap(terMap);
      usleep(250000);
      break;

    case char_hiker:
    case char_rival:

      //Evaluate the cheapest neighboring cell to travel to from the distance map
      minCost = INT_MAX;
      distMap_t *tempDistMap = rivalMap;

      if (c->characterType == char_hiker)
      {
        tempDistMap = hikerMap;
      }

      for (int i = 0; i < 8; i++)
      {
        //If the neighbor is cheaper then the current and not occupied by another character, update that as the best next move
        if (tempDistMap->distMap[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]].cost < minCost &&
            terMap->map[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]] != ter_character)
        {
          //Keep track of newX and newY for next move
          newX = c->xPos + xNeighbor[i];
          newY = c->yPos + yNeighbor[i];
          minCost = tempDistMap->distMap[c->yPos + yNeighbor[i]][c->xPos + xNeighbor[i]].cost;
        }
      }

      //Move the hiker/rival: fill in the old terrain they were on, save the new terrain they are moving to for later, then move them.
      terMap->map[c->yPos][c->xPos] = c->currTerrain;
      c->currTerrain = terMap->map[newY][newX];
      terMap->map[newY][newX] = ter_character;
      c->yPos = newY, c->xPos = newX;

      //Add cost and reinsert back in heap
      c->cost += minCost;
      heap_insert(&h, c);
      break;

    case char_pacer:
      //If directionalMove returns false, means cannot continue in the same direction. Flip direction
      if (!directionalMove(c, terMap))
      {
        switch (c->direction)
        {
        case NORTH:
          c->direction = SOUTH;
          break;
        case SOUTH:
          c->direction = NORTH;
          break;
        case EAST:
          c->direction = WEST;
          break;
        case WEST:
          c->direction = EAST;
          break;
        }
      }

      //Update cost, reinsert to heap
      c->cost += pathCost(c->currTerrain, c->characterType);
      heap_insert(&h, c);
      break;
    //Wanderer and RandWalker will have the same case. Wanderer's condition that it can only stay in the same terrain is checked
    // in the directionalMove function
    case char_wanderer:
    case char_randWalker:
      //If directionalMove returns false, change Wanderer/RandWalker direction randomly with a new direction
      if (!(directionalMove(c, terMap)))
      {
        while (1)
        {
          direction_t newDir = (enum direction)(rand() % 4);
          if (c->direction != newDir)
          {
            c->direction = newDir;
            break;
          }
        }
      }
      //Update cost, reinsert to heap
      c->cost += pathCost(c->currTerrain, c->characterType);
      heap_insert(&h, c);
      break;
    default:
      break;
    }

    //once moved, add back into queue

    //set cost to old cost + cost of new terrain
    //reinsert in heap
  }
}

void createWorld(map_t *startingMap)
{

  for (int i = 0; i < WORLD_DIM; i++)
  {
    for (int j = 0; j < WORLD_DIM; j++)
    {
      mapArray[i][j] = NULL;
    }
  }

  mapArray[worldYPos][worldXPos] = startingMap;
  distMap_t *hikerMap = malloc(sizeof(distMap_t));
  hikerMap->npcType = char_hiker;

  distMap_t *rivalMap = malloc(sizeof(distMap_t));
  rivalMap->npcType = char_rival;

  dijkstra(hikerMap, startingMap);
  dijkstra(rivalMap, startingMap);
  moveCharacters(hikerMap, rivalMap, mapArray[worldYPos][worldXPos]);

  printMap(mapArray[worldYPos][worldXPos]);
  printf("X Y (%d %d)\n", worldXPos, worldYPos);

  bool playing = true;

  //Loops through userInput, calls userMove function
  //3 Outcomes depending on what userMove returns (set to nextMove)
  // Return 0 = A NESW move, check bounds and if a map is there or not.
  //   creates one if no map exists
  // Return 1 = Fly to a new map, create a map if one doesn't exist
  // Return 2 = Quit
  while (playing)
  {
    char moveInput;
    int north = -1;
    int east = -1;
    int south = -1;
    int west = -1;

    printf("Enter Move (n, e, s, w, f, q): \n");
    scanf(" %c", &moveInput);
    int nextMove = userMove(moveInput);

    if (nextMove == 0)
    {
      //Checks if nextMove does not fall off world
      if (validateWorldMove(worldXPos, worldYPos))
      {

        //Creates new map if we travel to a new cell
        if (mapArray[worldYPos][worldXPos] == NULL)
        {

          //These four if statements gets my exits to use as parameters to
          //  generate my next map, checks the four possible maps around it.
          //This current north exit would equal the south exit of the map above it.
          //The south exit would equal the north exit of the map below it.
          //... and so forth
          if (validateWorldMove(worldXPos, worldYPos - 1))
          {
            if (mapArray[worldYPos - 1][worldXPos] != NULL)
            {
              north = mapArray[worldYPos - 1][worldXPos]->southExit;
            }
          }
          if (validateWorldMove(worldXPos, worldYPos + 1))
          {
            if (mapArray[worldYPos + 1][worldXPos] != NULL)
            {
              south = mapArray[worldYPos + 1][worldXPos]->northExit;
            }
          }
          if (validateWorldMove(worldXPos + 1, worldYPos))
          {
            if (mapArray[worldYPos][worldXPos + 1] != NULL)
            {
              east = mapArray[worldYPos][worldXPos + 1]->westExit;
            }
          }
          if (validateWorldMove(worldXPos - 1, worldYPos))
          {
            if (mapArray[worldYPos][worldXPos - 1] != NULL)
            {
              west = mapArray[worldYPos][worldXPos - 1]->eastExit;
            }
          }

          //creates the new map with those arguments above
          map_t *newMap = malloc(sizeof(map_t));
          generateMap(newMap, north, east, south, west);

          //Checks world borders, fills edge in with all rock if at world edge
          if (worldXPos == 0)
            newMap->map[newMap->westExit][0] = ter_border;
          if (worldXPos == 398)
            newMap->map[newMap->eastExit][79] = ter_border;
          if (worldYPos == 0)
            newMap->map[0][newMap->northExit] = ter_border;
          if (worldYPos == 398)
            newMap->map[20][newMap->southExit] = ter_border;

          mapArray[worldYPos][worldXPos] = newMap;
          dijkstra(hikerMap, newMap);
          dijkstra(rivalMap, newMap);
          printMap(mapArray[worldYPos][worldXPos]);
          printf("X Y (%d %d)\n", worldXPos, worldYPos);
        }
        //Prints already found map else wise
        else
        {
          dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
          dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
          printMap(mapArray[worldYPos][worldXPos]);
          printf("X Y (%d %d)\n", worldXPos, worldYPos);
        }
      }
      //If user falls off world (out of bounds), reprint previous map
      else
      {
        //Set our position to the previous position
        worldYPos = prevYPos;
        worldXPos = prevXPos;

        dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
        dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
        printMap(mapArray[worldYPos][worldXPos]);
        printf("You fell off the world!\n");
        printf("X Y (%d %d)\n", worldXPos, worldYPos);
      }
    }

    //Fly
    if (nextMove == 1)
    {

      prevYPos = worldYPos;
      prevXPos = worldXPos;

      printf("Enter X Y: ");
      scanf("%d %d", &worldXPos, &worldYPos);

      if (validateWorldMove(worldXPos, worldYPos))
      {
        if (mapArray[worldYPos][worldXPos] == NULL)
        {

          //These four if statements gets my exits to use as parameters to
          //  generate my next map, checks the four possible maps around it.
          //This current north exit would equal the south exit of the map above it.
          //The south exit would equal the north exit of the map below it.
          //... and so forth
          if (validateWorldMove(worldXPos, worldYPos + 1))
          {
            if (mapArray[worldYPos + 1][worldXPos] != NULL)
            {
              north = mapArray[worldYPos + 1][worldXPos]->southExit;
            }
          }
          if (validateWorldMove(worldXPos, worldYPos - 1))
          {
            if (mapArray[worldYPos - 1][worldXPos] != NULL)
            {
              south = mapArray[worldYPos - 1][worldXPos]->northExit;
            }
          }
          if (validateWorldMove(worldXPos + 1, worldYPos))
          {
            if (mapArray[worldYPos][worldXPos + 1] != NULL)
            {
              east = mapArray[worldYPos][worldXPos + 1]->westExit;
            }
          }
          if (validateWorldMove(worldXPos - 1, worldYPos))
          {
            if (mapArray[worldYPos][worldXPos - 1] != NULL)
            {
              west = mapArray[worldYPos][worldXPos - 1]->eastExit;
            }
          }

          //Creation of the new map
          map_t *newMap = malloc(sizeof(map_t));
          generateMap(newMap, north, east, south, west);

          //Checks world borders, fills edge with rock if at border
          if (worldXPos == 0)
            newMap->map[newMap->westExit][0] = ter_border;
          if (worldXPos == 398)
            newMap->map[newMap->eastExit][79] = ter_border;
          if (worldYPos == 0)
            newMap->map[0][newMap->northExit] = ter_border;
          if (worldYPos == 398)
            newMap->map[20][newMap->southExit] = ter_border;

          mapArray[worldYPos][worldXPos] = newMap;
          dijkstra(hikerMap, newMap);
          dijkstra(rivalMap, newMap);
          printMap(mapArray[worldYPos][worldXPos]);
          printf("X Y (%d %d)\n", worldXPos, worldYPos);
        }
        else
        {
          dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
          dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
          printMap(mapArray[worldYPos][worldXPos]);
          printf("X Y (%d %d)\n", worldXPos, worldYPos);
        }
      }
      else
      {
        worldYPos = prevYPos;
        worldXPos = prevXPos;

        dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
        dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
        printMap(mapArray[worldYPos][worldXPos]);
        printf("You fell off the world!\n");
        printf("X Y (%d %d)\n", worldXPos, worldYPos);
      }
    }

    if (nextMove == 2)
    {
      playing = false;
    }
  }

  //Done playing, dealloacate memory
  for (int i = 0; i < WORLD_DIM; i++)
  {
    for (int j = 0; j < WORLD_DIM; j++)
    {
      if (mapArray[i][j] != NULL)
      {
        //Not sure if I need to free my character array or if freeing the entire map
        // will take care of that. But have it added here for now
        free(mapArray[i][j]->characterArray);
        free(mapArray[i][j]);
      }
    }
  }
  free(hikerMap);
  free(rivalMap);
}

int userMove(char userInput)
{

  prevYPos = worldYPos;
  prevXPos = worldXPos;

  switch (userInput)
  {
  case ('n'):
    worldYPos--;
    return 0;
  case ('s'):
    worldYPos++;
    return 0;
  case ('e'):
    worldXPos++;
    return 0;
  case ('w'):
    worldXPos--;
    return 0;
  case ('f'):
    return 1;
  case ('q'):
    return 2;
  default:
    return -1;
  }
}

//Makes sure no moves go off the world 0-398 since 399x399 2D array
bool validateWorldMove(int x, int y)
{
  return (x >= 0 && x < 399 && y >= 0 && y < 399);
}

int main(int argc, char *argv[])
{

  uint32_t seed;

  //Checks for command lind arguments
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++)
    {
      if (!strcmp(argv[i], "--numtrainers"))
      {
        numTrainers = atoi(argv[i + 1]);
      }
    }
  }

  map_t *map = malloc(sizeof(map_t));
  seed = time(NULL);
  srand(seed);
  //srand(1646718313);
  printf("Using Seed: %u\n", seed);

  generateMap(map, -1, -1, -1, -1);
  createWorld(map);

  return 0;
}
