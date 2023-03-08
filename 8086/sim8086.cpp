#include <iostream>
#include <stdio.h>
#include <string.h>

#define INSTRUCTION_MASK 252
#define D_MASK 2
#define W_MASK 1

#define MOD_MASK 192
#define REG_MASK 56
#define RM_MASK 7

const char *REG_TABLE[2][8] = {
    {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}, 
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

void DecodeInstruction(unsigned char Byte1, unsigned char Byte2, FILE* DecodedAsm)
{
    unsigned char Instruction = (Byte1 & INSTRUCTION_MASK) >> 2;
    unsigned char D = (Byte1 & D_MASK) >> 1;
    unsigned char W = Byte1 & W_MASK;
    
    unsigned char Mod = (Byte2 & MOD_MASK) >> 6;
    unsigned char Reg = (Byte2 & REG_MASK) >> 3;
    unsigned char RM = Byte2 & RM_MASK;

    char *DecodedInstruction = (char *) malloc(20);

    if(D)
    {
        sprintf(DecodedInstruction, "%s %s, %s\n", "mov", REG_TABLE[W][Reg], REG_TABLE[W][RM]);
    }
    else
    {
        sprintf(DecodedInstruction, "%s %s, %s\n", "mov", REG_TABLE[W][RM], REG_TABLE[W][Reg]);
    }

    printf("%s", DecodedInstruction);
    fputs(DecodedInstruction, DecodedAsm);
    free(DecodedInstruction);
}

int main(int argc, char *argv[]) 
{
    if(argc < 2)
    {
        printf("\nNeed argument for filename\n");
        exit(-1);
    }

    FILE *Program;
    Program = fopen(argv[1], "rb");

    FILE *DecodedAsm;
    DecodedAsm = fopen("decoded.asm", "w");

    if(!Program || !DecodedAsm)
    {
        printf("Error opening file(s)");
        exit(-1);
    }

    fputs("bits 16\n", DecodedAsm);
    for(;;)
    {
        unsigned char Byte1 = fgetc(Program);
        unsigned char Byte2 = fgetc(Program);
        if(feof(Program))
        {
            break;
        }
        DecodeInstruction(Byte1, Byte2, DecodedAsm);
    }

    fclose(Program);
    fclose(DecodedAsm);
}