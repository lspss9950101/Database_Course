#ifndef DB_UTIL_H
#define DB_UTIL_H
#include "Command.h"
#include "Table.h"

typedef struct State {
    int saved_stdout;
} State_t;

State_t* new_State();
void print_prompt(State_t *state);
int match_condition(Command_t *cmd, int where_idx, User_t *user);
int match_conditions(Command_t *cmd, User_t *user);
void print_user(User_t *user, SelectArgs_t *sel_args);
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
void update_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
void delete_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd(Table_t *table, Command_t *cmd);
int handle_insert_cmd(Table_t *table, Command_t *cmd);
int handle_select_cmd(Table_t *table, Command_t *cmd);
int handle_update_cmd(Table_t *table, Command_t *cmd);
int handle_delete_cmd(Table_t *table, Command_t *cmd);
void print_help_msg();

#endif
