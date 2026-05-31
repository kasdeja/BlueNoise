#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct DitherStep
{
    unsigned char c1;
    unsigned char c2;
    float l1;
    float idl;
};

int main(int argc, char **argv)
{
    constexpr int size = 256;
    constexpr int sizeMask = size - 1;

    constexpr int width = 1024;
    constexpr int height = 1760;
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

    auto srgb = [](float v) {
        if (v <= 0.04045)
            return v / 12.92;
        else
            return pow((v + 0.055) / 1.055, 2.4);
    };

    constexpr int nSteps = 16;
    DitherStep *steps = new DitherStep[256];

    for(int n = 0; n < nSteps; n++)
    {
        float v1 = floor((255.0 * n) / (nSteps - 1));
        float v2 = floor((255.0 * (n + 1)) / (nSteps - 1));
        float l1 = srgb(v1 / 255.0);
        float l2 = srgb(v2 / 255.0);

        float idl = 65536.0 / (l2 - l1);

        steps[n].c1 = v1;
        steps[n].c2 = v2;
        steps[n].l1 = l1;
        steps[n].idl = n < (nSteps - 1) ? idl : 0;

        printf("n %d c1 %d c2 %d l1 %f idl %f\n", steps[n].c1, steps[n].c2, steps[n].l1, steps[n].idl);
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

    auto drawDither2Rect = [image, width, height, blueNoise, size, sizeMask, srgb, steps, nSteps](int x, int y, int w, int h, int r, int g, int b) {
        const int y2 = y + h;
        const int x2 = x + w;

        float lr = srgb(r / 255.0f);
        float lg = srgb(g / 255.0f);
        float lb = srgb(b / 255.0f);

        DitherStep &sr = steps[(int)((r / 255.0) * (nSteps - 1))];
        DitherStep &sg = steps[(int)((g / 255.0) * (nSteps - 1))];
        DitherStep &sb = steps[(int)((b / 255.0) * (nSteps - 1))];

        int thR = (lr - sr.l1) * sr.idl;
        int thG = (lg - sg.l1) * sg.idl;
        int thB = (lb - sb.l1) * sb.idl;

        for(int yy = y; yy < y2; yy++)
        {
            unsigned char *line = image + (width * yy + x) * 3;

            for(int xx = x; xx < x2; xx++)
            {
                int bn = blueNoise[(yy & sizeMask) * size + (xx & sizeMask)];
                line[0] = bn < thR ? sr.c2 : sr.c1;
                line[1] = bn < thG ? sg.c2 : sg.c1;
                line[2] = bn < thB ? sb.c2 : sb.c1;
                line += 3;
            }
        }
    };

    for(int n = 0; n < 16; n++)
    {
        int v = (n * 255) / 15;
        printf("v %d\n", v);
        drawPlainRect(10 + 60 * n, 10, 60, 120, v, v / 2, v / 4);
        drawDither2Rect(10 + 60 * n, 130, 60, 120, v, v / 2, v / 4);

        drawPlainRect(10 + 60 * n, 310, 60, 120, n * 2, n, n / 2);
        drawDither2Rect(10 + 60 * n, 430, 60, 120, n * 2, n, n / 2);

        drawPlainRect(10 + 60 * n, 610, 60, 120, v / 2.0f, v * 0.9833f, v * 0.4917f); // 127.5 , 250.75 , 125.375
        drawDither2Rect(10 + 60 * n, 730, 60, 120, v / 2.0f, v * 0.9833f, v * 0.4917f);

        drawPlainRect(10 + 60 * n, 910, 60, 120, v / 8.0f, v * 0.2458f, v * 0.1229);
        drawDither2Rect(10 + 60 * n, 1030, 60, 120, v / 8.0f, v * 0.2458f, v * 0.1229);

        drawPlainRect(10 + 60 * n, 1210, 60, 120, v, v, v);
        drawDither2Rect(10 + 60 * n, 1330, 60, 120, v, v, v);

        drawPlainRect(10 + 60 * n, 1510, 60, 120, n * 2, n * 2, n * 2);
        drawDither2Rect(10 + 60 * n, 1630, 60, 120, n * 2, n * 2, n * 2);
    }

    FILE *file2 = ::fopen("image2b.ppm", "wb");

    fprintf(file2, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(image, 1, width * height * 3, file2);
    fclose(file2);

    return 0;
}
