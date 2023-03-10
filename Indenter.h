/* Paolo Bresolin 1187162 */

#include <stdbool.h>
#include <stdio.h>

#define NORMAL 0
#define OPEN_BRACKET 1
#define CLOSE_BRACKET 2
#define NEW_LINE 3
#define APEX 4
#define QUOTES 5
#define POSSIBLE_COMMENT 6
#define COMMENT 7
#define POSSIBLE_END_COMM 8

#define N 20

/*Tipo di dato strutturato che serve a gestire dei caratteri con una politica FIFO.*/
typedef struct {
    char arrChar[N]; /*Array di char*/
    int insertIndex; /*Indice di $arrChar in cui deve essere inserito il prossimo elemento*/
} CharQueue;

/*Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.*/
typedef struct {
    FILE* in;   /*Puntatore al file di input*/
    FILE* out;  /*Puntatore al file di output*/
} InOutFiles;

/*Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.*/
typedef struct {
    int space;  /*Numero di spazi che compongono una singola indentazione*/
    int indent; /*Indentazione corrente*/
} Spacing;


void initArrChar(CharQueue*);
void updateIndex(int*, int);
void decreaseIndex(int*, int);
bool isBracketComment(const CharQueue*);
void printApexQuotes(FILE*, int);
void printIndent(FILE*, int);
void scanNext(FILE*, CharQueue*);
void fOpenBracket(FILE*, Spacing*, int*, const CharQueue*);
void fCloseBracket(FILE*, Spacing*, int*);
void maxOneBlank(FILE*, const CharQueue*);
void fNewLine(FILE*, int, int*, CharQueue*);
void fApexQuotes(FILE*, int, int*, int);
void fSpace(FILE*, int*);
void fNormal(FILE*, int, int*, char);
void fForwSlash(FILE*, int, int*);
void fStar(FILE*, int, int*, const CharQueue*);
bool isUniqWord(const int, const CharQueue*);
bool hasWord(const char [], const CharQueue*);
bool readTillFirstRound(InOutFiles*, Spacing*, int*, CharQueue*);
void readCondition(InOutFiles*, CharQueue*);
bool readAllSpaces(InOutFiles*, Spacing*, int*, CharQueue*);
void reachClosedBrack(InOutFiles*, Spacing*, int*, CharQueue*);
bool fSemicolon(InOutFiles*, const bool, Spacing*, int*, CharQueue*);
void printCharTillEnd(InOutFiles*, const bool, Spacing*, int*, CharQueue*);
void fLoop(InOutFiles*, Spacing*, int*, CharQueue*);
void prefix(char [], const char [], int);
void fPossibleLoop(InOutFiles*, const char [], Spacing*, int*, CharQueue*);
int readTillInstruction(InOutFiles*, Spacing*, int*, CharQueue*);
void readPrintBlock(InOutFiles*, Spacing*, int*, CharQueue*);
void readTillWhile(InOutFiles*, Spacing*, int*, CharQueue*);
void readTillSemic(InOutFiles*, Spacing*, int*, CharQueue*);
void fDo(InOutFiles*, Spacing*, int*, CharQueue*);
bool hasLetter(const char, const CharQueue*);
void fPossibleDo(InOutFiles*, Spacing*, int*, CharQueue*);
void fPossibleIf(InOutFiles*, Spacing*, int*, CharQueue*);
void indentation(InOutFiles*, Spacing*, int*, CharQueue*);
int copyCharWithIndentation(const char [], int, const char []);