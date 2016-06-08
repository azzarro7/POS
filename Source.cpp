#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include "./INIParser.h"
#include "conio.h"
#include <windows.h>
#include "string.h"
using namespace cv;
using namespace std;
using namespace cppiniparser;

/*
*****JAK CHCECIE ODPALIC Z DYSKU PROGAM iniparse.exe TO: .\pos_iniparse_2.06\iniparse\Debug     I W TYM FOLDERZE MUSI BYC PLIK images.ini   MOZE ON BYC ZAPISANY JAKO NORMALNY TXT W ANSI
*****JAK CHCECIE DEUBUGOWAC TO PLIK images.ini MUSI BYC W: .\pos_iniparse_2.06\iniparse\iniparse
*****W PODAWANIU SCIEZEK DO PLIKU INI MUSZA BYC ZNAKI "slash" a nie "backslash", BO TAKI JEST VISUAL, ALE W SAMYM PLIKU INI W MOZNA PODAC SOBIE SCIEZKE Z slash
*/

int i = 0, ileSciezek = 0;
string sciezki[999];
char takenstring[100];

//funkcja zwraca tablice ze sciezkami+liczbe sciezek
int wczytajSciezkiZini()
{
	ileSciezek = 0;
	//parsuje z pliku ini sciezki
	for (i = 0; i <= 999; i++)
	{
		std::string filename = "img" + std::to_string(i);	//potrzebne zeby zrobic img1, img2....
		GetPrivateProfileString(TEXT("images"), TEXT(filename.c_str()), TEXT("0"), takenstring, 100, TEXT("../images.ini"));	//w sekcji images szuka img1, img2... jezeli nie znajdzie np. img11 to wypisze "0"
																															//plik ini moze byc zapisany jako zwykle ANSI txt, w podawaniu sciezek do pliku ini musza byc znaki "slash" a nie "backslash"

																															//w takenstring jest zapisana dana sciezka, tzn kazdy znak jako char, wiec musze zapisac to jako pojedynczy string, a potem zrobic tablice tych stringow	
																															//jezeli wystapilo zero (brak sciezki w pliku ini) to nie wpisuj go do tablicy
		if (*takenstring != '0')
		{
			sciezki[i] = takenstring;
			ileSciezek++;
		}
	}
	return sciezki, ileSciezek;
}


Mat sklejanie(vector<Mat> in, int n);

int equalize()
{
	Mat temp, temp1; //temp1 - tymczasowa zmienna przechowuj¹ca obraz wyjœciowy;
	vector<Mat> channels;
	vector<Mat> input;
	vector<Mat> output;

	char* source_window = "Obraz wejsciowy";
	char* equalized_window = "Obraz przetworzony";
	char* Sklejanie_przed_window = "Sklejanie_przed";
	char* Sklejanie_po_window = "Sklejanie_po";

	// ladowanie obrazow do wektora
	input.push_back(imread("../1.jpg", 1));
	input.push_back(imread("../2.jpg", 1));
	input.push_back(imread("../3.jpg", 1));
	int n = 3; //iloœæ wczytanych obrazow

	if (!input[0].data)
	{
		cout << "blad: brak obrazow" << endl;
		return -1;
	}

	Mat sklejanie1; //sklejenie obrazow przed przetwarzaniem
	Mat sklejanie2; //sklejenie obrazów po przetwarzaniu

	
	sklejanie1 = sklejanie(input, n); //wywo³anie funkcji ³¹cz¹cej obrazy

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

Mat sklejanie(vector<Mat> in, int n) //in-wektor obrazów, n-iloœæ wczytanych obrazów
{
	//okreœlenie iloœci wierszy i kolumn
	int wie = 1;
	int kol = 3;
	//obliczenie szerokoœci i wysokoœci poszczegulnych miniaturek na podztawie iliœci wierszy i kolumn
	int s = 600. / kol;
	int w = 0.75*s;

	Mat sklej(wie*w, kol*s, CV_8UC3, CV_RGB(0, 0, 0)); //pusty obraz z czarnym t³em do sklejenia obrazow

	for (int i = 0;i<in.size();i++) //pêtla sklejaj¹ca ze sob¹ miniaturki obrazów
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

int main(int argc, char* argv[])
{
	wczytajSciezkiZini();	//funkcja zwraca tablice ze sciezkami+liczbe sciezek
							//wypisuje sciezki w konsoli jako sprawdzenie 
	for (i = 0; i < ileSciezek; i++)
	{
		printf(TEXT("%s\n"), sciezki[i].c_str());
	}

	//printf("liczba sciezek=%d",ileSciezek);
	//	cout << "Section: " << takenstring << endl;
	equalize();
	getchar();
	return 0;
}
