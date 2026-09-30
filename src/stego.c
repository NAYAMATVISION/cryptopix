#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Embeds a null-terminated string into the LSB of the image's pixel bytes.
 * Returns 0 on success, or -1 if the image capacity is too small.
 */
int stego_encode(BMPImage *img, const char *message) {
    if (!img || !img->data || !message) return -1;

    size_t msg_len = strlen(message) + 1; /* Include '\0' terminator */
    size_t bits_needed = msg_len * 8;

    /* Check if image has enough pixel bytes to hold all required bits */
    if (bits_needed > img->data_size) {
        fprintf(stderr, "Error: Message too long. Needed %zu bytes, but image capacity is %zu bytes.\n",
                bits_needed, img->data_size);
        return -1;
    }

    size_t byte_idx = 0;

    for (size_t i = 0; i < msg_len; i++) {
        unsigned char ch = (unsigned char)message[i];

        /* Extract bits from Most Significant (bit 7) to Least Significant (bit 0) */
        for (int bit = 7; bit >= 0; bit--) {
            int secret_bit = (ch >> bit) & 1;

            /* Clear the carrier byte's LSB with mask 0xFE (11111110), then OR the secret bit */
            img->data[byte_idx] = (img->data[byte_idx] & 0xFE) | secret_bit;
            byte_idx++;
        }
    }

    return 0;
}

/*
 * Extracts the hidden null-terminated message from the image.
 * Returns a dynamically allocated string on success (caller must free it), or NULL on failure.
 */
char* stego_decode(const BMPImage *img) {
    if (!img || !img->data) return NULL;

    size_t max_chars = img->data_size / 8;
    if (max_chars == 0) return NULL;

    /* Allocate buffer for recovered message */
    char *message = (char *)malloc(max_chars + 1);
    if (!message) {
        fprintf(stderr, "Error: Failed to allocate memory for decoded message.\n");
        return NULL;
    }

    size_t byte_idx = 0;
    size_t char_count = 0;

    while (byte_idx + 8 <= img->data_size) {
        unsigned char ch = 0;

        /* Reassemble 8 bits into a single byte */
        for (int bit = 7; bit >= 0; bit--) {
            int extracted_bit = img->data[byte_idx] & 1;
            ch |= (extracted_bit << bit);
            byte_idx++;
        }

        message[char_count++] = (char)ch;

        /* If we hit the null terminator, message extraction is complete */
        if (ch == '\0') {
            return message;
        }
    }

    /* If no null terminator was found, treat as no valid hidden message */
    free(message);
    return NULL;
}