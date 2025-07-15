#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define MAX_RULES 16
#define MAX_HOOKS 10
#define MAX_TAGS 10
#define MAX_STRING_LEN 256
#define MAX_LONG_STRING_LEN 2048

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


// --- HÀM MAIN ---
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

        case YAML_FLOW_SEQUENCE_START_TOKEN: // '['
            if (strcmp(current_key, "hooked") == 0) parsing_hooked_array = 1;
            else if (strcmp(current_key, "tags") == 0) parsing_tags_array = 1;
            break;
        
        case YAML_FLOW_SEQUENCE_END_TOKEN: // ']'
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

    // --- IN KẾT QUẢ ĐÃ PARSE ---
    int total_rules = rule_count + 1;
    if (rules[0].name[0] == '\0') total_rules = 0; // Xử lý trường hợp file rỗng

    printf("--- PARSED %d RULES ---\n\n", total_rules);
    for (int i = 0; i < total_rules; i++) {
        printf("- rule: %s\n", rules[i].name);
        printf("  id: %s\n", rules[i].id);
        printf("  des: %s\n", rules[i].description);
        printf("  path: %s\n", rules[i].path);
        printf("  fname: %s\n", rules[i].fname);
        printf("  action: %s\n", rules[i].action);
        printf("  priority: %s\n", rules[i].priority);
        printf("  output: %s\n", rules[i].output);
        printf("  condition: %s\n", rules[i].condition);
        
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