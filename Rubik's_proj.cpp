#include <iostream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;


void calibrate() {
  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cerr << "Error: Could not open camera." << endl;
    return;
  }

  cap.set(CAP_PROP_FRAME_WIDTH, 1280);
  cap.set(CAP_PROP_FRAME_HEIGHT, 720);

  Mat frame;

  int roiWidth = 300;
  int roiHeight = 300;
  int roiX = (cap.get(CAP_PROP_FRAME_WIDTH) - roiWidth) / 2;
  int roiY = (cap.get(CAP_PROP_FRAME_HEIGHT) - roiHeight) / 2;

  string colorNames[6] = {"White", "Yellow", "Red", "Blue", "Orange", "Green"};

  for (int i = 0; i < 6; i++) {
    cout << "Place the " << colorNames[i] << " face in the ROI and press SPACE."
         << endl;
    while (true) {
      cap >> frame;
      if (frame.empty()) {
        cerr << "Error: Could not read frame." << endl;
        return;
      }

      rectangle(frame, Rect(roiX, roiY, roiWidth, roiHeight), Scalar(0, 0, 0), 4);
      imshow("Calibration", frame);

      if (waitKey(1) == 32) { // Spacebar
        Mat face = frame(Rect(roiX, roiY, roiWidth, roiHeight)).clone();

        vector<int> hValues, sValues, vValues;
        
        for (int y = 0; y < face.rows; y++) {
          for (int x = 0; x < face.cols; x++) {
            Vec3b pixel = face.at<Vec3b>(y, x);
            float b = pixel[0] / 255.0f;
            float g = pixel[1] / 255.0f;
            float r = pixel[2] / 255.0f;

            float v = max(max(r, g), b);
            float s = v == 0 ? 0 : (v - min(min(r, g), b)) / v;
            float h = 0;
            if (v != 0) {
              float delta = v - min(min(r, g), b);
              if (delta != 0) {
                if (r == v)
                  h = (g - b) / delta;
                else if (g == v)
                  h = 2 + (b - r) / delta;
                else
                  h = 4 + (r - g) / delta;
                h *= 60;
                if (h < 0) h += 360;
              }
            }

            hValues.push_back(h);
            sValues.push_back(s * 255);
            vValues.push_back(v * 255);
          }
        }

        sort(hValues.begin(), hValues.end());
        sort(sValues.begin(), sValues.end());
        sort(vValues.begin(), vValues.end());
        Scalar medianColor = Scalar(
            hValues[hValues.size() / 2], 
            sValues[sValues.size() / 2], 
            vValues[vValues.size() / 2]
        );

        cout << colorNames[i] << ": " << medianColor << endl;
        break;
      }
      if (waitKey(1) == 27) {  // 'Esc' key
        return;
      }
    }
  }

  cap.release();
  destroyAllWindows();
}


void proj() {
  Scalar refColors[6] = {
    Scalar(23, 31, 215), // White
    Scalar(62, 181, 221), // Yellow
    Scalar(356, 200, 233), // Red
    Scalar(231, 165, 166), // Blue
    Scalar(10, 188, 254), // Orange
    Scalar(137, 228, 192)  // Green
  };

  Scalar rgbColors[6] = {
      Scalar(255, 255, 255), // White
      Scalar(0, 255, 255), // Yellow
      Scalar(0, 0, 255), // Red
      Scalar(255, 0, 0), // Blue
      Scalar(0, 165, 255), // Orange
      Scalar(0, 255, 0) // Green
  };

  string colorNames[6] = {"White", "Yellow", "Red", "Blue", "Orange", "Green"};

  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cerr << "Error: Could not open camera." << endl;
    return;
  }

  cap.set(CAP_PROP_FRAME_WIDTH, 1280);
  cap.set(CAP_PROP_FRAME_HEIGHT, 720);

  Mat frame;

  int roiWidth = 300;
  int roiHeight = 300;
  int roiX = (cap.get(CAP_PROP_FRAME_WIDTH) - roiWidth) / 2;
  int roiY = (cap.get(CAP_PROP_FRAME_HEIGHT) - roiHeight) / 2;

  while (true) {
    cap >> frame;
    if (frame.empty()) {
      cerr << "Error: Could not read frame." << endl;
      break;
    }

    rectangle(frame, Rect(roiX, roiY, roiWidth, roiHeight), Scalar(0, 0, 0), 4);
    putText(frame, "Place cube here, press SPACE to start.",
            Point(roiX, roiY + 330), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0),
            1);

    imshow("Rubik's Cube Face Detection", frame);

    if (waitKey(1) == 32) {  // Spacebar
      vector<string> cubeColors;

      Mat face = frame(Rect(roiX, roiY, roiWidth, roiHeight)).clone();

      int squareSize = roiWidth / 3;

      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          int x = j * squareSize;
          int y = i * squareSize;
          Mat square = face(Rect(x, y, squareSize, squareSize));

          // imshow("Square " + to_string(i * 3 + j), square);

          vector<int> hValues, sValues, vValues;

          for (int y = 0; y < square.rows; y++) {
            for (int x = 0; x < square.cols; x++) {
              Vec3b pixel = square.at<Vec3b>(y, x);
              float b = pixel[0] / 255.0f;
              float g = pixel[1] / 255.0f;
              float r = pixel[2] / 255.0f;

              float v = max(max(r, g), b);
              float s = v == 0 ? 0 : (v - min(min(r, g), b)) / v;
              float h = 0;
              if (v != 0) {
                float delta = v - min(min(r, g), b);
                if (delta != 0) {
                  if (r == v)
                    h = (g - b) / delta;
                  else if (g == v)
                    h = 2 + (b - r) / delta;
                  else
                    h = 4 + (r - g) / delta;
                  h *= 60;
                  if (h < 0) h += 360;
                }
              }

              hValues.push_back(h);
              sValues.push_back(s * 255);
              vValues.push_back(v * 255);
            }
          }

          sort(hValues.begin(), hValues.end());
          sort(sValues.begin(), sValues.end());
          sort(vValues.begin(), vValues.end());
          Scalar medianColor = Scalar(hValues[hValues.size() / 2],
                                      sValues[sValues.size() / 2],
                                      vValues[vValues.size() / 2]);

          double minDist = numeric_limits<double>::max();
          int closestColorIndex = -1;

          for (int k = 0; k < 6; k++) {
            double wH = 0.6;
            double wS = 0.3;
            double wV = 0.1;

            double hueDiff = min(abs(medianColor[0] - refColors[k][0]),
                                 360 - abs(medianColor[0] - refColors[k][0]));

            double dist = wH * pow(hueDiff, 2) +
                          wS * pow(medianColor[1] - refColors[k][1], 2) +
                          wV * pow(medianColor[2] - refColors[k][2], 2);

            if (dist < minDist) {
              minDist = dist;
              closestColorIndex = k;
            }
          }

          cubeColors.push_back(colorNames[closestColorIndex]);
          cout << "Square " << (i * 3 + j) << ": " << colorNames[closestColorIndex] << endl;
          rectangle(face, Rect(x, y, squareSize, squareSize), rgbColors[closestColorIndex], FILLED);
        }
      }
      cout <<  endl;
      imshow("Detected Face", face);
      imageutil::macOsWaitKey();
    }

    if (waitKey(1) == 27) {  // 'Esc' key
      break;
    }
  }

  cap.release();
}


int main() {
 
  // calibrate();
  
  proj();

  destroyAllWindows();

  return 0;
}