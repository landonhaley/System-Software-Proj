/*
 * Author: Christopher Buruchian
 * Project: Build a Scanner
 * Course: System Software 3402
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "symbolTable.h"

//GLOBALS
//Symbol_table symbolTable;

typedef struct{
    char name[IDENTS_MAX_CHARS+1];
    int tokenType;
}Lexeme_list;

Lexeme_list lexeme_list[MAX_NAME_TABLE_SIZE];
//This tracker is used to keep track of where in the list we are currently at. It is an incremental counter.
int lexeme_list_tracker = 0;
//This global is used to keep track of if any of the transition functions reached EOF and to signal to the DFA simulator to stop.
int REACHED_EOF = 0;

int main(int argc, char* argv[]) {

    //Creates our symbol table.
    //TODO: Refactor code to utilize symbolTable rather than an array of lexemes. (I plan on implementing this before the final project).
    //initializeSymbolTable(symbolTable);

    //Initializes symbolList and our lexeme_list_tracker.
    initializeSpecialSymbolList();
    lexeme_list_tracker = 0;

    //Removes any comments from the source code.
    generateCleanSourceInput();
    //Runs out DFAish machine.
    runDFAMachine();

    //Outputs the results to their respective files.
    outputLexemeTableFile();
    outputLexemeListToFile();
}

/**
 * This method simulates how a DFA would operate (kind of). It reads in characters from the input and decides which transitions are appropriate. It will continue to do so until it is stopped.
 */
void runDFAMachine(){
    FILE* inputFile;

    printf("\nRunning Machine...\n");

    inputFile = fopen(CLEAN_INPUT_FILENAME,"r");

    if(inputFile == NULL){
        printf("File \"%s\" has failed to open...\n", INPUT_FILENAME);
        return;
    }

    int charInput = 0;
    charInput = fgetc(inputFile);

    while(charInput != EOF){
        //Skips white space characters.
        if(charInput ==' ' || charInput == '\t' || charInput == '\r' || charInput == '\n'){
            charInput = fgetc(inputFile);
            continue;
        }

        if(isalpha(charInput)){
            //Handles identifierAndReserveWord transitions
            handleIdentifierAndReserveWordCase(inputFile, charInput);
        } else if (isdigit(charInput)){
            //Handles number transitions
            handleNumberCase(inputFile, charInput);
        } else {
            //Handles special symbols.
            handleSpecialSymbolCase(inputFile, charInput);
        }
        //If EOF reached in a special case break.
        if(REACHED_EOF == 0 && lookAheadFunction(inputFile) != EOF) {
            charInput = fgetc(inputFile);
        }else{
            break;
        }
    }

    //Closes our input file.
    fclose(inputFile);
}

/**
 * This method handles the outputting of the lexemeTable to the "lexemeTable.txt" file.
 */
void outputLexemeTableFile(){
    FILE* outputFile;
    outputFile = fopen(OUTPUT_LEXEMETABLE_FILENAME, "w");

    //Table Header:
    fprintf(outputFile, "%12s \t token_type\n", "lexeme");

    int i = 0;

    //Prints table
    for (i = 0; i < lexeme_list_tracker; i++){
        fprintf(outputFile, "%12s \t %d\n", lexeme_list[i].name, lexeme_list[i].tokenType);
    }

    fclose(outputFile);
}

/**
 * This method handles the outputting of the lexemeList to the "lexemelist.txt" file.
 */
void outputLexemeListToFile(){

    FILE* outputFile;
    outputFile = fopen(OUTPUT_LEXEMELIST_FILENAME, "w");

    int i = 0;
    for(i = 0; i < lexeme_list_tracker; i++){
        if(lexeme_list[i].tokenType == 2 || lexeme_list[i].tokenType == 3){
            fprintf(outputFile, "%d ", lexeme_list[i].tokenType);
            fprintf(outputFile, "%s ", lexeme_list[i].name);
        }else{
            fprintf(outputFile, "%d ", lexeme_list[i].tokenType);
        }
    }

    fclose(outputFile);
}

/**
 * This method handles the transitions that start with a special symbol.
 */
void handleSpecialSymbolCase(FILE* inputFile, char firstChar){
    int i = 0;
    char specialSymbol = -1;
    for (i = 0; i < NUM_OF_SPECIAL_SYMBOLS; i++){
        if(firstChar == specialSymbolList[i]){
            specialSymbol = firstChar;
        }
    }

    //Checks if the symbol inputted does not match any on the list.
    if(specialSymbol != firstChar){
        printf(ERROR_CODE_4);
        return;
    }

    int lookAheadChar = 0;

    switch(specialSymbol){
        case '+':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = plussym;
            lexeme_list_tracker++;
            break;
        case '/':
            lookAheadChar = lookAheadFunction(inputFile);
            if(lookAheadChar == '*'){
                handleCommentCase(inputFile, specialSymbol);
            }else{
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].tokenType = slashsym;
                lexeme_list_tracker++;
            }
            break;
        case '=':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = eqlsym;
            lexeme_list_tracker++;
            break;
        case '-':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = minussym;
            lexeme_list_tracker++;
            break;
        case '(':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = lparentsym;
            lexeme_list_tracker++;
            break;
        case ',':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = commasym;
            lexeme_list_tracker++;
            break;
        case '<':
            lookAheadChar = lookAheadFunction(inputFile);
            // <= case
            if(lookAheadChar == '='){
                fgetc(inputFile);
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].name[1] = '=';
                lexeme_list[lexeme_list_tracker].tokenType = leqsym;
                //neq case
            }else if(lookAheadChar == '>'){
                fgetc(inputFile);
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].name[1] = '>';
                lexeme_list[lexeme_list_tracker].tokenType = neqsym;
                // < case.
            }else{
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].tokenType = lessym;
            }
            lexeme_list_tracker++;
            break;
        case '*':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = multsym;
            lexeme_list_tracker++;
            break;
        case ')':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = rparentsym;
            lexeme_list_tracker++;
            break;
        case '.':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = periodsym;
            lexeme_list_tracker++;
            break;
        case '>':
            lookAheadChar = lookAheadFunction(inputFile);
            // >= case
            if(lookAheadChar == '='){
                fgetc(inputFile);
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].name[1] = '=';
                lexeme_list[lexeme_list_tracker].tokenType = geqsym;
                // > case
            }else{
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].tokenType = gtrsym;
            }
            lexeme_list_tracker++;
            break;
        case ';':
            lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
            lexeme_list[lexeme_list_tracker].tokenType = semicolonsym;
            lexeme_list_tracker++;
            break;
        case ':':
            lookAheadChar = lookAheadFunction(inputFile);
            // := case
            if(lookAheadChar == '='){
                fgetc(inputFile);
                lexeme_list[lexeme_list_tracker].name[0] = specialSymbol;
                lexeme_list[lexeme_list_tracker].name[1] = '=';
                lexeme_list[lexeme_list_tracker].tokenType = becomessym;
                lexeme_list_tracker++;
            }else{
                printf(ERROR_CODE_4);
            }
            break;
    }

}

/**
 * Handles comment case if any still exist after the source code is cleaned.
 */
void handleCommentCase(FILE* inputFile, char firstChar){
    int charInput = firstChar;
    while(charInput != EOF){
        charInput = fgetc(inputFile);
        if(charInput == '*'){
            charInput = fgetc(inputFile);
            while(charInput != '*'){
                charInput = fgetc(inputFile);
                if(charInput == EOF){
                    return;
                }
            }

            charInput = fgetc(inputFile);
            while(charInput != '/'){
                charInput = fgetc(inputFile);
                if(charInput == EOF){
                    return;
                }
            }
        }
        charInput = fgetc(inputFile);
    }
}

/**
 * This method handles the transitions that begin with a number.
 */
void handleNumberCase(FILE* inputFile, char firstChar){
    char numberString[INTEGER_NUMBER_PLACES_MAX+2];
    //Initializes string to 0. Gets rid of junk.
    memset(numberString, 0, INTEGER_NUMBER_PLACES_MAX);

    int index = 0;
    numberString[index] = firstChar;
    index++;

    int charInput = 0;
    int run = 0;
    int lookAheadChar = 0;

    lookAheadChar = lookAheadFunction(inputFile);
    if(isalpha(lookAheadChar) || isdigit(lookAheadChar)) {
        run = 1;
        charInput = fgetc(inputFile);
    }

    while(charInput != EOF && index < INTEGER_NUMBER_PLACES_MAX + 1 && run == 1){
        printf("ATOI: %d\n", atoi(numberString));
        numberString[index] = charInput;

        if(isalpha(charInput)){
            printf(ERROR_CODE_1);
            while(isalpha((fgetc(inputFile)))||isdigit(fgetc(inputFile))){}
            return;
        }

        lookAheadChar = lookAheadFunction(inputFile);

        if(!isdigit(lookAheadChar)){
            break;
        }

        charInput = fgetc(inputFile);
        index++;
    }

    if(index > INTEGER_NUMBER_PLACES_MAX || atoi(numberString) >= INTEGER_MAX){
        lookAheadChar = lookAheadFunction(inputFile);

        while(isdigit(charInput) && isdigit(lookAheadChar)){
            charInput = fgetc(inputFile);
        }


        if(charInput == EOF || lookAheadChar == EOF){
            REACHED_EOF = 1;
        }

        //Returns to machine.
        printf(ERROR_CODE_2);
        return;
    }

    if(charInput == EOF || lookAheadFunction(inputFile) == EOF || lookAheadChar == EOF){
        REACHED_EOF = 1;
    }

    numberString[index + 1] = '\0';

    //If it made it this far it is a valid numbersym.
    strcpy(lexeme_list[lexeme_list_tracker].name, numberString);
    lexeme_list[lexeme_list_tracker].tokenType = numbersym;
    lexeme_list_tracker++;
}

/**
 * This method handles reserveWords and Identifiers
 */
void handleIdentifierAndReserveWordCase(FILE *inputFile, char firstChar){
    //Build a string and check its length...
    char identifierString[IDENTS_MAX_CHARS + 1];
    //Initializes string to 0. Gets rid of junk.
    memset(identifierString, 0, IDENTS_MAX_CHARS + 1);

    int index = 0;
    identifierString[index] = firstChar;
    index++;

    int lookAheadChar = lookAheadFunction(inputFile);
    int charInput = 0;
    if(isdigit(lookAheadChar) || isalpha(lookAheadChar)) {
        charInput = fgetc(inputFile);
    }

    while(charInput != EOF && (isalpha(lookAheadChar)||isdigit(lookAheadChar))){
        //If identifer meets or exceeds the max chars, return without accepting it as a token.
        if(index >= IDENTS_MAX_CHARS){
            printf(ERROR_CODE_3);
            return;
        }
        identifierString[index] = charInput;
        lookAheadChar = lookAheadFunction(inputFile);
        if(isdigit(lookAheadChar) || isalpha(lookAheadChar)) {
            charInput = fgetc(inputFile);
        }
        index++;
    }

    identifierString[index] = '\0';

    if(lookAheadChar == EOF){
        REACHED_EOF = 1;
    }

    if(checkReservedWords(identifierString) == 1){
        //Found reserved word
        return;
    }else{
        //Otherwise it is an identifier.
        strcpy(lexeme_list[lexeme_list_tracker].name, identifierString);
        lexeme_list[lexeme_list_tracker].tokenType = identsym;
        lexeme_list_tracker++;
    }

}

int checkReservedWords(char* string){
    int i = 0;

    for(i = 0; i < RESERVERD_WORDS_MAX; i++){
        if(strcmp(string, word[i]) == 0){
            strcpy(lexeme_list[lexeme_list_tracker].name, word[i]);
            lexeme_list[lexeme_list_tracker].tokenType = wsym[i];
            lexeme_list_tracker++;
            return 1;
        }
    }

    return -1;
}

/**
 * This method initializes the ssym array with its appropriate values.
 */
void initializeSpecialSymbolList(){
    ssym['+'] = plussym;
    ssym['/'] = slashsym;
    ssym['='] = eqlsym;
    ssym['-'] = minussym;
    ssym['('] = lparentsym;
    ssym[','] = commasym;
    ssym['<'] = lessym;
    ssym['*'] = multsym;
    ssym[')'] = rparentsym;
    ssym['.'] = periodsym;
    ssym['>'] = gtrsym;
    ssym[';'] = semicolonsym;
    ssym[':'] = becomessym;
}

/**
 * This function generates the source code without any comments.
 */
void generateCleanSourceInput(){
    FILE* inputFile;
    FILE* cleanInputFile;

    printf("Reading Input...\n");

    inputFile = fopen(INPUT_FILENAME,"r");
    //Outputs clean input
    cleanInputFile = fopen(CLEAN_INPUT_FILENAME, "w");

    if(inputFile == NULL){
        printf("File \"%s\" has failed to open...\n", INPUT_FILENAME);
        return;
    }

    int charInput = 0;
    charInput = fgetc(inputFile);

    //Removes comments from the file.
    while(charInput != EOF){
        if(charInput == '/' && lookAheadFunction(inputFile) == '*'){
            charInput = fgetc(inputFile);
            if(charInput == '*'){
                charInput = fgetc(inputFile);
                while(charInput != '*'){
                    charInput = fgetc(inputFile);
                    if(charInput == EOF){
                        return;
                    }
                }

                charInput = fgetc(inputFile);
                while(charInput != '/'){
                    charInput = fgetc(inputFile);
                    if(charInput == EOF){
                        return;
                    }
                }
            }
        }else{
            printf("%c ", charInput);
            fputc(charInput,cleanInputFile);
        }

        charInput = fgetc(inputFile);
    }

    fclose(inputFile);
    fclose(cleanInputFile);
}

/**
 * This function looks ahead one character and returns the character that it finds in the inputFile.
 */
int lookAheadFunction(FILE* file){
    int nextChar = fgetc(file);
    fseek(file, -1 * sizeof(char), SEEK_CUR);
    return nextChar;
}