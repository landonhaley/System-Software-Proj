/**
 * Author: Christopher Buruchian
 * Project: Build a VM
 * Course: System Software 3402
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "vm.h"

#define DEBUG_MODE 0

/**
 * This enum maps the main ISA codes to ints.
 */
typedef enum{
    LIT = 1,
    OPR = 2,
    LOD = 3,
    STO = 4,
    CAL = 5,
    INC = 6,
    JMP = 7,
    JPC = 8,
    SIO1 = 9,
    SIO2 = 10,
    SIO3 = 11
}isa_codes;

/**
 * This enum maps each of the OPR operations to ints.
 */
typedef enum{
    RET = 0,
    NEG = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    ODD = 6,
    MOD = 7,
    EQL = 8,
    NEQ = 9,
    LSS = 10,
    LEQ = 11,
    GTR = 12,
    GEQ = 13
}opr_codes;

/**
 * Instruction stuct.
 */
typedef struct {
    int op;
    int  l;
    int  m;
}Instruction;

//Registers
int SP;
int BP;
int PC;
Instruction IR;

//Handles halt conditions.
int halt = 0;

//Init Methods.
void initializeRegisters();
void initializeStack(int* stack);

//Print Methods
void printProgram(Instruction* code, int instructionCount);
void printProgramExecution(int lineNumber, int* stack, int* activationRecordList);
char* codeString(int code);

//Execution Cycle Methods.
int executionCycle(int* stack, int* activationRecordList, int* activationRecordTracker);
int oprOperation(int* stack, int* activationRecordList, int* activationRecordTracker);

//Misc Methods.
int loadData(char* argv, Instruction* instructions);
void outputTxt(char* string, ...);
int base(int l,int* stack);

FILE* file;

int main(int argc, char* argv[]){

    //Init registers.
    initializeRegisters();

    //Init halt.
    halt = 0;

    //Allocate Stack and Activation Record List
    int stack[MAX_STACK_HEIGHT];
    int activationRecordList[MAX_STACK_HEIGHT];
    int activationRecordTracker = 0;

    //Init stack
    initializeStack(stack);

    //Instructions
    Instruction code[MAX_CODE_LENGTH];

    int instructionCount = loadData(argv[1], code);
    int i = 0;

    file = fopen("stacktrace.txt", "w");

    //Prints the program in interpreted assembly language with line numbers.
    printProgram(code, instructionCount);

    int lineNumber = 0;

    printProgramExecution(-1,stack,activationRecordList);

    while(1){

        lineNumber = PC;

        //Fetch Cycle.
        IR = code[PC++];

        //PC increments by 1 right after fetch cycle.

        //Execute Cycle.
        if(!executionCycle(stack, activationRecordList, &activationRecordTracker)){
            //If the execution cycle fails for some reason, break out of the loop and halt the program.
            printProgramExecution(lineNumber,stack,activationRecordList);
            if(halt){
                outputTxt("Successfully halted.");
            }
            break;
        }

        printProgramExecution(lineNumber,stack,activationRecordList);
    }

    fclose(file);
}

void printProgram(Instruction* code, int instructionCount){
    int i = 0;
    //Header
    outputTxt("\n Line\t OP\t\t L\t M\n");

    for(i = 0; i < instructionCount; i++){
        outputTxt(" %2d\t\t %s\t %d\t %d\n", i, codeString(code[i].op), code[i].l, code[i].m);
    }

    outputTxt("\n");
}

/**
 * This method exists as a wrapper method for the output of the program.
 *
 * If DEBUG_MODE is enabled, print to console...
 * Otherwise, print to stacktrace.txt file.
 */
void outputTxt(char* string, ...){

    va_list args;
    va_start(args, string);

    if(DEBUG_MODE){
        vprintf(string, args);
    }else{
        vfprintf(file, string, args);
    }

    va_end(args);
}

void printProgramExecution(int lineNumber, int* stack, int* activationRecordList){

    int i = 0, j = 0;

    if(lineNumber < 0) {
        outputTxt("                 %3s %3s %3s  %5s\n", "pc", "bp", "sp", "stack");
        outputTxt("Initial values   %3d %3d %3d  ", PC, BP, SP);
    }else{
        outputTxt("%3d  %s %3d %3d ", lineNumber, codeString(IR.op), IR.l, IR.m);
        outputTxt("%3d %3d %3d  ", PC, BP, SP);
    }

    for(i = 1; i <= SP; i++){

        if(activationRecordList[j] == i)
        {
            outputTxt("| ");
            j++;
        }
        outputTxt("%d ", stack[i]);
    }

    outputTxt("\n");
}

char* codeString(int code){
    switch((isa_codes) code){
        case LIT:
            return "lit";
        case OPR:
            return "OPR";
        case LOD:
            return "lod";
        case STO:
            return "sto";
        case CAL:
            return "cal";
        case INC:
            return "inc";
        case JMP:
            return "jmp";
        case JPC:
            return "jpc";
        case SIO1:
        case SIO2:
        case SIO3:
            return "sio";
    }
}
/**
 * This method initializes the registers to their appropriate default values.
 */
void initializeRegisters(){

    //Initialization of registers.
    SP = 0;
    BP = 1;
    PC = 0;

    //Initialization of Instruction Register (IR).
    IR.op = 0;
    IR.l = 0;
    IR.m = 0;
}

/**
 * This method determines which ISA code is chosen by the user.
 * @param stack This is the stack of the program
 */
int executionCycle(int* stack, int* activationRecordList, int* activationRecordTracker){

    switch((isa_codes)IR.op){
        case LIT:
            SP = SP + 1;
            stack[SP] = IR.m;
            break;
        case OPR:
            return oprOperation(stack, activationRecordList, activationRecordTracker);
        case LOD:
            SP = SP + 1;
            stack[SP] = stack[base(IR.l,stack) + IR.m];
            break;
        case STO:
            stack[base(IR.l, stack)+ IR.m] = stack[SP];
            SP = SP - 1;
            break;
        case CAL:
            activationRecordList[(*activationRecordTracker)++] = SP + 1;
            stack[SP + 1] = 0; //Space to return value.
            stack[SP + 2] = base(IR.l, stack);
            stack[SP + 3] = BP;
            stack[SP + 4] = PC;
            BP =  SP + 1;
            PC = IR.m;
            break;
        case INC:
            SP = SP + IR.m;
            break;
        case JMP:
            PC = IR.m;
            break;
        case JPC:
            if(stack[SP] == 0){
                PC = IR.m;
            }
            SP = SP - 1;
            break;
        case SIO1:
            printf("%d\n", stack[SP]);
            SP = SP - 1;
            break;
        case SIO2:
            SP = SP + 1;
            scanf("%d", &stack[SP]);
            break;
        case SIO3:
            halt = 1;
            return 0;
    }

    return 1;
}

/**
 * This method handles the OPR operations.
 */
int oprOperation(int* stack, int* activationRecordList, int* activationRecordTracker){
    switch((opr_codes)IR.m){
        case RET:
            if(BP == 1){
                //Clear the stack. End of Program.
                SP = 0;
                PC = 0;
                BP = 0;
                return 0;
            }
            activationRecordList[--(*activationRecordTracker)] = 0;
            SP = BP - 1;
            PC = stack[SP + 4];
            BP = stack[SP + 3];
            break;
        case NEG:
            stack[SP] = -stack[SP];
            break;
        case ADD:
            SP--;
            stack[SP] = stack[SP] + stack[SP + 1];
            break;
        case SUB:
            SP--;
            stack[SP] = stack[SP] - stack[SP + 1];
            break;
        case MUL:
            SP--;
            stack[SP] = stack[SP] * stack[SP + 1];
            break;
        case DIV:
            SP--;
            stack[SP] = stack[SP] / stack[SP + 1];
            break;
        case ODD:
            stack[SP] = stack[SP] % 2;
            break;
        case MOD:
            SP--;
            stack[SP] = stack[SP] % stack[SP + 1];
            break;
        case EQL:
            SP--;
            stack[SP] = stack[SP] == stack[SP + 1];
            break;
        case NEQ:
            SP--;
            stack[SP] = stack[SP] != stack[SP + 1];
            break;
        case LSS:
            SP--;
            stack[SP] = stack[SP] < stack[SP + 1];
            break;
        case LEQ:
            SP--;
            stack[SP] = stack[SP] <= stack[SP + 1];
            break;
        case GTR:
            SP--;
            stack[SP] = stack[SP] > stack[SP + 1];
            break;
        case GEQ:
            SP--;
            stack[SP] = stack[SP] >= stack[SP + 1];
            break;
    }

    return 1;
}

int base(int l,int* stack) // l stand for L in the instruction format
{
    int b1; //find base L levels down
    b1 = BP;
    while (l > 0)
    {
        b1 = stack[b1 + 1];
        l--; }
    return b1; }

/**
 * This method initializes the stack as per program guidelines.
 */
void initializeStack(int* stack){
    int i = 0;

    //Rather than just initialized stack[0],stack[1], and stack[2], I opted to initialize the whole stack to 0.
    //As there was sometimes junk left behind that needed to be cleared up in between runs.
    for(i = 0; i < MAX_STACK_HEIGHT; i++){
        stack[i] = 0;
    }
}

/**
 * This method takes in the passed filename when the program is ran and creates an array of instructions if possible.
 * @param argv This is the argument that contains the filename to be read if possible.
 * @param instructions This is the array of instructions that the instructions from the file will be saved into.
 * @return This method returns an int representing the number of instructions scanned from the file.
 *          As a form of error checking, it will also return a -1 if the file isn't read properly.
 */
int loadData(char* argv, Instruction* instructions){
    FILE *file;
    file = fopen(argv, "r");

    int instructionCount = 0;

    if(file == NULL){
        outputTxt("%s has failed to open...\n", argv);
        return -1;
    }

    //While the end of file isn't reached and the instruction count is less than the maximum, continue to scan instructions.
    while(!feof(file) && instructionCount < MAX_CODE_LENGTH){
        fscanf(file, "%d ", &instructions[instructionCount].op);
        fscanf(file, "%d ", &instructions[instructionCount].l);
        fscanf(file, "%d ", &instructions[instructionCount].m);
        instructionCount++;
    }

    fclose(file);

    return instructionCount;
}


