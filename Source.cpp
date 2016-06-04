#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat sklejanie(vector<Mat> in, int n);

int equalize()
{
	Mat temp, temp1; //temp1 - tymczasowa zmienna przechowuj�ca obraz wyj�ciowy;
	vector<Mat> channels;
	vector<Mat> input;
	vector<Mat> output;

	char* source_window = "Obraz wejsciowy";
	char* equalized_window = "Obraz przetworzony";
	char* Sklejanie_przed_window = "Sklejanie_przed";
	char* Sklejanie_po_window = "Sklejanie_po";

	// ladowanie obrazow do wektora
	input.push_back(imread("1.jpg", 1));
	input.push_back(imread("2.jpg", 1));
	input.push_back(imread("3.jpg", 1));
	int n = 3; //ilo�� wczytanych obrazow

	if (!input[0].data)
	{
		cout << "blad: brak obrazow" << endl;
		return -1;
	}

	Mat sklejanie1; //sklejenie obrazow przed przetwarzaniem
	Mat sklejanie2; //sklejenie obraz�w po przetwarzaniu

	
	sklejanie1 = sklejanie(input, n); //wywo�anie funkcji ��cz�cej obrazy

	for (int i = 0; i < input.size(); i++)
	{
	// konwersja na format Y Cr Cb z obrazu input do temp
	cvtColor(input[i], temp, CV_BGR2YCrCb);

	// rozdzielanie obrazu temp na kanaly Y Cr Cb
	split(temp, channels);

	// wyrownanie pierwszego kanalu (Y) odpowiedzialnego za jasnosc
	equalizeHist(channels[0], channels[0]);

	// zlozenie kanalow na powrot do obrazu output
	merge(channels, temp);
	cvtColor(temp, temp1, CV_YCrCb2BGR);

	output.push_back(temp1); //wpisanie przetworzonego obrazu do wektora output

	}

	sklejanie2 = sklejanie(output, n);

	//// wyswietlanie wynikow
	imshow(Sklejanie_przed_window, sklejanie1);
	imshow(Sklejanie_po_window, sklejanie2);


	/// Wait until user exits the program
	waitKey(0);
	return 0;
}

Mat sklejanie(vector<Mat> in, int n) //in-wektor obraz�w, n-ilo�� wczytanych obraz�w
{
	//okre�lenie ilo�ci wierszy i kolumn
	int wie = 1;
	int kol = 3;
	//obliczenie szeroko�ci i wysoko�ci poszczegulnych miniaturek na podztawie ili�ci wierszy i kolumn
	int s = 600. / kol;
	int w = 0.75*s;

	Mat sklej(wie*w, kol*s, CV_8UC3, CV_RGB(0, 0, 0)); //pusty obraz z czarnym t�em do sklejenia obrazow

	for (int i = 0;i<in.size();i++) //p�tla sklejaj�ca ze sob� miniaturki obraz�w
	{
		//okre�lenie kolejnych punkt�w od kt�rych b�d� wstawiane miniaturki
		int x = i%kol*s;
		int y = i / kol*w;
		//tworzenie miniaturek i sklejanie ich ze sob�
		Mat roi = sklej(Rect(x, y, s, w));
		resize(in[i], roi, roi.size());
	}
	return sklej;
}

int main()
{
	equalize();
}
