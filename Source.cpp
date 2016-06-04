#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int equalize()
{
	Mat input, output, temp;
	vector<Mat> channels;
	

	char* source_window = "Obraz wejsciowy";
	char* equalized_window = "Obraz przetworzony";

	// ladowanie obrazu
	input = imread("1.jpg", 1);

	if (!input.data)
	{
		cout << "Usage: ./Histogram_Demo <path_to_image>" << endl;
		return -1;
	}

	// konwersja na format Y Cr Cb z obrazu input do temp
	cvtColor(input, temp, CV_BGR2YCrCb);

	// rozdzielanie obrazu temp na kanaly Y Cr Cb
	split(temp, channels);

	// wyrownanie pierwszego kanalu (Y) odpowiedzialnego za jasnosc
	equalizeHist(channels[0], channels[0]);

	// zlozenie kanalow na powrot do obrazu output
	merge(channels, temp);
	cvtColor(temp, output, CV_YCrCb2BGR);
	// wyswietlanie wynikow
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);

	imshow(source_window, input);
	imshow(equalized_window, output);

	/// Wait until user exits the program
	waitKey(0);
	return 0;
}

int main()
{
	equalize();
}
