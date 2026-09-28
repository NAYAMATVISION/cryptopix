# CryptoPix

A lightweight C program that hides secret text messages inside 24-bit BMP images using Least Significant Bit (LSB) steganography—without noticeably changing how the image looks.

---

## What is this?

Steganography is the practice of hiding information in plain sight. Unlike encryption (which scrambles a message so nobody can read it), steganography hides the fact that a message even exists.

This project takes a standard, uncompressed `.bmp` picture, tucks the bits of a secret message into the image's pixel colors, and saves a new image. To your eyes, the original and modified images look identical. But with this program, you can extract the hidden text anytime.

---

## Why I Built This

I chose this project to get hands-on experience with concepts that usually stay theoretical in class:
* **Direct byte manipulation:** Reading and writing raw binary streams using `fread` and `fwrite`.
* **Bitwise operations in practice:** Using shifts (`<<`, `>>`), masks (`&`), and bitwise OR (`|`) to store bits inside individual color channels.
* **Hardware alignment & struct packing:** Learning why `#pragma pack(1)` is necessary to keep C compilers from messing up binary file headers with extra padding bytes.
* **Standard Linux workflow:** Writing modular code across `src/` and `include/`, automating builds with a `Makefile`, and tracking clean commits with Git.

---

## How It Works

### 1. The BMP Image Layout
A 24-bit BMP file is straightforward because it is uncompressed:
* **First 54 bytes:** File metadata (image width, height, color depth, file size). We copy this header untouched so the file stays a valid, openable image.
* **Remaining bytes:** The actual pixel array, stored as groups of 3 bytes for every pixel: Blue, Green, and Red (BGR).

### 2. The LSB Trick
Every character in your message is 8 bits (1 byte). For example, the letter `'A'` is `01000001` in binary.

We spread those 8 bits across 8 color bytes in the image:
1. We read an image byte (say, a red value of `200`, which is `11001000`).
2. We clear its last bit using `& 0xFE`.
3. We slip in 1 bit of our secret letter using `| bit`.
4. If the color was `200`, it might become `201`. 

Because a 1-unit change in a 0–255 color spectrum is invisible to the human eye, the image appears completely unchanged. When decoding, the program just reads the last bit of every byte, stitches 8 bits back into a character, and stops when it hits `'\0'`.

---

## Project Structure

```text
cryptopix/
├── include/
│   └── bmp.h          # BMP headers and function prototypes
├── src/
│   ├── main.c         # CLI handling and user input
│   ├── bmp.c          # Image loading, validation, and saving
│   └── stego.c        # Bit-level hiding and recovery logic
├── Makefile           # Automated build script
├── .gitignore         # Build artifacts and editor ignore list
├── LICENSE            # MIT License
└── README.md          # Project proposal and notes