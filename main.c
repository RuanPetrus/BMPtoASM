#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

void usage(FILE *stream)
{
    (void) stream;
    assert(0 && "Not implemented Yet");
}

void ByteArrayLE_to_uint8 (const unsigned char* byteArray, uint8_t* x, size_t offset)
{
    *x = (uint8_t)byteArray[offset];
}

void ByteArrayLE_to_uint16 (const unsigned char* byteArray, uint16_t* x, size_t offset)
{
  /* casts -before- shifting are necessary to prevent integer promotion 
     and to make the code portable no matter integer size: */

  *x = (uint16_t)byteArray[offset + 0] <<  0 | 
       (uint16_t)byteArray[offset + 1] <<  8 ;
}

void ByteArrayLE_to_uint32 (const unsigned char* byteArray, uint32_t* x, size_t offset)
{
  /* casts -before- shifting are necessary to prevent integer promotion 
     and to make the code portable no matter integer size: */

  *x = (uint32_t)byteArray[offset + 0] <<  0 | 
       (uint32_t)byteArray[offset + 1] <<  8 |
       (uint32_t)byteArray[offset + 2] <<  16 |
       (uint32_t)byteArray[offset + 3] <<  24 ;
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

    uint32_t width, height;
    uint32_t offset_to_pixels;

    ByteArrayLE_to_uint32((unsigned char *) content, &width, 18);
    ByteArrayLE_to_uint32((unsigned char *) content, &height, 22);

    ByteArrayLE_to_uint32((unsigned char *) content, &offset_to_pixels, 10);

    uint8_t r,g,b;

    uint32_t inicial_point = offset_to_pixels + 2;
    uint32_t final_point = inicial_point + (width * height * 3);

    for (size_t i = inicial_point; i < final_point; i+=3)
    {
        ByteArrayLE_to_uint8((unsigned char *) content, &r, i);
        ByteArrayLE_to_uint8((unsigned char *) content, &g, i + 1);
        ByteArrayLE_to_uint8((unsigned char *) content, &b, i + 2);
    }
    return 0;
}
