#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "heap.h"



#define WORLD_DIM 399
#define MAP_Y 21
#define MAP_X 80

typedef enum terrainType{

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
  ter_player,

} terType_t;

typedef enum NPC_Title{
 hiker,
 rival,
} npcTag_t;

typedef struct distanceMapCell{
  int cost;
  bool visited;
  heap_node_t *hn;
  int xPos, yPos;
}distMapCell_t;


typedef struct distanceMap{
  distMapCell_t distMap[MAP_Y][MAP_X];
  npcTag_t npcType;

} distMap_t;


typedef struct map{

  terType_t map[MAP_Y][MAP_X];
  int northExit;
  int southExit;
  int westExit;
  int eastExit;

} map_t;

typedef struct playerCharacter{
  int posX, posY;
 } pc_t;

//Global player variable
pc_t player;

int userMove(char userInput);
bool validateMove(int x, int y);
bool validMove(int x, int y);

//Global variabels for X and Y positions
int worldXPos = 199;
int worldYPos = 199;

//Global variables to track previous X & Y position.
//Useful for cases where player leaves world boundaries
int prevXPos = 199;
int prevYPos = 199;

//Creates a 2D World array of 399x399. Initalizes all to NULL
map_t *mapArray[WORLD_DIM][WORLD_DIM];

int32_t distMap_cmp(const void *key, const void *with) {
  return ((distMapCell_t *) key)->cost - ((distMapCell_t *) with)->cost;
}

//Function to determine terrain cell cost
int pathCost(terType_t terrain, npcTag_t npcTag){
  if(npcTag == hiker){
    switch(terrain){
    case ter_rock:
      return 15;
    case ter_tree:
      return 15;
    case ter_road:
      return 10;
    case ter_clearing:
      return 10;
    case ter_grassTall:
      return 15;
    default:
      return INT_MAX;
   }
  }

   if(npcTag == rival){
    switch(terrain){
    case ter_road:
      return 10;
    case ter_clearing:
      return 10;
    case ter_grassTall:
      return 20;
    default:
      return INT_MAX;
    }
   }

   return INT_MAX;
}

  void dijkstra(distMap_t *distMap, map_t *terrainMap){

  int x, y;
  int xNeighbor[] = {0, 1, 1, 1, 0, -1, -1, -1};
  int yNeighbor[] = {1, 1, 0, -1, -1, -1, 0, 1};
  heap_t h;
  distMapCell_t *d;


  //Initializes the X and Y positions
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      distMap->distMap[y][x].yPos = y;
      distMap->distMap[y][x].xPos = x;
    }
  }

  //Initalizes all nodes to cost infinity
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      distMap->distMap[y][x].cost = INT_MAX;
    }
  }

  //Source node based on player position set to cost 0
  distMap->distMap[player.posY][player.posX].cost = 0;

  heap_init(&h, distMap_cmp, NULL);

  //Insert all nodes to a min heap besides borders
  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
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
  while(h.size){

    d = heap_remove_min(&h);

    if(d->cost == INT_MAX){
      continue;
    }

    for(int i = 0; i < 8; i++){
      if(distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].hn){
        if(pathCost(terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]], distMap->npcType) != INT_MAX){
    	   if(distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].cost >
    	      d->cost + pathCost(terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]], distMap->npcType)){
    		        int newCost = d->cost + pathCost((terrainMap->map[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]]),
    						 distMap->npcType);
            		distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].cost = newCost;
            		heap_decrease_key_no_replace(&h, distMap->distMap[d->yPos + yNeighbor[i]][d->xPos + xNeighbor[i]].hn);
         }
        }
       }
    }
  }


  for(y = 0; y < MAP_Y; y++){
    for(x = 0; x < MAP_X; x++){
      if(distMap->distMap[y][x].cost != INT_MAX){
	       printf("%02d", distMap->distMap[y][x].cost % 100);
      }
      else{
	       printf("  ");
      }
      printf(" ");
    }
    printf("\n");
  }


}

void printMap(map_t *map){

  int x, y;

  for(y = 0; y < MAP_Y; y++){
    for(x = 0; x < MAP_X; x++){
      switch(map->map[y][x]){
	case ter_road:
        case ter_exit:
	  printf("#");
	  break;
	case ter_rock:
        case ter_border:
	  printf("%%");
	  break;
	case ter_grassTall:
	  printf(":");
	  break;
	case ter_tree:
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
        case ter_player:
	  printf("@");
	  break;
      }
    }
    printf("\n");
  }
}

bool  isBorder(int x, int y){
  return (x == 0 || x == 79 || y == 0 || y == 20);
}

bool validMove(int x, int y){
  return (x > 0 && x < 79 && y > 0 && y < 20);
}

void generatePath(map_t *map, int north, int east, int south, int west){
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
     if(north != -1)
     	randX1 = north;
     if(south != -1)
     	randX2 = south;
     if(west != -1)
     	randY1 = west;
     if(east != -1)
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
  for(int i = 0; i < tempPathX; i++){
    map->map[randY1][i] = ter_road;
  }
  for(int k = 78; k >= tempPathX; k--){
    map->map[randY2][k] = ter_road;
  }

  //Now connect the two horizontal paths vertically. Find out which
  // path is lower
  int largerYCord;
  int smallerYCord;
     if(randY1 > randY2){
       largerYCord = randY1;
       smallerYCord = randY2;
     }
     else{
       largerYCord = randY2;
       smallerYCord = randY1;
     }
  for(int k = smallerYCord; k < largerYCord; k++){
    map->map[k][tempPathX] = ter_road;
  }

  //North -> South Path, similar process
  int tempPathY = rand() % 19 + 1;
  map->map[tempPathY][randX1] = ter_road;
  for(int n = 0; n < tempPathY; n++){
    map->map[n][randX1] = ter_road;
  }
  for(int j = 19; j >= tempPathY; j--){
    map->map[j][randX2] = ter_road;
  }

  //Connect the two vertical paths horizontally
  int largerXCord;
  int smallerXCord;
  if(randX1 > randX2){
       largerXCord = randX1;
       smallerXCord = randX2;
     }
     else{
       largerXCord = randX2;
       smallerXCord = randX1;
     }
  for(int m = smallerXCord; m < largerXCord; m++){
    map->map[tempPathY][m] = ter_road;
  }

 }

void addCenterAndMart(map_t *map){
   //Add Poke Center and Poke Mart
   bool validPokeCenter = false;
   bool validPokeMart = false;
   int neighborsX[] = {1, 0, -1, 0}; //Allows us to the check the left, right, up, down
   int neighborsY[] = {0, -1, 0, 1}; // neighbors of a path cell

   int manhattanDistance = abs(worldXPos - 199) + abs(worldYPos - 199);
   double probability =  (double)(((-45*manhattanDistance) / 200) + 50) / 100;

   //Flat %5 chance
   if(probability < 0.05)
     probability = 0.05;
   //Distance 0 = center, always spawn
   if(manhattanDistance == 0)
     probability = 1.0;


   //Converts to a whole number
   probability *= 100;

   int spawnChanceCenter = rand() % 100;
   int spawnChanceMart = rand() % 100;

   //If the generator has a number from 0 - the probabiltiy, building will spawn
   if(spawnChanceCenter >= 0 && spawnChanceCenter < probability){
     //Loops through until we find a random path cell, check to see if we can add a
     // PokeCenter or PokeMart next to the path
     while(!validPokeCenter){
       int randX = rand() % 78 + 1;
       int randY = rand() % 19 + 1;
       if(map->map[randY][randX] == ter_road){
	 for(int i = 0; i < 3; i++){
	   //Every spot that is not a border or path is filled with a clearing up until
	   // this point. So if the current spot we are evaluating is a clearing, we
	   // can add a Poke Center here.
	   if(map->map[randY + neighborsY[i]][randX + neighborsX[i]] == ter_clearing){
	     map->map[randY + neighborsY[i]][randX + neighborsX[i]] = ter_pokeCenter;
	     validPokeCenter = true;
	     break;
	   }
	 }
       }
     }
   }
   if(spawnChanceMart >= 0 && spawnChanceMart < probability){
     while(!validPokeMart){
       int randX2 = rand() % 78 + 1;
       int randY2 = rand() % 19 + 1;
       if(map->map[randY2][randX2] == ter_road){
	 for(int i = 0; i < 3; i++){
	   if(map->map[randY2 + neighborsY[i]][randX2 + neighborsX[i]] == ter_clearing){
	     map->map[randY2 + neighborsY[i]][randX2 + neighborsX[i]] = ter_pokeMart;
	     validPokeMart = true;
	     break;
	   }
	 }
       }
     }
   }
}

void addGrass(map_t *map){
  int xCord = rand() % 78 + 1;
  int yCord = rand() % 19 + 1;
  int xCord2 = rand() % 78 + 1;
  int yCord2 = rand() % 19 + 1;
  int xCord3 = rand() % 78 + 1;
  int yCord3 = rand() % 19 + 1;

  //Same three for loops to create three regions (did three to reduce the chance of regions
  // merging into one big grass region)
  for(int i = 0; i < rand() % 6 + 3; i++){
    for(int k = 0; k < rand() % 15 + 5; k++){
      if(validMove(xCord + k, yCord + i) && map->map[yCord+i][xCord+k] == ter_clearing){
	map->map[yCord + i][xCord + k] = ter_grassTall;
      }
    }
  }

  for(int m = 0; m < rand() % 6 + 3; m++){
    for(int n = 0; n < rand() % 15 + 5; n++){
      if(validMove(xCord2 + n, yCord2 + m) && map->map[yCord2+m][xCord2+n] == ter_clearing){
	map->map[yCord2 + m][xCord2 + n] = ter_grassTall;
      }
    }
  }

  for(int m = 0; m < rand() % 6 + 3; m++){
    for(int n = 0; n < rand() % 15 + 5; n++){
      if(validMove(xCord3 + n, yCord3 + m) && map->map[yCord3+m][xCord3+n] == ter_clearing){
	map->map[yCord3 + m][xCord3 + n] = ter_grassTall;
      }
    }
  }


}


void addTrees(map_t *map){
  int numTrees = rand() % 21 + 20; //Generates 20-40 trees

  for(int i = 0; i < numTrees; i++){
    int randX = rand() % 78 + 1;
    int randY = rand() % 19 + 1;
    if(map->map[randY][randX] == ter_clearing){
      map->map[randY][randX] = ter_tree;
    }

  }
}

void addRocks(map_t * map){

  int numRocks = rand() % 21 + 20; //Generates 20-40 Rocks

  for(int i = 0; i < numRocks; i++){
    int randX = rand() % 78 + 1;
    int randY = rand() % 19 + 1;
    if(map->map[randY][randX] == ter_clearing){
      map->map[randY][randX] = ter_rock;
    }
  }

}

void generateMap(map_t *map, int north, int east, int south, int west){
  int NS_Border, WE_Border;

  for(NS_Border = 0; NS_Border < MAP_Y; NS_Border++){
    for(WE_Border = 0; WE_Border < MAP_X; WE_Border++){
      if(isBorder(WE_Border, NS_Border)){
	map->map[NS_Border][WE_Border] = ter_border;
      }
      else{
	//fills in every other cell that is not a border with clearings
	map->map[NS_Border][WE_Border] = ter_clearing;
      }
    }
  }

  generatePath(map, north, east, south, west);
  addCenterAndMart(map);
  addGrass(map);
  addTrees(map);
  addRocks(map);

  while(1){
    player.posX = rand() % 78 + 1;
    player.posY = rand() % 19 + 1;

    if(map->map[player.posY][player.posX] == ter_road){
      map->map[player.posY][player.posX] = ter_player;
      break;
    }
  }

}


void createWorld(map_t *startingMap){


  for(int i = 0; i < WORLD_DIM; i++){
    for(int j = 0; j < WORLD_DIM; j++){
      mapArray[i][j] = NULL;
    }
  }

  mapArray[worldYPos][worldXPos] = startingMap;
  distMap_t *hikerMap = malloc(sizeof(distMap_t));
  hikerMap->npcType = hiker;

  distMap_t *rivalMap = malloc(sizeof(distMap_t));
  rivalMap->npcType = rival;

  dijkstra(hikerMap, startingMap);
  dijkstra(rivalMap, startingMap);
  printMap(mapArray[worldYPos][worldXPos]);
  printf("X Y (%d %d)\n", worldXPos, worldYPos);




  bool playing = true;

  //Loops through userInput, calls userMove function
  //3 Outcomes depending on what userMove returns (set to nextMove)
  // Return 0 = A NESW move, check bounds and if a map is there or not.
  //   creates one if no map exists
  // Return 1 = Fly to a new map, create a map if one doesn't exist
  // Return 2 = Quit
  while(playing){
    char moveInput;
    int north = -1;
    int east =  -1;
    int south = -1;
    int west = -1;

    printf("Enter Move (n, e, s, w, f, q): \n");
    scanf(" %c", &moveInput);
    int nextMove = userMove(moveInput);

    if(nextMove == 0){
      //Checks if nextMove does not fall off world
      if(validateMove(worldXPos, worldYPos)){

	//Creates new map if we travel to a new cell
	if(mapArray[worldYPos][worldXPos] == NULL){

	  //These four if statements gets my exits to use as parameters to
	  //  generate my next map, checks the four possible maps around it.
	  //This current north exit would equal the south exit of the map above it.
	  //The south exit would equal the north exit of the map below it.
          //... and so forth
	  if(validateMove(worldXPos, worldYPos - 1)){
	    if(mapArray[worldYPos - 1][worldXPos] !=  NULL){
	      north = mapArray[worldYPos-1][worldXPos]->southExit;
	    }
	  }
	  if(validateMove(worldXPos, worldYPos + 1)){
	    if(mapArray[worldYPos+1][worldXPos] !=  NULL){
	      south = mapArray[worldYPos+1][worldXPos]->northExit;
	    }
	  }
	  if(validateMove(worldXPos + 1, worldYPos)){
	    if(mapArray[worldYPos][worldXPos+1] !=  NULL){
	      east = mapArray[worldYPos][worldXPos+1]->westExit;
	    }
	  }
	  if(validateMove(worldXPos - 1, worldYPos)){
	    if(mapArray[worldYPos][worldXPos-1] !=  NULL){
	      west = mapArray[worldYPos][worldXPos-1]->eastExit;
	    }
	  }

	  //creates the new map with those arguments above
	  map_t *newMap = malloc(sizeof(map_t));
	  generateMap(newMap, north, east, south, west);

	  //Checks world borders, fills edge in with all rock if at world edge
	    if(worldXPos == 0)
	      newMap->map[newMap->westExit][0] = ter_border;
	    if(worldXPos == 398)
	      newMap->map[newMap->eastExit][79] = ter_border;
	    if(worldYPos == 0)
	      newMap->map[0][newMap->northExit] = ter_border;
	    if(worldYPos == 398)
	      newMap->map[20][newMap->southExit] = ter_border;

	  mapArray[worldYPos][worldXPos] = newMap;
    dijkstra(hikerMap, newMap);
    dijkstra(rivalMap, newMap);
	  printMap(mapArray[worldYPos][worldXPos]);
	  printf("X Y (%d %d)\n", worldXPos, worldYPos);
	}
	//Prints already found map else wise
	else{
    dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
    dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
	  printMap(mapArray[worldYPos][worldXPos]);
	  printf("X Y (%d %d)\n", worldXPos, worldYPos);
	}
      }
      //If user falls off world (out of bounds), reprint previous map
      else{
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
    if(nextMove == 1){

      prevYPos = worldYPos;
      prevXPos = worldXPos;

      printf("Enter X Y: ");
      scanf("%d %d", &worldXPos, &worldYPos);


      if(validateMove(worldXPos, worldYPos)){
	  if(mapArray[worldYPos][worldXPos] == NULL){

	    //These four if statements gets my exits to use as parameters to
	    //  generate my next map, checks the four possible maps around it.
	    //This current north exit would equal the south exit of the map above it.
	    //The south exit would equal the north exit of the map below it.
	    //... and so forth
	    if(validateMove(worldXPos, worldYPos + 1)){
	      if(mapArray[worldYPos + 1][worldXPos] !=  NULL){
		north = mapArray[worldYPos+1][worldXPos]->southExit;
	      }
	    }
	    if(validateMove(worldXPos, worldYPos - 1)){
	      if(mapArray[worldYPos-1][worldXPos] !=  NULL){
		south = mapArray[worldYPos-1][worldXPos]->northExit;
	      }
	    }
	    if(validateMove(worldXPos + 1, worldYPos)){
	      if(mapArray[worldYPos][worldXPos+1] !=  NULL){
		east = mapArray[worldYPos][worldXPos+1]->westExit;
	      }
	    }
	    if(validateMove(worldXPos - 1, worldYPos)){
	      if(mapArray[worldYPos][worldXPos-1] !=  NULL){
		west = mapArray[worldYPos][worldXPos-1]->eastExit;
	      }
	    }

	    //Creation of the new map
	    map_t *newMap = malloc(sizeof(map_t));
	    generateMap(newMap, north, east, south, west);

	    //Checks world borders, fills edge with rock if at border
	    if(worldXPos == 0)
	      newMap->map[newMap->westExit][0] = ter_border;
	    if(worldXPos == 398)
	      newMap->map[newMap->eastExit][79] = ter_border;
	    if(worldYPos == 0)
	      newMap->map[0][newMap->northExit] = ter_border;
	    if(worldYPos == 398)
	      newMap->map[20][newMap->southExit] = ter_border;

	    mapArray[worldYPos][worldXPos] = newMap;
      dijkstra(hikerMap, newMap);
      dijkstra(rivalMap, newMap);
	    printMap(mapArray[worldYPos][worldXPos]);
	    printf("X Y (%d %d)\n", worldXPos, worldYPos);
	  }
	  else{
      dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
      dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
	    printMap(mapArray[worldYPos][worldXPos]);
	    printf("X Y (%d %d)\n", worldXPos, worldYPos);
	  }
      }
      else{
	worldYPos = prevYPos;
	worldXPos = prevXPos;

  dijkstra(hikerMap, mapArray[worldYPos][worldXPos]);
  dijkstra(rivalMap, mapArray[worldYPos][worldXPos]);
	printMap(mapArray[worldYPos][worldXPos]);
	printf("You fell off the world!\n");
	printf("X Y (%d %d)\n", worldXPos, worldYPos);
      }
    }

    if(nextMove == 2){
      playing = false;
    }
  }

  //Done playing, dealloacate memory
  for(int i = 0; i < WORLD_DIM; i++){
    for(int j = 0; j < WORLD_DIM; j++){
      if(mapArray[i][j] != NULL){
	       free(mapArray[i][j]);
      }
    }
  }
  free(hikerMap);
  free(rivalMap);
}

int userMove(char userInput){

  prevYPos = worldYPos;
  prevXPos = worldXPos;

  switch(userInput){
  case('n'):
    worldYPos--;
    return 0;
  case('s'):
    worldYPos++;
    return 0;
  case('e'):
    worldXPos++;
    return 0;
  case('w'):
    worldXPos--;
    return 0;
  case('f'):
    return 1;
  case('q'):
    return 2;
  default:
    return -1;
  }

}

//Makes sure no moves go off the world 0-398 since 399x399 2D array
bool validateMove(int x, int y){
  return (x >= 0 && x < 399 && y >= 0 && y < 399);
}

int main(int argc, char *argv[]){

  map_t *map = malloc(sizeof(map_t));
  //srand(time(NULL));
  srand(100);

  generateMap(map, -1, -1, -1, -1);
  createWorld(map);


    return 0;
}
