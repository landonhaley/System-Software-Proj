//
// Created by Christopher Buruchian on 6/18/16.
//

#include <stdio.h>
#include <string.h>
#include "symbolTable.h"

/**
 * This method will initalize our symbolTable to nil.
 */
void initializeSymbolTable(Symbol_table symbolTable){
    int i = 0;
    for (i = 0; i < MAX_NAME_TABLE_SIZE; i++) {
        symbolTable.table[i] = NULL;
    }
}

/**
 * This method calculates the hash for our symbolTable.
 */
unsigned int hash(Symbol_table* symbol_table, char *str) {
    unsigned int hashval;

    /* we start our hash out at 0 */
    hashval = 0;

    /* for each character, we multiply the old hash by 31 and add the current
     * character. */

    for(; *str != '\0'; str++) hashval = *str + (hashval << 5) - hashval;

    /* we then return the hash tokenType mod the hashtable size so that it will
     * fit into the necessary range
     */
    return hashval % symbol_table->size;
}

Name_Record* lookup_string(Symbol_table* symbol_table, char *str){
    Name_Record* name_record = NULL;
    unsigned int hashVal = hash(symbol_table, str);
    int q = 0;

    //Utilizing quadratic probing.
    while(symbol_table->table[(hashVal + q*q) % symbol_table->size] != NULL){
        name_record = symbol_table->table[(hashVal+q*q) % symbol_table->size];
        //We found a match.
        if(strcmp(str, name_record->name) == 0){
            return name_record;
        }
        //Keep looping until NULL is found.
        q++;
    }

    if(name_record )
    //Nothings found return null.
    return NULL;
}

int enter(Symbol_table* symbol_table, char *str, Name_Record* name_record){
    unsigned int hashVal = hash(symbol_table, str);
    int q = 0;

    //Utilizing quadratic probing.
    while(symbol_table->table[(hashVal + q*q) % symbol_table->size] != NULL){
        q++;
    }

    if(symbol_table->table[(hashVal + q*q) %symbol_table->size] != NULL){
        symbol_table->table[(hashVal + q*q) % symbol_table -> size] = name_record;
    }
}

