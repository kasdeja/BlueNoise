#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    constexpr int size = 256;
    constexpr int sizeMask = size - 1;

    constexpr int width = 1024;
    constexpr int height = 1024;
    constexpr int nPixels = width * height;

    int *blueNoise = new int[size * size];
    unsigned char *image = new unsigned char[nPixels * 3];

    FILE *file = ::fopen("noise4.bin", "rb");
    fread(blueNoise, sizeof(int), size * size, file);
    fclose(file);

    for(int n = 0; n < 3 * nPixels; n++)
    {
        image[n] = 0;
    }

    auto drawPlainRect = [image, width, height](int x, int y, int w, int h, int r, int g, int b) {
        const int y2 = y + h;

        for(int yy = y; yy < y2; yy++)
        {
            unsigned char *line = image + (width * yy + x) * 3;

            for(int xx = 0; xx < w; xx++)
            {
                line[0] = r;
                line[1] = g;
                line[2] = b;
                line += 3;
            }
        }
    };

    auto drawDither2Rect = [image, width, height, blueNoise, size, sizeMask](int x, int y, int w, int h, int r, int g, int b) {
        const int y2 = y + h;
        const int x2 = x + w;

        auto srgb = [](float v) {
            if (v <= 0.04045)
                return v / 12.92;
            else
                return pow((v + 0.055) / 1.055, 2.4);
        };

        //int thR = pow(r / 255.0, 2.2) * 65535;
        //int thG = pow(g / 255.0, 2.2) * 65535;
        //int thB = pow(b / 255.0, 2.2) * 65535;

        int thR = srgb(r / 255.0) * 65535;
        int thG = srgb(g / 255.0) * 65535;
        int thB = srgb(b / 255.0) * 65535;

        for(int yy = y; yy < y2; yy++)
        {
            unsigned char *line = image + (width * yy + x) * 3;

            for(int xx = x; xx < x2; xx++)
            {
                int bn = blueNoise[(yy & sizeMask) * size + (xx & sizeMask)];
                line[0] = bn < thR ? 255 : 0;
                line[1] = bn < thG ? 255 : 0;;
                line[2] = bn < thB ? 255 : 0;;
                line += 3;
            }
        }
    };

    for(int n = 0; n < 16; n++)
    {
        int v = (n * 255) / 15;
        printf("v %d\n", v);
        drawPlainRect(10 + 60 * n, 10, 60, 120, v, v, v);
        drawDither2Rect(10 + 60 * n, 130, 60, 120, v, v, v);

        drawPlainRect(10 + 60 * n, 310, 60, 120, n * 2, n * 2, n * 2);
        drawDither2Rect(10 + 60 * n, 430, 60, 120, n * 2, n * 2, n * 2);
    }

    FILE *file2 = ::fopen("image1.ppm", "wb");

    fprintf(file2, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(image, 1, width * height * 3, file2);
    fclose(file2);

    return 0;
}
