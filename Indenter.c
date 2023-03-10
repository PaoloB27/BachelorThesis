/* Paolo Bresolin 1187162 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Indenter.h"


/*
Inizializza tutti gli elementi di $arrChar in $queue con il valore NULL.
IOP queue Variabile strutturata di tipo CharQueue.
*/
void initArrChar(CharQueue* queue) {
    int i;
    for (i = 0; i < N; i++)
        queue->arrChar[i] = 0;
} /* initArrChar */

/*
Incrementa $index in modo tale che sia indice di una coda circolare di capacita` $capacity.
IP index Variabile di tipo intero da incrementare.
IP capacity Variabile di tipo intero che rappresenta il (massimo valore raggiungibile da $index) - 1.
OP index Intero incrementato come segue:
   ~ se ($index + 1) < $capacity, allora $index = ($index + 1);
   ~ se ($index + 1) == $capacity, allora $index = 0.
*/
void updateIndex(int* index, int capacity) {
    *index = (*index + 1) % capacity;
} /*updateIndex*/

/*
Decrementa $index in modo tale che sia indice di una coda circolare di capacita` $capacity.
IP index Variabile di tipo intero da decrementare.
IP capacity Variabile di tipo intero che rappresenta il (massimo valore raggiungibile da $index) - 1.
OP index Intero decrementato come segue:
   ~ se ($index - 1) < 0, allora $index = ($capacity - 1);
   ~ se ($index - 1) >= 0, allora $index = ($index - 1).
*/
void decreaseIndex(int* index, int capacity) {
    *index = (*index - 1 + capacity) % capacity;
} /*decreaseIndex*/

/*
Indica se il prossimo commento segue un carattere '}', eventualmente seguito da caratteri ' ' o '\n'.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OR isBracketComm Indicazione se il prossimo commento segua il carattere '}'.
*/
bool isBracketComment(const CharQueue* queue) {
    int i = queue->insertIndex;
    bool isBracketComm = false;
    decreaseIndex(&i, N);
    while (true) {
        decreaseIndex(&i, N);
        if ((queue->arrChar[i] == '}') || (queue->arrChar[i] == 0)) {
            isBracketComm = true;
            break;
        } /*if*/
        else if ((queue->arrChar[i] != '}') && (queue->arrChar[i] != '\n') && (queue->arrChar[i] != ' '))
            break;
    } /*while*/
    return isBracketComm;
} /*isBracketComment*/

/*
Stampa su file il carattere '\'' oppure '\"' in base al valore intero inserito in $label.
IOP out Puntatore al file sul quale scrivere.
IP label Intero che puo` essere soltanto $APEX o $LABEL.
OF Carattere '\'' oppure '\"' in base al valore intero inserito in $label.
*/
void printApexQuotes(FILE* out, int label) {
    if (label == APEX)
        fprintf(out, "\'");
    else
        fprintf(out, "\"");
} /*printApexQuotes*/

/*
Stampa su file il carattere '\n' seguito da $nIndent spazi.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi da stampare sul file dopo il carattere '\n'.
OF Carattere '\n' seguito da $nIndent spazi.
*/
void printIndent(FILE* out, int nIndent) {
    int i;
    fprintf(out, "\n");
    for (i = 0; i < nIndent; i++)
        fprintf(out, " ");
} /*printIndent*/

/*
Incrementa l'indice della coda di caratteri in modo da poter accogliere il nuovo carattere da leggere e lo legge.
IP in Puntatore al file da quale leggere.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
*/
void scanNext(FILE* in, CharQueue* queue) {
    updateIndex(&queue->insertIndex, N);
    fscanf(in, "%c", &queue->arrChar[queue->insertIndex]);
} /*scanNext*/

/*
Stampa su file il carattere '{' in modo tale che sia sempre preceduta da uno spazio,
secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IOP sp Variabile strutturata di tipo Spacing.
IOP status Intero che fornisce informazioni sui caratteri letti.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Carattere '{' secondo le regole di indentazione.
*/
void fOpenBracket(FILE* out, Spacing* sp, int* status, const CharQueue* queue) {
    int index = queue->insertIndex;
    bool newL = (*status == NEW_LINE);
    if ((*status != APEX) && (*status != QUOTES) && (*status != COMMENT) && (*status != POSSIBLE_END_COMM)) {
        sp->indent += sp->space;
        *status = OPEN_BRACKET;
    } /*if*/
    else if (*status == POSSIBLE_END_COMM)
        *status = COMMENT;
    while (true) {
        decreaseIndex(&index, N);
        if (queue->arrChar[index] != '\n') {
            if ((queue->arrChar[index] != ' ') || (newL))
                fprintf(out, "%c", ' ');
            break; /*Uscita perche` viene letto un carattere diverso da '\n'*/
        } /*if*/
    } /*Questo while serve per fare in modo che il carattere '{' sia sempre preceduto dal carattere ' '*/
    fprintf(out, "%c", '{');
} /*fOpenBracket*/

/*
Stampa su file il carattere '}' secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IOP sp Variabile strutturata di tipo Spacing.
IOP status Intero che fornisce informazioni sui caratteri letti.
OF Carattere '}' secondo le regole di indentazione.
*/
void fCloseBracket(FILE* out, Spacing* sp, int* status) {
    if ((*status != APEX) && (*status != QUOTES) && (*status != COMMENT) && (*status != POSSIBLE_END_COMM)) {
        sp->indent -= sp->space;
        printIndent(out, sp->indent);
        *status = CLOSE_BRACKET;
    } /*if*/
    else if (*status == POSSIBLE_END_COMM)
        *status = COMMENT;
    fprintf(out, "%s", "} ");
} /*fCloseBracket*/

/*
Stampa il carattere '\n' in modo tale che sul file di output possa esserci un massimo di una sola linea vuota
consecutiva.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Il carattere '\n' soltanto se:
  ~ non sono presenti linee vuote immendiatamente precedenti;
  ~ non sono stati letti caratteri che produrranno '\n' sul file di output.
*/
void maxOneBlank(FILE* out, const CharQueue* queue) {
    int index = queue->insertIndex;
    decreaseIndex(&index, N);
    if (queue->arrChar[index] == '\n') {
        decreaseIndex(&index, N);
        if ((queue->arrChar[index] != '\n') && (queue->arrChar[index] != 0)) 
            fprintf(out, "%c", '\n');
    } /*if*/
} /*maxOneBlank*/

/*
Stampa su file il carattere '\n' secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi totali di indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
OF Carattere '\n' secondo le regole di indentazione.
*/
void fNewLine(FILE* out, int nIndent, int* status, CharQueue* queue) {
    if (*status == POSSIBLE_COMMENT) 
        fprintf(out, "%c", '/');
    if ((*status == APEX) || (*status == QUOTES) || (*status == COMMENT))
        printIndent(out, nIndent);
    else if (*status == POSSIBLE_END_COMM) {
        *status = COMMENT;
        printIndent(out, nIndent);
    } /*else if*/
    else {
        if (nIndent == 0) /*Siamo all'esterno di una funzione.*/
            maxOneBlank(out, queue);
        *status = NEW_LINE;
    } /*else*/
} /*fNewLine*/

/*
Stampa su file il carattere '\'' oppure il carattere '\"' in base a $label, secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi totali di indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IP label Numero intero che puo` essere solo $APEX o $QUOTES e permette di sapere se l'ultimo carattere
   letto e` '\'' oppure '\"'.
OF Carattere '\'' o '\"' in base a $label, secondo le regole di indentazione.
*/
void fApexQuotes(FILE* out, int nIndent, int* status, int label) {
    if (*status == POSSIBLE_END_COMM)
        *status = COMMENT;
    else if (*status == POSSIBLE_COMMENT)
        fprintf(out, "%c", '/');
    else if (*status == label)
        *status = NORMAL;
    else if (*status == NEW_LINE) {
        printIndent(out, nIndent);
        *status = label;
    } /*else if*/
    else if ((*status == NORMAL) || (*status == POSSIBLE_COMMENT))
        *status = label;
    printApexQuotes(out, label);
} /*fApexQuotes*/

/*
Stampa su file il carattere ' ' secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IOP status Intero che fornisce informazioni sui caratteri letti.
OF Carattere ' ' secondo le regole di indentazione.
*/
void fSpace(FILE* out, int* status) {
    if (*status == POSSIBLE_COMMENT) {
        *status = NORMAL;
        fprintf(out, "/");
    } /*if*/
    else if (*status == POSSIBLE_END_COMM)
        *status = COMMENT;
    if ((*status != NEW_LINE) && (*status != OPEN_BRACKET) && (*status != CLOSE_BRACKET))
        fprintf(out, " ");
} /*fSpace*/

/*
Stampa su file il carattere $c secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi totali di indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IP + OF c Carattere da stampare secondo le regole di indentazione.
*/
void fNormal(FILE* out, int nIndent, int* status, char c) {
    if (*status == POSSIBLE_COMMENT) {
        fprintf(out, "%c", '/');
        *status = NORMAL;
    } /*if*/
    else if (*status == POSSIBLE_END_COMM)
        *status = COMMENT;
    else if ((*status == OPEN_BRACKET) || (*status == CLOSE_BRACKET) || (*status == NEW_LINE)) {
        printIndent(out, nIndent);
        *status = NORMAL;
    } /*if*/
    fprintf(out, "%c", c);
} /*fNormal*/

/*
Stampa su file il carattere '/' secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi totali di indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IP + OF '/' Carattere da stampare secondo le regole di indentazione.
*/
void fForwSlash(FILE* out, int nIndent, int* status) {
    if ((*status == QUOTES) || (*status == APEX))
        fprintf(out, "%c", '/');
    else if (*status == POSSIBLE_COMMENT)
        fNormal(out, nIndent, status, '/');
    else if (*status == POSSIBLE_END_COMM) {
        fprintf(out, "%c", '/');
        *status = NEW_LINE;
    } /*else if*/
    else
        *status = POSSIBLE_COMMENT;
} /*fForwSlash*/

/*
Stampa su file il carattere '*' secondo le regole di indentazione.
IOP out Puntatore al file sul quale scrivere.
IP nIndent Numero di spazi totali di indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
IP + OF c Carattere da stampare secondo le regole di indentazione.
*/
void fStar(FILE* out, int nIndent, int* status, const CharQueue* queue) {
    if (*status == POSSIBLE_COMMENT) {
        if (!isBracketComment(queue))
            printIndent(out, nIndent);
        fprintf(out, "%c%c", '/', '*');
        *status = COMMENT;
    } /*if*/
    else if (*status == COMMENT) {
        *status = POSSIBLE_END_COMM;
        fprintf(out, "%c", '*');
    } /*else if*/
    else
        fNormal(out, nIndent, status, '*');
} /*fStar*/

/*
Verifica se l'ultima parola letta, di lunghezza $wLen, e` a se` stante o e` parte di una parola precedente.
IP wLen Lunghezza dell'ultima parola letta.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OR Indicazione se l'ultima parola letta e` a se` stante.
*/
bool isUniqWord(const int wLen, const CharQueue* queue) {
    int index = queue->insertIndex;
    int i;
    for (i = 0; i < wLen; i++)
        decreaseIndex(&index, N);
    return ((queue->arrChar[index] == ' ') || (queue->arrChar[index] == '{') || (queue->arrChar[index] == '}') ||
        (queue->arrChar[index] == '\n') || (queue->arrChar[index] == ';') || (queue->arrChar[index] == '/') ||
        (queue->arrChar[index] == ')') || (queue->arrChar[index] == 0));
} /*isUniqWord*/

/*
Verifica se la stringa $word non e` suffisso di una stringa piu` lunga e, in caso non lo sia, verifica se
e` stata effettivamente letta immediatamente precentemente all'ultimo carattere letto.
IP word Stringa da cercare immediatamente prima dell'ultimo carattere letto.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OR Indicazione sulla corrispondenza tra $word e la stringa in $queue->arrChar immediatamente precedente all'
   ultimo carattere letto.
   Ritorna $false anche in caso $word sia suffisso di una stringa piu` lunga.
*/
bool hasWord(const char word[], const CharQueue* queue) {
    int i;
    int ind = queue->insertIndex;
    int len = strlen(word);
    if (!isUniqWord(len + 1, queue))
        return false; /*Se e` stata letta $word, allora e` suffisso di una stringa piu` lunga.*/
    for (i = 0; i < len; i++) {
        decreaseIndex(&ind, N);
        if (word[len - i - 1] != queue->arrChar[ind])
            return false; /*Non e` stata letta $word.*/
    } /*for*/
    return true; /*E` stata letta $word e non e` suffisso di una stringa piu` lunga.*/
} /*hasFor*/

/*
Legge i caratteri tra una keyword e la prima parentesi tonda che identifica l'inizio delle condizioni relative
alla keyword in questione e li stampa secondo le regole prestabilite nel file di output.
Verifica anche che la keyword non sia prefisso di un'altra stringa.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri che vanno dalla keyword alla prima tonda compresa.
OR Indicazione se la keyword e` a se` stante o se e` prefisso di un'altra stringa.
*/
bool readTillFirstRound(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    int nRoundBrack = 0;
    while (nRoundBrack == 0) {
        scanNext(files->in, queue);
        if ((queue->arrChar[queue->insertIndex] != '\n') && (queue->arrChar[queue->insertIndex] != ' ') &&
            (queue->arrChar[queue->insertIndex] != '(')) {
            indentation(files, sp, status, queue);
            break; /*Uscita perche` la keyword e` prefisso di una stringa piu` lunga.*/
        } /*if*/
        if (queue->arrChar[queue->insertIndex] != '\n') {
            fprintf(files->out, "%c", queue->arrChar[queue->insertIndex]);
            if (queue->arrChar[queue->insertIndex] == '(')
                nRoundBrack++;
        } /*if*/
    } /*while*/
    return nRoundBrack; /*Sara` 0 (cioe` false) se e solo se non si tratta di un vero for.*/
} /*readTillFirstRound*/

/*
Legge le istruzioni relative ad una keyword racchiuse tra parentesi tonde e le stampa sulla stessa linea.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri che seguono una tonda fino alla parentesi tonda di chiusura della condizione compresa.
*/
void readCondition(InOutFiles* files, CharQueue* queue) {
    int nRoundBrack = 1;
    while (nRoundBrack > 0) {
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] == '(') {
            fprintf(files->out, "%c", '(');
            nRoundBrack++;
        } /*if*/
        else if (queue->arrChar[queue->insertIndex] == ')') {
            fprintf(files->out, "%c", ')');
            nRoundBrack--;
        } /*else if*/
        else if (queue->arrChar[queue->insertIndex] != '\n')
            fprintf(files->out, "%c", queue->arrChar[queue->insertIndex]);
    } /*while*/
} /*readCondition*/

/*
Legge tutti gli spazi, senza stamparli sul file di output, fino al primo carattere diverso da una spazio.
Fornisce indicazioni su se il prossimo carattere diverso da uno spazio sia una graffa aperta.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Il primo carattere letto dal file di input che non sia uno spazio.
OR Indicazione se il primo carattere letto diverso da uno spazio e` una graffa aperta.
*/
bool readAllSpaces(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    while (true) {
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] != ' ') {
            if (queue->arrChar[queue->insertIndex] == '{') {
                sp->indent += sp->space;
                *status = OPEN_BRACKET;
                fprintf(files->out, "%s", " {");
                return true; /*Il primo carattere diverso da uno spazio e` '{'.*/
            } /*if*/
            else if (queue->arrChar[queue->insertIndex] != '\n') {
                sp->indent += sp->space;
                *status = NEW_LINE;
                indentation(files, sp, status, queue);
                break; /*Il primo carattere diverso da uno spazio non e` '{'.*/
            } /*else*/
        } /*if*/
    } /*while*/
    return false; /*Il primo carattere diverso da uno spazio non e` '{'.*/
} /*readAllSpaces*/

/*
Legge dal file di input e stampa sul file di output tutti i caratteri fino alla graffa chiusa compresa,
seguendo le regole di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Tutti i caratteri fino alla graffa chiusa compresa, seguendo le regole di indentazione.
*/
void reachClosedBrack(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    while (true) {
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] == '}') {
            fCloseBracket(files->out, sp, status);
            break; /*Uscita perche` e` stata trovata una graffa chiusa.*/
        } /*if*/
        else
            indentation(files, sp, status, queue);
    } /*while*/
} /*reachCloseBrack*/

/*
Stampa il carattere ';' sul file di output seguendo le regole di indentazione. Fornisce l' indicazione se il
carattere ';' chiude il blocco corrente oppure no.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IP brackfor Variabile booleana che indica se il carattere ';' e` stato letto all'interno di un blocco di graffe
   oppure no.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Il carattere ';' seguendo le regole di indentazione.
OR Indicazione se il carattere ';' chiude il blocco corrente oppure no.
*/
bool fSemicolon(InOutFiles* files, const bool brackFor, Spacing* sp, int* status, CharQueue* queue) {
    if ((*status == QUOTES) || (*status == APEX) || (*status == COMMENT) || (*status == POSSIBLE_END_COMM))
        fNormal(files->out, sp->indent, status, ';');
    else if (brackFor) {
        fNormal(files->out, sp->indent, status, ';');
        reachClosedBrack(files, sp, status, queue);
    } /*else if*/
    else {
        sp->indent -= sp->space;
        fNormal(files->out, sp->indent, status, ';');
        return true; /*E` stato letto il ';' che effettivamente chiude il blocco.*/
    } /*else*/
    return false; /*Non e` stato letto il ';' che effettivamente chiude il blocco.*/
} /*fSemicolon*/

/*
Stampa tutti i caratteri letti fino al carattere ';' che effettivamente chiude il blocco in questione compreso.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IP brackfor Variabile booleana che indica se il carattere ';' e` stato letto all'interno di un blocco di graffe
   oppure no.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri letti dal file di input fino al carattere ';' compreso, seguendo le regole di indentazione.
*/
void printCharTillEnd(InOutFiles* files, const bool brackFor, Spacing* sp, int* status, CharQueue* queue) {
    while (*status != CLOSE_BRACKET) {
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] == ';') {
            if (fSemicolon(files, brackFor, sp, status, queue))
                break; /*Uscita perche` e` stato letto il carattere ';' che effettivamente chiude il blocco.*/
        } /*if*/
        else {
            indentation(files, sp, status, queue);
            if ((queue->arrChar[queue->insertIndex] == ';') && !(brackFor)) {
                sp->indent -= sp->space;
                break; /*Uscita perche` e` stato letto il carattere ';' che effettivamente chiude il blocco.*/
            } /*Questo if serve a gestire eventuali cicli annidati.*/
        } /*else*/
    } /*while*/
} /*printCharTillEnd*/

/*
Legge dal file di input e stampa sul file di output il blocco corrispondente ad una keyword,
seguendo le regole di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Il blocco corrispondente ad una keyword, seguendo le regole di indentazione.
*/
void fLoop(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    if (readTillFirstRound(files, sp, status, queue)) {
        bool isBrack;
        readCondition(files, queue);
        isBrack = readAllSpaces(files, sp, status, queue);
        printCharTillEnd(files, isBrack, sp, status, queue);
    } /*if*/
} /*fLoop*/

/*
Salva in $prefix il prefisso di lunghezza $prefLen di $string.
IOP prefix Stringa contenente il prefisso di $string lungo $prefLen.
IP string Stringa da cui estrarre il prefisso.
IP prefLen Lunghezza del prefisso di $string da estrarre.
*/
void prefix(char prefix[], const char string[], int prefLen) {
    int i;
    for (i = 0; i < prefLen; i++)
        prefix[i] = string[i];
    prefix[prefLen] = 0;
} /*prefix*/

/*
Verifica se siamo in presenza della keyword $word e, in caso, la stampa con il blocco ad essa corrispondente,
seguendo le regole di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Due casi:
   ~ se siamo effettivamente in presenza della keyword $word: la keyword $word ed i caratteri che compongono il
     blocco corrispondente;
   ~ altrimenti: ultimo carattere di $word secondo le regole di indentazione.
*/
void fPossibleLoop(InOutFiles* files, const char word[], Spacing* sp, int* status, CharQueue* queue) {
    bool normal = true;
    int wordLen = strlen(word);
    int prefLen = wordLen - 1;
    char* prefixWord = malloc(sizeof(char) * wordLen);
    assert(prefixWord != NULL); 
    prefix(prefixWord, word, prefLen);
    if ((*status != COMMENT) && (*status != APEX) && (*status != QUOTES) && (*status != POSSIBLE_END_COMM)) {
        if (hasWord(prefixWord, queue)) {
            normal = false;
            *status = NORMAL;
            fprintf(files->out, "%c", word[prefLen]);
            fLoop(files, sp, status, queue);
        } /*if*/
    } /*if*/
    free(prefixWord);
    if (normal)
        fNormal(files->out, sp->indent, status, word[prefLen]);
} /*fPossibleLoop*/

/*
Legge dal file di input i caratteri fino al primo carattere delle istruzioni relative alla keyword "do" e
li stampa sul file di output seguendo le regole di indentazione.
Fornisce indicazioni se il "do" sia prefisso di una stringa piu` lunga e, altrimenti, se sia seguito da un
blocco di istruzioni racchiuso tra graffe oppure da un'unica istruzione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OR Un intero che puo` assumere solo i valori -1, 0, 1 con il seguente significato:
   ~ -1: "do" e` prefisso di una stringa piu` lunga;
   ~  0: "do" e` seguito da un'unica istruzione e non da un blocco racchiuso tra parentesi graffe.
   ~  1: "do" e` seguito da un blocco di istruzioni racchiuso tra parentesi graffe;
OF Due casi:
   ~ se siamo effettivamente in presenza della keyword "do": i caratteri che la seguono, fino al primo carattere
     delle istruzioni relative ad essa;
   ~ altrimenti: il carattere successivo alla keyword "do".
*/
int readTillInstruction(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    int infoVal = -1;
    while (true) {
        scanNext(files->in, queue);
        if ((queue->arrChar[queue->insertIndex] == ' ') || (queue->arrChar[queue->insertIndex] == '\n')) {
            if(infoVal < 0)
                infoVal = 0;
        } /*if*/
        else if (queue->arrChar[queue->insertIndex] == '{') {
            sp->indent += sp->space;
            *status = OPEN_BRACKET;
            fprintf(files->out, "%s", " {");
            return 1; /*Il "do" e` seguito da un blocco di istruzioni racchiuso tra graffe.*/
        } /*else if*/   
        else {
            if (infoVal == 0) {
                sp->indent += sp->space;
                *status = NEW_LINE;
            } /*if*/
            indentation(files, sp, status, queue);
            return infoVal; /*Il "do" coinvolge un'unica istruzione se infoVal == 0, e` prefisso altrimenti.*/
        } /*else*/
    } /*while*/
} /*readTillInstruction*/

/*
Stampa tutti i caratteri di un blocco compresi tra la graffa di apertura e quella di chiusura compresa.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri compresi tra due graffe che compongono un blocco.
*/
void readPrintBlock(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    int nBrack = 1;
    while (nBrack) { /*Si esce dal ciclo quando $nBrack == 0.*/
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] == '{') {
            nBrack++;
            fOpenBracket(files->out, sp, status, queue);
        } /*if*/
        else if (queue->arrChar[queue->insertIndex] == '}') {
            nBrack--;
            fCloseBracket(files->out, sp, status);
            if (!nBrack)
                *status = NORMAL;
        } /*else if*/
        else
            indentation(files, sp, status, queue);
    } /*while*/
} /*readPrintBlock*/

/*
Vengono letti dal file di input e stampati sul file di output tutti i caratteri fino a quando si individua la
keyword "while".
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri che precedono la keyword "while" e la keyword stessa.
*/
void readTillWhile(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    while (true) {
        scanNext(files->in, queue);
        if ((*status != COMMENT) && (*status != POSSIBLE_END_COMM) &&
            (queue->arrChar[queue->insertIndex] == 'e')) {
            if (hasWord("whil", queue)) {
                fprintf(files->out, "%c", 'e');
                break; /*Uscita perche` e` stata trovata la keyword "while".*/
            } /*if*/
            else
                fNormal(files->out, sp->indent, status, 'e');
        } /*if*/
        else if ((queue->arrChar[queue->insertIndex] != '\n') && (queue->arrChar[queue->insertIndex] != ' '))
            indentation(files, sp, status, queue);
    } /*while*/
} /*readTillWhile*/

/*
Legge dal file di input e stampa sul file di output i caratteri fino al ';' compreso, seguendo le regole di
indentazione. Viene letto anche un ulteriore carattere successivo, in modo da evitare interferenze nel caso
di cicli innestati.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF I caratteri letti che precedono il carattere ';', il carattere ';' ed il carattere subito successivo.
   Si noti che la lettura non puo` mai fallire, poiche` il file di input si conclude sempre con il carattere
   '\n'.
*/
void readTillSemic(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    while (true) {
        scanNext(files->in, queue);
        if (queue->arrChar[queue->insertIndex] == ';') {
            fNormal(files->out, sp->indent, status, ';');
            *status = NEW_LINE;
            scanNext(files->in, queue);
            indentation(files, sp, status, queue);
            break; /*Uscita perche` trovato ';'.*/
        } /*if*/
        else if (queue->arrChar[queue->insertIndex] != '\n')
            indentation(files, sp, status, queue);
    } /*while*/
} /*readTillSemic*/

/*
Legge dal file di input e stampa sul file di output il blocco corrispondente alla keyword "do" se quest'ultima
viene individuata, altrimenti stampa il carattere successivo a "do" se prefisso di una stringa piu` lunga.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Due casi:
   ~ se la stringa "do" e` a se stante: blocco corrispondente alla keyword "do";
   ~ se la stringa "do" e` prefisso di una stringa piu` lunga: il carattere immediatamente successivo a "do".
*/
void fDo(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    int outcome = readTillInstruction(files, sp, status, queue);
    if (outcome >= 0) {
        if (outcome == 1)
            readPrintBlock(files, sp, status, queue);
        else {
            printCharTillEnd(files, false, sp, status, queue);
            printIndent(files->out, sp->indent);
        } /*else*/
        readTillWhile(files, sp, status, queue);
        readTillFirstRound(files, sp, status, queue);
        readCondition(files, queue);
        readTillSemic(files, sp, status, queue);
    } /*if*/
} /*fDo*/

/*
Verifica se il carattere $letter e` stato letto immediatamente precentemente all'ultimo carattere letto.
IP letter Carattere da cercare in $queue.
IP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OR Indicazione sulla corrispondenza tra $letter e l'ultimo carattere letto in $queue->arrChar.
*/
bool hasLetter(const char letter, const CharQueue* queue) {
    int index = queue->insertIndex;
    decreaseIndex(&index, N);
    return (queue->arrChar[index] == letter);
} /*hasLetter*/

/*
Verifica se siamo in presenza della keyword "do" e, in caso, la stampa con il blocco ad essa corrispondente,
seguendo le regole di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Due casi:
   ~ se siamo effettivamente in presenza della keyword "do": la keyword "do" ed i caratteri che compongono il
     blocco corrispondente;
   ~ altrimenti: il carattere 'o' secondo le regole di indentazione.
*/
void fPossibleDo(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    bool normal = true;
    if ((*status != COMMENT) && (*status != APEX) && (*status != QUOTES) && (*status != POSSIBLE_END_COMM)) {
        if (hasLetter('d', queue) && isUniqWord(2, queue)) {
            normal = false;
            *status = NORMAL;
            fprintf(files->out, "%c", 'o');
            fDo(files, sp, status, queue);
        } /*if*/
    } /*if*/
    if (normal)
        fNormal(files->out, sp->indent, status, 'o');
} /*fPossibleDo*/

/*
Verifica se siamo in presenza della keyword "if" e, in caso, la stampa con il blocco ad essa corrispondente,
seguendo le regole di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione.
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Due casi:
   ~ se siamo effettivamente in presenza della keyword "if": la keyword "if" ed i caratteri che compongono il
     blocco corrispondente;
   ~ altrimenti: il carattere 'f' secondo le regole di indentazione.
*/
void fPossibleIf(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    bool normal = true;
    if ((*status != COMMENT) && (*status != APEX) && (*status != QUOTES) && (*status != POSSIBLE_END_COMM)) {
        if (hasLetter('i', queue) && isUniqWord(2, queue)) {
            normal = false;
            *status = NORMAL;
            fprintf(files->out, "%c", 'f');
            fLoop(files, sp, status, queue);
        } /*if*/
    } /*if*/
    if (normal)
        fNormal(files->out, sp->indent, status, 'f');
} /*fPossibleIf*/

/*
Seleziona la funzione necessaria a stampare l'ultimo carattere letto in $queue su file secondo le regole
di indentazione.
IOP files Tipo di dato strutturato che contiene un puntatore al file di input ed un puntatore al file di output.
IOP sp Tipo di dato strutturato che serve a gestire la spaziatura dovuta all'indentazione. 
IOP status Intero che fornisce informazioni sui caratteri letti.
IOP queue Variabile strutturata di tipo CharQueue contenente gli ultimi caratteri letti.
OF Carattere da stampare su file secondo le regole di indentazione.
*/
void indentation(InOutFiles* files, Spacing* sp, int* status, CharQueue* queue) {
    if (queue->arrChar[queue->insertIndex] == '{') 
        fOpenBracket(files->out, sp, status, queue);
    else if (queue->arrChar[queue->insertIndex] == '}')
        fCloseBracket(files->out, sp, status);
    else if (queue->arrChar[queue->insertIndex] == '\n')
        fNewLine(files->out, sp->indent, status, queue);
    else if (queue->arrChar[queue->insertIndex] == '\'')
        fApexQuotes(files->out, sp->indent, status, APEX);
    else if (queue->arrChar[queue->insertIndex] == '\"')
        fApexQuotes(files->out, sp->indent, status, QUOTES);
    else if (queue->arrChar[queue->insertIndex] == ' ')
        fSpace(files->out, status);
    else if (queue->arrChar[queue->insertIndex] == '/')
        fForwSlash(files->out, sp->indent, status);
    else if (queue->arrChar[queue->insertIndex] == '*')
        fStar(files->out, sp->indent, status, queue);
    else if (queue->arrChar[queue->insertIndex] == 'r')
        fPossibleLoop(files, "for", sp, status, queue);
    else if (queue->arrChar[queue->insertIndex] == 'e')
        fPossibleLoop(files, "while", sp, status, queue);
    else if (queue->arrChar[queue->insertIndex] == 'o')
        fPossibleDo(files, sp, status, queue);
    else if (queue->arrChar[queue->insertIndex] == 'f')
        fPossibleIf(files, sp, status, queue);
    else
        fNormal(files->out, sp->indent, status, queue->arrChar[queue->insertIndex]);
} /*indentation*/

/*
IP nameFileIn Nome del file da elaborare.
IP nSpace Numero di spazi di cui e` composta un'indentazione.
IP nameFileOut Nome del file elaborato.
IF File $nameFileIn da elaborare seguendo le regole di indentazione.
OF File $nameFileOut elaborato secondo le regole di indentazione.
OR Esito:
   0: elaborazione riuscita;
  -1: apertura fallita di $nameFileIn;
  -2: apertura fallita di $nameFileOut.
*/
int copyCharWithIndentation(const char nameFileIn[], int nSpace, const char nameFileOut[]) {
    int status = NORMAL;
    CharQueue queue;
    Spacing sp;
    InOutFiles files;
    initArrChar(&queue);
    queue.insertIndex = 0;
    sp.space = nSpace;
    sp.indent = 0;
    files.in = fopen(nameFileIn, "r");
    if (files.in == NULL)
        return -1; /*Errore nel file di input.*/
    files.out = fopen(nameFileOut, "w");
    if (files.out == NULL) {
        fclose(files.in);
        return -2; /*Errore nel file di output.*/
    } /* if */
    while (true) { /*Il ciclo e` interrotto quando feof(inF) == true.*/
        fscanf(files.in, "%c", &queue.arrChar[queue.insertIndex]);
        if (feof(files.in))
            break; /*Si e` raggiunta la fine del file di input.*/
        indentation(&files, &sp, &status, &queue);
        updateIndex(&queue.insertIndex, N);
    } /* while */
    fclose(files.in);
    fprintf(files.out, "%c", '\n'); /*Rende il file di output un file di testo vero e proprio.*/
    fclose(files.out);
    return 0; /*Elaborazione eseguita con successo.*/
} /*copyCharWithIndentation*/
