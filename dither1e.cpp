#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    constexpr int size = 256;
    constexpr int sizeMask = size - 1;

    constexpr int width = 1024;
    constexpr int height = 1200;
    constexpr int nPixels = width * height;

    int *blueNoise = new int[size * size];
    unsigned char *image = new unsigned char[nPixels * 3];

    FILE *file = ::fopen("test4.bin", "rb");
    fread(blueNoise, sizeof(int), size * size, file);
    fclose(file);

    for(int n = 0; n < 3 * nPixels; n++)
    {
        image[n] = 0;
    }

    auto drawPlainRect = [image, width, height](int x, int y, int w, int h, float r, float g, float b) {
        const int y2 = y + h;
        const unsigned char rr = r;
        const unsigned char gg = g;
        const unsigned char bb = b;

        for(int yy = y; yy < y2; yy++)
        {
            unsigned char *line = image + (width * yy + x) * 3;

            for(int xx = 0; xx < w; xx++)
            {
                line[0] = rr;
                line[1] = gg;
                line[2] = bb;
                line += 3;
            }
        }
    };

    auto drawDither2Rect = [image, width, height, blueNoise, size, sizeMask](int x, int y, int w, int h, float r, float g, float b) {
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
                int bn1 = blueNoise[(yy & sizeMask) * size + (xx & sizeMask)];
                int bn2 = blueNoise[(yy & sizeMask) * size + ((xx + 23) & sizeMask)];
                int bn3 = blueNoise[((yy + 47) & sizeMask) * size + (xx & sizeMask)];
                line[0] = bn1 < thR ? 255 : 0;
                line[1] = bn2 < thG ? 255 : 0;
                line[2] = bn3 < thB ? 255 : 0;
                line += 3;
            }
        }
    };

    for(int n = 0; n < 1004; n++)
    {
        float v = (n * 255.0f) / 1003.0f;

        printf("v %f\n", v);
        drawPlainRect(10 + n, 10, 1, 120, v, v / 2.0f, v / 4.0f);
        drawDither2Rect(10 + n, 130, 1, 120, v, v / 2.0f, v / 4.0f);

        drawPlainRect(10 + n, 310, 1, 120, v / 8.0f, v / 16.0f, v / 32.0f);
        drawDither2Rect(10 + n, 430, 1, 120, v / 8.0f, v / 16.0f, v / 32.0f);

        drawPlainRect(10 + n, 570, 1, 120, v / 2.0f, n / 4.0f, n / 8.0f);
        drawDither2Rect(10 + n, 690, 1, 120, v / 2.0f, n / 4.0f, n / 8.0f);

        drawPlainRect(10 + n, 860, 1, 120, v / 8.0f, n / 16.0f, n / 32.0f);
        drawDither2Rect(10 + n, 980, 1, 120, v / 8.0f, n / 16.0f, n / 32.0f);
    }

    FILE *file2 = ::fopen("image1e.ppm", "wb");

    fprintf(file2, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(image, 1, width * height * 3, file2);
    fclose(file2);

    return 0;
}
