#include "CImg.h"
#include "iostream"
using namespace cimg_library;
using namespace std;
int main()
{
    float windows[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    CImg<unsigned char> image("house.tiff"),
        gray(image.width(), image.height(), 1, 1, 0),
        imgR(image.width(), image.height(), 1, 3, 0),
        imgG(image.width(), image.height(), 1, 3, 0),
        imgB(image.width(), image.height(), 1, 3, 0),
        temp(image.width(), image.height(), 1, 1, 0);
    // for all pixels x,y in image
    cimg_forXY(image, x, y)
    {

        imgR(x, y, 0, 0) = image(x, y, 0, 0),     // Red component of image sent to imgR
            imgG(x, y, 0, 1) = image(x, y, 0, 1), // Green component of image sent to imgG
            imgB(x, y, 0, 2) = image(x, y, 0, 2); // Blue component of image sent to imgB

        // Separation of channels
        int R = (int)image(x, y, 0, 0);
        int G = (int)image(x, y, 0, 1);
        int B = (int)image(x, y, 0, 2);
        // Arithmetic addition of channels for gray
        int grayValue = (int)(0.33 * R + 0.33 * G + 0.33 * B);
        // saving píxel values into image information
        gray(x, y, 0, 0) = grayValue;
    }

    cimg_forXY(image, x, y)
    {
        int sum = 0;
        for (int i = x - 1; i < x + 2; i++)
        {
            for (int j = y - 1; j < y + 2; j++)
            {
                if ((i >= 0 && i <= image.width()) && (j >= 0 && j <= image.height()))
                {
                    sum += (int)(windows[j - y + 1][i - x + 1] * ((int)gray(i, j, 0, 0)));
                    cout << (int)(windows[j - y + 1][i - x + 1] * ((int)gray(i, j, 0, 0))) << " +";
                }
            }
        }
        cout << sum << "value" << endl;
        temp(x, y, 0, 0) = (sum >= 0) ? (sum >= 255) ? 255 : sum : 0;
        cout << (int)temp(x, y, 0, 0) << " Result" << endl;
    }

    CImgDisplay main_disp(image, "Original"),
        draw_dispGr(gray, "Gray"),
        draw_disptemp(temp, "temped");
    // CImgDisplay main_disp(image, "Click a point");
    // CImgDisplay main_disp(image.channel(0), "Image");
    while (!main_disp.is_closed())
    {
        main_disp.wait();
    }
    return 0;
}
