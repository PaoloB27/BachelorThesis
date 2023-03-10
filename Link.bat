REM Linka fino a 5 file oggetto e crea un file eseguibile dandogli il nome del primo file oggetto.
REM Digitare il primo file oggetto senza l'estensione (.o) e ogni altro file oggetto con l'estensione (.o).
REM Es: Link List_Tester List.o
gcc -o %1.exe %1.o %2 %3 %4 %5
