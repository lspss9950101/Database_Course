#ifndef COMMAND_H
#define COMMAND_H

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
    UPDATE_CMD,
    DELETE_CMD,
};

enum {
	EQ = 1,
	NEQ,
	GT,
	LT,
	GE,
	LE,
};

enum {
	AND = 1,
	OR,
};

enum {
	AGGREGATION = 1,
	LIST,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct WhereArgs {
	char *field;
	int op;
	union{
		char* arg_char;
		double arg_num;
	}literal;
	int junction;
} WhereArgs_t;

typedef struct SelectArgs {
    int type;
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
} SelectArgs_t;

typedef struct UpdateArgs {
	char *field, *literal;	
} UpdateArgs_t;

typedef struct {
    union{
	    SelectArgs_t sel_args;
	    UpdateArgs_t upd_args;
    };
    WhereArgs_t *whe_args;
    int where_len;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
    CmdArg_t cmd_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
int add_where_condition(Command_t *cmd, const WhereArgs_t *argument);
void cleanup_Command(Command_t *cmd);

#endif
