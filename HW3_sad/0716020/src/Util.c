#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "UpdateState.h"
#include "DeleteState.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Check if user matches where conditions
///
int match_condition(Command_t *cmd, int where_idx, User_t* user){
	WhereArgs_t argument = cmd->cmd_args.whe_args[where_idx];
	int numeric;
	int field_num;
	char* field_char;
	if(!strncmp(argument.field, "id", 2)){
		numeric = 1;
		field_num = user->id;
	}else if(!strncmp(argument.field, "name", 4)){
		numeric = 0;
		field_char = user->name;
	}else if(!strncmp(argument.field, "email", 5)){
		numeric = 0;
		field_char = user->email;
	}else if(!strncmp(argument.field, "age", 3)){
		numeric = 1;
		field_num = user->age;
	}

	if(numeric){
		switch(argument.op){
			case EQ:
				if(field_num == argument.literal.arg_num)return 1;
				else return 0;
			case NEQ:
				if(field_num != argument.literal.arg_num)return 1;
				else return 0;
			case GT:
				if(field_num > argument.literal.arg_num)return 1;
				else return 0;
			case LT:
				if(field_num < argument.literal.arg_num)return 1;
				else return 0;
			case GE:
				if(field_num >= argument.literal.arg_num)return 1;
				else return 0;
			case LE:
				if(field_num <= argument.literal.arg_num)return 1;
				else return 0;
		}
	}else{
		if(argument.op == EQ){
			return !strcmp(field_char, argument.literal.arg_char);
		}else if(argument.op == NEQ){
			return strcmp(field_char, argument.literal.arg_char);
		}
	}
	return 0;
}

///
/// Match all conditions
///
int match_conditions(Command_t *cmd, User_t *user){
	int where_len = cmd->cmd_args.where_len;
	if(where_len == 0)return 1;
	else if(where_len == 1)return match_condition(cmd, 0, user);
	else if(where_len == 2){
		if(cmd->cmd_args.whe_args[0].junction == AND){
			return match_condition(cmd, 0, user) && match_condition(cmd, 1, user); 
		}else if(cmd->cmd_args.whe_args[0].junction == OR){
			return match_condition(cmd, 0, user) || match_condition(cmd, 1, user); 
		}
	}
	return 0;
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }

    if (idxList) {
        for (idx = offset; idx < idxListLen; idx++) {
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
            print_user(get_User(table, idxList[idx]), &(cmd->cmd_args.sel_args));
        }
    } else {
        for (idx = offset; idx < table->len; idx++) {
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
            print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
        }
    }
}

///
/// Print aggregations
///
void print_aggr(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd){
	size_t idx;
	int count = 0, sum_id = 0, sum_age = 0;
	int limit = cmd->cmd_args.sel_args.limit;
	int offset = cmd->cmd_args.sel_args.offset;

	if(offset == -1)offset = 0;

	for(idx = offset; idx < idxListLen; idx++){
		if(limit != -1 && (idx - offset) >= limit)break;
		count++;
		User_t *user = get_User(table, idxList[idx]);
		sum_id += user->id;
		sum_age += user->age;
	}
	double avg_id = (count > 0 ? sum_id / (double)count : 0), avg_age = (count > 0 ? sum_age / (double)count : 0);

	printf("(");
	for(idx = 0; idx < cmd->cmd_args.sel_args.fields_len; idx++) {
		if(idx > 0)printf(", ");
		if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count", 5)){
			printf("%d", count);
		}else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "sum", 3)){
			if(!strncmp(cmd->cmd_args.sel_args.fields[idx]+4, "id", 2)){
				printf("%d", sum_id);
			}else if(!strncmp(cmd->cmd_args.sel_args.fields[idx]+4, "age", 3)){
				printf("%d", sum_age);
			}
		}else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "avg", 3)){
			if(!strncmp(cmd->cmd_args.sel_args.fields[idx]+4, "id", 2)){
				printf("%.3lf", avg_id);
			}else if(!strncmp(cmd->cmd_args.sel_args.fields[idx]+4, "age", 3)){
				printf("%.3lf", avg_age);
			}
		}
	}
	printf(")\n");
}

///
/// Update users
///
void update_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd){
	size_t idx;
	if(idxListLen > 1 && !strncmp(cmd->cmd_args.upd_args.field, "id", 2))return;
	for(idx = 0; idx < idxListLen; idx++){
		User_t user = *get_User(table, idxList[idx]);
		
		if(!strncmp(cmd->cmd_args.upd_args.field, "id", 2)){
			user.id = atoi(cmd->cmd_args.upd_args.literal);
		}else if(!strncmp(cmd->cmd_args.upd_args.field, "name", 4)){
			memcpy(user.name, cmd->cmd_args.upd_args.literal, sizeof(char)*(strlen(cmd->cmd_args.upd_args.literal)+1));
		}else if(!strncmp(cmd->cmd_args.upd_args.field, "email", 5)){
			memcpy(user.email, cmd->cmd_args.upd_args.literal, sizeof(char)*(strlen(cmd->cmd_args.upd_args.literal)+1));
		}else if(!strncmp(cmd->cmd_args.upd_args.field, "age", 3)){
			user.age = atoi(cmd->cmd_args.upd_args.literal);
		}
		update_user(table, idxList[idx], &user);
	}
}

///
///
///
void delete_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd){
	size_t idx;
	for(idx = 0; idx < idxListLen; idx++){
		delete_user(table, idxList[idx] - idx);
	}
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    } else if (!strncmp(cmd->args[0], "update", 6)){
	handle_update_cmd(table, cmd);
	return UPDATE_CMD;
    } else if (!strncmp(cmd->args[0], "delete", 6)){
	handle_delete_cmd(table, cmd);
	return DELETE_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
    int idxList[table->len];
    size_t idxListLen = 0;
    for(int idx = 0; idx < table->len; idx++){
	    if(match_conditions(cmd, get_User(table, idx)))idxList[idxListLen++] = idx;
    }
    if(cmd->cmd_args.sel_args.type == LIST)print_users(table, idxList, idxListLen, cmd);
    else if(cmd->cmd_args.sel_args.type == AGGREGATION)print_aggr(table, idxList, idxListLen, cmd);
    return table->len;
}

///
/// The return value is the number of rows select from table
/// If the update operation success, then change the input arg
/// `cmd->type` to UPDATE_CMD
///
int handle_update_cmd(Table_t *table, Command_t *cmd){
	cmd->type = UPDATE_CMD;
	update_state_handler(cmd, 1);
	
	int idxList[table->len];
	size_t idxListLen = 0;
	for(int idx = 0; idx < table->len; idx++){
		if(match_conditions(cmd, get_User(table, idx)))idxList[idxListLen++] = idx;
	}
	update_users(table, idxList, idxListLen, cmd);
	return table->len;

}

int handle_delete_cmd(Table_t *table, Command_t *cmd){
	cmd->type = DELETE_CMD;
	delete_state_handler(cmd, 1);

	int idxList[table->len];
	size_t idxListLen = 0;
	for(int idx = 0; idx < table->len; idx++){
		if(match_conditions(cmd, get_User(table, idx)))idxList[idxListLen++] = idx;
	}
	delete_users(table, idxList, idxListLen, cmd);
	return table->len;
}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

