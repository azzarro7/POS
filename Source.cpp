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

/*
*****JAK CHCECIE ODPALIC Z DYSKU PROGAM iniparse.exe TO: .\pos_iniparse_2.06\iniparse\Debug     I W TYM FOLDERZE MUSI BYC PLIK images.ini   MOZE ON BYC ZAPISANY JAKO NORMALNY TXT W ANSI
*****JAK CHCECIE DEUBUGOWAC TO PLIK images.ini MUSI BYC W: .\pos_iniparse_2.06\iniparse\iniparse
*****W PODAWANIU SCIEZEK DO PLIKU INI MUSZA BYC ZNAKI "slash" a nie "backslash", BO TAKI JEST VISUAL, ALE W SAMYM PLIKU INI W MOZNA PODAC SOBIE SCIEZKE Z slash
*/

vector<Mat> input;
vector<Mat> output;
Mat sklejanie1; //sklejenie obrazow przed przetwarzaniem
Mat sklejanie2; //sklejenie obraz�w po przetwarzaniu
char* Sklejanie_przed_window = "Sklejanie_przed";
char* Sklejanie_po_window = "Sklejanie_po";

int n = 0; //ilo�� wczytanych obrazow

//funkcja zwraca tablice ze sciezkami+liczbe sciezek
void wczytaj()
{

	char sciezka[100];

	//parsuje z pliku ini sciezki
	for (int i = 0; i <= 999; i++)
	{
		std::string filename = "img" + std::to_string(i);	//potrzebne zeby zrobic img1, img2....
		GetPrivateProfileString(TEXT("images"), TEXT(filename.c_str()), TEXT("0"), sciezka, 100, TEXT("../images.ini"));

		//w sekcji images szuka img1, img2... jezeli nie znajdzie np. img11 to wypisze "0"
		//plik ini moze byc zapisany jako zwykle ANSI txt, w podawaniu sciezek do pliku ini musza byc znaki "slash" a nie "backslash"
		//w takenstring jest zapisana dana sciezka, tzn kazdy znak jako char, wiec musze zapisac to jako pojedynczy string, a potem zrobic tablice tych stringow	
		//jezeli wystapilo zero (brak sciezki w pliku ini) to nie wpisuj go do tablicy

		if (*sciezka != '0')
		{
			input.push_back(imread(sciezka, 1));
			n++;
		}
	}
}


int equalize()
{
	Mat temp, temp1; //temp1 - tymczasowa zmienna przechowuj�ca obraz wyj�ciowy;
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

Mat sklejanie(vector<Mat> in, int n) //in-wektor obraz�w, n-ilo�� wczytanych obraz�w
{
	//okre�lenie ilo�ci wierszy i kolumn na podstawie liczby wczytywanych obraz�w
	int wie = ceil((double)n/2);
	int kol = ceil((double)n/wie);
	//obliczenie szeroko�ci i wysoko�ci poszczegolnych miniaturek na podstawie liczby wierszy i kolumn
	int s = 600. / kol;
	int w = 0.75*s;

	Mat sklej(wie*w, kol*s, CV_8UC3, CV_RGB(0, 0, 0)); //pusty obraz z czarnym t�em do sklejenia obrazow

	for (int i = 0; i<in.size(); i++) //p�tla sklejaj�ca ze sob� miniaturki obraz�w
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
	wczytaj();	//funkcja zwraca tablice ze sciezkami+liczbe sciezek

	equalize();

	sklejanie1 = sklejanie(input, n); //wywo�anie funkcji ��cz�cej obrazy
	sklejanie2 = sklejanie(output, n);

	//// wyswietlanie wynikow
	imshow(Sklejanie_przed_window, sklejanie1);
	imshow(Sklejanie_po_window, sklejanie2);

	//zapisywanie wynikow
	save();

	waitKey(0);

	return 0;
}
