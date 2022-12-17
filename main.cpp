#include "iostream"
#include <mpi.h>
#include "CImg.h"
#include "pthread.h";
using namespace cimg_library;
using namespace std;

// parameters
#define MAXSIZE 10
#define image_res 512
#define threadNo 2
// images
CImg<unsigned char> image("house.tiff"), gray(image.width(), image.height(), 1, 1, 0), temp(image.width(), image.height(), 1, 1, 0);
// data
int data[image_res][image_res];
int data2[image_res][image_res];
// windows
float windows[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
float windows2[3][3] = {{-2, -1, -0}, {-1, 1, 1}, {0, 1, 2}};

void slave(int threadId, int myid)
{
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
                            sum += (int)(windows[j - q + 1][i - k + 1] * ((int)data[j][i]));
                        else
                            sum += (int)(windows2[j - q + 1][i - k + 1] * ((int)data[j][i]));

                        // cout << (int)(windows[j - q + 1][i - k + 1] * ((int)gray(i, j, 0, 0))) << " +";
                    }
                }
            }
            // cout << sum << endl;
            data2[q][k] = sum;
        }
    }
}

int main(int argc, char **argv)
{
    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Status Status;
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
            data[y][x] = grayValue;
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
    MPI_Bcast(data, image.width() * image.height(), MPI_INT, 0, MPI_COMM_WORLD);
    int i;
    for (i = 0; i < threadNo; i++)
        if (pthread_create(&thread[i], NULL, slave, i, myid) != 0)
            perror("pthread_create fails");

    for (i = 0; i < threadNo; i++)
        if (pthread_join(thread[i], NULL) != 0)
            perror("pthread_join fails");

    cimg_forXY(image, x, y)
    {
        temp(x, y, 0, 0) = (data2[y][x] >= 0) ? (int)((data2[y][x] >= 255) ? 255 : data2[y][x]) : 0;
    }

    CImgDisplay draw_disptemp(temp, "temped");

    while (!draw_disptemp.is_closed())
    {
        draw_disptemp.wait();
    }

    MPI_Finalize();

    return 0;
}