//
// Created by Christopher Buruchian on 6/18/16.
//

#ifndef SYSTEMSOFTPROJ_SCANNER_H
#define SYSTEMSOFTPROJ_SCANNER_H

#define RESERVERD_WORDS_MAX 15  /*Number of reserved words*/
#define INTEGER_MAX 32767 /*Maximum integer value*/
#define INTEGER_NUMBER_PLACES_MAX 5
#define IDENTS_MAX_CHARS 11 /*Maximum number of chars for idents.*/
#define nestmax 5 /*Maximum depth of block testing*/
#define strmax 256 /*Maximum length of strings*/
#define NUM_OF_SPECIAL_SYMBOLS 13


//INPUT/OUTPUT File names
#define INPUT_FILENAME "input.txt"
#define CLEAN_INPUT_FILENAME "cleaninput.txt"
#define OUTPUT_LEXEMETABLE_FILENAME "lexemetable.txt"
#define OUTPUT_LEXEMELIST_FILENAME "lexemelist.txt"

//ERROR CODES
#define ERROR_CODE_1 "***ERROR!:Variable does not start with letter***\n"
#define ERROR_CODE_2 "***ERROR!:Number too long***\n"
#define ERROR_CODE_3 "***ERROR!:Name too long***\n"
#define ERROR_CODE_4 "***ERROR!:INVALID SYMBOLS***\n"

//Initalize functions.
void runDFAMachine();
void initializeSpecialSymbolList();
void generateCleanSourceInput();
int lookAheadFunction(FILE* file);

//Handle special cases
void handleIdentifierAndReserveWordCase(FILE *inputFile, char firstChar);
void handleNumberCase(FILE* inputFile, char firstChar);
void handleSpecialSymbolCase(FILE* inputFile, char firstChar);
void handleCommentCase(FILE* inputFile, char firstChar);
void outputLexemeListToFile();
void outputLexemeTableFile();

int checkReservedWords(char* string);

//Declaration of Token Types
typedef enum {
    nulsym = 1,
    identsym,
    numbersym,
    plussym,
    minussym,
    multsym,
    slashsym,
    oddsym,
    eqlsym,
    neqsym,
    lessym,
    leqsym,
    gtrsym,
    geqsym,
    lparentsym,
    rparentsym,
    commasym,
    semicolonsym,
    periodsym,
    becomessym,
    beginsym,
    endsym,
    ifsym,
    thensym,
    whilesym,
    dosym,
    callsym,
    constsym,
    varsym,
    procsym,
    writesym,
    readsym,
    elsesym
}token_type;

char *word[] = {
                "null",
                "begin",
                "call",
                "const",
                "do",
                "else",
                "end",
                "if",
                "odd",
                "procedure",
                "read",
                "then",
                "var",
                "while",
                "write"
};

int wsym[] = {
              nulsym,
              beginsym,
              callsym,
              constsym,
              dosym,
              elsesym,
              endsym,
              ifsym,
              oddsym,
              procsym,
              readsym,
              thensym,
              varsym,
              whilesym,
              writesym
};

char specialSymbolList[15] = {'+', '/', '=', '-', '(', ',', '<', '*', ')', '.','>',';', ':'};

int ssym[256];

#endif //SYSTEMSOFTPROJ_SCANNER_H
