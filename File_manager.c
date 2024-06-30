#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

enum {
    max_sub = 3,
    max_len_name = 100,
    BUFFER_SIZE = 200
};

typedef struct {
    long id;
    char name[max_len_name];
    double mark[max_sub];
} pupil;

bool isFull(const char *fname) {
    FILE *f = fopen(fname, "r");
    if ( !f )
        return false;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);

    fclose(f);
    
    return file_size != 0;
}

void freadp ( const char *fname, pupil *stdnt, int size ) {
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
        printf("ID %d: ", i+1);
        scanf("%ld%*c", &stdnt[i].id);
        fprintf( f, "%s%ld\n", "ID: ", stdnt[i].id );
        
        printf("name %d: ", i+1);
        assert( fgets( stdnt[i].name, sizeof(stdnt[i].name), stdin ) );
        fprintf( f, "%s%s", "name: ", stdnt[i].name );
        
        fprintf( f, "%s", "marks: " );
        for( int j = 0; j < max_sub; ++j ){
            printf("mark %d: ", j+1);
            scanf("%lf%*c", &stdnt[i].mark[j]);
            fprintf( f, "%.2lf ", stdnt[i].mark[j] );
        }
        fputs("\n",f);
    }
    
    fclose(f);
}

void fdisplay ( const char *fname ){
    FILE *f = fopen(fname,"r");
    assert(f);
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    
    char *buffer = (char*)malloc(file_size * sizeof(char));
    assert(buffer);
    
    fread(buffer, sizeof(char), file_size, f);
    
    fwrite(buffer, sizeof(char), file_size, stdout);
    
    fclose(f);
}

long fsearch ( const char *fname, const char *search ) {
    char line[BUFFER_SIZE];
    long int line_positions[BUFFER_SIZE] = {0};
    int count = 0, student_data_size = 2, current_line = 0, x = 1;
    FILE *f = fopen(fname,"r");
    assert(f);
    
    while (fgets(line, sizeof(line), f)) {
        line_positions[++current_line] = ftell(f);
        if (strcasestr(line, search)) {
            if( line[0] != 'I' ){
                student_data_size++;
                fseek(f, line_positions[current_line - 2], SEEK_SET);
            }
            else
                printf("%s", line);
            while (fgets(line, sizeof(line), f) && count < student_data_size) {
                printf("%s", line);
                count++;
            }
            break;
        }
    }

    fclose(f);
    
    if ( count ) {
        if ( student_data_size != 2 )
            x++;
        
        return line_positions[current_line - x];
    }
    return -1;
}

void fadd ( const char *fname, pupil *stdnt, int size, const char *str ) {
    int i = size+1;
    FILE *f = fopen(fname,"a");
    assert(f);
    
    if ( isdigit(*str) ) {
        stdnt[i].id = atoi(str);
        fprintf( f, "%s%ld", "ID: ", stdnt[i].id );
        
        printf("name %d: ", i+1);
        assert( fgets( stdnt[i].name, sizeof(stdnt[i].name), stdin ) );
        fprintf( f, "%s%s", "name: ", stdnt[i].name );
    } else {
        printf("ID %d: ", i+1);
        scanf("%ld%*c", &stdnt[i].id);
        fprintf( f, "%s%ld\n", "ID: ", stdnt[i].id );
        
        strcpy( stdnt[i].name, str );
        fprintf( f, "%s%s", "name: ", stdnt[i].name );
    }
    
    fprintf( f, "%s", "marks: " );
    for( int j = 0; j < max_sub; ++j ){
        printf("mark %d: ", j+1);
        scanf("%lf%*c", &stdnt[i].mark[j]);
        fprintf( f, "%.2lf ", stdnt[i].mark[j] );
    }
    fputs("\n",f);
    
    fclose(f);
}

void fmodify ( const char *fname, const char *str, pupil *stdnt, long position ) {
    FILE *f = fopen(fname,"r+");
    assert(f);
    fseek(f, position, SEEK_SET);
    for ( int k = 0; k < 3; ++k )
        fputs( "\n", f );
    fseek(f, position, SEEK_SET);
    int i = 0;
    
    for( ; ; ++i )
        if ( !strcasecmp(stdnt[i].name, str) )
            break;
            
            
    printf(" new ID %d: ", i+1);
    scanf("%ld%*c", &stdnt[i].id);
    fprintf( f, "%s%ld\n", "ID: ", stdnt[i].id );
        
    printf("new name %d: ", i+1);
    assert( fgets( stdnt[i].name, sizeof(stdnt[i].name), stdin ) );
    fprintf( f, "%s%s", "name: ", stdnt[i].name );
        
    fprintf( f, "%s", "marks: " );
    for( int j = 0; j < max_sub; ++j ){
        printf("mark %d: ", j+1);
        scanf("%lf%*c", &stdnt[i].mark[j]);
        fprintf( f, "%.2lf ", stdnt[i].mark[j] );
    }
    fputs("\n",f);
    
    fclose(f);
}

void sremove ( const char *fname, pupil *stdnt, long position ) {
    FILE *file = fopen(fname, "r+"),
         *f    = fopen(fname, "r");
         
    char *line = NULL, *l = NULL;
    
    int lin_amount_delet = 4;
        
    long len = 0;
    
    fseek(file, 0, SEEK_END);
    fputs("\n", file);
    fseek(file, position, SEEK_SET);
    fseek(f, position, SEEK_SET);
    
    for ( int i = 0; i < lin_amount_delet; ++i )
        getline(&l, &len, f);
    
    while (getline(&line, &len, file) >= 0) {
        
        fseek(file, -strlen(line), SEEK_CUR);
            
        fwrite(l, sizeof(char), strlen(l), file);
            
        if(getline(&l, &len, f) >= 0);
        else break;
    }
    
    fseek(file, ftell(file), SEEK_SET);
    ftruncate(fileno(file), ftell(file)-2);

    fclose(file);
    fclose(f);
}

int main()
{
    int size, choice;
    long position, len;
    pupil *stdnt;
    char *fname = "lecture X", *search, secchoice;
    
    
    FILE* F;
    if ( !(F = fopen(fname, "r")) )
        F = fopen(fname, "w");
    fclose(F);
    
    printf(" how many students you got, cupcake? ");
    scanf("%d%*c", &size);
    
    stdnt = ( pupil* ) calloc ( size, sizeof( pupil ) );
    assert( stdnt ); // this is the same as "if ( f == NULL )" blah blah blah...
    
    freadp ( fname, stdnt, size );
    
    printf("\n here's students list:\n");
    fdisplay ( fname );
    
    printf("\n now tell me sunshine whome you're looking for: ");
    getline(&search, &len, stdin);
    
    if ( (position = fsearch ( fname, search )) > -1){
        printf("\n now you find IT what you wich to do with ITS data?\n"
                " 1.Delete it?\n"
                " 2.modify it?\n");
        scanf("%d%*c", &choice);
        if ( choice == 1 )
            sremove ( fname, stdnt, position );
        else
            fmodify ( fname, search, stdnt, position );
        
    } else {
        printf("\n seems like %s doesn't exist!\n"
                " wanna add it?[Y/N]", search);
        scanf("%c%*c", &secchoice);
        if ( tolower(secchoice) == 'y' )
            fadd ( fname, stdnt, size, search );
    }
    
    printf("\n here's the new list:\n");
    fdisplay ( fname );
    
    return 0;
}
