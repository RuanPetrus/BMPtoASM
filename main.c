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

typedef struct {
	uint8_t red, green, blue;
} Pixel;

float map(float n, float start1, float stop1, float start2, float stop2) 
{
	return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

uint8_t bmp_to_asm (Pixel pixel) 
{
	uint8_t r = (uint8_t) map(pixel.red, 0, 255, 0, 7);
	uint8_t g = (uint8_t) map(pixel.green, 0, 255, 0, 7);
	uint8_t b = (uint8_t) map(pixel.blue, 0, 255, 0, 3);

	return r | (g << 3) | (b << 6); 
}

void pixel_array_to_asm(Pixel* pixels, uint32_t width, uint32_t height) 
{
	 FILE *fptr;
	 fptr = fopen("image.s","w");
	 fprintf(fptr, "teste: .word %d, %d\n.byte ", width, height);

	uint32_t total =  width * height;

	for (uint32_t j = 0; j < height; j++) {
		for (uint32_t i = 0; i < width; i++) {
			uint32_t index = width * (height - j - 1) + i;
			uint8_t color = bmp_to_asm(pixels[index]);
			fprintf(fptr, "%d,", color);
		}
	}

	fclose(fptr);
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

    uint32_t inicial_point = offset_to_pixels;
	if (inicial_point % 4 != 0) inicial_point += 2;
    uint32_t final_point = inicial_point + (width * height * 3);

	Pixel *pixels = malloc(3 * 8 * width * height); 

	uint8_t padding = (4 - ((width * 3) % 4)) % 4;

	size_t arr_count = 0;
    for (size_t i = inicial_point; i < final_point; i+=3)
    {
		size_t j = i - inicial_point;

		if (padding != 0 && j % (width*3 + padding) >= width*3) {
			printf("pulou %zu\n", j);
			i -= 2;
			continue;
		}

        ByteArrayLE_to_uint8((unsigned char *) content, &r, i + 0);
        ByteArrayLE_to_uint8((unsigned char *) content, &g, i + 1);
        ByteArrayLE_to_uint8((unsigned char *) content, &b, i + 2);

		pixels[arr_count].red = r;
		pixels[arr_count].green = g;
		pixels[arr_count].blue = b;
		arr_count++;
		//printf("%d %d %d  j:%zu\n", r, g, b, j);
    }

	// pixel_array_to_asm(pixels, width, height);
	Pixel pixel;
	pixel.red = 0;
	pixel.green = 100;
	pixel.blue = 25;
	printf("%d", bmp_to_asm(pixel));


    return 0;
}
