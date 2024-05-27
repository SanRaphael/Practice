#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int* max_prefix(string smp)
{
	int n = smp.size();
	int* arr = new int[n];
	arr[0] = 0;
	int j = 0;
	int i = 1;
	while (i < n)
	{
		if (smp[i] == smp[j])
		{
			arr[i] = j + 1;
			i++;
			j++;
		}
		else
		{
			if (j == 0)
			{
				arr[i] = 0;
				i++;
			}
			else
			{
				j = arr[j - 1];
			}
		}
	}
	return arr;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	cout << "Введите поисковое слово: " << endl;
	string sample;
	cin >> sample;
	int length = sample.size();
	int* mp = new int[length];
	mp = max_prefix(sample);
	cout << "Массив совпадающих максимальных префиксов" << endl;
	for (int i = 0; i < length; i++)
	{
		cout << mp[i] << " ";
	}
	cout << endl;
	ifstream in("test.txt");
	string text;
	in >> text;
	int pointer_txt = 0;
	int pointer_smp = 0;
	while (pointer_txt != text.size())
	{
		if (text[pointer_txt] != sample[pointer_smp])
		{
			if (pointer_smp != 0)
			{
				pointer_smp = mp[pointer_smp - 1];
			}
			else
			{
				pointer_txt++;
			}
		}
		else
		{
			pointer_txt++;
			pointer_smp++;
			if (pointer_smp == length)
			{
				pointer_smp = 0;
				pointer_txt =  0;
				cout << "Слово в тексте найдено" << endl;
				break;
			}
		}
	}
	if (pointer_txt == text.size()) cout << "Слово в тексте не было найдено" << endl;
	delete[] mp;
	return 0;
}