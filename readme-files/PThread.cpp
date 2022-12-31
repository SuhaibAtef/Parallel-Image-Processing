#include "iostream"
#include "CImg.h"
#include "pthread.h"
using namespace cimg_library;
using namespace std;

// parameters
#define MAXSIZE 10
#define image_res 512
#define threadNo 2
// images
CImg<unsigned char> image("house.tiff"), gray(image.width(), image.height(), 1, 1, 0), temp1(image.width(), image.height(), 1, 1, 0), temp2(image.width(), image.height(), 1, 1, 0);
// data
int data1[image_res][image_res];
int data2[image_res][image_res];
int data3[image_res][image_res];

// windows
float windows[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
float windows2[3][3] = {{-2, -1, -0}, {-1, 1, 1}, {0, 1, 2}};

void *slave(void *i)
{
    int threadId = *((int *)i);
    cout << threadId << endl;
    for (int q = (threadId * image_res) / (threadNo); q < (threadId * (image_res / threadNo)) + (image_res / threadNo); q++)
    {
        for (int k = 0; k < image.width(); k++)
        {
            int sum0 = 0, sum1 = 0;
            for (int i = k - 1; i < k + 2; i++)
            {
                for (int j = q - 1; j < q + 2; j++)
                {
                    if ((i >= 0 && i <= image.width()) && (j >= 0 && j <= image.height()))
                    {

                        sum0 += (int)(windows[j - q + 1][i - k + 1] * ((int)data1[j][i]));

                        sum1 += (int)(windows2[j - q + 1][i - k + 1] * ((int)data1[j][i]));

                        // cout << (int)(windows[j - q + 1][i - k + 1] * ((int)gray(i, j, 0, 0))) << " +";
                    }
                }
            }
            // cout << sum << endl;
            data2[q][k] = sum0;
            data3[q][k] = sum1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    pthread_t thread[threadNo];
    cimg_forXY(image, x, y)
    {

        // Separation of channels
        int R = (int)image(x, y, 0, 0);
        int G = (int)image(x, y, 0, 1);
        int B = (int)image(x, y, 0, 2);
        // Arithmetic addition of channels for gray
        int grayValue = (int)(0.33 * R + 0.33 * G + 0.33 * B);
        // saving píxel values into image information
        gray(x, y, 0, 0) = grayValue;
        data1[y][x] = grayValue;
    }
    CImgDisplay main_disp(image, "Original"),
        draw_dispGr(gray, "Gray");
    while (!main_disp.is_closed())
    {
        main_disp.wait();
    }

    // Bcast gray level image
    int i;
    for (i = 0; i < threadNo; i++)
        if (pthread_create(&thread[i], NULL, slave, (void *)(&i)) != 0)
            perror("pthread_create fails");

    for (i = 0; i < threadNo; i++)
        if (pthread_join(thread[i], NULL) != 0)
            perror("pthread_join fails");

    cimg_forXY(image, x, y)
    {
        temp1(x, y, 0, 0) = (data2[y][x] >= 0) ? (int)((data2[y][x] >= 255) ? 255 : data2[y][x]) : 0;
        temp2(x, y, 0, 0) = (data3[y][x] >= 0) ? (int)((data3[y][x] >= 255) ? 255 : data3[y][x]) : 0;
    }

    CImgDisplay draw_disptemp(temp1, "Filter01");
    CImgDisplay draw_disptemp2(temp2, "Filter02");

    while (!draw_disptemp.is_closed())
    {
        draw_disptemp.wait();
    }

    return 0;
}