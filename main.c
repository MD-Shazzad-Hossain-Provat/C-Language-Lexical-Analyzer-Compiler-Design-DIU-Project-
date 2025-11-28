#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 100

// Structure for symbol table
struct Symbol
{
    char name[50];
    char type[20];
    char value[50];
} symbolTable[MAX];

int symbolCount = 0;

// Check if keyword
bool isKeyword(char *word)
{
    char keywords[][10] = {"int", "float", "char", "return", "void"};
    for (int i = 0; i < 5; i++)
    {
        if (strcmp(word, keywords[i]) == 0) return true;
    }
    return false;
}

// Validate variable name (regex simulation ^a+$|^a+b*$)
bool isValidVariable(char *word)
{
    int i = 0;
    if (word[0] != 'a') return false;
    while (word[i] == 'a') i++;
    while (word[i] == 'b') i++;
    return (word[i] == '\0');
}

// Check if already in symbol table
bool isInSymbolTable(char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0) return true;
    }
    return false;
}

// Add variable or function to symbol table
void addToSymbolTable(char *name, char *type, char *value)
{
    if (!isInSymbolTable(name))
    {
        strcpy(symbolTable[symbolCount].name, name);
        strcpy(symbolTable[symbolCount].type, type);
        strcpy(symbolTable[symbolCount].value, value);
        symbolCount++;
    }
}

int main()
{
    FILE *fp = fopen("input.c", "r"); // Input C file
    if (!fp)
    {
        printf("Error opening file\n");
        return 1;
    }

    char line[200];
    char currentType[20] = "";

    printf("LEXICAL ANALYSIS OUTPUT:\n\n");

    while (fgets(line, sizeof(line), fp))
    {
        int len = strlen(line);
        for (int i = 0; i < len; i++)
        {
            if (isspace(line[i])) continue;

            // Preprocessor directive
            if (line[i] == '#')
            {
                printf("Preprocessor Directive: %s", line);
                break;
            }

            // Keywords and identifiers
            if (isalpha(line[i]))
            {
                char token[50];
                int j = 0;
                while (isalnum(line[i]) || line[i]=='_')
                {
                    token[j++] = line[i++];
                }
                token[j] = '\0';
                i--;

                if (isKeyword(token))
                {
                    printf("Keyword: %s\n", token);
                    strcpy(currentType, token); // save for next variable
                }
                else if (strcmp(token, "main") == 0)
                {
                    printf("Function: %s\n", token);
                    addToSymbolTable(token, "function (int)", "--");
                }
                else if (strcmp(token, "printf") == 0)
                {
                    printf("Function: %s\n", token);
                }
                else
                {
                    // Identifier (variable)
                    char value[50] = "--";
                    char *eq = strchr(line, '=');
                    if (eq != NULL)
                    {
                        eq++; // move past '='
                        while (*eq == ' ' || *eq == '\t') eq++;
                        int k = 0;
                        if (currentType[0]=='c')   // char literal
                        {
                            if (*eq=='\'')
                            {
                                value[k++] = *eq++;
                                value[k++] = *eq++;
                                value[k++] = *eq++;
                                value[k] = '\0';
                            }
                        }
                        else     // int or float
                        {
                            while (*eq && *eq!=';' && *eq!='\n') value[k++] = *eq++;
                            value[k] = '\0';
                        }
                    }

                    // Print recognition or error
                    if (isValidVariable(token))
                    {
                        printf("Identifier: %s (Recognized)\n", token);
                    }
                    else
                    {
                        printf("Error: Variable '%s' is not recognized. It doesn't match the allowed pattern.\n", token);
                    }

                    // Add to symbol table regardless of validity
                    addToSymbolTable(token, currentType, value);
                }
            }

            // Numbers
            else if (isdigit(line[i]))
            {
                char num[20];
                int j = 0;
                while (isdigit(line[i]) || line[i]=='.')
                {
                    num[j++] = line[i++];
                }
                num[j] = '\0';
                i--;
                printf("Number: %s\n", num);
            }

            // Character literals
            else if (line[i] == '\'')
            {
                char ch[5];
                int j = 0;
                ch[j++] = line[i++]; // opening quote
                ch[j++] = line[i++]; // character
                ch[j++] = line[i++]; // closing quote
                ch[j] = '\0';
                printf("Character Literal: %s\n", ch);
            }

            // String literals
            else if (line[i] == '"')
            {
                char str[100];
                int j = 0;
                str[j++] = line[i++];
                while (line[i]!='"' && line[i]!='\0') str[j++] = line[i++];
                str[j++] = '"';
                str[j] = '\0';
                printf("String Literal: %s\n", str);
            }

            // Operators
            else if (strchr("=;(){}", line[i]))
            {
                printf("Operator: %c\n", line[i]);
            }
        }
    }

    fclose(fp);

    // Print Symbol Table
    printf("\nSYMBOL TABLE:\n");
    printf("%-10s %-15s %-20s\n", "Name", "Type", "Value/Initial Value");
    printf("------------------------------------------------\n");
    for (int i = 0; i < symbolCount; i++)
    {
        printf("%-10s %-15s %-20s\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].value);
    }

    return 0;
}

