#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "hashtable.h"

#define MAX_LINE_LENGTH 1024

typedef struct table_vals_s {
    char * key;
    char * min_val;

} table_vals_s;


char * load_query(FILE * file, char ** q_name)
{
    char line[MAX_LINE_LENGTH];
    char * sequence = NULL;
    size_t sequence_capacity = 0;
    size_t sequence_length = 0;

    while (fgets(line, sizeof(line), file))
    {
	
	//skip initial line
	//printf("LINE: %s", line);
	//printf("SL: %ld\n", sequence_length);
	if (line[0] == '>' && sequence_length == 0)
	{
	    *q_name = malloc(sizeof(char) * strlen(line));
	    memcpy(*q_name, line + 1, strlen(line)-2);
	    (*q_name)[strlen(line)-2] = 0;
	    //printf("QNAME: %s\n", *q_name);
	    //printf("QN complete\n");
	    continue;
	}
	
	//moving to next query
	else if (line[0] == '>')
	{
	    fseek(file, -strlen(line), SEEK_CUR);
	    break;
	}
	
	else
	{
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
    if (sequence)
    {
	sequence[sequence_length] = '\0';
    }
    
      

    return sequence;
    
    
}



int int_min(int a, int b)
{
    return a < b ? a : b;
}

int my_count_cmp(char * a, char * b, int * cmp, int mode, int * matched)
{
    //b -> query
    int len = strlen(b);//int_min(strlen(a), strlen(b));
    int i;
    int ret = 0;

    for (i = 0; i < len; i++)
    {
	*cmp += 1;
	//printf("%d: a: %c\t b: %c\n", i, a[i], b[i]);
	ret = a[i] - b[i];
	if (ret != 0)
	{
	    break;
	}

    }

    if (mode > 0)
    {
	*matched = i;
    }
    
    return ret;
}


char * get_min(char * kmer, int kmer_len, int min_len, int * final_hash)
{
    char * cur_ptr = kmer;
    char * min_ptr = NULL;
    int tmp_hash = 0;
    *final_hash = INT_MAX;
	

    while (cur_ptr + min_len <= kmer + kmer_len)
    {
	//printf("Hashing: %c%c\n", *cur_ptr,*(cur_ptr+1));
	tmp_hash = string_hash_function(cur_ptr, min_len);
	//printf("Tmp hash: %d\n", tmp_hash);
	if (tmp_hash < *final_hash)
	{
	    *final_hash = tmp_hash;
	    min_ptr = cur_ptr;
	}
	cur_ptr++;

    }
    //printf("Minmizer: %c%c\n", *(min_ptr), *(min_ptr+1));
    
    return min_ptr;
}


int * find_mins(hash_table_s * hash_table, int kmer_len, int min_len, char * fasta_str, int fasta_len, int * max_val)
{
    int * output_array = malloc(sizeof(int) * fasta_len);
    char * cur_kmer = fasta_str;
    char * cur_min = NULL;
    char * temp_str = NULL;
    int hash_val=0;
    int unique_mval=*max_val;
    table_vals_s * vals = NULL;

    int i;

    //printf("fasta: %s\n", fasta_str);
    
    while (cur_kmer + kmer_len <= fasta_str + fasta_len)
    {
	/* if (*max_val > 0) */
	/*     printf("On %ld\n", cur_kmer - fasta_str); */
	
	cur_min = get_min(cur_kmer, kmer_len, min_len, &hash_val);
        
	
	temp_str = calloc(sizeof(char), min_len +  1);
	memcpy(temp_str, cur_min, min_len);
	//printf("Min_str: %s\n", temp_str);
	
	vals = search(hash_table, temp_str);

	//new minimizer
	if (vals == NULL)
	{
	    //printf("Adding!\n");
	    vals = calloc(sizeof(table_vals_s), 1);
	    vals->key = temp_str;
	    vals->min_val=unique_mval++;

	    insert(hash_table, temp_str, vals);
	}
	else
	{
	    free(temp_str);
	}

	//printf("Adding %d to OA at %d\n", vals->min_val, (cur_kmer-fasta_str));
	*(output_array +  (cur_kmer - fasta_str)) = vals->min_val;
	    
	    
	cur_kmer += 1;
	
    }

    /* for(i=0; i < fasta_len-kmer_len+1; i++) */
    /* { */
    /* 	printf("OA[%d]: %d\n", i, output_array[i]) ; */
    /* } */
    //printf("UMV: %d\n", unique_mval);
    *max_val = unique_mval-1;
    //printf("MV: %d\n", *max_val);
    return output_array;
}

void write_outputs(uint * min_output_array, char * hash_table, char * fast_output_file_base, int k, int w, int fasta_len, int max_val, char * open_style)
{

    float d_size[] = {1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
    //float d_size[] = {1.0, 0.1};
    FILE * f_out_file;// = fopen(fast_output_file, "wb");
    int i, j, d;
    int unique_count = 0;
    uint allowed = 0;
    int length = 0;
    char * output_fp = calloc(1, strlen(fast_output_file_base) + 100);
    strncpy(output_fp, fast_output_file_base, strlen(fast_output_file_base));
    
    printf("MAX VAL: %d\n", max_val);
			      
			      

    for(i=0; i < sizeof(d_size)/sizeof(d_size[0]); i++)
    {
	sprintf(output_fp, "%s_k-%d_w-%d_d-%.2f.fa2", fast_output_file_base, k, w, d_size[i]);
	f_out_file = fopen(output_fp, open_style);
	if (f_out_file == NULL)
	    printf("FOPEN ERROR\n");
	

	unique_count = 0;
	length = 0;
	allowed = d_size[i] * max_val;

	for(j=0; j<(fasta_len-k+1); j++)
	{
	    //printf("On %d\n", j);
	    //printf("Comparing %d to %d\n", *(min_output_array + j), allowed);
	    if (*(min_output_array + j) >= allowed)
	    {
		//printf("Continuing\n");
		continue;
	    }
	    //printf("Writing it!\n");
	    fwrite(min_output_array + j, sizeof(uint), 1, f_out_file);
	    //printf("Written!\n");
	    length+=1;
	}

	printf("D: %.2f\tLength: %d\t Original Length: %d\n", d_size[i], length, fasta_len);
	fclose(f_out_file);
    }



}

void proc_queries(char * query_fp, char * output_fp, hash_table_s * query_table, hash_table_s * hash_table, int kmer_len, int min_len, int max_val, float delta)
{
    int mv=max_val;
    int no_min = 0;
    int collision = 0;

    
    int * transformed_q = NULL;

    FILE * q_file = fopen(query_fp, "r");
    char * new_output_fp = malloc(strlen(output_fp) +100);
    sprintf(new_output_fp, "%s_k-%d_w-%d_d-%.2f.qa2", output_fp, kmer_len, min_len, delta);

    /* printf("New ofp: %s\n", new_output_fp); */
	
    FILE * output_file = fopen(new_output_fp, "w");
    
    int * original_len_counts = calloc(100, sizeof(int));
    int * new_len_counts = calloc(100, sizeof(int));

    int * temp_str;
    char * out_string[5096];
    char * q_name;
    char * query = load_query(q_file, &q_name);
    mem_s * vals = NULL;

    int i, j, length;
    int q_count = 0;
    int allowed;
    
    /* if (output_file ==NULL) */
    /* { */
    /* 	printf("OF Error!\n"); */
    /* } */
    
    
    //note, we're assuming unique queries to avoid having to manually check collisions -> if a key exists in the hashtable, we expect this was caused by minimizers
    while(query)
    {
	//printf("On query %s\n", query);
	q_name[strlen(q_name)] = '\0';
	original_len_counts[strlen(query)-1] += 1;
	q_count++;

	/* if (mv == 0) */
	/*     mv = 1; */
	transformed_q = find_mins(hash_table, kmer_len, min_len, query, strlen(query), &mv);

	

	//printf("Transform complete\n");
	//printf("Adding %d to OA at %d\n", vals->min_val, (cur_kmer-fasta_str));
	//*(output_array +  (cur_kmer - fasta_str)) = vals->min_val;
	

	//write_outputs(proc_query, hash_table, output_fp, k, w, fasta_len, mv, "a")


	length = 0;
	allowed = delta * max_val;
	//printf("Allowed: %d\t diff: %d\n", allowed, strlen(query) - kmer_len);


	if ((int)strlen(query) - kmer_len < 0)
	{
	    no_min++;
	    //printf("Skipping\n");
	    sprintf(out_string, "%s:%d\n", q_name, 0);
	    fputs(out_string, output_file);
	    query = load_query(q_file, &q_name);
	    continue;
	}
	temp_str = calloc(strlen(query), sizeof(int));
	for(j=0; j<strlen(query) - kmer_len + 1; j++)
	{
	    //printf("On %d\n", j);
	    //printf("Comparing %d to %d\n", *(transformed_q + j), allowed);
	    if (*(transformed_q + j) >= allowed)
	    {
		//printf("Continuing\n");
		continue;
	    }
	    //printf("Writing it!\n");
	    //fwrite(transformed_q + j, sizeof(uint), 1, output_file);
	    //printf("Written!\n");
	    *(temp_str + length) = *(transformed_q + j);
	    length+=1;
	}
	
	sprintf(out_string, "%s:%d\n", q_name, length);
	fputs(out_string, output_file);

	if (length < 1)
	{
	    no_min++;
	    query = load_query(q_file, &q_name);
	    continue;
	}
	
	fwrite(transformed_q + j, sizeof(uint), length, output_file);
	//printf("Written2\n");
	
	vals = search(query_table, temp_str);

	//new minimizer
	if (vals == NULL)
	{
	    vals = calloc(sizeof(mem_s), 1);
	    vals->value = temp_str;
	    vals->len = length;//new_mval++;

	    insert(query_table, temp_str, vals);
	}
	else
	{

	    collision++;
	    free(temp_str);
	}


	
	

	    
	query = load_query(q_file, &q_name);

    }
    
    printf("Queries at %.2f\tTotal: %d\tCollisions: %d\t no_min: %d\n", delta, q_count, collision, no_min);
}



int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s <kmer> <min_len> <input_fp> <query_fp> <fasta_out> <query_out>\n", argv[0]);
        return 1;
    }

    int kmer_len = atoi(argv[1]);
    int min_len = atoi(argv[2]);
    char * input_fp = argv[3];
    char * query_fp = argv[4];
    //char * query_mode = argv[5];
    char * fast_output_fp = argv[5];
    char * query_output_fp = argv[6];

    /* char test_fasta[] = "AABABABAABAAAAB"; */
    /* int test_kmer = 4; */
    /* int test_min = 2; */


    char * fasta_str = NULL;
    int * sa = NULL;
    int fasta_len = 0;
    int k = 0;
    int i;
    int max_min = 0;
    float d_size[] = {1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
    int * mins_output_array = NULL;


    //read_all(input_fp, &fasta_str, &sa, &fasta_len, &k);
    fasta_str = process_fasta(input_fp);
    fasta_len = strlen(fasta_str);
    

    //fasta_str[fasta_len - 1] = 0;

    //TEST TEST TEST
    //fasta_len = strlen(test_fasta);
	

    hash_table_s *hash_table = create_table(fasta_len *2, string_hash_function, string_key_compare);
    

    mins_output_array = find_mins(hash_table, kmer_len, min_len, fasta_str, fasta_len, &max_min);
    write_outputs(mins_output_array, hash_table, fast_output_fp, kmer_len, min_len, fasta_len, max_min, "w");


    for (i=0; i < sizeof(d_size) / sizeof(d_size[0]); i++)
    {
	hash_table_s *query_table = create_table(fasta_len *2, string_hash_function, mem_key_compare);
	proc_queries(query_fp, query_output_fp, query_table, hash_table, kmer_len, min_len, max_min, d_size[i]);

	
    }
    
    return 0;

}
