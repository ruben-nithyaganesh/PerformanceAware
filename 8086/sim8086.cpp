#include <iostream>
#include <stdio.h>
#include <string.h>

#define IMM_TO_REG_MASK     0b11110000
#define REG_TO_MEM_MASK     0b11111100
#define D_MASK              0b00000010
#define W_MASK              0b00000001

#define MOD_MASK            0b11000000
#define REG_MASK            0b00111000
#define RM_MASK             0b00000111

#define MOD_REG_RM(Byte)    unsigned char Mod = (Byte & MOD_MASK) >> 6; \
                            unsigned char Reg = (Byte & REG_MASK) >> 3; \
                            unsigned char RM = Byte & RM_MASK;


const char *REG_TABLE[2][8] = {
    {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}, 
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

const char *EXPRESSION_TABLE[8] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};


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
    DecodedAsm = fopen("files/decoded.asm", "w");

    if(!Program || !DecodedAsm)
    {
        printf("Error opening file(s)");
        exit(-1);
    }

    fputs("bits 16\n", DecodedAsm);
    for(;;)
    {
        unsigned char Byte1 = fgetc(Program);

        if(feof(Program))
        {
            break;
        }

        char DecodedInstruction[20];

        if((Byte1 & 0b11111100) == 0b10001000) // Register/memory to/from register
        {
            
            unsigned char Byte2 = fgetc(Program);

            unsigned char Mod = (Byte2 & MOD_MASK) >> 6;
            unsigned char D = (Byte1 & D_MASK) >> 1;
            unsigned char W = Byte1 & W_MASK;
            unsigned char Reg = (Byte2 & REG_MASK) >> 3;
            unsigned char RM = Byte2 & RM_MASK;

            switch(Mod)
            {
                case 0b11: //mod is 11, register to register mov
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

                case 0b01: //mod is 01, one optional byte
                {
                    unsigned char Byte3 = fgetc(Program);
                    uint8_t Disp_8 = (uint8_t) Byte3;
                    if(Disp_8 == 0)
                    {
                        if(D == 0)
                        {
                            sprintf(DecodedInstruction, "mov [%s], %s\n", EXPRESSION_TABLE[RM], REG_TABLE[W][Reg]);
                        }
                        else
                        {
                            sprintf(DecodedInstruction, "mov %s, [%s]\n", REG_TABLE[W][Reg], EXPRESSION_TABLE[RM]);
                        }
                    }
                    else
                    {
                        if(D == 0)
                        {
                            sprintf(DecodedInstruction, "mov [%s + %d], %s\n", EXPRESSION_TABLE[RM], Disp_8, REG_TABLE[W][Reg]);
                        }
                        else
                        {
                            sprintf(DecodedInstruction, "mov %s, [%s + %d]\n", REG_TABLE[W][Reg], EXPRESSION_TABLE[RM], Disp_8);
                        }
                    }
                }break;

                case 0b10: //mod is 10, two optinal bytes
                {
                    unsigned char Byte3 = fgetc(Program);
                    unsigned char Byte4 = fgetc(Program);
                    uint16_t Disp_16 = (uint16_t(Byte4) << 8) | Byte3;
                    if(D == 0)
                    {
                        sprintf(DecodedInstruction, "mov [%s + %d], %s\n", EXPRESSION_TABLE[RM], Disp_16, REG_TABLE[W][Reg]);
                    }
                    else
                    {
                        sprintf(DecodedInstruction, "mov %s, [%s + %d]\n", REG_TABLE[W][Reg], EXPRESSION_TABLE[RM], Disp_16);
                    }
                }break;

                case 0b00: //no displacement, no optional bytes
                {
                    if(RM == 0b110)
                    {
                        unsigned char Byte3 = fgetc(Program);
                        unsigned char Byte4 = fgetc(Program);
                        uint16_t Disp_16 = (uint16_t(Byte4) << 8) | Byte3;
                        if(D == 0)
                        {
                            sprintf(DecodedInstruction, "mov [%d], %s\n", Disp_16, REG_TABLE[W][Reg]);
                        }
                        else
                        {
                            sprintf(DecodedInstruction, "mov %s, [%d]\n", REG_TABLE[W][Reg], Disp_16);
                        }
                    }
                    else
                    {
                        if(D == 0)
                        {
                            sprintf(DecodedInstruction, "mov [%s], %s\n", EXPRESSION_TABLE[RM], REG_TABLE[W][Reg]);
                        }
                        else
                        {
                            sprintf(DecodedInstruction, "mov %s, [%s]\n", REG_TABLE[W][Reg], EXPRESSION_TABLE[RM]);
                        }
                    }
                }break;
            }

        }

        else if((Byte1 & 0b11111110) == 0b11000110) // Immediate to register/memory
        {
            unsigned char W = (Byte1 & 0b00000001);
            
            unsigned char Byte2 = fgetc(Program);
            MOD_REG_RM(Byte2)

            switch(Mod){
                case 0b00:{
                    if(W)
                    {
                        unsigned char Byte3 = fgetc(Program);
                        unsigned char Byte4 = fgetc(Program);
                        uint16_t Immediate = (uint16_t(Byte4) << 8) | Byte3;
                        sprintf(DecodedInstruction, "mov %s, word %d\n", EXPRESSION_TABLE[RM], Immediate);
                    }
                    else
                    {
                        unsigned char Byte3 = fgetc(Program);
                        uint8_t Immediate = (uint8_t) Byte3;
                        sprintf(DecodedInstruction, "mov %s, byte %d\n", EXPRESSION_TABLE[RM], Immediate);
                    }
                }break;

                case 0b01:{

                }break;

                case 0b10:{

                }break;

                case 0b11:{

                }break;
            }

             
        }


        else if((Byte1 & IMM_TO_REG_MASK) == 0b10110000) // Immediate to register
        {
            unsigned char Byte2 = fgetc(Program);
            unsigned char W = (Byte1 & 0b00001000) >> 3;
            unsigned char Reg = (Byte1 & 0b00000111);
            if(W)
            {
                unsigned char Byte3 = fgetc(Program);
                uint16_t Immediate = (uint16_t(Byte3) << 8) | Byte2;
                sprintf(DecodedInstruction, "%s %s, %d\n", "mov", REG_TABLE[W][Reg], Immediate);
            }
            else
            {
                uint8_t Immediate = (uint8_t) Byte2;
                sprintf(DecodedInstruction, "%s %s, %d\n", "mov", REG_TABLE[W][Reg], Immediate);
            }
        }
        fputs(DecodedInstruction, DecodedAsm);

    }

    fclose(Program);
    fclose(DecodedAsm);
}
