#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Command.h"
#include "DeleteState.h"

void delete_state_handler(Command_t *cmd, size_t arg_idx){
    cmd->cmd_args.whe_args = NULL;
    cmd->cmd_args.where_len = 0;
    while(arg_idx < cmd->args_len) {
        if (!strncmp(cmd->args[arg_idx], "from", 4)) {
            table_state_handler_del(cmd, arg_idx+1);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}
void table_state_handler_del(Command_t *cmd, size_t arg_idx){
    if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "table", 5)) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } else if(!strncmp(cmd->args[arg_idx], "where", 5)){
	    where_state_handler_del(cmd, arg_idx+1);
	    return;
	}
    }
    cmd->type = UNRECOG_CMD;
    return;
}
void where_state_handler_del(Command_t *cmd, size_t arg_idx){
	if(arg_idx < cmd->args_len){

		size_t idx = 0;
		int numeric;
		char* str = cmd->args[arg_idx];
		WhereArgs_t argument;
		if(!strncmp(str, "id", 2)){
			idx += 2;
			numeric = 1;
			argument.field = strdup("id");
		}else if(!strncmp(str, "name", 4)){
			idx += 4;
			numeric = 0;
			argument.field = strdup("name");
		}else if(!strncmp(str, "email", 5)){
			idx += 5;
			numeric = 0;
			argument.field = strdup("email");
		}else if(!strncmp(str, "age", 3)){
			idx += 3;
			numeric = 1;
			argument.field = strdup("age");
		}

		if(idx >= strlen(str)){
			str = cmd->args[++arg_idx];
			idx = 0;
		}

		if(!strncmp(str+idx, "=", 1)){
			idx += 1;
			argument.op = EQ;
		}else if(!strncmp(str+idx, "!=", 2)){
			idx += 2;
			argument.op = NEQ;
		}else if(!strncmp(str+idx, ">=", 2)){
			idx += 2;
			argument.op = GE;
		}else if(!strncmp(str+idx, "<=", 2)){
			idx += 2;
			argument.op = LE;
		}else if(!strncmp(str+idx, ">", 1)){
			idx += 1;
			argument.op = GT;
		}else if(!strncmp(str+idx, "<", 1)){
			idx += 1;
			argument.op = LT;
		}

		if(idx >= strlen(str)){
			str = cmd->args[++arg_idx];
			idx = 0;
		}

		if(numeric){
			sscanf(str+idx, "%lf", &argument.literal.arg_num);
		}else{
			argument.literal.arg_char = strdup(str+idx);
		}

		if(arg_idx+1 < cmd->args_len)str = cmd->args[arg_idx + 1];
		if(!strncmp(str, "and", 3)){
			arg_idx++;
			argument.junction = AND;
			add_where_condition(cmd, &argument);
			where_state_handler_del(cmd, ++arg_idx);
		}else if(!strncmp(str, "or", 2)){
			arg_idx++;
			argument.junction = OR;
			add_where_condition(cmd, &argument);
			where_state_handler_del(cmd, ++arg_idx);
		}else{
			argument.junction = 0;
			add_where_condition(cmd, &argument);
		}
		
		arg_idx++;
		if(arg_idx == cmd->args_len)return;
	}
	cmd->type = UNRECOG_CMD;
	return;
}
