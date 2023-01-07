#include "iostream"
#include <mpi.h>
#include "CImg.h"
#include "pthread.h"
using namespace cimg_library;
using namespace std;

// parameters
#define MAXSIZE 10
#define image_res 512
#define threadNo 2

struct arg_struct
{
    int threadID;
    int processID;
} args[threadNo];

// images
CImg<unsigned char> image("house.tiff"), gray(image.width(), image.height(), 1, 1, 0), temp(image.width(), image.height(), 1, 1, 0);

// data
int data1[image_res][image_res];
int data2[image_res][image_res];

// windows
float topSobel[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
float bottomSobel[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
float emboss[3][3] = {{-2, -1, -0}, {-1, 1, 1}, {0, 1, 2}};
float blur[3][3] = {{0.0625, 0.125, 0.0625}, {0.125, 0.25, 0.125}, {0.0625, 0.125, 0.0625}};
float leftSobel[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
float rightSobel[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
float outline[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
float sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

int *filters[8] = {(int *)topSobel, (int *)bottomSobel, (int *)emboss, (int *)blur, (int *)leftSobel, (int *)rightSobel, (int *)outline, (int *)sharpen};
string filtersName[8] = {"Top Sobel", "Bottom Sobel", "Emboss", "Blur", "Left Sobel", "Right Sobel", "Outline", "Sharpen"};

void *slave(void *arguments)
{
    int threadId = (int)((arg_struct *)arguments)->threadID;
    int myid = (int)((arg_struct *)arguments)->processID;
    // cout << myid << endl;
    for (int q = (threadId * image_res) / (threadNo); q < (threadId * (image_res / threadNo)) + (image_res / threadNo); q++)
    {
        for (int k = 0; k < image.width(); k++)
        {
            int sum = 0;
            for (int i = k - 1; i < k + 2; i++)
            {
                for (int j = q - 1; j < q + 2; j++)
                {
                    if ((i >= 0 && i <= image.width()) && (j >= 0 && j <= image.height()))
                    {
                        if (myid == 0)
                        {
                            sum += ((int)bottomSobel[j - q + 1][i - k + 1] * ((int)data1[j][i]));
                        }
                        else
                            sum += ((int)(emboss[j - q + 1][i - k + 1] * ((int)data1[j][i])));
                    }
                }
            }
            data2[q][k] = sum;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    pthread_t thread[threadNo];
    if (myid == 0)
    {
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
    }
    if (myid == 0)
    {
        CImgDisplay main_disp(image, "Original"),
            draw_dispGr(gray, "Gray");
        while (!main_disp.is_closed())
        {
            main_disp.wait();
        }
    }
    // Bcast gray level image
    MPI_Bcast(data1, image.width() * image.height(), MPI_INT, 0, MPI_COMM_WORLD);
    int i;

    for (i = 0; i < threadNo; i++)
    {
        args[i].threadID = i;
        args[i].processID = myid;
        if (pthread_create(&thread[i], NULL, slave, (void *)&args[i]) != 0)
            perror("pthread_create fails");
    }
    for (i = 0; i < threadNo; i++)
        if (pthread_join(thread[i], NULL) != 0)
            perror("pthread_join fails");

    cimg_forXY(image, x, y)
    {
        temp(x, y, 0, 0) = (data2[y][x] >= 0) ? (int)((data2[y][x] >= 255) ? 255 : data2[y][x]) : 0;
    }

    CImgDisplay draw_disptemp(temp, (myid == 0) ? "Bottom Sobel" : "Emboss");

    while (!draw_disptemp.is_closed())
    {
        draw_disptemp.wait();
    }

    MPI_Finalize();

    return 0;
}