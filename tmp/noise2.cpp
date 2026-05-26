#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    constexpr int size = 256;
    constexpr int sizeMask = size - 1;
    constexpr int boxBlurSize = 8;
    constexpr int boxBlurSizeHalf = boxBlurSize / 2;
    constexpr float boxBlurNorm = 1.0f / boxBlurSize;

    int nPixels = size * size;

    float *dots = new float[nPixels];
    float *noise = new float[nPixels];
    float *bb1 = new float[nPixels];
    float *bb2 = new float[nPixels];
    unsigned char *data = new unsigned char[nPixels];
    
    for(int n = 0; n < nPixels; n++)
    {
        dots[n] = 0.0f;
        noise[n] = 0.0f;
        data[0] = 0.0f;
    }

    auto pix = [size, sizeMask](int x, int y) {
        return (x & sizeMask) + (y & sizeMask) * size;
    };

    dots[pix(size / 2, size / 2)] = 1.0f;

    auto bbHoriz = [&](float *in, float *out) {
        for(int y = 0; y < size; y++)
        {
            float *src = in + y * size;
            float *dst = out + y * size;
            float sum = 0.0f;

            for(int n = 0; n < boxBlurSize; n++)
            {
                sum += src[(n - boxBlurSizeHalf) & sizeMask];
            }

            for(int n = 0; n < size; n++)
            {
                dst[n] = sum * boxBlurNorm;
                sum += src[(n + boxBlurSizeHalf) & sizeMask];
                sum -= src[(n - boxBlurSizeHalf) & sizeMask];
            }
        }
    };

    auto bbVert = [&](float *in, float *out) {
        for(int x = 0; x < size; x++)
        {
            float *src = in + x;
            float *dst = out + x;
            float sum = 0.0f;

            for(int n = 0; n < boxBlurSize; n++)
            {
                sum += src[((n - boxBlurSizeHalf) & sizeMask) * size];
            }

            for(int n = 0; n < size; n++)
            {
                dst[n * size] = sum * boxBlurNorm;
                sum += src[((n + boxBlurSizeHalf) & sizeMask) * size];
                sum -= src[((n - boxBlurSizeHalf) & sizeMask) * size];
            }
        }
    };

    for(int ii = 0; ii < 64; ii++)
    {
    for(int i = 0; i < 1024; i++)
    {
        printf("ii %d i %d\n", ii, i);
        bbHoriz(dots, bb1);
        bbVert(bb1, bb2);
        bbHoriz(bb2, bb1);
        bbVert(bb1, bb2);
        bbHoriz(bb2, bb1);
        bbVert(bb1, bb2);
        bbHoriz(bb2, bb1);
        bbVert(bb1, bb2);
        bbHoriz(bb2, bb1);
        bbVert(bb1, noise);

        int idx = 0;
        int yy = -1;
        int xx = -1;
        float pMin = RAND_MAX;

        for(int y = 0; y < size; y++)
        {
            for(int x = 0; x < size; x++)
            {
                float v = noise[idx] + (float)random() / RAND_MAX * 0.0000001;
                //printf("v %f\n", v);
                if (v < pMin && dots[idx] == 0.0f)
                {
                    xx = x;
                    yy = y;
                    pMin = v;
                }
                idx++;
            }
        }
        printf("xx %d, yy %d\n", xx, yy);
        if (xx != -1 && yy != -1)
        {
            dots[pix(xx, yy)] = ii;//1.0f;
        }
    }
    }

    float pMax = 0.0f;

    for(int n = 0; n < nPixels; n++)
    {
        if (dots[n] > pMax)
            pMax = dots[n];
    }

    float gain = 255.0f / pMax;
    //float gain = 255.0f;

    for(int n = 0; n < nPixels; n++)
    {
        data[n] = floor(dots[n] * gain);
    }

    FILE *file = ::fopen("test2.pgm", "wb");

    fprintf(file, "P5\n%d %d\n%d\n", size, size, 255);
    fwrite(data, 1, size * size, file);
    fclose(file);

    return 0;
}
