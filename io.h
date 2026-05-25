#ifndef IO_H
# define IO_H

class character;
class game_pokemon;
typedef int16_t pair_t[2];

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_print_message_queue(uint32_t y, uint32_t x);
void io_trainer_battle(character *aggressor, character *defender);
void io_wild_battle(game_pokemon encounter);
void io_choose_starter(void);

#endif
