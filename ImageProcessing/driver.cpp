#include <iostream>
#include <utility>
#include "Image.h"

using namespace std;

bool blackDetected (int pixel) {return pixel < 50;}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_file>" << endl;
        return 1;
    }

    // Read the image file name from the command line argument
    const char* imageFileName = argv[1];

    //load image + get rows/cols
	Image img(imageFileName); int rows = img.get_rows(), cols = img.get_cols();
    int bodiesOfBlack = 0;

    //store pixels that have already been checked
    bool CheckedPixels[rows][cols]{};
    
    //till end of matrix
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {

            //if black pixel found... store all connected pixels.
            if (CheckedPixels[i][j] == false and blackDetected ( (int)img[i][j])) {
                cout << "I enterered at " << i << " " << j 
                    << " and the clr here is " << (int)img[i][j] << endl;

                //add to checked pixels boolean matrix
                CheckedPixels[i][j] = true;

                //create queue to store all pixels to check
                queue<pair<int,int>> PixelsToCheck; 
                //init it with value where black pixel encountered
                PixelsToCheck.push(pair<int,int>(i, j));

                //store number of pixels to check
                int pixelCount = 1;

                //start iterative loop to check all pixels in vicinity
                while (!PixelsToCheck.empty()) {

                    //store first element in variable and then remove it from the queue
                    pair<int, int> pix = PixelsToCheck.front(); 
                    cout << "PIX: " << pix.first << " "<< pix.second << endl;
                    PixelsToCheck.pop(); pixelCount--;

                    cout << "Current no. of pixels to check: " << pixelCount << endl;
                   
                    //if current pixel is black
                    if (blackDetected((int)img[pix.first][pix.second])) {
                        // Check pixel above
                        pair<int, int> north(pix.first, pix.second - 1);
                        if (north.first >= 0 && north.first < cols && 
                            north.second >= 0 && north.second < rows &&
                            CheckedPixels[north.first][north.second] == false) {
                            cout << "north added //" << endl;
                            PixelsToCheck.push(north);
                            CheckedPixels[north.first][north.second] = true;
                            pixelCount++;
                        }

                        // Check pixel to the right
                        pair<int, int> east(pix.first + 1, pix.second);
                        if (east.first >= 0 && east.first < cols && 
                            east.second >= 0 && east.second < rows && 
                            !CheckedPixels[east.first][east.second]) {
                            cout << "east added //" << endl;
                            PixelsToCheck.push(east);
                            CheckedPixels[east.first][east.second] = true;
                            pixelCount++;
                        }

                        // Check pixel below
                        pair<int, int> south(pix.first, pix.second + 1);
                        if (south.first >= 0 && south.first < cols && 
                            south.second >= 0 && south.second < rows && 
                            !CheckedPixels[south.first][south.second]) {
                            cout << "south added //" << endl;
                            PixelsToCheck.push(south);
                            CheckedPixels[south.first][south.second] = true;
                            pixelCount++;
                        }

                        // Check pixel to the left
                        pair<int, int> west(pix.first - 1, pix.second);
                        if (west.first >= 0 && west.first < cols && 
                            west.second >= 0 && west.second < rows && 
                            !CheckedPixels[west.first][west.second]) {
                            cout << "west added //" << endl;
                            PixelsToCheck.push(west);
                            CheckedPixels[west.first][west.second] = true;
                            pixelCount++;
                        }
                    }
                }
                bodiesOfBlack += 1;
            }
        }
    }

    cout << "Bodies of black: " << bodiesOfBlack << endl;

    img.show();

    return 0;
}
