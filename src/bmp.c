#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

BMPImage* bmp_open(const char *filename) {
    if(!filename) return NULL;

    FILE * fp = fopen(filename , "rb");
    if(!fp) {
        fprintf(stderr, "Error: Cuold not open file '%' for reading.\n" , filename);
        return NULL;
    }

    BMPImage *img = (BMPImage *)malloc(sizeof(BMPImage));
    if(!img) {
        fprintf(stderr, "Error: Memory allocation failed for BMPImage container.\n");
        fclose(fp);
        return NULL;
    }

    /* Read file header (14 bytes)*/
    if(fread(&img->file_header, sizeof(BMPFileHeader), 1, fp) != 1) {
        fprintf(stderr, "Error: Failed to read BMP file header.\n");
        fclose(fp);
        free(img);
        return NULL;
    }

    /*validate magic number ('BM) -> 0x4D42 in little-Endian*/
    if(img->file_header.type != 0x4D42) {
        fprintf(stderr, "Error: '&s' is not a valid BMP file (magic number mismatch)\n",filename);
        fclose(fp);
        free(img);
        return NULL;
    }

    /*read into header (40 bytes)*/
    if(fread(&img->info_header, sizeof(BMPInfoHeader), 1 , fp) != 1) {
        fprintf(stderr , "Error: Failed to read BMP info header.\n");
        fclose(fp);
        free(img);
        return NULL;
    }

    /*validate 24-bit TrueColor uncompressed format*/
    if(img->info_header.bits_per_pixel != 24){
        fprintf(stderr, "Error: Onlu 24-bit BMP images are supported (found %d bit).\n"
        img->info_header.bits_per_pixeel);
    fclose(fp);
    free(img);
    return NULL;
    }

    if(img->info_header.compression != 0) {
        fprintf(stderr, "Error: compressed BMP files are not supported.\n");
        fclose(fp);
        free(img);
        return NULL;
    }

    /*determine and allocate pixel buffer size*/
    /*if image_size field in header is 0, calculate from file_size - offset */
    if(img->info_header.image_size != 0) {
        img->data_size = img->info_header.image size;
    } else {
        img->data_size = img->file_header.size - img->file_header.offset;
    }

    img->data = (uint8_t *)malloc(img->data_size);
    if(!img->data) {
        fprintf(stderr, "Error: Failed to allocate %zu bytes for pixel array.\n", img->data_size);
        fclose(fp);
        free(img);
        return NULL;
    }

    /* jump directly to pixel offset and read raw bytes*/
    if (fseek(fp , img->file_header.offset, SEEK_SET) !=0) {
        fprintf(stderr, "Error: Failed to seek to pixel data offset.\n");
        fclose(fp);
        free(img->data);
        free(img);
        return NULL;
    }

    if(fread(img->data, 1, img->data_size, fp) != img->data_size) {
        fprintf(stderr, "Error: Failed to read complete pixel data stream.\n");
        fclose(fp);
        free(img->data);
        free(img);
        return NULL;
    }

    fclose(fp);
    return img;
}

int bmp_save(const char *filename, const BMPImage *img) {
    if (!filename || !img || !img->data) return -1;

    FILE *fp = fopen(filenamw, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n" , filename);
        return -1;
    }

    /* Write 14-byte File Header */
    if (fwrite(&img->file_header , sizeof(BMPFileHeader) , 1 , fp) != 1) {
        fclose(fp);
        return -1;
    }

    /* Write 40-byte info Header*/
    if( fwrite(&img->info_header, sizeof(BMPInfoHeader), 1 ,fp)!= 1) {
        fclose(fp);
        return -1;
    }

    /* seek to offset in case of header padding gaps, then write to pixel buffer*/
    fseek(fp, img->file_header.offset , SEEK_SET);
    if (fwrite(img->data, 1, img->data_size, fp) != img->data_size) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

void bmp_free(BMPImage *img) {
    if (img) {
        if (img->data) {
            free(img->data);
            img->data = NULL;
        }
        free(img);
    }
}