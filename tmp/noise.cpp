#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    float *noise = new float[65536];
    unsigned char *data = new unsigned char[65536];

    float xp = 0.0f;
    float yp = 0.0f;

    float a = 0.01;

    for(int n = 0; n < 65536; n++)
    {
        float x = (float)random() / RAND_MAX;
        float y = a * (yp + (x - xp));

        xp = x;
        yp = y;
    }

    for(int n = 0; n < 65536; n++)
    {
        float x = (float)random() / RAND_MAX;
        float y = a * (yp + (x - xp));

        if (y >= 0)
            noise[n] = sqrt(sqrt(y)) + y * 30;
        else
            noise[n] = -sqrt(sqrt(-y)) + y * 30;

        xp = x;
        yp = y;
    }

    float xmax = -100.0f;
    float xmin = 100.0f;

    for(int n = 0; n < 65536; n++)
    {
        float v = noise[n];
        if (v > xmax)
            xmax = v;
        if (v < xmin)
            xmin = v;
    }

    printf("xmin %f xmax %f\n", xmin, xmax);
    float gain = 255.0f / (xmax - xmin);
    float avg = 0.0f;

    for(int n = 0; n < 65536; n++)
    {
        noise[n] = (noise[n] - xmin) * gain;
        data[n] = floor(noise[n]);
        avg += noise[n];
    }

    avg /= 65536.0f;

    printf("avg %f\n", avg);


    FILE *file = ::fopen("test.pgm", "wb");

    fprintf(file, "P5\n%d %d\n%d\n", 256, 256, 255);
    fwrite(data, 1, 65536, file);
    fclose(file);

    return 0;
}
