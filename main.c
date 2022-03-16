#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void usage(FILE *stream)
{
    assert(0 && "Not implemented Yet");
}

char *slurp_file(const char *file_path, size_t *size)
{
    char *buffer = NULL;

    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        goto error;
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        goto error;
    }

    long m = ftell(f);
    if (m < 0) {
        goto error;
    }

    buffer = malloc(sizeof(char) * m);

    if (buffer == NULL) {
        goto error;
    }

    if (fseek(f, 0, SEEK_SET) < 0 ) {
        goto error;
    }

    size_t n = fread(buffer, 1, m, f);

    assert(n == (size_t) m);

    if (ferror(f)) {
        goto error;
    }

    if (size) {
        *size = n;
    }

    fclose(f);

    return buffer;

error:  
    if (f) {
        fclose(f);
    }

    if (buffer) {
        free(buffer);
    }

    return NULL;
}



int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Fail to read file\n");
        usage(stderr);
        exit(1);
    }

    size_t content_size = 0;
    char *image_path = argv[1];

    char *content = slurp_file(image_path, &content_size);

    return 0;
}
