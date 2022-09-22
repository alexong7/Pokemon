# Pokemon
Created throughout the Spring 2022 semester for CS327

Core implementation of the popular Pokémon video game playable in the terminal!

Players start a choice of any three random Pokémon (Yes, a Mewtwo can be your starter).
The Pokédex data is pulled from over 15 versions of Pokémon titles.

# Characters

### Player - P
As the the player, roam through each map and travel to new maps in the world by following the exit paths.
Battle trainers and defeat all their Pokémon to win!
You'll probably want to collect some Pokémon of your own however; do this by entering tall grass `:`
Heal any Pokemon by entering the PokéCenters `C` and buy items at the PokéMarts `M`!

## Trainers

### Hiker - H
Can travel over any terrain and will chase the Player using Dijkstra's algorithm to calculate the cost of each walking through each terrain.

### Rival - R
Can travel over any terrain and will chase the Player using Dijkstra's algorithm to calculate the cost of each walking through each terrain.

### Pacer - P
Walks only left to right or up and down until it hits a boundary and can only stay in the terrain biome it spawned on (grass, forest, etc)

### Wanderer - W
Similar to pacer but can traverse to other terrain biomes

### Stationary - S
Stays in one spot

### Random Walker - N
Will walk in any direction one way until it hits a boundary, then will walk in one of the other three directions.
Can traverse any biome 


