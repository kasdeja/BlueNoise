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
    float *dots = new float[nPixels];
    float *falloff = new float[nPixels];
    unsigned char *data = new unsigned char[nPixels];
    
    for(int n = 0; n < nPixels; n++)
    {
        noise[n] = 0.0f;
        data[0] = 0.0f;
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
            double xx = x - 128;
            double yy = y - 128;
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

    for(int i = 0; i <= 65535; i++)
    {
        int dx = 0;
        int dy = 0;
        int idx = 0;
        float pMin = 655360000.0f;

        for(int y = 0; y < size; y++)
        {
            for(int x = 0; x < size; x++)
            {
                float v = noise[idx++];
                if (v < pMin && dots[pix(x, y)] == 0.0f)
                {
                    dx = x;
                    dy = y;
                    pMin = v;
                }
            }
        }

        printf("i %d dx %d dy %d\n", i, dx, dy);
        addFalloff(dx, dy, 0.1);
        dots[pix(dx, dy)] = (i + 0.00001) / 65535.0f;
    }


    // Normalize and save
    float *img = dots;
    float pMax = 0.0f;

    for(int n = 0; n < nPixels; n++)
    {
        if (img[n] > pMax)
            pMax = img[n];
    }

    float gain = 255.0f / pMax;

    for(int n = 0; n < nPixels; n++)
    {
        data[n] = floor(img[n] * gain + 0.49999);
    }

    FILE *file = ::fopen("test3.pgm", "wb");

    fprintf(file, "P5\n%d %d\n%d\n", size, size, 255);
    fwrite(data, 1, size * size, file);
    fclose(file);

    return 0;
}
