#ifndef PARSEFILES_H
#define PARSEFILES_H

#include "pokeWorld.h"

class Pokemon;
class Moves;
class PokemonMoves;
class PokemonSpecies;
class Experience;
class TypeNames;

void parsePokemon(std::string filepath);
void parseMoves(std::string filepath);
void parsePokemonMoves(std::string filepath);
void parsePokemonSpecies(std::string filepath);
void parseExperience(std::string filepath);
void parseTypeNames(std::string filepath);

#endif
