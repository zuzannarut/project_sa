#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define MAX_LINE_LENGTH 1024

const char * max(const char * a, const char * b)
{
    return a > b ? a : b;
}

char * process_fasta(char * fasta_fp)
{

    FILE *file = fopen(fasta_fp, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    char * sequence = NULL;
    size_t sequence_capacity = 0;
    size_t sequence_length = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '>') {
	    continue;
        } else {
            size_t line_length = strlen(line);
            if (line[line_length - 1] == '\n') {
                line_length--; // Don't count newline
            }
	    
            if (sequence_length + line_length + 1 > sequence_capacity) {
                sequence_capacity = sequence_length + line_length + 2 + 1024; // Add extra space to reduce allocations
                sequence = (char *)realloc(sequence, sequence_capacity);
            }
	    
            memcpy(sequence + sequence_length, line, line_length);
            sequence_length += line_length;
            sequence[sequence_length] = '\0';
        }
    }

    //add $, shift Null
    sequence[sequence_length] = '$';
    sequence[sequence_length+1] = '\0';
    
      

    fclose(file);
    //printf("%s\n", sequence);
    return sequence;
}

int read_all(char * read_fp, char ** fasta_ptr, int ** sa_ptr, int * fasta_len, int * k)
{
    int count = 0;
    FILE *file = fopen(read_fp, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    fread(k, sizeof(int), 1, file);
    fread(fasta_len, sizeof(int), 1, file);
    printf("FASTA LEN: %d\n", *fasta_len);
    
    *fasta_ptr = malloc((*fasta_len + 1) * sizeof(char)); //yeah, yeah, i know
    *sa_ptr = malloc(*fasta_len * sizeof(int));

    count = 0;
    while (count < sizeof(char) * *fasta_len)
    {
	count += fread(*fasta_ptr, sizeof(char), *fasta_len - count, file);
	printf("interim READ %d\n", count);
    }
    printf("READ %d\n", count);
    (*fasta_ptr)[*fasta_len] = '\0';
    //printf("READ %d\n", count);

    /* count = 0; */
    /* while (count < sizeof(int) * *fasta_len) */
    /* { */
	
    /* 	count += fread(*sa_ptr, 1, sizeof(int) * *fasta_len - count, file); */
    /* 	//printf("interim READ %d\n", count); */
    /* } */
    /* printf("READ %d\n", count); */
    
    fclose(file);

    return 0;
}
