//
// Created by Christopher Buruchian on 6/18/16.
//


#ifndef SYSTEMSOFTPROJ_SYMBOLTABLE_H
#define SYSTEMSOFTPROJ_SYMBOLTABLE_H

#define MAX_NAME_TABLE_SIZE 5000
#define IDENTS_MAX_CHARS 11 /*Maximum number of chars for idents.*/

//Table structs.
typedef struct{
    int kind;
    char name[IDENTS_MAX_CHARS];
    int val;
    int level;
    int adr;
} Name_Record;

typedef struct{
    int size;
    Name_Record* table[MAX_NAME_TABLE_SIZE];
} Symbol_table;

//Table methods.
void initializeSymbolTable(Symbol_table symbolTable);
unsigned int hash(Symbol_table* symbol_table, char *str);
Name_Record* lookup_string(Symbol_table* symbol_table, char *str);
int enter(Symbol_table* symbol_table, char *str, Name_Record* name_record);

#endif //SYSTEMSOFTPROJ_SYMBOLTABLE_H
