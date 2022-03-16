#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

void usage(FILE *stream)
{
    (void) stream;
    assert(0 && "Not implemented Yet");
}

void ByteArrayLE_to_uint16 (const char* byteArray, uint16_t* x, size_t offset)
{
  /* casts -before- shifting are necessary to prevent integer promotion 
     and to make the code portable no matter integer size: */

  *x = (uint16_t)byteArray[offset + 0] <<  0 | 
       (uint16_t)byteArray[offset + 1] <<  8 ;
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

    uint16_t x, y;

    for (size_t i = 0; i < 50; i+=2) {
        ByteArrayLE_to_uint16(content, &x, i);
        printf("Pos: %zu - Valor: %u\n", i , x);
    }
    
    return 0;
}
