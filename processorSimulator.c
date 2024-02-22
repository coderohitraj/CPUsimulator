#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 40
#define REGISTER_COUNT 8
#define INSTRUCTION_SIZE 16

typedef struct
{
    char memory[MEMORY_SIZE][INSTRUCTION_SIZE + 1];
    char registers[REGISTER_COUNT][INSTRUCTION_SIZE + 1];
    int programCounter;
} Processor;

// function prototypes
void initProcessor(Processor *processor);
void loadProgram(Processor *processor, const char *programFile);
void loadData(Processor *processor, const char *dataFile);
void executeProgram(Processor *processor);
void executeInstruction(Processor *processor, const char *instruction);
void executeALUOperation(Processor *processor, int opcode, int iBit, int operand1, int operand2, int operand3);
void executeDataTransfer(Processor *processor, int opcode, int registerIndex, int memoryLocation);
int string_to_int(const char *string);
void int_to_string(char *line, int number);

int main()
{
    Processor simulator;
    initProcessor(&simulator);
    loadProgram(&simulator, "program.txt");
    loadData(&simulator, "data.txt");
    executeProgram(&simulator);

    return 0;
}

void initProcessor(Processor *processor)
{
    processor->programCounter = 0;

    // initialize registers to zero
    int i, j;
    for (i = 0; i < REGISTER_COUNT; i++)
    {
        for (j = 0; j < INSTRUCTION_SIZE; j++)
        {
            processor->registers[i][j] = '0';
        }
        processor->registers[i][j] = '\0';
    }

    for (i = 0; i < MEMORY_SIZE; i++)
    {
        for (j = 0; j < INSTRUCTION_SIZE + 1; j++)
        {
            processor->memory[i][j] = '\0';
        }
    }
}

void loadProgram(Processor *processor, const char *programFile)
{
    FILE *file = fopen(programFile, "r");
    if (file == NULL)
    {
        perror("Error opening program file");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    char line[20];
    while (fgets(line, 20, file) != NULL && index < MEMORY_SIZE)
    {
        line[16] = '\0';
        strcpy(processor->memory[index], line);
        index++;
    }

    fclose(file);
}

void loadData(Processor *processor, const char *dataFile)
{
    FILE *file = fopen(dataFile, "r");
    if (file == NULL)
    {
        perror("Error opening data file");
        exit(EXIT_FAILURE);
    }

    char line[20];
    while (fgets(line, 20, file) != NULL)
    {
        // int address, value;
        line[13] = '\0';
        line[4] = '\0';
        strcpy(processor->memory[string_to_int(line)], line + 5);
    }

    fclose(file);
}

int string_to_int(const char *string)
{
    int number = 0;
    char c;
    int i = 0;
    while ((c = string[i]) != '\0')
    {
        number = number * 2 + (c - '0');
        i++;
    }

    return number;
}

void executeProgram(Processor *processor)
{
    while (processor->programCounter < MEMORY_SIZE && processor->memory[processor->programCounter][0] != '\0')
    {
        char *instruction = processor->memory[processor->programCounter];
        executeInstruction(processor, instruction);
        processor->programCounter++;
    }

    // register values at last
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        printf("R%d: %d\n", i, string_to_int(processor->registers[i]));
    }
}

void executeInstruction(Processor *processor, const char *instruction)
{

    int ins = string_to_int(instruction);

    int operand3 = ins & 0b111;
    int operand2 = (ins >> 3) & 0b1111;
    int operand1 = (ins >> 7) & 0b111;
    int iBit = (ins >> 10) & 0b1;
    int opcode = (ins >> 11) & 0b1111;
    int tBit = (ins >> 15) & 0b1;

    if (tBit == 0)
    {
        // alu
        executeALUOperation(processor, opcode, iBit, operand1, operand2, operand3);
    }
    else
    {
        // data transfer instruction
        executeDataTransfer(processor, opcode, operand1, operand2);
    }
}

void executeALUOperation(Processor *processor, int opcode, int iBit, int operand1, int operand2, int operand3)
{
    int ArthOp1, ArthOp2 = string_to_int(processor->registers[operand3]);
    if (iBit == 1)
    {
        ArthOp1 = operand2;
    }
    else
    {
        ArthOp1 = string_to_int(processor->registers[operand2]);
    }
    switch (opcode)
    {
    case 0:
        int_to_string(processor->registers[operand1], ArthOp1 + ArthOp2);
        break;
    case 1:
        int_to_string(processor->registers[operand1], ArthOp1 - ArthOp2);
        break;
    case 2:
        int_to_string(processor->registers[operand1], ArthOp1 & ArthOp2);
        break;
    case 3:
        int_to_string(processor->registers[operand1], ArthOp1 | ArthOp2);
        break;
    case 4:
        int_to_string(processor->registers[operand1], ArthOp1 ^ ArthOp2);
        break;
    }
}

void executeDataTransfer(Processor *processor, int opcode, int registerIndex, int memoryLocation)
{
    if (opcode == 5)
    {
        strcpy(processor->registers[registerIndex], processor->memory[memoryLocation]);
    }
    else if (opcode == 6)
    {
        strcpy(processor->memory[memoryLocation], processor->registers[registerIndex]);
    }
}

void int_to_string(char *line, int number)
{
    line[16] = '\0';
    int i = 15;
    while (number != 0)
    {
        line[i] = '0' + number % 2;
        number /= 2;
        i--;
    }

    while (i > -1)
    {
        line[i--] = '0';
    }
}