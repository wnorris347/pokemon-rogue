#include <unistd.h>
#include <ncurses.h>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <string>
#include <string.h>
#include <algorithm>
#include <cmath>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "battle.h"
#include "db_parse.h"

int select_move(game_pokemon p){
  char key, move_list[80], current_move[80];
  int i;

  sprintf(move_list, "1. %s (%d/%d) ", p.moves.at(0).name, p.moves.at(0).current_pp, p.moves.at(0).max_pp);


  for(i = 1; i < (int)p.moves.size(); i++){
    sprintf(current_move, "%d. %s (%d/%d) ", i + 1, p.moves.at(i).name, p.moves.at(i).current_pp, p.moves.at(i).max_pp);
    strcat(move_list, current_move);
  }

  strcat(move_list, "<: Back");

  while(1){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "%s", move_list);
    switch(key = getch()){
      case '1':
      if(p.moves.at(0).current_pp){
        return 0;
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "That move is out of PP!");
      }
      break;
      case '2':
      if(p.moves.size() >= 2){
        if(p.moves.at(1).current_pp){
          return 1;
        }else{
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "That move is out of PP!");
        }
      }
      break;
      case '3':
      if(p.moves.size() >= 3){
        if(p.moves.at(2).current_pp){
          return 2;
        }else{
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "That move is out of PP!");
        }
      }
      break;
      case '4':
      if(p.moves.size() == 4){
        if(p.moves.at(3).current_pp){
          return 3;
        }else{
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "That move is out of PP!");
        }
      }
      break;
      case '<':
      return -1;
      break;
      default:
      break;
    }
  }
}

int swap_pokemon(game_pokemon *active){
  int i;
  char key;
  char poke_list[120], next_poke[120];

  sprintf(poke_list, "1. %s (%d/%d) ", world.pc.team.at(0).name, world.pc.team.at(0).battleStat[stat_hp], world.pc.team.at(0).currentStat[stat_hp]);


  for(i = 1; i < (int)world.pc.team.size(); i++){
    sprintf(next_poke, "%d. %s (%d/%d) ", i + 1, world.pc.team.at(i).name, world.pc.team.at(i).battleStat[stat_hp], world.pc.team.at(i).currentStat[stat_hp]);
    strcat(poke_list, next_poke);
  }

  strcat(poke_list, "<: Back");

  while(1){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "%s", poke_list);
    switch(key = getch()){
      case '1':
      if(world.pc.team.at(0).battleStat[stat_hp] && &world.pc.team.at(0) != active){
        return 0;
      }else if(!world.pc.team.at(0).battleStat[stat_hp]){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(0).name);
        getch();
      }else if(&world.pc.team.at(0) == active){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(0).name);
        getch();
      }
      break;
      case '2':
      if(world.pc.team.size() >= 2){
        if(world.pc.team.at(1).battleStat[stat_hp] && &world.pc.team.at(1) != active){
          return 1;
        }else if(!world.pc.team.at(1).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(1).name);
          getch();
        }else if(&world.pc.team.at(1) == active){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(1).name);
          getch();
        }
      }
      break;
      case '3':
      if(world.pc.team.size() >= 3){
        if(world.pc.team.at(2).battleStat[stat_hp] && &world.pc.team.at(2) != active){
          return 2;
        }else if(!world.pc.team.at(2).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(2).name);
          getch();
        }else if(&world.pc.team.at(2) == active){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(2).name);
          getch();
        }
      }
      break;
      case '4':
      if(world.pc.team.size() >= 4){
        if(world.pc.team.at(3).battleStat[stat_hp] && &world.pc.team.at(3) != active){
          return 3;
        }else if(!world.pc.team.at(3).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(3).name);
          getch();
        }else if(&world.pc.team.at(3) == active){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(3).name);
          getch();
        }
      }
      break;
      case '5':
      if(world.pc.team.size() >= 5){
        if(world.pc.team.at(4).battleStat[stat_hp] && &world.pc.team.at(4) != active){
          return 4;
        }else if(!world.pc.team.at(4).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(4).name);
          getch();
        }else if(&world.pc.team.at(4) == active){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(4).name);
          getch();
        }
      }
      break;
      case '6':
      if(world.pc.team.size() == 6){
        if(world.pc.team.at(5).battleStat[stat_hp] && &world.pc.team.at(5) != active){
          return 5;
        }else if(!world.pc.team.at(5).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is in no condition to battle!", world.pc.team.at(5).name);
          getch();
        }else if(&world.pc.team.at(5) == active){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s is already in battle!", world.pc.team.at(5).name);
          getch();
        }
      }
      break;
      case '<':
      return -1;
      break;
      default:
      break;
    }
  }
}

int use_potion(){
  int i;
  char key;
  char poke_list[120], next_poke[120];

  sprintf(poke_list, "1. %s (%d/%d) ", world.pc.team.at(0).name, world.pc.team.at(0).battleStat[stat_hp], world.pc.team.at(0).currentStat[stat_hp]);


  for(i = 1; i < (int)world.pc.team.size(); i++){
    sprintf(next_poke, "%d. %s (%d/%d) ", i + 1, world.pc.team.at(i).name, world.pc.team.at(i).battleStat[stat_hp], world.pc.team.at(i).currentStat[stat_hp]);
    strcat(poke_list, next_poke);
  }

  strcat(poke_list, "<: Back");

  while(1){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "Use on which Pokemon?");
    getch();
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "%s", poke_list);
    switch(key = getch()){
      case '1':
      if(world.pc.team.at(0).battleStat[stat_hp] && world.pc.team.at(0).battleStat[stat_hp] < world.pc.team.at(0).currentStat[stat_hp]){
        return 0;
      }else if(!world.pc.team.at(0).battleStat[stat_hp] || world.pc.team.at(0).currentStat[stat_hp] == world.pc.team.at(0).battleStat[stat_hp]){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It won't have any effect!");
        getch();
      }
      break;
      case '2':
      if(world.pc.team.size() >= 2){
        if(world.pc.team.at(1).battleStat[stat_hp] && world.pc.team.at(1).battleStat[stat_hp] < world.pc.team.at(1).currentStat[stat_hp]){
          return 1;
        }else if(!world.pc.team.at(1).battleStat[stat_hp] || world.pc.team.at(1).currentStat[stat_hp] == world.pc.team.at(1).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '3':
      if(world.pc.team.size() >= 3){
        if(world.pc.team.at(2).battleStat[stat_hp] && world.pc.team.at(2).battleStat[stat_hp] < world.pc.team.at(2).currentStat[stat_hp]){
          return 2;
        }else if(!world.pc.team.at(2).battleStat[stat_hp] || world.pc.team.at(2).currentStat[stat_hp] == world.pc.team.at(2).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '4':
      if(world.pc.team.size() >= 4){
        if(world.pc.team.at(3).battleStat[stat_hp] && world.pc.team.at(3).battleStat[stat_hp] < world.pc.team.at(3).currentStat[stat_hp]){
          return 3;
        }else if(!world.pc.team.at(3).battleStat[stat_hp] || world.pc.team.at(3).currentStat[stat_hp] == world.pc.team.at(3).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '5':
      if(world.pc.team.size() >= 5){
        if(world.pc.team.at(4).battleStat[stat_hp] && world.pc.team.at(4).battleStat[stat_hp] < world.pc.team.at(4).currentStat[stat_hp]){
          return 4;
        }else if(!world.pc.team.at(4).battleStat[stat_hp] || world.pc.team.at(4).currentStat[stat_hp] == world.pc.team.at(4).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '6':
      if(world.pc.team.size() == 6){
        if(world.pc.team.at(5).battleStat[stat_hp] && world.pc.team.at(5).battleStat[stat_hp] < world.pc.team.at(5).currentStat[stat_hp]){
          return 5;
        }else if(!world.pc.team.at(5).battleStat[stat_hp] || world.pc.team.at(5).currentStat[stat_hp] == world.pc.team.at(5).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '<':
      return -1;
      break;
      default:
      break;
    }
  }
}

int use_revive(){
  int i;
  char key;
  char poke_list[120], next_poke[120];

  sprintf(poke_list, "1. %s (%d/%d) ", world.pc.team.at(0).name, world.pc.team.at(0).battleStat[stat_hp], world.pc.team.at(0).currentStat[stat_hp]);


  for(i = 1; i < (int)world.pc.team.size(); i++){
    sprintf(next_poke, "%d. %s (%d/%d) ", i + 1, world.pc.team.at(i).name, world.pc.team.at(i).battleStat[stat_hp], world.pc.team.at(i).currentStat[stat_hp]);
    strcat(poke_list, next_poke);
  }

  strcat(poke_list, "<: Back");

  while(1){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "Use on which Pokemon?");
    mvprintw(0, 70, "--more--");
    getch();
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "%s", poke_list);
    switch(key = getch()){
      case '1':
      if(!world.pc.team.at(0).battleStat[stat_hp]){
        return 0;
      }else if(world.pc.team.at(0).battleStat[stat_hp]){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It won't have any effect!");
        getch();
      }
      break;
      case '2':
      if(world.pc.team.size() >= 2){
        if(!world.pc.team.at(1).battleStat[stat_hp]){
          return 1;
        }else if(world.pc.team.at(1).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '3':
      if(world.pc.team.size() >= 3){
        if(!world.pc.team.at(2).battleStat[stat_hp]){
          return 2;
        }else if(world.pc.team.at(2).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '4':
      if(world.pc.team.size() >= 4){
        if(!world.pc.team.at(3).battleStat[stat_hp]){
          return 3;
        }else if(world.pc.team.at(3).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '5':
      if(world.pc.team.size() >= 5){
        if(!world.pc.team.at(4).battleStat[stat_hp]){
          return 4;
        }else if(world.pc.team.at(4).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '6':
      if(world.pc.team.size() == 6){
        if(!world.pc.team.at(5).battleStat[stat_hp]){
          return 5;
        }else if(world.pc.team.at(5).battleStat[stat_hp]){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It won't have any effect!");
          getch();
        }
      }
      break;
      case '<':
      return -1;
      break;
      default:
      break;
    }
  }
}

int has_pp_remaining(game_pokemon p){
  int i;

  for(i = 0; i < (int)p.moves.size(); i++){
    if(p.moves.at(i).current_pp){
      return 1;
    }
  }

  return 0;
}

//Index 0 of pair is item used, index 1 is pokemon slot the item will be used on (unless a pokeball is used)
void open_bag(int is_trainer_battle, pair_t item){
  char key;
  int item_used;



  item_used = 0;
  while(!item_used){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "Choose item: 1. Potion: x%d  2. Revive x%d  3. Pokeball x%d  <: Back", world.pc.num_potions, world.pc.num_revives, world.pc.num_p_balls);
    switch(key = getch()){
      case '1':
      if(world.pc.num_potions > 0){
        item[0] = 1;
        item[1] = use_potion();
        if(item[1] != -1){
          item_used = 1;
        }
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "You have no potions left!");
      }
      break;
      case '2':
      if(world.pc.num_revives > 0){
        item[0] = 2;
        item[1] = use_revive();
        if(item[1] != -1){
          item_used = 1;
        }
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "You have no revives left!");
        getch();
      }
      break;
      case '3':
      if(is_trainer_battle){
        io_queue_message("The trainer blocked the ball!");
        io_queue_message("Don't be a thief!");
        io_print_message_queue(0,0);
        getch();
      }else if(world.pc.num_p_balls > 0){
        item[0] = 3;
        item[1] = -1;
        item_used = 1;
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "You have no revives left!");
        getch();
      }
      break;
      case '<':
      item[0] = -1;
      item[1] = -1;
      return;
      break;
      default:
      break;
    }
  }
}

int forget_move(game_pokemon *p, pokemon_move new_move){
  int i, exit;
  char key, key2;
  char move_list[120], next_move[120];

  sprintf(move_list, "1. %s  ", p->moves.at(0).name);


  for(i = 1; i < (int)p->moves.size(); i++){
    sprintf(next_move, "%d. %s ", i + 1, p->moves.at(i).name);
    strcat(move_list, next_move);
  }

  strcat(move_list, "<: Cancel");

  while(1){
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "Which move should be forgotten?");
    getch();
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "%s", move_list);
    switch(key = getch()){
      case '1':
      return 0;
      break;
      case '2':
      return 1;
      break;
      case '3':
      return 2;
      break;
      case '4':
      return 3;
      break;
      case '<':
      exit = 0;
      while(!exit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Give up on learning %s?  1. Yes  2. No", new_move.name);
        switch(key2 = getch()){
          case '1':
          return -1;
          break;
          case '2':
          break;
        }
      }
      default:
      break;
    }
  }
}

void level_up(game_pokemon *p, int initial_level){
  int i;
  int hp_missing;
  int move_index, move_to_forget;
  int key, exit;
  move_db tmp;
  pokemon_move new_move;

  move(0,0);
  clrtoeol();
  mvprintw(0, 0, "%s grew to level %d!", p->name, p->level);
  getch();

  hp_missing = p->currentStat[stat_hp] - p->battleStat[stat_hp];

  p->currentStat[0] = ((((p->baseStat[0] + p->iv[0]) * 2) * p->level) / 100) + p->level + 10;
  p->battleStat[0] = p->currentStat[0] - hp_missing;

  for(i = 1; i < 6; i++){
    p->currentStat[i] = p->battleStat[i] = ((((p->baseStat[i] + p->iv[i]) * 2) * p->level) / 100) + 5;
  }

  for(i = 0; i < (int)p->level_up_moves.size(); i++){
    if(p->level_up_moves.at(i).level > initial_level && p->level_up_moves.at(i).level <= p->level){
      if(p->level_up_moves.at(i).move_id < 10000){
        move_index = p->level_up_moves.at(i).move_id;
      }else{
        move_index = 826 + (p->level_up_moves.at(i).move_id - 10000);
      }
      tmp = moves[move_index];
      strcpy(new_move.name, tmp.identifier);
      new_move.name[0] = std::toupper(new_move.name[0]);
      new_move.move_type = (tmp.type_id < 18) ? static_cast<pokemon_type_t>(tmp.type_id - 1) : type_shadow;
      new_move.power = (tmp.power == INT_MAX) ? 0 : tmp.power;
      new_move.max_pp = tmp.pp;
      new_move.current_pp = tmp.pp;
      new_move.priority = tmp.priority;
      new_move.accuracy = tmp.accuracy;

      if(p->moves.size() < 4){
        move(0,0);
        clrtoeol();
        mvprintw(0,0,"%s learned %s", p->name, new_move.name);
        getch();
        p->moves.push_back(new_move);
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s wants to learn %s.", p->name, new_move.name);
        getch();

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "But %s can't remember any more moves.", p->name);
        getch();

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Give up a move to make room for %s?  1. Yes  2. No", new_move.name);
        
        exit = 0;
        while(!exit){
          switch(key = getch()){
            case '1':
            move_to_forget = forget_move(p, new_move);

            if(move_to_forget == -1){
              move(0,0);
              clrtoeol();
              mvprintw(0, 0, "%s did not learn %s", p->name, new_move.name);
              getch();
            }else{
              io_queue_message("1, 2, and... poof!");
              io_queue_message("%s forgot %s", p->name, p->moves.at(move_to_forget).name);
              p->moves.erase(p->moves.begin() + move_to_forget);
              p->moves.push_back(new_move);
              io_queue_message("And...");
              io_queue_message("%s learned %s!", p->name, new_move.name);

              io_print_message_queue(0,0);
              getch();
            }

            exit = 1;
            break;
            case '2':

            move(0,0);
            clrtoeol();
            mvprintw(0, 0, "%s did not learn %s", p->name, new_move.name);

            exit = 1;
            break;
            default:
            break;
          }
        }
      }
    }
  }
}

void evolve(game_pokemon *p){
  char key;
  pokemon_species_db new_data;
  int hp_missing;
  int exit, move_to_forget;
  pokemon_move_db *index;
  pokemon_move new_move;
  move_db tmp;
  std::vector<int> move_ids;
  int i, j, move_index;
  pokemon_types_db *type_index;
  char old_name[50];

  move(0,0);
  clrtoeol();
  mvprintw(0, 0, "What?");
  getch();

  mvprintw(0,0,"%s is evolving!  (Cancel with x, continue with any other key)", p->name);
  getch();

  switch(key = getch()){
    case 'x':
    case 'X':
    move(0,0);
    clrtoeol();
    mvprintw(0, 0, "Huh? %s stopped evolving", p->name);
    getch();
    return;
    break;
    default:
    break;
  }

  strcpy(old_name, p->name);

  new_data = species[p->evolves_into_id];

  strcpy(p->name, new_data.identifier);

  p->name[0] = std::toupper(p->name[0]);

  for(i = 1; i < 6553; i += 6){
    if(pokemon_stats[i].pokemon_id == new_data.id){
      for(j = 0; j < 6; j++){
        p->baseStat[j] = pokemon_stats[i + j].base_stat;
      }
      break;
    }
  }

  p->species_id = new_data.id;

  hp_missing = p->currentStat[stat_hp] - p->battleStat[stat_hp];

  p->currentStat[0] = ((((p->baseStat[0] + p->iv[0]) * 2) * p->level) / 100) + p->level + 10;
  p->battleStat[0] = p->currentStat[0] - hp_missing;

  for(i = 1; i < 6; i++){
    p->currentStat[i] = p->battleStat[i] = ((((p->baseStat[i] + p->iv[i]) * 2) * p->level) / 100) + 5;
  }

  move(0,0);
  clrtoeol();
  mvprintw(0,0, "Wow! %s evolved into %s!", old_name, p->name);

  p->level_up_moves.clear();

  index = &pokemon_moves[new_data.id];
  while(index && index->pokemon_id <= new_data.id){
    if(index->pokemon_id == new_data.id && index->pokemon_move_method_id == 1 && std::find(move_ids.begin(), move_ids.end(), index->move_id) == move_ids.end()){
      p->level_up_moves.push_back(*index);
      move_ids.push_back(index->move_id);
    }
    index++;
  }

  for(i = 0; i < (int)p->level_up_moves.size(); i++){
    if(p->level_up_moves.at(i).level == p->level){
      if(p->level_up_moves.at(i).move_id < 10000){
        move_index = p->level_up_moves.at(i).move_id;
      }else{
        move_index = 826 + (p->level_up_moves.at(i).move_id - 10000);
      }
      tmp = moves[move_index];
      strcpy(new_move.name, tmp.identifier);
      new_move.name[0] = std::toupper(new_move.name[0]);
      new_move.move_type = (tmp.type_id < 18) ? static_cast<pokemon_type_t>(tmp.type_id - 1) : type_shadow;
      new_move.power = (tmp.power == INT_MAX) ? 0 : tmp.power;
      new_move.max_pp = tmp.pp;
      new_move.current_pp = tmp.pp;
      new_move.priority = tmp.priority;
      new_move.accuracy = tmp.accuracy;

      if(p->moves.size() < 4){
        move(0,0);
        clrtoeol();
        mvprintw(0,0,"%s learned %s", p->name, new_move.name);
        getch();
        p->moves.push_back(new_move);
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s wants to learn %s.", p->name, new_move.name);
        getch();

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "But %s can't remember any more moves.", p->name);
        getch();

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Give up a move to make room for %s?  1. Yes  2. No", new_move.name);
        
        exit = 0;
        while(!exit){
          switch(key = getch()){
            case '1':
            move_to_forget = forget_move(p, new_move);

            if(move_to_forget == -1){
              move(0,0);
              clrtoeol();
              mvprintw(0, 0, "%s did not learn %s", p->name, new_move.name);
              getch();
            }else{
              io_queue_message("1, 2, and... poof!");
              io_queue_message("%s forgot %s", p->name, p->moves.at(move_to_forget).name);
              p->moves.erase(p->moves.begin() + move_to_forget);
              p->moves.push_back(new_move);
              io_queue_message("And...");
              io_queue_message("%s learned %s!", p->name, new_move.name);

              io_print_message_queue(0,0);
              getch();
            }

            exit = 1;
            break;
            case '2':

            move(0,0);
            clrtoeol();
            mvprintw(0, 0, "%s did not learn %s", p->name, new_move.name);

            exit = 1;
            break;
            default:
            break;
          }
        }
      }
    }
  }

  type_index = &pokemon_types[p->species_id];
  while(type_index && type_index->pokemon_id <= p->species_id){
    if(type_index->pokemon_id == p->species_id){
      if((type_index + 1)->pokemon_id == p->species_id){
        p->types[0] = static_cast<pokemon_type_t>(type_index->type_id - 1);
        p->types[1] = static_cast<pokemon_type_t>((type_index + 1)->type_id - 1);
      }else{
        p->types[0] = static_cast<pokemon_type_t>(type_index->type_id - 1);
        p->types[1] = type_null;
      }
      break;
    }
    type_index++;
  }

  find_evolves(p);
}

void calculate_xp(game_pokemon fainted_pokemon, std::vector<game_pokemon *> pokemon_to_distribute){
  int base_exp, half_exp, initial_level;
  double calc, tmp;
  int i, j;

  move(0,0);
  clrtoeol();
  mvprintw(0, 0, "Your team gained experience!");
  getch();

  for(i = 0; i < (int) world.pc.team.size(); i++){
    if(world.pc.team.at(i).level < 100){
      calc = fainted_pokemon.baseExp * fainted_pokemon.level;
      calc /= 5.0;
      tmp = (2 * fainted_pokemon.level);
      tmp += 10;
      tmp /= (fainted_pokemon.level + world.pc.team.at(i).level + 10.0);
      calc *= pow(tmp, 2.5);
      calc += 1;
      base_exp = (int) calc;
      half_exp = base_exp * 0.5;
      if(world.pc.team.at(i).battleStat[stat_hp]){
        if(std::find(pokemon_to_distribute.begin(), pokemon_to_distribute.end(), &world.pc.team.at(i)) != pokemon_to_distribute.end()){
          world.pc.team.at(i).currentExp += base_exp;
        }else{
          world.pc.team.at(i).currentExp += half_exp;
        }
      }

      initial_level = world.pc.team.at(i).level;

      for(j = 1; world.pc.team.at(i).currentExp > (experience[(world.pc.team.at(i).xp_growth_group * 100) + 1 + world.pc.team.at(i).level + j].experience); j++){
        world.pc.team.at(i).level = initial_level + j;
      }

      if(world.pc.team.at(i).level > initial_level){
        level_up(&world.pc.team.at(i), initial_level);
        world.pc.team.at(i).leveled_last_battle = 1;
      }
    }
  }
}

int calculate_damage(game_pokemon attacker, game_pokemon defender, pokemon_move move, int is_crit, int is_stab){
  double current;
  int result;
  double crit_mod, stab_mod, random;
  if(move.power == 0){
    return 0;
  }

  if(defender.type_effectiveness[move.move_type] < 0.2){
    return 0;
  }

  crit_mod = is_crit ? 1.5 : 1;
  stab_mod = is_stab ? 1.5 : 1;
  random = ((rand() % 16) + 85) / 100.0;



  current = ((2 * attacker.level) + 2);
  current *= move.power;
  current *= attacker.battleStat[stat_attack];
  current /= (defender.battleStat[stat_defense] * 1.0);
  current /= 50.0;
  current += 2;
  current *= (crit_mod * stab_mod * random * defender.type_effectiveness[move.move_type]);

  result = (int) current;
  if(result == 0){
    return 1;
  }else{
    return result;
  }
}

//Returns 0 on PC victory, 1 on PC loss
int trainer_battle_loop(std::vector<game_pokemon> npc_pokemon, char *trainer_name){
  char key;
  int i;
  int turn_taken, player_went_first, npc_fainted, player_fainted, player_move_hit, npc_move_hit;
  int pc_first_pokemon, pc_pokemon_remaining;
  game_pokemon *active_player_pokemon, active_npc_pokemon, *healed;
  pokemon_move chosen_move, npc_chosen_move;
  int pc_priority, npc_priority;
  int move_slot, pokemon_to_swap, pokemon_to_heal;
  char battle_message[100];
  int hp_healed, damage_dealt;
  int is_stab, is_crit;
  pair_t bag_pair;
  std::vector<game_pokemon *> fought_current_foe;

  pc_first_pokemon = -1;
  pc_pokemon_remaining = 0;

  for(i = 0; i < (int)world.pc.team.size(); i++){
    if(world.pc.team.at(i).battleStat[stat_hp]){
      if(pc_first_pokemon < 0){
        pc_first_pokemon = i;
      }
      pc_pokemon_remaining++;
    }
  }

  active_npc_pokemon = npc_pokemon.at(0);
  active_player_pokemon = &world.pc.team.at(pc_first_pokemon);

  fought_current_foe.push_back(active_player_pokemon);

  while(1){
    move(22, 0);
    clrtoeol();
    move(23, 0);
    clrtoeol();
    mvprintw(22, 0, "%s: %s Lvl: %d", trainer_name, active_npc_pokemon.name, active_npc_pokemon.level);
    mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
    npc_fainted = 0;
    if(has_pp_remaining(active_npc_pokemon)){
      npc_chosen_move = active_npc_pokemon.moves.at(rand() % active_npc_pokemon.moves.size());
      while(!npc_chosen_move.current_pp){
        npc_chosen_move = active_npc_pokemon.moves.at(rand() % active_npc_pokemon.moves.size());
      }
    }else{
      npc_chosen_move = struggle;
    }

    npc_priority = npc_chosen_move.priority;

    turn_taken = 0;
    while(!turn_taken){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "What will you do? 1. Fight  2. Bag  3. Run  4. Pokemon");
      switch(key = getch()){
        case '1':
        if(has_pp_remaining(*active_player_pokemon)){
          move_slot = select_move(*active_player_pokemon);
          if(move_slot != -1){
            chosen_move = active_player_pokemon->moves.at(move_slot);
            active_player_pokemon->moves.at(move_slot).current_pp--;
            pc_priority = chosen_move.priority;
            turn_taken = 1;
          }
        }else{
          chosen_move = struggle;
          pc_priority = chosen_move.priority;
          turn_taken = 1;
        }
        break;
        case '2':
        open_bag(1, bag_pair);
        if(bag_pair[0] != -1){
          turn_taken = 1;
          pc_priority = 100;
        }
        if(bag_pair[0] == 1){
          strcpy(chosen_move.name, "potion");
          pokemon_to_heal = bag_pair[1];
          world.pc.num_potions--;
          chosen_move.accuracy = 100;
        }else if(bag_pair[0] == 2){
          strcpy(chosen_move.name, "revive");
          pokemon_to_heal = bag_pair[1];
          world.pc.num_revives--;
          chosen_move.accuracy = 100;
        }
        break;
        case '3':
        move(0,0);
        clrtoeol();
        printw("You can't run from a trainer battle!");
        getch();
        break;
        case '4':
        pokemon_to_swap = swap_pokemon(active_player_pokemon);
        if(pokemon_to_swap != -1){
          strcpy(chosen_move.name, "switch");
          pc_priority = 100;
          turn_taken = 1;
          chosen_move.accuracy = 100;
        }
        break;
        default:
        break;
      }
    }

    player_went_first = 0;
    npc_fainted = 0;
    player_fainted = 0;

    player_move_hit = (rand() % 100) < chosen_move.accuracy;
    npc_move_hit = (rand() % 100) < npc_chosen_move.accuracy;

    //Resolve priority and speed ties
    if(pc_priority > npc_priority || (pc_priority == npc_priority && (active_player_pokemon->battleStat[stat_speed] > active_npc_pokemon.battleStat[stat_speed] || (active_player_pokemon->battleStat[stat_speed] == active_npc_pokemon.battleStat[stat_speed] && rand() % 2 == 0)))){
      player_went_first = 1;
      if(!strcmp(chosen_move.name, "switch")){
        sprintf(battle_message, "%s! Switch out! Come back!", active_player_pokemon->name);
        io_queue_message(battle_message);

        active_player_pokemon = &world.pc.team.at(pokemon_to_swap);

        if(std::find(fought_current_foe.begin(), fought_current_foe.end(), active_player_pokemon) == fought_current_foe.end()){
          fought_current_foe.push_back(active_player_pokemon);
        }

        sprintf(battle_message, "Go! %s!", active_player_pokemon->name);
        io_queue_message(battle_message);

        io_print_message_queue(0,0);
        getch();
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
      }else if(!strcmp(chosen_move.name, "potion")){
        healed = &world.pc.team.at(pokemon_to_heal);
        if(healed->battleStat[stat_hp] + 20 >= healed->currentStat[stat_hp]){
          hp_healed = healed->currentStat[stat_hp] - healed->battleStat[stat_hp];
          healed->battleStat[stat_hp] = healed->currentStat[stat_hp];
        }else{
          hp_healed = 20;
          healed->battleStat[stat_hp] += 20;
        }
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's HP was restored by %d!", healed->name, hp_healed);
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
        getch();
      }else if(!strcmp(chosen_move.name, "revive")){
        healed = &world.pc.team.at(pokemon_to_heal);
        healed->battleStat[stat_hp] = healed->currentStat[stat_hp] / 2;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s recovered from fainting!", healed->name);
        getch();
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s used %s!", active_player_pokemon->name, chosen_move.name);
        getch();

        if(chosen_move.move_type == active_player_pokemon->types[0] || chosen_move.move_type == active_player_pokemon->types[1]){
          is_stab = 1;
        }else{
          is_stab = 0;
        }

        if(rand() % 255 < (active_player_pokemon->baseStat[stat_speed] / 2) && chosen_move.power != 0 && active_npc_pokemon.type_effectiveness[chosen_move.move_type] != 0 && player_move_hit){
          is_crit = 1;
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "A critical hit!");
          getch();
        }else{
          is_crit = 0;
        }

        damage_dealt = calculate_damage(*active_player_pokemon, active_npc_pokemon, chosen_move, is_crit, is_stab);

        if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] < 0.2 && chosen_move.power != 0 && player_move_hit){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It doesn't affect %s...", active_npc_pokemon.name);
          getch();
        }else if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] < 1 && chosen_move.power != 0 && player_move_hit){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It's not very effective...");
          getch();
        }else if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] > 1 && chosen_move.power != 0 && player_move_hit){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It's super effective!");
          getch();
        }

        if(!player_move_hit){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s's attack missed!", active_player_pokemon->name);
          damage_dealt = 0;
          getch();
        }

        if(damage_dealt >= active_npc_pokemon.battleStat[stat_hp]){
          active_npc_pokemon.battleStat[stat_hp] = 0;
          npc_pokemon.erase(npc_pokemon.begin());
          npc_fainted = 1;
        }else{
          active_npc_pokemon.battleStat[stat_hp] -= damage_dealt;
        }
      }
    }

    if(!npc_fainted){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "%s's %s used %s!", trainer_name, active_npc_pokemon.name, npc_chosen_move.name);
      getch();

      if(npc_chosen_move.move_type == active_npc_pokemon.types[0] || chosen_move.move_type == active_npc_pokemon.types[1]){
        is_stab = 1;
      }else{
        is_stab = 0;
      }

      if(rand() % 255 < (active_npc_pokemon.baseStat[stat_speed] / 2) && npc_chosen_move.power != 0 && active_player_pokemon->type_effectiveness[chosen_move.move_type] != 0 && npc_move_hit){
        is_crit = 1;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "A critical hit!");
        getch();
      }else{
        is_crit = 0;
      }

      damage_dealt = calculate_damage(active_npc_pokemon, *active_player_pokemon, npc_chosen_move, is_crit, is_stab);

      if(active_player_pokemon->type_effectiveness[npc_chosen_move.move_type] < 0.2 && npc_chosen_move.power != 0 && npc_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It doesn't affect %s...", active_player_pokemon->name);
        getch();
      }else if(active_player_pokemon->type_effectiveness[npc_chosen_move.move_type] < 1 && npc_chosen_move.power != 0 && npc_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's not very effective...");
        getch();
      }else if(active_player_pokemon->type_effectiveness[npc_chosen_move.move_type] > 1 && npc_chosen_move.power != 0 && npc_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's super effective!");
        getch();
      }

      if(!npc_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's attack missed!", active_npc_pokemon.name);
        damage_dealt = 0;
        getch();
      }

      if(damage_dealt >= active_player_pokemon->battleStat[stat_hp]){
        active_player_pokemon->battleStat[stat_hp] = 0;
        pc_pokemon_remaining--;
        player_fainted = 1;
      }else{
        active_player_pokemon->battleStat[stat_hp] -= damage_dealt;
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
      }
    }else{
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "%s's %s fainted!", trainer_name, active_npc_pokemon.name);
      getch();
      calculate_xp(active_npc_pokemon, fought_current_foe);
      if(npc_pokemon.size()){
        fought_current_foe.clear();
        fought_current_foe.push_back(active_player_pokemon);
        active_npc_pokemon = npc_pokemon.at(0);
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s sent out %s!", trainer_name, active_npc_pokemon.name);
        getch();
        move(22,0);
        clrtoeol();
        mvprintw(22, 0, "%s: %s Lvl: %d", trainer_name, active_npc_pokemon.name, active_npc_pokemon.level);
      }else{
        return 0;
      }
    }

    if(player_fainted){
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "%s fainted!            --more--", active_player_pokemon->name);
      getch();
      if(pc_pokemon_remaining){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Choose next Pokemon            --more--");
        getch();
        pokemon_to_swap = swap_pokemon(active_player_pokemon);
        active_player_pokemon = &world.pc.team.at(pokemon_to_swap);

        if(std::find(fought_current_foe.begin(), fought_current_foe.end(), active_player_pokemon) == fought_current_foe.end()){
          fought_current_foe.push_back(active_player_pokemon);
        }

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Go! %s!", active_player_pokemon->name);
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
        getch();
      }else{
        return 1;
      }
    }

    if(!player_fainted && !player_went_first){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "%s used %s!", active_player_pokemon->name, chosen_move.name);
      getch();

      if(chosen_move.move_type == active_player_pokemon->types[0] || chosen_move.move_type == active_player_pokemon->types[1]){
        is_stab = 1;
      }else{
        is_stab = 0;
      }

      if(rand() % 255 < (active_player_pokemon->baseStat[stat_speed] / 2) && chosen_move.power != 0 && active_npc_pokemon.type_effectiveness[chosen_move.move_type] != 0 && player_move_hit){
        is_crit = 1;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "A critical hit!");
        getch();
      }else{
        is_crit = 0;
      }

      damage_dealt = calculate_damage(*active_player_pokemon, active_npc_pokemon, chosen_move, is_crit, is_stab);

      if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] < 0.2 && chosen_move.power != 0 && player_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It doesn't affect %s...", active_npc_pokemon.name);
        getch();
      }else if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] < 1 && chosen_move.power != 0 && player_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's not very effective...");
        getch();
      }else if(active_npc_pokemon.type_effectiveness[chosen_move.move_type] > 1 && chosen_move.power != 0 && player_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's super effective!");
        getch();
      }

      if(!player_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's attack missed!", active_player_pokemon->name);
        damage_dealt = 0;
        getch();
      }

      if(damage_dealt >= active_npc_pokemon.battleStat[stat_hp]){
        active_npc_pokemon.battleStat[stat_hp] = 0;
        npc_pokemon.erase(npc_pokemon.begin());
        npc_fainted = 1;
      }else{
        active_npc_pokemon.battleStat[stat_hp] -= damage_dealt;
        move(22,0);
        clrtoeol();
        mvprintw(22, 0, "%s: %s Lvl: %d", trainer_name, active_npc_pokemon.name, active_npc_pokemon.level);
      }
    }

    //Second check after other possible player action
    if(npc_fainted && !player_went_first){
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "%s's %s fainted!", trainer_name, active_npc_pokemon.name);
      getch();
      calculate_xp(active_npc_pokemon, fought_current_foe);
      fought_current_foe.clear();
      fought_current_foe.push_back(active_player_pokemon);
      if(npc_pokemon.size()){
        active_npc_pokemon = npc_pokemon.at(0);
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s sent out %s!", trainer_name, active_npc_pokemon.name);
        getch();
        move(22,0);
        clrtoeol();
        mvprintw(22, 0, "%s: %s Lvl: %d", trainer_name, active_npc_pokemon.name, active_npc_pokemon.level);
      }else{
        return 0;
      }
    }
  }
}

//Returns 0 on wild pokemon fainting, 1 on PC losing, 2 on Pokeball throw
int wild_battle_loop(game_pokemon *wild){
  char key;
  int i;
  int turn_taken, player_went_first, wild_fainted, player_fainted, player_move_hit, wild_move_hit;
  int pc_first_pokemon, pc_pokemon_remaining;
  game_pokemon *active_player_pokemon, *healed;
  pokemon_move chosen_move, wild_chosen_move;
  int pc_priority, wild_priority;
  int move_slot, pokemon_to_swap, pokemon_to_heal;
  char battle_message[100];
  int hp_healed, damage_dealt;
  int is_stab, is_crit;
  pair_t bag_pair;
  std::vector<game_pokemon *> fought_current_foe;

  pc_first_pokemon = -1;
  pc_pokemon_remaining = 0;

  for(i = 0; i < (int)world.pc.team.size(); i++){
    if(world.pc.team.at(i).battleStat[stat_hp]){
      if(pc_first_pokemon < 0){
        pc_first_pokemon = i;
      }
      pc_pokemon_remaining++;
    }
  }

  active_player_pokemon = &world.pc.team.at(pc_first_pokemon);

  fought_current_foe.push_back(active_player_pokemon);

  move(0, 0);
  clrtoeol();
  mvprintw(0, 0, "Go! %s!", active_player_pokemon->name);
  getch();

  while(1){
    move(22, 0);
    clrtoeol();
    move(23, 0);
    clrtoeol();
    mvprintw(22, 0, "%s Lvl: %d", wild->name, wild->level);
    mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
    wild_fainted = 0;
    if(has_pp_remaining(*wild)){
      wild_chosen_move = wild->moves.at(rand() % wild->moves.size());
      while(!wild_chosen_move.current_pp){
        wild_chosen_move = wild->moves.at(rand() % wild->moves.size());
      }
    }else{
      wild_chosen_move = struggle;
    }

    wild_priority = wild_chosen_move.priority;

    turn_taken = 0;
    while(!turn_taken){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "What will you do? 1. Fight  2. Bag  3. Run  4. Pokemon");
      switch(key = getch()){
        case '1':
        if(has_pp_remaining(*active_player_pokemon)){
          move_slot = select_move(*active_player_pokemon);
          if(move_slot != -1){
            chosen_move = active_player_pokemon->moves.at(move_slot);
            active_player_pokemon->moves.at(move_slot).current_pp--;
            pc_priority = chosen_move.priority;
            turn_taken = 1;
          }
        }else{
          chosen_move = struggle;
          pc_priority = chosen_move.priority;
          turn_taken = 1;
        }
        break;
        case '2':
        open_bag(0, bag_pair);
        if(bag_pair[0] != -1){
          turn_taken = 1;
          pc_priority = 100;
        }
        if(bag_pair[0] == 1){
          strcpy(chosen_move.name, "potion");
          pokemon_to_heal = bag_pair[1];
          world.pc.num_potions--;
          chosen_move.accuracy = 100;
        }else if(bag_pair[0] == 2){
          strcpy(chosen_move.name, "revive");
          pokemon_to_heal = bag_pair[1];
          world.pc.num_revives--;
          chosen_move.accuracy = 100;
        }else if(bag_pair[0] == 3){
          world.pc.num_p_balls--;
          return 2;
        }
        break;
        case '3':
        strcpy(chosen_move.name, "run");
        turn_taken = 1;
        pc_priority = 100;
        chosen_move.accuracy = 100;
        break;
        case '4':
        pokemon_to_swap = swap_pokemon(active_player_pokemon);
        if(pokemon_to_swap != -1){
          strcpy(chosen_move.name, "switch");
          turn_taken = 1;
          pc_priority = 100;
          chosen_move.accuracy = 100;
        }
        break;
        default:
        break;
      }
    }

    player_went_first = 0;
    wild_fainted = 0;
    player_fainted = 0;

    player_move_hit = (rand() % 100) < chosen_move.accuracy;
    wild_move_hit = (rand() % 100) < wild_chosen_move.accuracy;

    //Resolve priority and speed ties
    if(pc_priority > wild_priority || (pc_priority == wild_priority && (active_player_pokemon->battleStat[stat_speed] > wild->battleStat[stat_speed] || (active_player_pokemon->battleStat[stat_speed] == wild->battleStat[stat_speed] && rand() % 2 == 0)))){
      player_went_first = 1;
      if(!strcmp(chosen_move.name, "switch")){
        sprintf(battle_message, "%s! Switch out! Come back!", active_player_pokemon->name);
        io_queue_message(battle_message);

        active_player_pokemon = &world.pc.team.at(pokemon_to_swap);

        if(std::find(fought_current_foe.begin(), fought_current_foe.end(), active_player_pokemon) == fought_current_foe.end()){
          fought_current_foe.push_back(active_player_pokemon);
        }

        sprintf(battle_message, "Go! %s!", active_player_pokemon->name);
        io_queue_message(battle_message);

        io_print_message_queue(0,0);
        getch();
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
      }else if(!strcmp(chosen_move.name, "potion")){
        healed = &world.pc.team.at(pokemon_to_heal);
        if(healed->battleStat[stat_hp] + 20 >= healed->currentStat[stat_hp]){
          hp_healed = healed->currentStat[stat_hp] - healed->battleStat[stat_hp];
          healed->battleStat[stat_hp] = healed->currentStat[stat_hp];
        }else{
          hp_healed = 20;
          healed->battleStat[stat_hp] += 20;
        }
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's HP was restored by %d!", healed->name, hp_healed);
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
        getch();
      }else if(!strcmp(chosen_move.name, "revive")){
        healed = &world.pc.team.at(pokemon_to_heal);
        healed->battleStat[stat_hp] = healed->currentStat[stat_hp] / 2;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s recovered from fainting!", healed->name);
        getch();
      }else if(!strcmp(chosen_move.name, "run")){
        if((rand() % 100) < 10){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "Can't escape!");
          getch();
        }else{
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "Got away safely");
          getch();
          return 0;
        }
      }else{
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s used %s!", active_player_pokemon->name, chosen_move.name);
        getch();

        if(chosen_move.move_type == active_player_pokemon->types[0] || chosen_move.move_type == active_player_pokemon->types[1]){
          is_stab = 1;
        }else{
          is_stab = 0;
        }

        if(rand() % 255 < (active_player_pokemon->baseStat[stat_speed] / 2) && chosen_move.power != 0 && wild->type_effectiveness[chosen_move.move_type] > 0.2){
          is_crit = 1;
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "A critical hit!");
          getch();
        }else{
          is_crit = 0;
        }

        damage_dealt = calculate_damage(*active_player_pokemon, *wild, chosen_move, is_crit, is_stab);

        if(wild->type_effectiveness[chosen_move.move_type] < 0.2 && chosen_move.power != 0){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It doesn't affect %s...", wild->name);
          getch();
        }else if(wild->type_effectiveness[chosen_move.move_type] < 1 && chosen_move.power != 0){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It's not very effective...");
          getch();
        }else if(wild->type_effectiveness[chosen_move.move_type] > 1 && chosen_move.power != 0){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "It's super effective!");
          getch();
        }

        if(!player_move_hit){
          move(0,0);
          clrtoeol();
          mvprintw(0, 0, "%s's attack missed!", active_player_pokemon->name);
          damage_dealt = 0;
          getch();
        }

        if(damage_dealt >= wild->battleStat[stat_hp]){
          wild->battleStat[stat_hp] = 0;
          wild_fainted = 1;
        }else{
          wild->battleStat[stat_hp] -= damage_dealt;
        }
      }
    }

    if(!wild_fainted){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "The wild %s used %s!", wild->name, wild_chosen_move.name);
      getch();

      if(wild_chosen_move.move_type == wild->types[0] || wild_chosen_move.move_type == wild->types[1]){
        is_stab = 1.5;
      }else{
        is_stab = 1;
      }

      if(rand() % 255 < (wild->baseStat[stat_speed] / 2) && wild_chosen_move.power != 0 && active_player_pokemon->type_effectiveness[wild_chosen_move.move_type] > 0.2){
        is_crit = 1;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "A critical hit!");
        getch();
      }else{
        is_crit = 0;
      }

      damage_dealt = calculate_damage(*wild, *active_player_pokemon, wild_chosen_move, is_crit, is_stab);

      if(active_player_pokemon->type_effectiveness[wild_chosen_move.move_type] == 0 && wild_chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It doesn't affect %s...", active_player_pokemon->name);
        getch();
      }else if(active_player_pokemon->type_effectiveness[wild_chosen_move.move_type] < 1 && wild_chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's not very effective...");
        getch();
      }else if(active_player_pokemon->type_effectiveness[wild_chosen_move.move_type] > 1 && wild_chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's super effective!");
        getch();
      }

      if(!wild_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's attack missed!", active_player_pokemon->name);
        damage_dealt = 0;
        getch();
      }

      if(damage_dealt >= active_player_pokemon->battleStat[stat_hp]){
        active_player_pokemon->battleStat[stat_hp] = 0;
        pc_pokemon_remaining--;
        player_fainted = 1;
      }else{
        active_player_pokemon->battleStat[stat_hp] -= damage_dealt;
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
      }
    }else{
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "Wild %s fainted!", wild->name);
      getch();
      calculate_xp(*wild, fought_current_foe);
      return 0;
    }

    if(player_fainted){
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "%s fainted!            --more--", active_player_pokemon->name);
      getch();
      if(pc_pokemon_remaining){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Choose next Pokemon            --more--");
        getch();
        pokemon_to_swap = swap_pokemon(active_player_pokemon);
        active_player_pokemon = &world.pc.team.at(pokemon_to_swap);

        if(std::find(fought_current_foe.begin(), fought_current_foe.end(), active_player_pokemon) == fought_current_foe.end()){
          fought_current_foe.push_back(active_player_pokemon);
        }

        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "Go! %s!", active_player_pokemon->name);
        move(23,0);
        clrtoeol();
        mvprintw(23, 0, "You: %s Lvl: %d HP: %d/%d", active_player_pokemon->name, active_player_pokemon->level, active_player_pokemon->battleStat[stat_hp], active_player_pokemon->currentStat[stat_hp]);
        getch();
      }else{
        return 1;
      }
    }

    if(!player_fainted && !player_went_first){
      move(0,0);
      clrtoeol();
      mvprintw(0, 0, "%s used %s!", active_player_pokemon->name, chosen_move.name);
      getch();

      if(chosen_move.move_type == active_player_pokemon->types[0] || chosen_move.move_type == active_player_pokemon->types[1]){
        is_stab = 1;
      }else{
        is_stab = 0;
      }

      if(rand() % 255 < (active_player_pokemon->baseStat[stat_speed] / 2) && chosen_move.power != 0 && wild->type_effectiveness[chosen_move.move_type] > 0.2){
        is_crit = 1;
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "A critical hit!");
        getch();
      }else{
        is_crit = 0;
      }

      damage_dealt = calculate_damage(*active_player_pokemon, *wild, chosen_move, is_crit, is_stab);

      if(wild->type_effectiveness[chosen_move.move_type] == 0 && chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It doesn't affect %s...", wild->name);
        getch();
      }else if(wild->type_effectiveness[chosen_move.move_type] < 1 && chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's not very effective...");
        getch();
      }else if(wild->type_effectiveness[chosen_move.move_type] > 1 && chosen_move.power != 0){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "It's super effective!");
        getch();
      }

      if(!player_move_hit){
        move(0,0);
        clrtoeol();
        mvprintw(0, 0, "%s's attack missed!", active_player_pokemon->name);
        damage_dealt = 0;
        getch();
      }

      if(damage_dealt >= wild->battleStat[stat_hp]){
        wild->battleStat[stat_hp] = 0;
        wild_fainted = 1;
      }else{
        wild->battleStat[stat_hp] -= damage_dealt;
        move(22,0);
        clrtoeol();
        mvprintw(22, 0, "%s Lvl: %d", wild->name, wild->level);
      }
    }

    //Second check after other possible player action
    if(wild_fainted && !player_went_first){
      move(0,0);
      clrtoeol();
      mvprintw(0,0, "The wild %s fainted!", wild->name);
      getch();
      calculate_xp(*wild, fought_current_foe);
      return 0;
    }
  }
}