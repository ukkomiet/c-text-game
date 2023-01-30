#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int max = 1000;

// array of characters in the game, initialized to NULL
character** characters = NULL;
// count of characters
unsigned int count = 0;


void add(const char* name, int hp, const char* weapon_name, int weapon_dmg) {

    int name_l = strlen(name);
    int weapon_name_l = strlen(weapon_name);

    // check if the character is already in the game
    if (count != 0) {
        for (unsigned int i = 0; i < count; i++) {
            character* temp = characters[i];
            if (strcmp(name, temp->name) == 0) {
                printf("Fighter \"%s\" is already in the database.\n", temp->name);
                return;
            }
        }
    }

    // create the character from parameters
    character* tba = malloc(sizeof(character));
    tba->name = malloc(name_l+1);
    tba->weapon_name = malloc(weapon_name_l+1);
    strcpy(tba->name, name);
    strcpy(tba->weapon_name, weapon_name);
    tba->hp = hp;
    tba->exp = 0;
    tba->weapon_dmg = weapon_dmg;


    // add the character by first increasing character count and followingly reallocating more memory
    count++;
    if (count == 1) {
        characters = malloc(sizeof(character*));
    } else {
        characters = realloc(characters, (count) * sizeof(character*));
    }
    characters[count-1] = tba;

    printf("SUCCESS\n");
    return;
}

void attack(const char* attacker_name, const char* target_name) {

    // check who the attacker and target is
    character* attacker = NULL;
    int attacker_index = 0;
    character* target = NULL;
    int target_index = 0;

    for (unsigned int i = 0; i < count; i++) {
        character* temp = characters[i];
        if (strcmp(temp->name, attacker_name) == 0) {
            attacker = characters[i];
            attacker_index = i;
        }
        if (strcmp(temp->name, target_name) == 0) {
            target = characters[i];
            target_index = i;
        }
    }

    // check if the attacker or target was found
    if (!attacker) {
        printf("Attacker \"%s\" is not in the database.\n", attacker_name);
        return;
    }
    if (!target) {
        printf("Target \"%s\" is not in the database.\n", target_name);
        return;
    }
    // check if the attacker and target are the same
    if (attacker_index == target_index) {
        printf("Attacker \"%s\" cannot attack to itself.\n", attacker_name);
        return; 
    }

    unsigned int gained_exp = 0;
    if (target->hp <= attacker->weapon_dmg) { // target dies
        gained_exp = target->hp;
        attacker->exp = attacker->exp + gained_exp;
        target->hp = 0;

    } else {                                // target lives
        target->hp = target->hp - attacker->weapon_dmg;
        gained_exp = attacker->weapon_dmg;
        attacker->exp = attacker->exp + gained_exp;
    }
    // update the character array
    characters[attacker_index] = attacker;
    characters[target_index] = target;

    //print results
    printf("%s attacked %s with %s by %d damage.\n", attacker_name, target_name, attacker->weapon_name, attacker->weapon_dmg);
    printf("%s has %d hit points remaining.\n", target_name, target->hp);
    printf("%s gained %u experience points.\n", attacker_name, gained_exp);

    printf("SUCCESS\n");
    return;
}

void print_status() {

    // no sorting
    for (unsigned int i = 0; i < count; i++) {
        character* temp = characters[i];
        printf("%s %d %u %s %d\n", temp->name, temp->hp, temp->exp, temp->weapon_name, temp->weapon_dmg);
    }

    printf("SUCCESS\n");
    return;
}

void write(char* filename) {

    // open stream to file
    FILE* fp;
    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file\n");
        return;
    }

    //write every character to file
    for (unsigned int i = 0; i < count; i++) {
        character* temp = characters[i];
        fprintf(fp, "%s %d %u %s %d\n", temp->name, temp->hp, temp->exp, temp->weapon_name, temp->weapon_dmg);
    }
    fclose(fp);

    printf("SUCCESS\n");
    return;
}

void load(char* filename, char* lineBuffer) {

    // open stream to file
    FILE* fp;
    fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open file %s for reading.\n", filename);
        return;
    }

    // reset everything before loading
    for (unsigned int i = 0; i < count; i++) {
        character* temp = characters[i];
        free(temp->name);
        free(temp->weapon_name);
        free(temp);
    }
    count = 0;
    free(characters);
    characters = NULL;

    // add all characters from file using a buffer character string
    lineBuffer = fgets(lineBuffer, max, fp);
    while (lineBuffer) {
        char name[100] = "\0";
        unsigned int hp = 0;
        unsigned int exp = 0;
        char weapon_name[100] = "\0";
        unsigned int weapon_dmg = 0;
        sscanf(lineBuffer, "%s %d %u %s %d", name, &hp, &exp, weapon_name, &weapon_dmg);

        count++;
        characters = realloc(characters, sizeof(character) * count);

        int name_l = strlen(name);
        int weapon_name_l = strlen(weapon_name);

        character* temp = malloc(sizeof(character));
        temp->name = malloc(name_l+1);
        strcpy(temp->name, name);
        temp->weapon_name = malloc(weapon_name_l+1);
        strcpy(temp->weapon_name, weapon_name);
        temp->hp = hp;
        temp->exp = exp;
        temp->weapon_dmg = weapon_dmg;

        characters[count-1] = temp;

        lineBuffer = fgets(lineBuffer, max, fp);
    }
    fclose(fp);

    printf("SUCCESS\n");
    return;
}

void quit() {

    for (unsigned int i = 0; i < count; i++) {
        character* temp = *(characters+i);
        free(temp->name);
        free(temp->weapon_name);
        free(temp);
    }
    free(characters);
    printf("SUCCESS\n");
    return;
}



int main(void) {

    char str[1000]; // input line buffer
    char command;

    while (1) {

        fgets(str, max, stdin); // acquire input string from command line 
        sscanf(str, "%c", &command); // determine the command from string and determine following action

        char name1[100]; // input buffer for name 1
        char name2[100]; // input buffer for name 2
        int hp;
        int weapon_dmg;
        int ret;

        switch (command) {
            case 'A':
                // scan the input line
                ret = sscanf(str, "A %s %d %s %d\n", name1, &hp, name2, &weapon_dmg); 

                if(ret == EOF) {
                    printf("Error scanning input\n");
                    break;
                }
                if (ret != 4) {
                    printf("A should be followed by exactly 4 arguments.\n");
                    break;
                }
                            	
                if (weapon_dmg < 1) {
                    printf("Max damage cannot be lower than 1\n");
                    break;
                }
                if (hp < 1) {
                    printf("HP cannot be lower than 1\n");
                    break;
                }

                add(name1, hp, name2, weapon_dmg);
                break;

            case 'H':
                // scan the input line
                ret = sscanf(str, "H %s %s\n", name1, name2);
                if (ret == EOF) {
                    printf("Error scanning input\n");
                    break;
                }
                if (ret != 2) {
                    printf("H should be followed by exactly 2 arguments.\n");
                    break;
                }

                attack(name1, name2);
                break;

            case 'L':
                print_status();
                break;

            case 'W':
                // scan the input line
                sscanf(str, "W %s\n", name1);
                write(name1);
                break;

            case 'O':
                // scan the input line
                sscanf(str, "O %s\n", name1);
                load(name1, str);
                break;

            case 'Q':
                quit();
                return 0;

            default:
                //invalid command
                printf("Invalid command %c\n", command);
        }
    }
}
