#include <iostream>
#include <vector>
#include <algorithm>
#include<fstream>
#include"math.h"

using namespace std;


void jarvis_algoritm(ifstream& in)
{
	vector<pair<double, double>> multiple_points;
	while (!in.eof())
	{
		double fst;
		double sec;
		in >> fst >> sec;
		multiple_points.push_back({ fst,sec });
	}
	int size = multiple_points.size();
	pair<double, double> p0 = multiple_points[0];
	for (int i = 0; i < size; i++)
	{
		double x_tek = multiple_points[i].first;
		double y_tek = multiple_points[i].second;
		if (p0.first > x_tek || (p0.first == x_tek && p0.second > y_tek))
		{
			p0.first = x_tek;
			p0.second = y_tek;
			swap(multiple_points[0], multiple_points[i]);
		}
	}

	for (int i = 0; i < size; i++)
	{
		cout << multiple_points[i].first << " " << multiple_points[i].second << endl;
	}
	cout << endl;
	vector<pair<double, double>> hull;
	int curr_id = 0;
	int count = 0;
	pair<double, double> pred_vec = { 0, 0 };
	while (true)
	{
		int id = -13;
		pair<double, double> point_of_hull = multiple_points[curr_id];
		hull.push_back(point_of_hull);
		double min_polar_triang = INT_MAX;
		double cos_a = cos(atan2(pred_vec.second, pred_vec.first));
		double sin_a = sin(atan2(pred_vec.second, pred_vec.first));
		for (int i = 0; i < size; i++)
		{
			if (i == curr_id) continue;
			pair<double, double> next_p = multiple_points[i];
			next_p.first = next_p.first - point_of_hull.first;
			next_p.second = next_p.second - point_of_hull.second;
			double polar_triang = 0;
			if (count == 0)
			{
				polar_triang = atan2(next_p.second, next_p.first);
			}
			else
			{
				double x = next_p.first * cos_a + next_p.second * sin_a;
				double y = -next_p.first * sin_a + next_p.second * cos_a;
				if (abs(y) < 0.00000001)
				{
					polar_triang = atan2(0, x);
					if (x > 0) hull.pop_back();
				}
				else
				{
					polar_triang = atan2(y, x);
				}
			}
			//cout << polar_triang << " ";
			if (polar_triang < min_polar_triang)
			{
				min_polar_triang = polar_triang;
				id = i;
			}
		}
		//cout << endl;
		if (id == 0) 
		{
			break;
		}
		count++;
		pred_vec.first = multiple_points[id].first - multiple_points[curr_id].first;
		pred_vec.second = multiple_points[id].second - multiple_points[curr_id].second;
		//cout << pred_vec.first << " " << pred_vec.second << endl;
		curr_id = id;
	}
	cout << "“очки образующие выпуклую оболочку:" << endl;
	for (int i = 0; i < hull.size(); i++)
	{
		cout << hull[i].first << " " << hull[i].second << endl;
	}
}
int main()
{
	setlocale(LC_ALL, "Russian");
	ifstream in("test.txt");
	jarvis_algoritm(in);
	return 0;
}