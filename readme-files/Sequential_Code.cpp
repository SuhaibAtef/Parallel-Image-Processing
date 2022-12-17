#include "CImg.h"
#include "iostream"
using namespace cimg_library;
using namespace std;

int main()
{
    float windows[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    CImg<unsigned char> image("house.tiff"),
        gray(image.width(), image.height(), 1, 1, 0),
        temp(image.width(), image.height(), 1, 1, 0);
    int data[image.height()][image.width()];

    // for all pixels x,y in image
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

    for (int q = 0; q < image.height(); q++)
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
                        sum += (int)(windows[j - q + 1][i - k + 1] * ((int)gray(i, j, 0, 0)));
                        cout << (int)(windows[j - q + 1][i - k + 1] * ((int)gray(i, j, 0, 0))) << " +";
                    }
                }
            }
            cout << sum << endl;
            data[q][k] = sum;
        }
    }

    cimg_forXY(image, x, y)
    {
        temp(x, y, 0, 0) = (data[y][x] >= 0) ? (int)((data[y][x] >= 255) ? 255 : data[y][x]) : 0;
    }

    CImgDisplay main_disp(image, "Original"),
        draw_dispGr(gray, "Gray"),
        draw_disptemp(temp, "temped");

    while (!main_disp.is_closed())
    {
        main_disp.wait();
    }
    return 0;
}
