#ifndef SERVER_IMAGE_PROCESSING_H
#define SERVER_IMAGE_PROCESSING_H
#define THRESH 180

#include <utility>
#include <vector>
#include <queue>

using namespace std;

//todo?: Otsu's Algorithm
bool blackDetected(int pixel) { return pixel < THRESH; }

int charCounter(vector<vector<uint8_t>>& img, int rows, int cols) {
    int bodiesOfBlack = 0;

    //store pixels that have already been checked
    bool CheckedPixels[rows][cols]{};

    //till the matrix ends...
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            // If black pixel found... store all connected pixels.
            if (CheckedPixels[i][j] == false && blackDetected((int)img[i][j])) {
                bodiesOfBlack += 1;

                //create queue to store all pixels to check
                queue<pair<int, int>> PixelsToCheck;
                //init it with value where black pixel encountered
                PixelsToCheck.push(pair<int, int>(i, j));

                //start iterative loop to check all pixels in vicinity
                while (!PixelsToCheck.empty()) {

                    //store first element in variable and then remove it from the queue
                    pair<int, int> pix = PixelsToCheck.front();
                    PixelsToCheck.pop();

                    //if current pixel is black
                    if (blackDetected((int)img[pix.first][pix.second])) {

                        //add it to lists of checked pixels
                        CheckedPixels[pix.first][pix.second] = true;

                        //store pixels in cardinal directions
                        pair<int, int> neighbors[] = {
                            {pix.first - 1, pix.second}, //west
                            {pix.first, pix.second - 1}, //north
                            {pix.first, pix.second + 1}, //south
                            {pix.first + 1, pix.second}  //east
                        };

                        for (const auto& neighbor : neighbors) {
                            int x = neighbor.first;
                            int y = neighbor.second;
                            //if pixel is in bounds and not already encountered
                            if (x >= 0 && x < rows && y >= 0 && y < cols &&
                                !CheckedPixels[x][y]) {
                                //then add to queue and mark as checked
                                PixelsToCheck.push(neighbor);
                                CheckedPixels[x][y] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return bodiesOfBlack;
}

#endif
