#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t bits_per_px;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_resolution;
    uint32_t y_resolution;
    uint32_t number_of_colors;
    uint32_t important_colors;
} BMP_header;

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Pixel;

typedef struct {
    BMP_header header;
    Pixel **data;
} BMP_image;

#pragma pack(pop)

// Function declaration
void debugging();
void freeImage(BMP_image*);
void freeMatrix(int**, int, int);
void matrixFromImage(char*, bool);
void printOptimizedMatrix(int**, int, int);
void openImage(BMP_image*, char*);
void readImage(FILE*, BMP_image*);
void printMatrix(int**, int, int);

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc == 3) {
        if (strcmp(argv[1], "-o") == 0) { matrixFromImage(argv[2], 1); }
        else if (strcmp(argv[2], "-o") == 0) { matrixFromImage(argv[1], 1); }
        else { printf("ERROR: Arguments not valid\n"); return 1; }
    } else if (argc == 2) {
        matrixFromImage(argv[1], 0);
    } else { printf("ERROR: Invalid arguments provided\n"); return 1; }

    return 0;
}

// Function definition
void matrixFromImage(char *filename, bool optimize) {
    BMP_image image;
    openImage(&image, filename);

    int **matrix;
    matrix = (int**)malloc(image.header.height * sizeof(void*));
    for (int i = 0; i < image.header.height; ++i) {
        matrix[i] = (int*)malloc(image.header.width * sizeof(int));
        for (int j = 0; j < image.header.width; ++j) {
            matrix[i][j] = image.data[i][j].r / 255;
        }
    }

    if (!optimize) printMatrix(matrix, image.header.height, image.header.width);
    else printOptimizedMatrix(matrix, image.header.height, image.header.width);

    freeImage(&image);
    freeMatrix(matrix, image.header.height, image.header.width);
}

void openImage(BMP_image *image, char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { printf("ERROR: in opening file\n"); return; }

    (void)!fread(&image->header, sizeof(BMP_header), 1, fp);

    // debugging(*image);

    fseek(fp, image->header.offset, SEEK_SET);
    readImage(fp, image);

    fclose(fp);
}

void readImage(FILE *fp, BMP_image *image) {
    int bytesToRead = ((image->header.bits_per_px * image->header.width + 31) / 32) * 4;
    int totalPixel = bytesToRead / sizeof(Pixel) + 1;

    image->data = (Pixel**)malloc(image->header.height * sizeof(void*));
    for (int i = image->header.height - 1; i >= 0; --i) {
        image->data[i] = (Pixel*)malloc(totalPixel * sizeof(Pixel));

        (void)!fread(image->data[i], 1, bytesToRead, fp);
    }
}

void printMatrix(int **matrix, int height, int width) {
    printf("\nconst matrix[%d][%d] = {\n", height, width);
    for (int i = 0; i < height; ++i) {
        printf("%4c{", ' ');
        for (int j = 0; j < width; ++j) {
            printf("%d,", matrix[i][j]);
        }
        printf("},\n");
    }
    printf("};\n\n");
}

void printOptimizedMatrix(int **matrix, int height, int width) {
    int countHigh = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (matrix[i][j]) ++countHigh;
        }
    }
    printf("\nconst matrix[%d][2] = {", countHigh);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (matrix[i][j]) {
                printf("{%d,%d},", i, j);
            }
        }
    }
    printf("};\n\n");
}

void freeMatrix(int **matrix, int height, int width) {
    for (int i = 0; i < height; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

void freeImage(BMP_image *image) {
    for (int i = image->header.height - 1; i >= 0; --i) {
        free(image->data[i]);
    }
    free(image->data);
} 

void debugging(BMP_image image) {
    int wide = 35;
    int padding = -25;
    for (int i = 0; i < wide; ++i) { printf("%c", "-\n"[i == wide - 1]); }
    printf(" %*s |  %c%c\n", padding, "Type", image.header.type, image.header.type >> 8);
    printf(" %*s |  %d\n", padding, "Size", image.header.size);
    printf(" %*s |  %d\n", padding, "Reserved1", image.header.reserved1);
    printf(" %*s |  %d\n", padding, "Reserved2", image.header.reserved2);
    printf(" %*s |  %d\n", padding, "Offset", image.header.offset);
    printf(" %*s |  %d\n", padding, "Header size", image.header.header_size);
    printf(" %*s |  %d\n", padding, "Width", image.header.width);
    printf(" %*s |  %d\n", padding, "Height", image.header.height);
    printf(" %*s |  %d\n", padding, "Color planes", image.header.color_planes);
    printf(" %*s |  %d\n", padding, "Bits/pixel", image.header.bits_per_px);
    printf(" %*s |  %d\n", padding, "Compression", image.header.compression);
    printf(" %*s |  %d\n", padding, "Image size", image.header.image_size);
    printf(" %*s |  %d\n", padding, "Horizontal Resolution", image.header.x_resolution);
    printf(" %*s |  %d\n", padding, "Vertical Resolution", image.header.y_resolution);
    printf(" %*s |  %d\n", padding, "Number of colors", image.header.number_of_colors);
    printf(" %*s |  %d\n", padding, "Important colors", image.header.important_colors);
    for (int i = 0; i < wide; ++i) { printf("%c", "-\n"[i == wide - 1]); }

    // printf("\n-- PIXEL MATRIX --\n");
    // for (int i = 0; i < image.header.height; ++i) {
    //     for (int j = 0; j < image.header.width; ++j) {
    //         printf("[%3d,%3d,%3d]\n", image.data[i][j].r, image.data[i][j].g, image.data[i][j].b);
    //     }
    //     printf("\n");
    // }
}
