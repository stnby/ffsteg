#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

typedef struct {
    char sig[8];
    uint32_t width;
    uint32_t height;
} header_t;

typedef struct {
    uint16_t r, g, b, a;
} pixel_t;

uint16_t encode_bit(uint16_t color, bool bit) {
    if (color % 2 == bit)
        return color;
    color > 0 ? color-- : color++;
    return color;
}

class image {
private:
    header_t header;
    pixel_t *pixel;
    uint64_t img_len;
public:
    void read_img() {
        if (fread(&header, 1, sizeof(header_t), stdin) != sizeof(header_t)) {
            fputs("incomplete header\n", stderr);
            exit(1);
        }
        if (memcmp("farbfeld", header.sig, strlen("farbfeld"))) {
            fputs("invalid magic\n", stderr);
            exit(1);
        }
        header.width = ntohl(header.width);
        header.height = ntohl(header.height);
        img_len = (uint64_t)header.width * header.height;
        pixel = (pixel_t*)malloc(img_len * sizeof(pixel_t));
        if (!pixel) {
            fputs("unable to allocate memory.\n", stderr);
            exit(1);
        }
        if (fread(pixel, sizeof(pixel_t), img_len, stdin) != img_len) {
            fputs("unexpected EOF\n", stderr);
            exit(1);
        }
        for (size_t i = 0; i < img_len; i++) {
            pixel[i].r = ntohs(pixel[i].r);
            pixel[i].g = ntohs(pixel[i].g);
            pixel[i].b = ntohs(pixel[i].b);
            pixel[i].a = ntohs(pixel[i].a);
        }
    }
    void write_img() {
        header.width = htonl(header.width);
        header.height = htonl(header.height);
        if (fwrite(&header, sizeof(header_t), 1, stdout) != 1) {
            fputs("unable to write header\n", stderr);
            exit(1);
        }
        for (size_t i = 0; i < img_len; i++) {
            pixel[i].r = htons(pixel[i].r);
            pixel[i].g = htons(pixel[i].g);
            pixel[i].b = htons(pixel[i].b);
            pixel[i].a = htons(pixel[i].a);
        }
        if (fwrite(pixel, sizeof(pixel_t), img_len, stdout) != img_len) {
            fputs("write error\n", stderr);
            exit(1);
        }
    }
    unsigned encode_msg(const char* str) {
        unsigned ret = 0;
        uint64_t pixel_pos;
        bool occupied[img_len], bit;
        memset(occupied, 0, img_len);
        srand48(strlen(str)); // encode message with message length as a seed.
        for (unsigned i = 0; str[i] != '\0'; i++) {
            if (str[i] == '0')
                bit = 0;
            else if (str[i] == '1')
                bit = 1;
            else
                continue; // skip every character that is not '0' or '1'.
            do { // pseudo-randomly find unoccupied pixel.
                pixel_pos = lrand48() % img_len;
            } while (occupied[pixel_pos]);
            occupied[pixel_pos] = 1; // and mark it as occupied.
            uint16_t *color = (uint16_t*)&pixel[pixel_pos];
            int channel = lrand48() % 3; // pseudo-randomly choose color channel (skip alpha channel).
            color[channel] = encode_bit(color[channel], bit);
            ret++;
        }
        return ret; // return encoded message length.
    }
    void decode_msg(unsigned msg_len) {
        uint64_t pixel_pos;
        bool occupied[img_len];
        memset(occupied, 0, img_len);
        srand48(msg_len);
        for (unsigned i = 0; i < msg_len; i++) {
            do { // pseudo-randomly find unoccupied pixel.
                pixel_pos = lrand48() % img_len;
            } while (occupied[pixel_pos]);
            occupied[pixel_pos] = 1; // and mark it as occupied.
            uint16_t *color = (uint16_t*)&pixel[pixel_pos];
            int channel = lrand48() % 3; // pseudo-randomly choose color channel (skip alpha channel).
            printf("%d", color[channel] % 2); // read encoded bit.
        }
        puts("");
    }
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage:\n\t%1$s enc [bintext]\n\t%1$s dec [binlen]\n", argv[0]);
        return 1;
    }
    image img;
    img.read_img(); // read farbfeld image from stdin.
    if (!strcmp(argv[1], "enc")) {
        // output to stderr because we are outputing farbfeld image data to stdout.
        fprintf(stderr, "encoded: %u bits\n", img.encode_msg(argv[2]));
        img.write_img();
    }
    else if (!strcmp(argv[1], "dec"))
        img.decode_msg(atoi(argv[2])); // decode message with message length as a seed.
    return 0;
}