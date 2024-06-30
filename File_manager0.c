#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

enum {
    max_sub = 3,
    stud_data = 3
};

bool isFull(const char *fname) {
    FILE *f = fopen(fname, "r");
    if ( !f )
        return false;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);

    fclose(f);
    
    return file_size != 0;
}

void finput ( const char *fname, int size ) {
    if ( isFull( fname ) ) {
        char answer;
        printf(" seems like your file isn't empty honey-bee!\n still wanna write over it? [Y/N]: ");
        scanf("%c%*c", &answer);
        if ( tolower(answer) == 'n' )
            return;
        else if ( tolower(answer) == 'y' )
            printf(" alright then, you're the boss sunshine\n");
    }
    
    FILE *f = fopen(fname,"w+");
    assert(f);
    
    for ( int i = 0; i < size; ++i ) {
        char *buffer;
        long bufsize;
        printf("ID %d: ", i+1);
        getline(&buffer, &bufsize, stdin);
        fprintf( f, "%s%s", "ID: ", buffer );
        
        printf("name %d: ", i+1);
        getline(&buffer, &bufsize, stdin);
        fprintf( f, "%s%s", "name: ", buffer );
        
        fprintf( f, "%s", "marks: " );
        for( int j = 0; j < max_sub; ++j ){
            printf("mark %d: ", j+1);
            getline(&buffer, &bufsize, stdin);
            fprintf( f, "%.*s ", (int)strlen(buffer)-1, buffer );
        }
        fputs("\n",f);
    }
    
    fclose(f);
}

void fdisplay ( const char *fname ){
    FILE *f = fopen(fname,"r");
    assert(f);
    
    char c;
    while ((c = fgetc(f)) != EOF) {
        putchar(c);
    }
    
    //we can use this too
    /***********
    char *line;
    long bufsize;
    while (getline(&line, &bufsize, f) > -1)
        printf("%s", line);
    ************/
    //or this
    /***********
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    ************/
    
    fclose(f);
}

long* fsearch ( const char *fname, const char *search ) {
    char *line = NULL;
    long bufsize, *position = (long*) malloc ( 2 * sizeof(long));
    position[1] = -1;
    FILE *f = fopen(fname,"r");
    assert(f);
    
    while (getline(&line, &bufsize, f) != -1) {
        if (strstr(line, search)) {
            for ( int i = 0; i < stud_data; ++i ){
                position[1] = ftell(f);
                printf("%s",line);
                getline(&line, &bufsize, f);
            }
            break;
        }
        position[0] = ftell(f);
    }

    fclose(f);
    
    return position;
}

void fadd ( const char *fname, int size, const char *str ) {
    int i = size+1;
    long bufsize;
    char *buffer;
    
    FILE *f = fopen(fname,"a");
    assert(f);
    
    fprintf( f, "%s%s", "ID: ", str );
        
    printf("name %d: ", i+1);
    getline(&buffer, &bufsize, stdin);
    fprintf( f, "%s%s", "name: ", buffer );
        
    fprintf( f, "%s", "marks: " );
    for( int j = 0; j < max_sub; ++j ){
        printf("mark %d: ", j+1);
        getline(&buffer, &bufsize, stdin);
        fprintf( f, "%.*s ", (int)strlen(buffer)-1, buffer );
    }
    fputs("\n",f);
    
    fclose(f);
}

void fmodify ( const char *fname, const char *str, long *position ) {
    char *buffer = NULL;
    long bufsize;
    
    FILE *f = fopen(fname,"r+");
    assert(f);
    fseek(f, position[0], SEEK_SET);
    
    printf("new ID: ");
    getline(&buffer, &bufsize, stdin);
    fprintf( f, "%s%s", "ID: ", buffer );
        
    printf("new name: ");
    getline(&buffer, &bufsize, stdin);
    fprintf( f, "%s%s", "name: ", buffer );
        
    fprintf( f, "%s", "marks: " );
    for( int j = 1; j <= max_sub; ++j ){
        printf("mark %d: ", j);
        getline(&buffer, &bufsize, stdin);
        fprintf( f, "%.*s ", (int)strlen(buffer)-1, buffer );
    }
    fputs("\n",f);
    
    fclose(f);
}
long fcpy(FILE *dest, FILE *src, long start_cpy_position) {
    char c;
    
    // Set the position from where to start copying in the source file
    fseek(src, start_cpy_position, SEEK_SET);
    
    // Copy characters from source to destination until End Of File
    while ((c = fgetc(src)) != EOF)
        fputc(c, dest);
    
}

void sremove ( const char *fname, long *position ) {
    char *fn = "temp";
    
    // Open the original file in read mode and the temporary file in write mode
    FILE *file = fopen(fname, "r+"),
         *f    = fopen(fn, "w");
    
    // Copy the content after the section to be removed to the temporary file
    fcpy(f, file, position[1]);
    
    //reopen temp on read only
    f = freopen(fn, "r", f);
    
    // Set the position to start overwriting in the original file
    fseek(file, position[0], SEEK_SET);
    
    // Overwrite the content from the temporary file back to the original file
    // starting from position[0]
    fcpy(file, f, 0);
    
    //changing the original file's size to take off the garbage at the end
    ftruncate(fileno(file), ftell(file));
    
    fclose(file);
    fclose(f);
    
    //remove temp
    remove(fn);
}

int main()
{
    int size, choice;
    long *position, len;
    char *fname = "lecture X", *search, secchoice;
    
    FILE* F;
    if ( !(F = fopen(fname, "r")) )
        F = fopen(fname, "w");
    fclose(F);
    
    printf(" how many students you got, cupcake? ");
    scanf("%d%*c", &size);
    
    finput ( fname, size );
    
    printf("\n here's students list:\n");
    fdisplay ( fname );
    
    printf("\n now tell me sunshine whome you're looking for: ");
    getline(&search, &len, stdin);
    
    position = fsearch ( fname, search );
    
    if ( position[1] != -1){
        printf("\n now you find IT what you wich to do with ITS data?\n"
                " 1.Delete it?\n"
                " 2.modify it?\v");
        scanf("%d%*c", &choice);
        if ( choice == 1 )
            sremove ( fname, position );
        else
            fmodify ( fname, search, position );
        
    } else {
        printf("\n seems like %s doesn't exist!\n"
                " wanna add it?[Y/N] ", search);
        scanf("%c%*c", &secchoice);
        if ( tolower(secchoice) == 'y' )
            fadd ( fname, size, search );
    }
    
    printf("\n here's the new list:\n");
    fdisplay ( fname );
    
    return 0;
}
