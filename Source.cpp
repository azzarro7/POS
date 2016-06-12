#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include "./INIParser.h"
#include "conio.h"
#include <windows.h>
#include "string.h"
#include <cmath>
using namespace cv;
using namespace std;
using namespace cppiniparser;

vector<Mat> input;
vector<Mat> output;
Mat sklejanie1; //sklejenie obrazow przed przetwarzaniem
Mat sklejanie2; //sklejenie obrazów po przetwarzaniu
char* Sklejanie_przed_window = "Sklejanie_przed";
char* Sklejanie_po_window = "Sklejanie_po";

int n = 0; ///iloœæ wczytanych obrazow

char path_of_converted_imgs[100];	///zmienna, ktora przechowuje adres folderu w ktorym beda zapisane obrazy po przetworzeniu

/**
	\brief		funkcja zwraca tablice ze sciezkami, liczbe sciezek i adres folderu w ktorym beda zapisane obrazy po przetworzeniu
	\details	parsuje z pliku ini sciezki, plik ini moze byc zapisany jako zwykle ANSI txt, w podawaniu sciezek do pliku ini musza byc znaki "slash" a nie "backslash" czyli np. "../images.ini"
**/
void wczytaj()
{
	char sciezka[100];	///zmienna w ktorej jest przechowywana dana jedna odczytana sciezka
	for (int i = 0; i <= 999; i++)
	{
		std::string filename = "img" + std::to_string(i);	///potrzebne zeby zrobic img1, img2....
		GetPrivateProfileString(TEXT("images"), TEXT(filename.c_str()), TEXT("0"), sciezka, 100, TEXT("../images.ini"));	/// w sekcji [images] szuka sciezek img1, img2... je¿eli nie znajdzie juz wiecej "img" to zacznie zwracac zera

		///jezeli nie znajdzie juz wiecej sciezek to wypisze "0", a jak napotka "0" to nie wpisze go do tablicy
		if (*sciezka != '0')	///jezeli wystapilo zero (brak sciezki w pliku ini) to nie wpisuj go do tablicy
		{
			input.push_back(imread(sciezka, 1));	///zapisuje do input pobrane sciezki z pliku ini, tworzy tablice sciezek
			n++;	///iloœæ wczytanych obrazow
		}
	}
	
	GetPrivateProfileString(TEXT("converted_imgs"), TEXT("path"), TEXT("0"), path_of_converted_imgs, 100, TEXT("./images.ini"));	///do zmiennej path_of_converted_imgs przypisuje adres folderu w ktorym beda zapisane obrazy po przetworzeniu
	//printf(TEXT("%s\n"), path_of_converted_imgs);	///a to jest do wyswietlania sciezki folderu
	
	/// \return zwraca tablice ze sciezkami oraz adres folderu w ktorym beda zapisane obrazy po przetworzeniu
}


int equalize()
{
	Mat temp, temp1; //temp1 - tymczasowa zmienna przechowuj¹ca obraz wyjœciowy;
	vector<Mat> channels;

	if (!input[0].data)
	{
		cout << "blad: brak obrazow" << endl;
		return -1;
	}


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

	return 0;
}

Mat sklejanie(vector<Mat> in, int n) //in-wektor obrazów, n-iloœæ wczytanych obrazów
{
	//okreœlenie iloœci wierszy i kolumn na podstawie liczby wczytywanych obrazów
	int wie = ceil((double)n/2);
	int kol = ceil((double)n/wie);
	//obliczenie szerokoœci i wysokoœci poszczegolnych miniaturek na podstawie liczby wierszy i kolumn
	int s = 600. / kol;
	int w = 0.75*s;

	Mat sklej(wie*w, kol*s, CV_8UC3, CV_RGB(0, 0, 0)); //pusty obraz z czarnym t³em do sklejenia obrazow

	for (int i = 0; i<in.size(); i++) //pêtla sklejaj¹ca ze sob¹ miniaturki obrazów
	{
		//okreœlenie kolejnych punktów od których bêd¹ wstawiane miniaturki
		int x = i%kol*s;
		int y = i / kol*w;

		//tworzenie miniaturek i sklejanie ich ze sob¹
		Mat roi = sklej(Rect(x, y, s, w));
		resize(in[i], roi, roi.size());
	}
	return sklej;
}

int save()
{
	char saveDir[999];
	int bytes;

	for (int i = 0; i < input.size(); i++)
	{
		bytes = sprintf(saveDir, "../przetworzone/img_%i_przed.jpg", i);
		imwrite(saveDir, input[i]);
	}

	for (int i = 0; i < output.size(); i++)
	{
		bytes = sprintf(saveDir, "../przetworzone/img_%i_po.jpg", i);
		imwrite(saveDir, output[i]);
	}

	imwrite("../przetworzone/przed.jpg", sklejanie1);
	imwrite("../przetworzone/po.jpg", sklejanie2);
	return 0;
}

int main(int argc, char* argv[])
{
	wczytaj();	///funkcja zwraca tablice ze sciezkami, liczbe sciezek i adres folderu w ktorym beda zapisane obrazy po przetworzeniu

	equalize();

	sklejanie1 = sklejanie(input, n); //wywo³anie funkcji ³¹cz¹cej obrazy
	sklejanie2 = sklejanie(output, n);

	//// wyswietlanie wynikow
	imshow(Sklejanie_przed_window, sklejanie1);
	imshow(Sklejanie_po_window, sklejanie2);

	//zapisywanie wynikow
	save();

	waitKey(0);

	return 0;
}
