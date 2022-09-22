#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "pokeWorld.h"
#include "parseFiles.h"

std::vector<Pokemon> pokemonVector;
std::vector<Moves> movesVector;
std::vector<PokemonMoves> pokemonMovesVector;
std::vector<PokemonSpecies> pokemonSpeciesVector;
std::vector<Experience> experienceVector;
std::vector<TypeNames> typeNamesVector;
std::vector<PokemonStats> pokemonStatsVector;

void parsePokemon(std::string filepath)
{
    filepath.append("pokemon.csv");
    std::ifstream file;

    file.open(filepath);

    std::string line;

    std::string id, identifier, species_id, height, weight, base_experience, order, is_default;

    // Gets rid of first line
    std::getline(file, line);
    // std::cout << line << std::endl;

    while (std::getline(file, id, ','))
    {
        std::getline(file, identifier, ',');
        std::getline(file, species_id, ',');
        std::getline(file, height, ',');
        std::getline(file, weight, ',');
        std::getline(file, base_experience, ',');
        std::getline(file, order, ',');
        std::getline(file, is_default, '\n');

        // std::cout << id << "," << identifier << "," << species_id << "," << height << "," << weight << "," << base_experience << ","
        //           << order << "," << is_default << std::endl;

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
    // std::cout << line << std::endl;

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
        // std::cout << id << "," << identifier << "," << generation_id << "," << type_id << "," << power << "," << pp << "," << accuracy
        //           << "," << priority << "," << target_id << "," << damage_class_id << "," << effect_id << "," << effect_chance << ","
        //           << contest_type_id << "," << contest_effect_id << "," << super_contest_effect_id << std::endl;

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
    // std::cout << line << std::endl;

    while (std::getline(file, pokemon_id, ','))
    {
        std::getline(file, version_group_id, ',');
        std::getline(file, move_id, ',');
        std::getline(file, pokemon_move_method_id, ',');
        std::getline(file, level, ',');
        std::getline(file, order, '\n');

        // Only pass Pokemon Moves from Game Version 20
        if (std::stoi(version_group_id) != 20)
        {
            continue;
        }

        // Print
        // std::cout << pokemon_id << "," << version_group_id << "," << move_id << "," << pokemon_move_method_id << "," << level
        //           << "," << order << std::endl;

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
    // std::cout << line << std::endl;

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
        // std::cout << id << "," << identifier << "," << generation_id << "," << evolves_from_species_id << "," << color_id << "," << shape_id
        //           << "," << habitat_id << "," << gender_rate << "," << capture_rate << "," << base_happiness << "," << is_baby << ","
        //           << hatch_counter << "," << has_gender_differences << "," << growth_rate_id << "," << forms_switchable << ","
        //           << is_legendary << "," << is_mythical << "," << order << "," << conquest_order << std::endl;

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
    // std::cout << line << std::endl;

    while (std::getline(file, growth_rate_id, ','))
    {
        std::getline(file, level, ',');
        std::getline(file, experience, '\n');

        // std::cout << growth_rate_id << "," << level << "," << experience << std::endl;

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
    // std::cout << line << std::endl;

    while (std::getline(file, type_id, ','))
    {
        std::getline(file, local_language_id, ',');
        std::getline(file, name, '\n');

        // Checks for English lines only (local_language_id == 9)
        if (std::stoi(local_language_id) != 9)
            continue;

        // std::cout << type_id << "," << local_language_id << "," << name << std::endl;

        TypeNames temp = TypeNames(std::stoi(type_id), std::stoi(local_language_id), name);
        typeNamesVector.push_back(temp);
    }
}

void parsePokemonStats(std::string filepath)
{
    filepath.append("pokemon_stats.csv");
    std::ifstream file;
    std::string line;
    file.open(filepath);

    std::string pokemon_id, stat_id, base_stat, effort;

    // Gets rid of first line
    std::getline(file, line);
    // std::cout << line << std::endl;

    while (std::getline(file, pokemon_id, ','))
    {
        std::getline(file, stat_id, ',');
        std::getline(file, base_stat, ',');
        std::getline(file, effort, '\n');

        // std::cout << pokemon_id << "," << stat_id << "," << base_stat << "," << effort << std::endl;

        PokemonStats temp = PokemonStats(std::stoi(pokemon_id), std::stoi(stat_id), std::stoi(base_stat), std::stoi(effort));
        pokemonStatsVector.push_back(temp);
    }
}