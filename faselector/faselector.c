#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <glib.h>

#define NL_STRIP(s, l) if (s[l - 1] == '\n') s[l - 1] = '\0'

/*
 * Select a set of sequence IDs from a fasta file
 * Usage: faselector -d id_list_file -i input_file [ -k keep_file] [ -r reject_file ]
 *
 * Uses gnulib hash tables, hence needs to be compiled with something like:
 * cc `pkg-config --cflags --libs glib-2.0` -o tools/bin/faselector tools/src/faselector.c
*/

void usage(char *pname);

int main(int argc, char **argv)
{
    FILE *fin, *fid, *fkeep, *freject;
    char *line = NULL, *line2, *pspace;
    char buffer_i[BUFSIZ], buffer_k[BUFSIZ], buffer_r[BUFSIZ];
    char *id_file, *input_file, *keep_file, *reject_file;
    size_t len, blen = 0;
    int append = 0, help = 0;
    int show = 0;
    int c;

    id_file = input_file = keep_file = reject_file = NULL;
    opterr = 0;

    while ((c = getopt (argc, argv, "d:i:k:r:ah")) != -1)
        switch (c) {
            case 'd':
                id_file = strdup(optarg);
                break;
            case 'k':
                keep_file = strdup(optarg);
                break;
            case 'a':
                append = 1;
                break;
            case 'h':
                help = 1;
                break;
            case 'r':
                reject_file = strdup(optarg);
                break;
            case 'i':
                input_file = strdup(optarg);
                break;
            case '?':
                fprintf (stderr, "Unknown option character '\\x%x'.\n", optopt);
                return 1;
        }

    if (help) {
        usage(argv[0]);
        return 0;
    }

    if (! (id_file && input_file) ) {
        fprintf(stderr, "Missing options: need ID file [-d] and input file [-i]\n");
        return 1 ;
    }

    if (! (keep_file || reject_file) ) {
        fprintf(stderr, "Missing options: need either/both of keep file [-k] and reject file [-r]\n");
        return 1;
    }

    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);

    if ((fid = fopen(id_file, "r")) == NULL) {
        fprintf(stderr, "Unable to open file %s\n", id_file);
        return 2;
    }
    while ((len = getline(&line, &blen, fid)) != -1) {
        NL_STRIP(line, len);
        g_hash_table_insert(hash, strdup(line), "1");
    }
    fclose(fid);

    if ((fin = fopen(input_file, "r")) == NULL) {
        fprintf(stderr, "Unable to open file %s\n", input_file);
        return 2;
    }
    setbuf(fin, buffer_i);

    if (keep_file) {
        if ((fkeep = fopen(keep_file, append ? "a" : "w")) == NULL) {
            fprintf(stderr, "Unable to open file %s\n", keep_file);
            return 2;
        }
        setbuf(fkeep, buffer_k);
    }

    if (reject_file) {
        if ((freject = fopen(reject_file, append ? "a" : "w")) == NULL) {
            fprintf(stderr, "Unable to open file %s\n", reject_file);
            return 2;
        }
        setbuf(freject, buffer_r);
    }

    while ((len = getline(&line, &blen, fin)) != -1) {
        NL_STRIP(line, len);
        // Accessions shouldn't contain spaces -
        // remove anything from the header from the first space character
        if (pspace = index(line, ' '))
            line2 = strndup(line, pspace - line);
        else
            line2 = line;
        if (*line == '>') {
            if (g_hash_table_lookup(hash, line2 + 1) != NULL) {
                show = 1;
                if (keep_file)
                    fprintf(fkeep, "%s\n", line);
            } else {
                show = 0;
                if (reject_file)
                    fprintf(freject, "%s\n", line);
            }
        } else {
            if (show) {
                if (keep_file)
                    fprintf(fkeep, "%s\n", line);
            } else {
                if (reject_file)
                    fprintf(freject, "%s\n", line);
            }
        }
        if (pspace)
            free(line2);
    }

    g_hash_table_destroy(hash);
    free(line);
    free(input_file);
    free(id_file);

    if (reject_file) {
        fflush(freject);
        fclose(freject);
        free(reject_file);
    }
    if (keep_file) {
        fflush(fkeep);
        fclose(fkeep);
        free(keep_file);
    }
    fclose(fin);

    return 0;
}

void usage(char *pname) {
    printf("Usage: %s\n", pname);
    printf("\t-d FILE : list of accessions, one per line\n");
    printf("\t-i FILE : input fasta file\n");
    printf("\t-k FILE : output file of accessions to keep\n");
    printf("\t-r FILE : output file of accessions to discard\n");
    printf("\t-a      : append to output files, rather than writing new\n");
    printf("\n");
}
