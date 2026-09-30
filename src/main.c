#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *prog_name) {
    printf("============================================================\n");
    printf(" CryptoPix: 24-Bit BMP Image Steganography Engine in C\n");
    printf("============================================================\n");
    printf("Usage:\n");
    printf("  Encode: %s -e <input.bmp> <\"message\"> <output.bmp>\n", prog_name);
    printf("  Decode: %s -d <encoded.bmp>\n", prog_name);
    printf("============================================================\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* Encode Mode: ./cryptopix -e input.bmp "secret message" output.bmp */
    if (strcmp(argv[1], "-e") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Error: Invalid number of arguments for encoding.\n");
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        const char *input_file = argv[2];
        const char *message = argv[3];
        const char *output_file = argv[4];

        printf("[*] Loading carrier image: %s\n", input_file);
        BMPImage *img = bmp_open(input_file);
        if (!img) {
            return EXIT_FAILURE;
        }

        printf("[*] Embedding payload (%zu bytes required)...\n", (strlen(message) + 1) * 8);
        if (stego_encode(img, message) != 0) {
            bmp_free(img);
            return EXIT_FAILURE;
        }

        printf("[*] Saving stego-image: %s\n", output_file);
        if (bmp_save(output_file, img) != 0) {
            bmp_free(img);
            return EXIT_FAILURE;
        }

        bmp_free(img);
        printf("[+] Success: Message embedded and written to '%s'!\n", output_file);
        return EXIT_SUCCESS;
    }

    /* Decode Mode: ./cryptopix -d encoded.bmp */
    else if (strcmp(argv[1], "-d") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Error: Invalid number of arguments for decoding.\n");
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }

        const char *encoded_file = argv[2];

        printf("[*] Loading stego-image: %s\n", encoded_file);
        BMPImage *img = bmp_open(encoded_file);
        if (!img) {
            return EXIT_FAILURE;
        }

        printf("[*] Extracting hidden payload...\n");
        char *secret = stego_decode(img);
        if (!secret) {
            fprintf(stderr, "[-] No valid hidden message found or decoding failed.\n");
            bmp_free(img);
            return EXIT_FAILURE;
        }

        printf("\n================ RECOVERED MESSAGE ================\n");
        printf("%s\n", secret);
        printf("====================================================\n\n");

        free(secret);
        bmp_free(img);
        return EXIT_SUCCESS;
    }

    /* Invalid Flag */
    fprintf(stderr, "Error: Unknown option '%s'. Use -e or -d.\n", argv[1]);
    print_usage(argv[0]);
    return EXIT_FAILURE;

}