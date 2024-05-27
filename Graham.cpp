#include <iostream>
#include <vector>
#include <algorithm>
#include<fstream>

using namespace std;


bool comp_ang(pair<int, int> a, pair<int, int> b)
{
	double tr1 = atan2(a.second, a.first);
	double tr2 = atan2(b.second, b.first);
	//int vec_prod = a.first * b.second - a.second * b.first;
	int len_vec_a = a.first * a.first + a.second * a.second;
	int len_vec_b = b.first * b.first + b.second * b.second;
	//return vec_prod > 0 || (vec_prod == 0 && len_vec_a < len_vec_b);
	return tr1 < tr2 || (tr1 == tr2 && len_vec_a < len_vec_b);
}
void graham_algoritm(ifstream& in)
{
	vector<pair<int, int>> multiple_points;
	while (!in.eof())
	{
		int fst;
		int sec;
		in >> fst >> sec;
		multiple_points.push_back({ fst,sec });
	}
	int size = multiple_points.size();
	pair<int, int> p0 = multiple_points[0];
	for (int i = 0; i < size; i++)
	{
		int x_tek = multiple_points[i].first;
		int y_tek = multiple_points[i].second;
		if (p0.second > y_tek || (p0.second == y_tek && p0.first > x_tek))
		{
			p0.first = x_tek;
			p0.second = y_tek;
			swap(multiple_points[0], multiple_points[i]);
		}
	}
	for (int i = 0; i < size; i++)
	{
		multiple_points[i].first -= p0.first;
		multiple_points[i].second -= p0.second;
	}
	sort(multiple_points.begin(),multiple_points.end(), comp_ang);
	vector<pair<int, int>> hull = { multiple_points[0], multiple_points[1] };
	for (int i = 2; i < multiple_points.size(); i++) {
		while (hull.size() >= 2) {
			int last = hull.size() - 1;
			int x1 = hull[last - 1].first;
			int x2 = hull[last].first;
			int x3 = multiple_points[i].first;
			int y1 = hull[last - 1].second;
			int y2 = hull[last].second;
			int y3 = multiple_points[i].second;
			int vec_product = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
			if (vec_product <= 0) {
				hull.pop_back();
			}
			else break;
		}
		hull.push_back(multiple_points[i]);
	}
	cout << "ќтсортированные точки по пол€рному углу относительно p0:" << endl;
	for (int i = 0; i < size; i++)
	{
		multiple_points[i].first += p0.first;
		multiple_points[i].second += p0.second;
		cout << multiple_points[i].first << " " << multiple_points[i].second << endl;
	}
	cout << "“очки образующие выпуклую оболочку:" << endl;
	for (pair<int, int> point : hull) {
		point.first += p0.first;
		point.second += p0.second;
		cout << point.first << " " << point.second << endl;
	}
}
int main()
{
	setlocale(LC_ALL, "Russian");
	ifstream in("test2.txt");
	graham_algoritm(in);
	return 0;
}