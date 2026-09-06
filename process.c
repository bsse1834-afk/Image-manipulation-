#include <stdio.h>
#include <stdlib.h>
#include "process.h"

void apply_grayscale(Image *img) {
    if (!img) return;
    for (int i = 0; i < img->width * img->height; i++) {
        unsigned char gray = (unsigned char)(0.299 * img->pixels[i].r + 0.587 * img->pixels[i].g + 0.114 * img->pixels[i].b);
        img->pixels[i].r = gray;
        img->pixels[i].g = gray;
        img->pixels[i].b = gray;
    }
}

void adjust_brightness(Image *img, int factor) {
    if (!img) return;
    for (int i = 0; i < img->width * img->height; i++) {
        int r = img->pixels[i].r + factor;
        int g = img->pixels[i].g + factor;
        int b = img->pixels[i].b + factor;

        img->pixels[i].r = (r > 255) ? 255 : ((r < 0) ? 0 : r);
        img->pixels[i].g = (g > 255) ? 255 : ((g < 0) ? 0 : g);
        img->pixels[i].b = (b > 255) ? 255 : ((b < 0) ? 0 : b);
    }
}

void apply_invert(Image *img) {
    if (!img) return;
    for (int i = 0; i < img->width * img->height; i++) {
        img->pixels[i].r = 255 - img->pixels[i].r;
        img->pixels[i].g = 255 - img->pixels[i].g;
        img->pixels[i].b = 255 - img->pixels[i].b;
    }
}

void flip_horizontal(Image *img) {
    if (!img) return;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width / 2; x++) {
            int idx1 = y * img->width + x;
            int idx2 = y * img->width + (img->width - 1 - x);

            Pixel temp = img->pixels[idx1];
            img->pixels[idx1] = img->pixels[idx2];
            img->pixels[idx2] = temp;
        }
    }
}

void flip_vertical(Image *img) {
    if (!img) return;
    for (int y = 0; y < img->height / 2; y++) {
        for (int x = 0; x < img->width; x++) {
            int idx1 = y * img->width + x;
            int idx2 = (img->height - 1 - y) * img->width + x;

            Pixel temp = img->pixels[idx1];
            img->pixels[idx1] = img->pixels[idx2];
            img->pixels[idx2] = temp;
        }
    }
}

Image* rotate_90(Image *img) {
    if (!img) return NULL;
    Image *out = create_image(img->height, img->width);
    if (!out) return NULL;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int src_idx = y * img->width + x;
            int dst_x = img->height - 1 - y;
            int dst_y = x;
            int dst_idx = dst_y * out->width + dst_x;

            out->pixels[dst_idx] = img->pixels[src_idx];
        }
    }
    return out;
}

Image* crop_image(Image *img, int start_x, int start_y, int new_w, int new_h) {
    if (!img) return NULL;
    Image *out = create_image(new_w, new_h);
    if (!out) return NULL;

    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            int src_idx = (start_y + y) * img->width + (start_x + x);
            int dst_idx = y * new_w + x;
            out->pixels[dst_idx] = img->pixels[src_idx];
        }
    }
    return out;
}

Image* apply_blur(Image *img) {
    if (!img) return NULL;
    Image *out = create_image(img->width, img->height);
    if (!out) return NULL;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int sum_r = 0, sum_g = 0, sum_b = 0, count = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < img->width && ny >= 0 && ny < img->height) {
                        int idx = ny * img->width + nx;
                        sum_r += img->pixels[idx].r;
                        sum_g += img->pixels[idx].g;
                        sum_b += img->pixels[idx].b;
                        count++;
                    }
                }
            }

            int out_idx = y * img->width + x;
            out->pixels[out_idx].r = sum_r / count;
            out->pixels[out_idx].g = sum_g / count;
            out->pixels[out_idx].b = sum_b / count;
        }
    }
    return out;
}

Image* apply_sharpen(Image *img) {
    if (!img) return NULL;
    Image *out = create_image(img->width, img->height);
    if (!out) return NULL;

    int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int sum_r = 0, sum_g = 0, sum_b = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0) nx = 0;
                    if (nx >= img->width) nx = img->width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= img->height) ny = img->height - 1;

                    int idx = ny * img->width + nx;
                    int weight = kernel[dy + 1][dx + 1];

                    sum_r += img->pixels[idx].r * weight;
                    sum_g += img->pixels[idx].g * weight;
                    sum_b += img->pixels[idx].b * weight;
                }
            }

            int out_idx = y * img->width + x;
            out->pixels[out_idx].r = (sum_r > 255) ? 255 : ((sum_r < 0) ? 0 : sum_r);
            out->pixels[out_idx].g = (sum_g > 255) ? 255 : ((sum_g < 0) ? 0 : sum_g);
            out->pixels[out_idx].b = (sum_b > 255) ? 255 : ((sum_b < 0) ? 0 : sum_b);
        }
    }
    return out;
}
