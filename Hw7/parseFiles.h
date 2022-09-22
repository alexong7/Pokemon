#ifndef PARSEFILES_H
#define PARSEFILES_H

#include "pokeWorld.h"
#include <vector>

class Pokemon
{
public:
    int id, species_id, height, weight, base_experience, order, is_default, level;
    std::string identifier;

    Pokemon(int id, std::string identifier, int species_id, int height, int weight, int base_experience, int order, int is_default)
    {
        this->id = id;
        this->identifier = identifier;
        this->species_id = species_id;
        this->height = height;
        this->weight = weight;
        this->base_experience = base_experience;
        this->order = order;
        this->is_default = is_default;
        level = -1;
    }
};

class Moves
{
public:
    int id, generation_id, type_id, power, pp, accuracy, priority, target_id, damage_class_id, effect_id, effect_chance,
        contest_type_id, contest_effect_id, super_contest_effect_id;
    std::string identifier;

    Moves(int id, std::string identifier, int generation_id, int type_id, int power, int pp, int accuracy, int priority, int target_id,
          int damage_class_id, int effect_id, int effect_chance, int contest_type_id, int contest_effect_id, int super_contest_effect_id)
    {
        this->id = id;
        this->identifier = identifier;
        this->generation_id = generation_id;
        this->type_id = type_id;
        this->power = power;
        this->pp = pp;
        this->accuracy = accuracy;
        this->priority = priority;
        this->target_id = target_id;
        this->damage_class_id = damage_class_id;
        this->effect_id = effect_id;
        this->effect_chance = effect_chance;
        this->contest_type_id = contest_type_id;
        this->contest_effect_id = contest_effect_id;
        this->super_contest_effect_id = super_contest_effect_id;
    }
};

class PokemonMoves
{
public:
    int pokemon_id, version_group_id, move_id, pokemon_move_method_id, level, order;

    PokemonMoves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order)
    {
        this->pokemon_id = pokemon_id;
        this->version_group_id = version_group_id;
        this->move_id = move_id;
        this->pokemon_move_method_id = pokemon_move_method_id;
        this->level = level;
        this->order = order;
    }
};

class PokemonSpecies
{
public:
    int id, generation_id, evolves_from_species_id, evolution_chain_id, color_id, shape_id, habitat_id, gender_rate, capture_rate,
        base_happiness, is_baby, hatch_counter, has_gender_differences, growth_rate_id, forms_switchable, is_legendary, is_mythical,
        order, conquest_order;
    std::string identifier;
    PokemonSpecies(int id, std::string identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id,
                   int color_id, int shape_id, int habitat_id, int gender_rate, int capture_rate, int base_happiness, int is_baby,
                   int hatch_counter, int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary,
                   int is_mythical, int order, int conquest_order)
    {
        this->id = id;
        this->identifier = identifier;
        this->generation_id = generation_id;
        this->evolves_from_species_id = evolves_from_species_id;
        this->evolution_chain_id = evolution_chain_id;
        this->color_id = color_id;
        this->shape_id = shape_id;
        this->habitat_id = habitat_id;
        this->gender_rate = gender_rate;
        this->capture_rate = capture_rate;
        this->base_happiness = base_happiness;
        this->is_baby = is_baby;
        this->hatch_counter = hatch_counter;
        this->has_gender_differences = has_gender_differences;
        this->growth_rate_id = growth_rate_id;
        this->forms_switchable = forms_switchable;
        this->is_legendary = is_legendary;
        this->is_mythical = is_mythical;
        this->order = order;
        this->conquest_order = conquest_order;
    }
};

class Experience
{
public:
    int growth_rate_id, level, experience;

    Experience(int growth_rate_id, int level, int experience)
    {
        this->growth_rate_id = growth_rate_id;
        this->level = level;
        this->experience = experience;
    }
};

class TypeNames
{
public:
    int type_id, local_language_id;
    std::string name;

    TypeNames(int type_id, int local_language_id, std::string name)
    {
        this->type_id = type_id;
        this->local_language_id = local_language_id;
        this->name = name;
    }
};

class PokemonStats
{
public:
    int pokemon_id, stat_id, base_stat, effort;

    PokemonStats(int pokemon_id, int stat_id, int base_stat, int effort)
    {
        this->pokemon_id = pokemon_id;
        this->stat_id = stat_id;
        this->base_stat = base_stat;
        this->effort = effort;
    }
};

extern std::vector<Pokemon> pokemonVector;
extern std::vector<Moves> movesVector;
extern std::vector<PokemonMoves> pokemonMovesVector;
extern std::vector<PokemonSpecies> pokemonSpeciesVector;
extern std::vector<Experience> experienceVector;
extern std::vector<TypeNames> typeNamesVector;
extern std::vector<PokemonStats> pokemonStatsVector;

void parsePokemon(std::string filepath);
void parseMoves(std::string filepath);
void parsePokemonMoves(std::string filepath);
void parsePokemonSpecies(std::string filepath);
void parseExperience(std::string filepath);
void parseTypeNames(std::string filepath);
void parsePokemonStats(std::string filepath);

#endif
