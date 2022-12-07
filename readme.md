# Parallel Image Processing using MPI and PThread

![](https://img.shields.io/badge/Lang-C%2B%2B-violet)
![](https://img.shields.io/badge/Team-4-green)
![](https://img.shields.io/badge/Course-CPE551-red)

## Purpose:

This is a project for the CPE551-Parallel Programming University Course in Jordan University of Science and Technology.

## Background:

Image Processing is one of the main subjects mentioned these days, where you have an input image and have a task to achieve such as clear noise, crop image and applying filters. Since Image Processing requires performing operations on each pixel in the image, the need for achieving a low computation time using faster computers with multi-core processors has risen.

## Idea:

We are trying to achieve image processing using MPI and PThread to utilize the usage of the multi-core processor.

## Process:

We have an input **images** and a specific **filter** that we have to apply to the images. The images are distributed between the processors using MPI so that every processor can apply the filter on the image and then return the result to the Master Processor. Moreover, each processor divides the image to multiple section and assigns each section to a predefined thread to apply the filter on.

![Image-Preview](readme-files/Image-Preview.png)

## How to run the code

1. First install ImageMagick Library using this [link](https://linuxhint.com/install-latest-image-magick-ubuntu-22-04/).

2. Clone the project.

```bash
$ git clone repo-link
```

3. Open folder.

```bash
$ cd repo-name/
```

4. Allow execution permission

```bash
$ chmod +x testing.sh
```

5. Run ./run.sh file.

```bash
$ ./run.sh
```

## Team

[@Abd Al-Aziz Abu Baker](https://github.com/Abdelaziz2096)

[@Feras Hamam](https://github.com/FerasHamam)

[@Rami Haider](https://github.com/Rami2025)

[@Suhaib Atef](https://github.com/SuhaibAtef)

## License

[MIT](https://choosealicense.com/licenses/mit/)
