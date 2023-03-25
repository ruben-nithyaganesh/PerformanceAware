#include <iostream>
#include <stdio.h>
#include <string.h>

#define INSTRUCTION_MASK    0b11111100
#define D_MASK              0b00000010
#define W_MASK              0b00000001

#define MOD_MASK            0b11000000
#define REG_MASK            0b00111000
#define RM_MASK             0b00000111

const char *REG_TABLE[2][8] = {
    {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}, 
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

void DecodeInstruction(unsigned char Byte1, 
                       unsigned char Byte2, 
                       unsigned char Byte3, 
                       unsigned char Byte4, 
                       unsigned char Mod,
                       FILE* DecodedAsm)
{
    unsigned char Instruction = (Byte1 & INSTRUCTION_MASK) >> 2;
    unsigned char D = (Byte1 & D_MASK) >> 1;
    unsigned char W = Byte1 & W_MASK;
    
    //Mod is already computed and passed in
    unsigned char Reg = (Byte2 & REG_MASK) >> 3;
    unsigned char RM = Byte2 & RM_MASK;

    char *DecodedInstruction = (char *) malloc(20);

    const char* Operand1;
    const char* Operand2;

    switch(Mod)
    {
        case 0b00:
        {
            Operand1 = REG_TABLE[W][Reg];
            uint16_t Immediate = (uint16_t(Byte4) << 8) | Byte3;
            
        }break;

        case 0b01:
        {

        }break;

        case 0b10:
        {

        }break;

        case 0b11:
        {
            if(D)
            {
                sprintf(DecodedInstruction, "%s %s, %s\n", "mov", REG_TABLE[W][Reg], REG_TABLE[W][RM]);
            }
            else
            {
                sprintf(DecodedInstruction, "%s %s, %s\n", "mov", REG_TABLE[W][RM], REG_TABLE[W][Reg]);
            }
        }break;
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
        unsigned char Byte3;
        unsigned char Byte4;

        unsigned char Mod = (Byte2 & MOD_MASK) >> 6;
        if(Mod == 0b01)
        {
            unsigned char Byte3 = fgetc(Program);
        }
        else if(Mod == 0b00 || Mod == 0b01) 
        {
            unsigned char Byte3 = fgetc(Program);
            unsigned char Byte4 = fgetc(Program);
        }
        if(feof(Program))
        {
            break;
        }
        DecodeInstruction(Byte1, Byte2, Byte3, Byte4, Mod, DecodedAsm);
    }

    fclose(Program);
    fclose(DecodedAsm);
}