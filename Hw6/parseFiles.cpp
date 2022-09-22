#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "pokeWorld.h"

class Pokemon
{
public:
    int id, species_id, height, weight, base_experience, order, is_default;
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
    }

    // ~Pokemon();
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
/*id,identifier,generation_id,evolves_from_species_id,evolution_chain_id,color_id,shape_id,habitat_id,gender_rate,
capture_rate,base_happiness,is_baby,hatch_counter,has_gender_differences,growth_rate_id,forms_switchable,
is_legendary,is_mythical,order,conquest_order */

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

std::vector<Pokemon> pokemonVector;
std::vector<Moves> movesVector;
std::vector<PokemonMoves> pokemonMovesVector;
std::vector<PokemonSpecies> pokemonSpeciesVector;
std::vector<Experience> experienceVector;
std::vector<TypeNames> typeNamesVector;

void parsePokemon(std::string filepath)
{
    filepath.append("pokemon.csv");
    std::ifstream file;

    file.open(filepath);

    std::string line;

    std::string id, identifier, species_id, height, weight, base_experience, order, is_default;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, id, ','))
    {
        std::getline(file, identifier, ',');
        std::getline(file, species_id, ',');
        std::getline(file, height, ',');
        std::getline(file, weight, ',');
        std::getline(file, base_experience, ',');
        std::getline(file, order, ',');
        std::getline(file, is_default, '\n');

        std::cout << id << "," << identifier << "," << species_id << "," << height << "," << weight << "," << base_experience << ","
                  << order << "," << is_default << std::endl;

        // Which way do I call this object
        // Pokemon temp(id, identifier, species_id, height, weight, base_experience, order, is_default);
        // Pokemon *temp = new Pokemon (id, identifier, species_id, height, weight, base_experience, order, is_default);

        Pokemon temp = Pokemon(std::stoi(id), identifier, std::stoi(species_id), std::stoi(height),
                               std::stoi(weight), std::stoi(base_experience), std::stoi(order), std::stoi(is_default));
        pokemonVector.push_back(temp);
    }
}

void parseMoves(std::string filepath)
{
    filepath.append("moves.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string id, identifier, generation_id, type_id, power, pp, accuracy, priority, target_id, damage_class_id,
        effect_id, effect_chance, contest_type_id, contest_effect_id, super_contest_effect_id;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, id, ','))
    {
        std::getline(file, identifier, ',');
        std::getline(file, generation_id, ',');
        std::getline(file, type_id, ',');
        std::getline(file, power, ',');
        std::getline(file, pp, ',');
        std::getline(file, accuracy, ',');
        std::getline(file, priority, ',');
        std::getline(file, target_id, ',');
        std::getline(file, damage_class_id, ',');
        std::getline(file, effect_id, ',');
        std::getline(file, effect_chance, ',');
        std::getline(file, contest_type_id, ',');
        std::getline(file, contest_effect_id, ',');
        std::getline(file, super_contest_effect_id, '\n');

        // Print
        std::cout << id << "," << identifier << "," << generation_id << "," << type_id << "," << power << "," << pp << "," << accuracy
                  << "," << priority << "," << target_id << "," << damage_class_id << "," << effect_id << "," << effect_chance << ","
                  << contest_type_id << "," << contest_effect_id << "," << super_contest_effect_id << std::endl;

        // Chekcs for any empty fields, sets to -1
        if (!generation_id.length())
            generation_id = "-1";
        if (!type_id.length())
            type_id = "-1";
        if (!power.length())
            power = "-1";
        if (!pp.length())
            pp = "-1";
        if (!accuracy.length())
            accuracy = "-1";
        if (!priority.length())
            priority = "-1";
        if (!target_id.length())
            target_id = "-1";
        if (!damage_class_id.length())
            damage_class_id = "-1";
        if (!effect_id.length())
            effect_id = "-1";
        if (!effect_chance.length())
            effect_chance = "-1";
        if (!contest_type_id.length())
            contest_type_id = "-1";
        if (!contest_effect_id.length())
            contest_effect_id = "-1";
        if (!super_contest_effect_id.length())
            super_contest_effect_id = "-1";

        // Which way do I call this object
        // Pokemon temp(id, identifier, species_id, height, weight, base_experience, order, is_default);
        // Pokemon *temp = new Pokemon (id, identifier, species_id, height, weight, base_experience, order, is_default);

        Moves temp = Moves(std::stoi(id), identifier, std::stoi(generation_id), std::stoi(type_id),
                           std::stoi(power), std::stoi(pp), std::stoi(accuracy), std::stoi(priority), std::stoi(target_id),
                           std::stoi(damage_class_id), std::stoi(effect_id), std::stoi(effect_chance), std::stoi(contest_type_id),
                           std::stoi(contest_effect_id), std::stoi(super_contest_effect_id));
        movesVector.push_back(temp);
    }
}

void parsePokemonMoves(std::string filepath)
{
    filepath.append("pokemon_moves.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string pokemon_id, version_group_id, move_id, pokemon_move_method_id, level, order;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, pokemon_id, ','))
    {
        std::getline(file, version_group_id, ',');
        std::getline(file, move_id, ',');
        std::getline(file, pokemon_move_method_id, ',');
        std::getline(file, level, ',');
        std::getline(file, order, '\n');

        // Print
        std::cout << pokemon_id << "," << version_group_id << "," << move_id << "," << pokemon_move_method_id << "," << level
                  << "," << order << std::endl;

        // Pokemon_moves.csv only has empty fields on the order position
        if (!order.length())
            order = "-1";

        PokemonMoves temp = PokemonMoves(std::stoi(pokemon_id), std::stoi(version_group_id), std::stoi(move_id),
                                         std::stoi(pokemon_move_method_id), std::stoi(level), std::stoi(order));
        pokemonMovesVector.push_back(temp);
    }
}

void parsePokemonSpecies(std::string filepath)
{
    filepath.append("pokemon_species.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string id, identifier, generation_id, evolves_from_species_id, evolution_chain_id, color_id, shape_id, habitat_id, gender_rate,
        capture_rate, base_happiness, is_baby, hatch_counter, has_gender_differences, growth_rate_id, forms_switchable, is_legendary,
        is_mythical, order, conquest_order;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, id, ','))
    {
        std::getline(file, identifier, ',');
        std::getline(file, generation_id, ',');
        std::getline(file, evolves_from_species_id, ',');
        std::getline(file, evolution_chain_id, ',');
        std::getline(file, color_id, ',');
        std::getline(file, shape_id, ',');
        std::getline(file, habitat_id, ',');
        std::getline(file, gender_rate, ',');
        std::getline(file, capture_rate, ',');
        std::getline(file, base_happiness, ',');
        std::getline(file, is_baby, ',');
        std::getline(file, hatch_counter, ',');
        std::getline(file, has_gender_differences, ',');
        std::getline(file, growth_rate_id, ',');
        std::getline(file, forms_switchable, ',');
        std::getline(file, is_legendary, ',');
        std::getline(file, is_mythical, ',');
        std::getline(file, order, ',');
        std::getline(file, conquest_order, '\n');

        // Print
        std::cout << id << "," << identifier << "," << generation_id << "," << evolves_from_species_id << "," << color_id << "," << shape_id
                  << "," << habitat_id << "," << gender_rate << "," << capture_rate << "," << base_happiness << "," << is_baby << ","
                  << hatch_counter << "," << has_gender_differences << "," << growth_rate_id << "," << forms_switchable << ","
                  << is_legendary << "," << is_mythical << "," << order << "," << conquest_order << std::endl;

        // These are the only fields that go empty in this CSV file
        if (!evolves_from_species_id.length())
            evolves_from_species_id = "-1";
        if (!habitat_id.length())
            habitat_id = "-1";
        if (!shape_id.length())
            shape_id = "-1";
        if (!conquest_order.length())
            conquest_order = "-1";

        PokemonSpecies temp = PokemonSpecies(std::stoi(id), identifier, std::stoi(generation_id), std::stoi(evolves_from_species_id),
                                             std::stoi(evolution_chain_id), std::stoi(color_id), std::stoi(shape_id), std::stoi(habitat_id),
                                             std::stoi(gender_rate), std::stoi(capture_rate), std::stoi(base_happiness), std::stoi(is_baby),
                                             std::stoi(hatch_counter), std::stoi(has_gender_differences), std::stoi(growth_rate_id),
                                             std::stoi(forms_switchable), std::stoi(is_legendary), std::stoi(is_mythical), std::stoi(order),
                                             std::stoi(conquest_order));
        pokemonSpeciesVector.push_back(temp);
    }
}

void parseExperience(std::string filepath)
{
    filepath.append("experience.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string growth_rate_id, level, experience;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, growth_rate_id, ','))
    {
        std::getline(file, level, ',');
        std::getline(file, experience, '\n');

        std::cout << growth_rate_id << "," << level << "," << experience << std::endl;

        Experience temp = Experience(std::stoi(growth_rate_id), std::stoi(level), std::stoi(experience));
        experienceVector.push_back(temp);
    }
}

void parseTypeNames(std::string filepath)
{
    filepath.append("type_names.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string type_id, local_language_id, name;

    // Gets rid of first line
    std::getline(file, line);
    std::cout << line << std::endl;

    while (std::getline(file, type_id, ','))
    {
        std::getline(file, local_language_id, ',');
        std::getline(file, name, '\n');

        // Checks for English lines only (local_language_id == 9)
        if (std::stoi(local_language_id) != 9)
            continue;

        std::cout << type_id << "," << local_language_id << "," << name << std::endl;

        TypeNames temp = TypeNames(std::stoi(type_id), std::stoi(local_language_id), name);
        typeNamesVector.push_back(temp);
    }
}