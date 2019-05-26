#ifndef UPDATE_STATE_H
#define UPDATE_STATE_H

#include "Command.h"

void update_state_handler(Command_t *cmd, size_t arg_idx);
void table_state_handler_upd(Command_t *cmd, size_t arg_idx);
void set_state_handler(Command_t *cmd, size_t arg_idx);
void where_state_handler_upd(Command_t *cmd, size_t arg_idx);

#endif
