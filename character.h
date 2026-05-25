#ifndef CHARACTER_H
# define CHARACTER_H

# include <cstdint>
# include <vector>

# include "pair.h"
# include "db_parse.h"

#define DIJKSTRA_PATH_MAX (INT_MAX / 2)

typedef enum __attribute__ ((__packed__)) movement_type {
  move_hiker,
  move_rival,
  move_pace,
  move_wander,
  move_sentry,
  move_explore,
  move_swim,
  move_pc,
  num_movement_types
} movement_type_t;

typedef enum __attribute__ ((__packed__)) character_type {
  char_pc,
  char_hiker,
  char_rival,
  char_swimmer,
  char_other,
  num_character_types
} character_type_t;

extern const char *char_type_name[num_character_types];

typedef enum pokemon_type{
  type_normal,
  type_fighting,
  type_flying,
  type_poison,
  type_ground,
  type_rock,
  type_bug,
  type_ghost,
  type_steel,
  type_fire,
  type_water,
  type_grass,
  type_electric,
  type_psychic,
  type_ice,
  type_dragon,
  type_dark,
  type_fairy,
  num_pokemon_types,
  type_typeless,
  type_shadow,
  type_null
} pokemon_type_t;

typedef enum stats{
  stat_hp,
  stat_attack,
  stat_defense,
  stat_spatk,
  stat_spdef,
  stat_speed,
  num_stats
} stats_t;

class pokemon_move {
 public:
  char name[50];
  pokemon_type_t move_type;
  int power;
  int max_pp, current_pp;
  int priority;
  int accuracy;
};

class game_pokemon {
 public:
  char name[30];
  int species_id;
  std::vector<pokemon_move> moves;
  std::vector<pokemon_move_db> level_up_moves;
  pokemon_type_t types[2];
  int level;
  int currentStat[num_stats], baseStat[num_stats], iv[num_stats], battleStat[num_stats];
  char gender;
  int isShiny;
  double type_effectiveness[num_pokemon_types];
  int currentExp;
  int baseExp;
  int xp_growth_group;
  int leveled_last_battle;
  int evolves_into_id;
  int evolves_at_level;
};

class character {
 public:
  virtual ~character() {}
  pair_t pos;
  char symbol;
  int next_turn;
  int seq_num;
  std::vector<game_pokemon> team;
};

class npc : public character {
 public:
  character_type_t ctype;
  movement_type_t mtype;
  int defeated;
  pair_t dir;
};

class pc : public character {
  public:
    int num_revives, num_potions, num_p_balls;
};

/* character is defined in poke327.h to allow an instance of character
 * in world without including character.h in poke327.h                 */

int32_t cmp_char_turns(const void *key, const void *with);
void delete_character(void *v);

extern void (*move_func[num_movement_types])(character *, pair_t);

int pc_move(char);
bool is_pc(character *c);

#endif
