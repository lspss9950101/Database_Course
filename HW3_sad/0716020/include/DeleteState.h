#ifndef DELETE_STATE_H
#define DELETE_STATE_H

#include "Command.h"
void delete_state_handler(Command_t *cmd, size_t arg_idx);
void table_state_handler_del(Command_t *cmd, size_t arg_idx);
void where_state_handler_del(Command_t *cmd, size_t arg_idx);

#endif
