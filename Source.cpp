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
/// \brief Wektor do ktorego wczytywane sa obrazy wejsciowe
vector<Mat> input; 
/// \brief Wektor do ktorego zrzucane sa obrazy po przetworzniu 
vector<Mat> output; 
/// \brief Zmienna przechowujaca sklejone obrazy przed przetwarzaniem
Mat sklejanie1; 
/// \brief Zmienna przechowujaca sklejone obrazy po przetwarzaniu
Mat sklejanie2; 
/// \brief Nazwa okna, w ktorym pokazywane sa obrazy przed przetworzeniem
char* Sklejanie_przed_window = "Sklejanie_przed";
/// \brief Nazwa okna w ktorym pokazywane sa obrazy przetworzone
char* Sklejanie_po_window = "Sklejanie_po"; 
/// \brief Ilosc wczytanych obrazow
int n = 0; 
/// \brief Zmienna, ktora przechowuje adres folderu w ktorym beda zapisane obrazy po przetworzeniu
char path_of_converted_imgs[100];	

/**
	\brief		Funkcja zwraca tablice ze sciezkami, liczbe sciezek i adres folderu w ktorym beda zapisane obrazy po przetworzeniu
	\details	Parsuje z pliku ini sciezki, plik ini moze byc zapisany jako zwykle ANSI txt, w podawaniu sciezek do pliku ini musza byc znaki "slash" a nie "backslash" czyli np. "../images.ini"
**/
void wczytaj()
{
	char sciezka[100];	/// \param sciezka[100] zmienna w ktorej jest przechowywana dana jedna sciezka odczytana w kazdym korku petli poprzez funkcje GetPrivateProfileString
	for (int i = 0; i <= 999; i++) /// for (int i = 0; i <= 999; i++) -petla wczytujaca sciezki do tablicy
	{
		std::string filename = "img" + std::to_string(i);	
		GetPrivateProfileString(TEXT("images"), TEXT(filename.c_str()), TEXT("0"), sciezka, 100, TEXT("../images.ini"));	/// GetPrivateProfileString Funkcja, ktora w sekcji [images] szuka sciezek img1, img2..., jezeli nie znajdzie juz wiecej "img" to zacznie zwracac zera,
		
		if (*sciezka != '0')	/// if (*sciezka != '0') -jezeli wystapilo zero (brak sciezki w pliku ini) to nie wpisuje go do tablicy
		{
			input.push_back(imread(sciezka, 1));	/// input.push_back -zapisuje do input pobrane sciezki z pliku ini, tworzy tablice sciezek.
			n++;
		}
	}
	
	GetPrivateProfileString(TEXT("converted_imgs"), TEXT("path"), TEXT("0"), path_of_converted_imgs, 100, TEXT("./images.ini"));	/// \param path_of_converted_imgs Zmienna z przypisanym adresem folderu w ktorym beda zapisane obrazy po przetworzeniu.
	/// \return zwraca tablice ze sciezkami oraz adres folderu w ktorym beda zapisane obrazy po przetworzeniu
}

/**
\brief Funkcja wyrownujaca histogram
\details Funkcja ta pobiera obrazy z wektora wejsciowego, kazdy obraz po kolei kowertuje z przestrzeni RGB na YCrCb.
Nastepnie poszczegolne kanaly (Y, Cr, Cb) sa rozdzielane, dla kanalu Y wyrownywany jest histogram, a nastepnie kanaly sa na powrot laczone ze soba.
Obrazy przetworzone wpisywane sa do wektora wyjsciowego.
**/
int equalize()
{
	Mat temp, temp1; /// \param temp- tymczasowa zmienna przechowujaca obraz przekonwertowany do przestrzeni YCrCb \param temp1 tymczasowa zmienna z obrazem temp przekonwertowanym z powrotem do przestrzeni BGR
	vector<Mat> channels; /// \param channels wektor z poszczegolnymi kanalami obrazu temp

	if (!input[0].data) /// \param if(!input[0].data) jezeli nie ma danych wejsciowych to funkcja sie nie wykona
	{
		cout << "blad: brak obrazow" << endl;
		return -1;
	}


	for (int i = 0; i < input.size(); i++)
	{
	cvtColor(input[i], temp, CV_BGR2YCrCb); 	// \param cvtColor- funkcja konwertujaca obrazy (obraz wejscowy, obraz wejsciowy, rodzaj konwersji)

	split(temp, channels); 	// \param split funkcja rozdzielajaca obrazu temp na kanaly Y Cr Cb

	/// \param equalizeHist wyrownanie pierwszego kanalu (Y) odpowiedzialnego za jasnosc
	equalizeHist(channels[0], channels[0]);

	merge(channels, temp);	/// \param merge zlozenie kanalow na powrot do obrazu output
	cvtColor(temp, temp1, CV_YCrCb2BGR);

	output.push_back(temp1); /// \param output.push_back wpisanie przetworzonego obrazu do wektora output
	}

	return 0;
}
/**
\brief Funkcja tworzaca mozaike z miniatur wczytanych obrazow
\details Na podstawie liczby wczytanych obrazkow obliczana jest ilosc kolumn i wierszy mozajki. Szerokosc miniatur jest wyliczana na podstawie liczby kolumn,
natomiast wysokosc jest wyliczana osobno dla kazdej z miniatur. Dzieki temu zostaja zachowane proporcje obrazkow. Poszczegulne miniatury zostaja nastepnie ze soba polaczone,
poprzez naniesienie ich na pusty obraz.
**/
Mat sklejanie(vector<Mat> in, int n) /// \param in wektor obrazow, n ilosc wczytanych obrazow
{
	// okreslenie ilosci wierszy i kolumn na podstawie liczby wczytywanych obrazow
	int wie = ceil((double)n / 3); /// \param wie ilosc wierszy
	int kol = ceil((double)n / wie); /// \param kol ilosc kolumn

	std::vector<float> w; /// \param w wektor wysokosci obrazow wejsciowych
	std::vector<int> w1; /// \param w1 wektor wysokosci miniaturek
	std::vector<float> s; /// \param s wektor szerokosci obrazow wejsciowych
	std::vector<float> ratio; /// \param ratio wektor szerokosci obrazow wejsciowych
	int w_max = 0;/// \param w_max maksymalna wysokosc

	//  s1 = 500. / kol obliczenie szerokosci i wysokosci poszczegulnych miniaturek
	int s1 = 500. / kol; /// \param s1 Zmienna z zapisana informacja o szerokosci kolumny w mozaice.
	for (int i = 0; i < in.size(); i++)
	{
		w.push_back(in[i].cols);
		s.push_back(in[i].rows);
		ratio.push_back(s[i] / w[i]);
		w1.push_back(s1*ratio[i]);
		if (w1[i] > w_max) w_max = w1[i];
	}

	Mat sklej(wie*w_max, kol*s1, CV_8UC3, CV_RGB(0, 0, 0)); /// \param  sklej pusty obraz z czarnym tlem do sklejenia obrazow

															/// for (int i = 0; i<in.size(); i++) petla sklejajaca ze soba miniaturki obrazow
	for (int i = 0; i<in.size(); i++)
	{

		//brief okreslenie kolejnych punktow od ktorych beda wstawiane miniaturki
		int x = i%kol*s1; /// \param x wspolrzedna x do sklejania miniatur
		int y = i / kol*w_max; /// \param y wspolrzedna y do sklejania miniatur

		/// roi = sklej(Rect(x, y, s1, w1[i])) -tworzenie miniaturek i sklejanie ich ze soba
		Mat roi = sklej(Rect(x, y, s1, w1[i]));
		resize(in[i], roi, roi.size());
	}
	return sklej; /// \return funkcja zwraca mozajke sklejonych obrazkow
}
/**
\brief Funkcja zapisujaca obrazy.

\details Funkcja ta bierze wektory obrazow wejsciowych, wyjsciowych, oraz zestawienia obrazow nieprzetworzonych i przetworzonych
nastepnie zapisuje je do folderu wskazanego w pliku ini. Obrazom nadawane sa nowe nazwy. Zestawienie obrazow nieprzetworzonych zapisywane jest jako
przed.jpg, obrazow przetworzonych jako po.jpg, a kolejnym obrazom przetworzonym i nieprzetworzonym nadawane sa nazwy img_(numer)_przed i img_(numer)_po.

**/
int save()
{
	char saveDir[999]; /// \param savedir[999] tablica char ze sciezka do folderu docelowego
	int bytes; /// \param bytes zmienna potrzeba no dzialania sprintf

	for (int i = 0; i < input.size(); i++)
	{
		bytes = sprintf(saveDir, "../przetworzone/img_%i_przed.jpg", i); /// Funkcja sprintf - tworzenie sciezki do zapisu
		imwrite(saveDir, input[i]); /// Funkcja imwrite - zapisywanie obrazkow nieprzetworzonych
	}

	for (int i = 0; i < output.size(); i++)
	{
		bytes = sprintf(saveDir, "../przetworzone/img_%i_po.jpg", i); //tworzenie sciezki do zapisu
		imwrite(saveDir, output[i]); //zapisywanie obrazkow przetworzonych
	}

	imwrite("../przetworzone/przed.jpg", sklejanie1); // zapisywanie zestawienia obrazkow nieprzetworzonych
	imwrite("../przetworzone/po.jpg", sklejanie2); //zapisywanie zestawienia obrazkow przetworzonych
	return 0;
}
/// \brief Funkcja glowna programu.
/// \details W funkcji glownej programu wywolywane sa kolejno funkcje wczytaj, equalize, sklejanie oraz wyswietlane sa sklejone mozaiki, na koncy wywolywana jest funkcja sklej i wstrzymywane jest dzialanie programu
int main() 
{
	wczytaj();	// funkcja zwraca tablice ze sciezkami, liczbe sciezek i adres folderu w ktorym beda zapisane obrazy po przetworzeniu

	equalize();

	sklejanie1 = sklejanie(input, n); //wywolanie funkcji laczacej obrazy
	sklejanie2 = sklejanie(output, n);

	// wyswietlanie wynikow
	imshow(Sklejanie_przed_window, sklejanie1);
	imshow(Sklejanie_po_window, sklejanie2);

	//zapisywanie wynikow
	save();

	waitKey(0);

	return 0; /// \returns Funkcja main zwraca wartosc 0.
}
