#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    constexpr int size = 256;
    constexpr int sizeHalf = size / 2;
    constexpr int sizeMask = size - 1;
    constexpr int boxBlurSize = 8;
    constexpr int boxBlurSizeHalf = boxBlurSize / 2;
    constexpr float boxBlurNorm = 1.0f / boxBlurSize;

    int nPixels = size * size;

    float *noise = new float[nPixels];
    float *falloff = new float[nPixels];
    int *dots = new int[nPixels];
    bool *dotsBitmap = new bool[nPixels];
    unsigned char *data = new unsigned char[nPixels];
    
    for(int n = 0; n < nPixels; n++)
    {
        noise[n] = 0.0f;
        dots[n] = 0;
        dotsBitmap[n] = false;
    }

    auto pix = [size, sizeMask](int x, int y) {
        return (x & sizeMask) + (y & sizeMask) * size;
    };

    for(int n = 0; n < nPixels; n++)
    {
        noise[n] = (1.0 + random()) / RAND_MAX / (1LL << 24);
    }

    for(int y = 0; y < size; y++)
    {
        for(int x = 0; x < size; x++)
        {
            double xx = x - sizeHalf;
            double yy = y - sizeHalf;
            double d  = sqrt(xx * xx + yy * yy);

            falloff[y * size + x] = exp(-d * 0.1);
        }
    }

    auto addFalloff = [&](int dx, int dy, float val) {
        for(int y = 0; y < size; y++)
        {
            float *src = falloff + ((y - dy + sizeHalf) & sizeMask) * size;
            float *dst = noise + y * size;

            for(int x = 0; x < size; x++)
            {
                dst[x] += src[(x - dx + sizeHalf) & sizeMask] * val;
            }
        }
    };

    for(int i = 0; i < nPixels; i++)
    {
        int dx = 0;
        int dy = 0;
        int idx = 0;
        float pMin = 655360000.0f;

        for(int y = 0; y < size; y++)
        {
            for(int x = 0; x < size; x++, idx++)
            {
                float v = noise[idx];
                if (v < pMin && dotsBitmap[idx] == false)
                {
                    dx = x;
                    dy = y;
                    pMin = v;
                }
            }
        }

        printf("i %d dx %d dy %d\n", i, dx, dy);
        addFalloff(dx, dy, 0.1);

        idx = pix(dx, dy);
        dots[idx] = i;
        dotsBitmap[idx] = true;
    }


    // Normalize and save
    // float *img = dots;
    // float pMax = 0.0f;
    //
    // for(int n = 0; n < nPixels; n++)
    // {
    //     if (img[n] > pMax)
    //         pMax = img[n];
    // }
    //
    // float gain = 255.0f / pMax;

    int black = 0;
    int white = 0;

    for(int n = 0; n < nPixels; n++)
    {
        // data[n] = floor(img[n] * gain);
        float v = pow(dots[n] / 65535.0f, 1.0 / 2.2);
        //float v = dots[n] / 65535.0f;

        // if (v < 0.5)
        // {
        //     data[n] = 255;
        //     white++;
        // }
        // else
        // {
        //     data[n] = 0;
        //     black++;
        // }
        data[n] = v * 255.0f;
    }

    printf("white %d black %d\n", white, black);

    FILE *file = ::fopen("test4.pgm", "wb");

    fprintf(file, "P5\n%d %d\n%d\n", size, size, 255);
    fwrite(data, 1, size * size, file);
    fclose(file);

    return 0;
}
