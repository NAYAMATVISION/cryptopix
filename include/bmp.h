#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdio.h>

/*
 * Force compiler to disable byte padding.
 * Standard BMP headers must map 1:1 to the binary bytes on disk.
 */
#pragma pack(push, 1)

/* 14-byte File Header */
typedef struct {
    uint16_t type;             /* Magic identifier: must be 0x4D42 ('BM') */
    uint32_t size;             /* Total file size in bytes */
    uint16_t reserved1;        /* Reserved; must be 0 */
    uint16_t reserved2;        /* Reserved; must be 0 */
    uint32_t offset;           /* Offset to start of pixel data (usually 54) */
} BMPFileHeader;

/* 40-byte DIB Info Header (BITMAPINFOHEADER format) */
typedef struct {
    uint32_t size;             /* Header size in bytes (40) */
    int32_t  width;            /* Image width in pixels */
    int32_t  height;           /* Image height in pixels (positive = bottom-up) */
    uint16_t planes;           /* Number of color planes (must be 1) */
    uint16_t bits_per_pixel;   /* Bits per pixel (24 for RGB) */
    uint32_t compression;      /* Compression method (0 = uncompressed BI_RGB) */
    uint32_t image_size;       /* Size of raw image data (can be 0 for BI_RGB) */
    int32_t  x_pixels_per_meter;
    int32_t  y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} BMPInfoHeader;

#pragma pack(pop)

/* Image container struct holding metadata and raw pixel bytes in memory */
typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    uint8_t *data;             /* Dynamic heap buffer holding raw BGR pixel bytes */
    size_t data_size;          /* Total size of pixel array in bytes */
} BMPImage;

/* Function prototypes for BMP file I/O (implemented in src/bmp.c) */
BMPImage* bmp_open(const char *filename);
int bmp_save(const char *filename, const BMPImage *img);
void bmp_free(BMPImage *img);

/* Function prototypes for Steganography (implemented in src/stego.c) */
int stego_encode(BMPImage *img, const char *message);
char* stego_decode(const BMPImage *img);

#endif /* BMP_H */