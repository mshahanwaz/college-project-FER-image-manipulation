#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 32
#define HEIGHT 16

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
void duplicateImage(BMP_image);
int createImage();
void matrixToImage(Pixel**);
int randomColor();

int main() {
    srand(time(NULL));

    if (createImage() == 0) printf("SUCCESS: Created successfully!\n");
    else printf("ERROR: Occured while creating");

    return 0;
}

// Function definition
int createImage() {
    FILE *fpw = fopen("../bmps/new.bmp", "wb");
    if (!fpw) { printf("ERROR: Not able to create new file\n"); return 1; }

    BMP_image image;

    int bytesToWrite = ((24 * WIDTH + 31) / 32) * 4;
    int totalPixel = bytesToWrite / sizeof(Pixel) + 1;

    // simple BMP format header
    BMP_header header = { 0x4D42, 54 + bytesToWrite * HEIGHT, 0, 0, 54, 40, WIDTH, HEIGHT, 1, 24, 0, 0, 0, 0, 0, 0 };

    (void)!fwrite(&header, sizeof(BMP_header), 1, fpw);

    image.data = (Pixel**)malloc(HEIGHT * sizeof(void*));
    for (int i = 0; i < HEIGHT; ++i) {
        image.data[i] = (Pixel*)malloc(totalPixel * sizeof(Pixel));
    }

    matrixToImage(image.data);

    for (int i = HEIGHT - 1; i >= 0; --i) {
        (void)!fwrite(image.data[i], bytesToWrite, 1, fpw);
    }

    fclose(fpw);
    freeImage(&image);

    return 0;
}

void matrixToImage(Pixel **data) {
    int matrix[HEIGHT][WIDTH] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,1,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    };

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            uint8_t color = matrix[i][j] * 255;
            data[i][j] = (Pixel){ color, color, color };
        }
    }
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
