#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "netlink.h"

#define COMMAND_NONE -1
#define COMMAND_AND   0
#define COMMAND_OR    1

#define OPERATOR_EQUALS 0
#define OPERATOR_IN     1

#define MAX_FIELD_SIZE  64
#define MAX_VALUE_SIZE  64

struct command {
    int flag;                 // AND / OR / NONE
    int operator;             // == / in
    char field[MAX_FIELD_SIZE]; 
    char value[MAX_VALUE_SIZE]; 
};

#define MAX_RULES 100
#define MAX_HOOKS 10
#define MAX_TAGS 10
#define MAX_STRING_LEN 256
#define MAX_LONG_STRING_LEN 1024

typedef struct {
    char name[MAX_STRING_LEN];
    char id[64];
    char description[1024];
    char path[1024];
    char fname[MAX_STRING_LEN];
    char action[64];
    char priority[64];
    char output[1024];
    char condition[MAX_LONG_STRING_LEN];
    
    char hooked[MAX_HOOKS][64];
    int hooked_count;
    
    char tags[MAX_TAGS][64];
    int tags_count;
} edr_rule_t;

edr_rule_t rules[MAX_RULES];
int rule_count = 0;

char current_key[128] = {0};
int expect_value = 0;
int parsing_hooked_array = 0;
int parsing_tags_array = 0;

void process_scalar(yaml_token_t token) {
    edr_rule_t *current_rule = &rules[rule_count];

    if (parsing_hooked_array) {
        if (current_rule->hooked_count < MAX_HOOKS) {
            strncpy(current_rule->hooked[current_rule->hooked_count++], (char*)token.data.scalar.value, 64 - 1);
        }
        return;
    }
    if (parsing_tags_array) {
        if (current_rule->tags_count < MAX_TAGS) {
            strncpy(current_rule->tags[current_rule->tags_count++], (char*)token.data.scalar.value, 64 - 1);
        }
        return;
    }

    if (!expect_value) { 
        strncpy(current_key, (char *)token.data.scalar.value, sizeof(current_key) - 1);
    } else {
        if (strcmp(current_key, "rule") == 0) strncpy(current_rule->name, (char*)token.data.scalar.value, sizeof(current_rule->name) - 1);
        else if (strcmp(current_key, "id") == 0) strncpy(current_rule->id, (char*)token.data.scalar.value, sizeof(current_rule->id) - 1);
        else if (strcmp(current_key, "des") == 0) strncpy(current_rule->description, (char*)token.data.scalar.value, sizeof(current_rule->description) - 1);
        else if (strcmp(current_key, "path") == 0) strncpy(current_rule->path, (char*)token.data.scalar.value, sizeof(current_rule->path) - 1);
        else if (strcmp(current_key, "fname") == 0) strncpy(current_rule->fname, (char*)token.data.scalar.value, sizeof(current_rule->fname) - 1);
        else if (strcmp(current_key, "action") == 0) strncpy(current_rule->action, (char*)token.data.scalar.value, sizeof(current_rule->action) - 1);
        else if (strcmp(current_key, "priority") == 0) strncpy(current_rule->priority, (char*)token.data.scalar.value, sizeof(current_rule->priority) - 1);
        else if (strcmp(current_key, "output") == 0) strncpy(current_rule->output, (char*)token.data.scalar.value, sizeof(current_rule->output) - 1);
        else if (strcmp(current_key, "condition") == 0) strncpy(current_rule->condition, (char*)token.data.scalar.value, sizeof(current_rule->condition) - 1);
    }
}

int parse_conditions(const char *input_str, struct command *cmds, int max_cmds) {
    char buffer[1024];
    strncpy(buffer, input_str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *tokens[256];
    int token_count = 0;

    char *token = strtok(buffer, " ");
    while (token && token_count < 256) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }

    int i = 0, cmd_count = 0;
    int current_flag = COMMAND_NONE;

    while (i < token_count - 2 && cmd_count < max_cmds) {
        char *field = tokens[i];
        char *op = tokens[i + 1];
        char *val = tokens[i + 2];

        struct command *c = &cmds[cmd_count];
        c->flag = current_flag;

        strncpy(c->field, field, MAX_FIELD_SIZE - 1);
        c->field[MAX_FIELD_SIZE - 1] = '\0';

        strncpy(c->value, val, MAX_VALUE_SIZE - 1);
        c->value[MAX_VALUE_SIZE - 1] = '\0';

        if (strcmp(op, "==") == 0) {
            c->operator = OPERATOR_EQUALS;
        } else if (strcmp(op, "in") == 0) {
            c->operator = OPERATOR_IN;
        } else {
            fprintf(stderr, "Unknown operator: %s\n", op);
            return -1;
        }

        cmd_count++;
        i += 3;

        if (i < token_count) {
            if (strcmp(tokens[i], "and") == 0) {
                current_flag = COMMAND_AND;
            } else if (strcmp(tokens[i], "or") == 0) {
                current_flag = COMMAND_OR;
            } else {
                current_flag = COMMAND_NONE;
            }
            i++;
        }
    }

    return cmd_count;
}


int main() {
    FILE *fh = fopen("config.yaml", "r");
    if (!fh) {
        perror("fopen: config.yaml");
        return 1;
    }

    yaml_parser_t parser;
    yaml_token_t token;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fh);

    int done = 0;
    while (!done) {
        if (!yaml_parser_scan(&parser, &token)) {
            fprintf(stderr, "Error parsing YAML\n");
            break;
        }

        switch (token.type) {
        case YAML_STREAM_END_TOKEN:
            done = 1;
            break;

        case YAML_BLOCK_ENTRY_TOKEN: 
            if (rules[rule_count].name[0] != '\0') { 
                 if(rule_count < MAX_RULES -1) {
                    rule_count++;
                 }
            }
            rules[rule_count].hooked_count = 0;
            rules[rule_count].tags_count = 0;
            break;

        case YAML_KEY_TOKEN:
            expect_value = 0;
            break;
        case YAML_VALUE_TOKEN:
            expect_value = 1;
            break;

        case YAML_SCALAR_TOKEN:
            process_scalar(token);
            break;

        case YAML_FLOW_SEQUENCE_START_TOKEN: 
            if (strcmp(current_key, "hooked") == 0) parsing_hooked_array = 1;
            else if (strcmp(current_key, "tags") == 0) parsing_tags_array = 1;
            break;
        
        case YAML_FLOW_SEQUENCE_END_TOKEN:
            parsing_hooked_array = 0;
            parsing_tags_array = 0;
            break;

        default:
            break;
        }

        yaml_token_delete(&token);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    int total_rules = rule_count + 1;
    if (rules[0].name[0] == '\0') total_rules = 0; 

    struct edr_event_hdr event;

    printf("--- PARSED %d RULES ---\n\n", total_rules);
    for (int i = 0; i < total_rules; i++) {
        // printf("- rule: %s\n", rules[i].name);
        // printf("  id: %s\n", rules[i].id);
        // printf("  des: %s\n", rules[i].description);
        // printf("  path: %s\n", rules[i].path);
        // printf("  fname: %s\n", rules[i].fname);
        // printf("  action: %s\n", rules[i].action);
        // printf("  priority: %s\n", rules[i].priority);
        // printf("  output: %s\n", rules[i].output);
        // printf("  condition: %s\n", rules[i].condition);


        event.name = 

        struct command cmds[10];

        int n = parse_conditions(rules[i].condition, cmds, 10);

        for (int i = 0; i < n; ++i) {
            printf("Command %d:\n", i);
            printf("  flag     : %s\n",
                cmds[i].flag == COMMAND_AND ? "AND" :
                cmds[i].flag == COMMAND_OR  ? "OR" : "NONE");
            printf("  field    : %s\n", cmds[i].field);
            printf("  operator : %s\n", cmds[i].operator == OPERATOR_EQUALS ? "==" : "in");
            printf("  value    : %s\n", cmds[i].value);
            printf("\n");
        }

        
        printf("  hooked: [");
        for (int j = 0; j < rules[i].hooked_count; j++) {
            printf("%s%s", rules[i].hooked[j], (j == rules[i].hooked_count - 1) ? "" : ", ");
        }
        printf("]\n");

        printf("  tags: [");
        for (int j = 0; j < rules[i].tags_count; j++) {
            printf("%s%s", rules[i].tags[j], (j == rules[i].tags_count - 1) ? "" : ", ");
        }

        printf("]\n\n");
    }

    return 0;
}