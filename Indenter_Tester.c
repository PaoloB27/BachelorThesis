/* Paolo Bresolin 1187162 */

#include <stdlib.h>

#include "Indenter.h"


int main(int argc, char** argv) {
    int outcome = copyCharWithIndentation(argv[1], atoi(argv[2]), argv[3]);
    if (outcome == -1)
        perror(argv[1]);
    else if (outcome == -2)
        perror(argv[2]);
    return 0;
} /*main*/
