#include <iostream>
#include <utility>
#include "Image.h"
#include <queue>

using namespace std;

bool blackDetected(int pixel) { return pixel < 60; }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_file>" << endl;
        return 1;
    }

    // Read the image file name from the command line argument
    const char* imageFileName = argv[1];

    // Load image + get rows/cols
    Image img(imageFileName);
    int rows = img.get_rows(), cols = img.get_cols();
    int bodiesOfBlack = 0;

    // Store pixels that have already been checked
    bool CheckedPixels[rows][cols]{};

    // Iterate through the image
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            // If black pixel found... store all connected pixels.
            if (CheckedPixels[i][j] == false && blackDetected((int)img[i][j])) {
                bodiesOfBlack += 1;

                queue<pair<int, int>> PixelsToCheck; // Declare PixelsToCheck queue
                PixelsToCheck.push(pair<int, int>(i, j));

                while (!PixelsToCheck.empty()) {
                    pair<int, int> pix = PixelsToCheck.front();
                    PixelsToCheck.pop();

                    if (blackDetected((int)img[pix.first][pix.second])) {
                        CheckedPixels[pix.first][pix.second] = true;

                        // Explore neighboring pixels
                        pair<int, int> neighbors[] = {
                            {pix.first - 1, pix.second}, // left
                            {pix.first, pix.second - 1}, // above
                            {pix.first, pix.second + 1}, // below
                            {pix.first + 1, pix.second}  // right
                        };

                        for (const auto& neighbor : neighbors) {
                            int x = neighbor.first;
                            int y = neighbor.second;
                            if (x >= 0 && x < rows && y >= 0 && y < cols &&
                                !CheckedPixels[x][y]) {
                                PixelsToCheck.push(neighbor);
                                CheckedPixels[x][y] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "Bodies of black: " << bodiesOfBlack << endl;

    img.show();

    return 0;
}
