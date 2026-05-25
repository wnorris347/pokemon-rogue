#ifndef BATTLE_H
# define BATTLE_H

# include <vector>

class game_pokemon;

int trainer_battle_loop(std::vector<game_pokemon, std::allocator<game_pokemon>> npc_pokemon, char *trainer_name);
int wild_battle_loop(game_pokemon *wild);
void evolve(game_pokemon *p);

#endif