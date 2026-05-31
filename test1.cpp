#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{
    constexpr int steps = 8;

    auto srgb = [](float v) {
        if (v <= 0.04045)
            return v / 12.92;
        else
            return pow((v + 0.055) / 1.055, 2.4);
    };

    float pl = 0.0f;

    for(int n = 0; n < steps; n++)
    {
        float v = (255.0 * n) / (steps - 1);
        float vv = floor(v);
        float l = srgb(vv / 255.0);

        printf("n %d v %f vv %f l %f dl %f\n", n, v, vv, l, l - pl);
        pl = l;
    }

    printf("\n");

    for(int n = 0; n < steps; n++)
    {
        float v1 = floor((255.0 * n) / (steps - 1));
        float v2 = floor((255.0 * (n + 1)) / (steps - 1));
        float l1 = srgb(v1 / 255.0);
        float l2 = srgb(v2 / 255.0);

        printf("n %d v1 %f v2 %f l %f dl %f\n", n, v1, v2, l1, l2 - l1);
    }


    // printf("\n");
    //
    // for(int n = 0; n < 256; n++)
    // {
    //     float v = (n / 255.0) * (steps - 1);
    //     float vv = floor(v);
    //
    //     printf("n %d v %f vv %f\n", n, v, vv);
    // }

    return 0;
}
