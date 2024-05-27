#include<iostream> 
#include<fstream> 
#include<vector> 
#include<string> 
#include<sstream> 
#include"C:\Users\79061\source\repos\Graph\13.02.2024\Graph.h" 
#include <algorithm> 

using namespace std;
void fill(vector<vector<pair<int, int>>>& a, vector<vector<int>>& b, istream& in1, istream& in2);
int bin_search(vector<pair<int, int>> a, pair<int, int> X);
void convert_to_graph(vector<vector<pair<int, int>>> rms, vector<vector<pair<int, int>>> psg, Graph& gr, int cnt);
void print_rooms(vector<vector<pair<int, int>>> rooms);
void BFS(vector<vector<pair<int, int>>>& graph, int start, int end);
int main()
{
    setlocale(LC_ALL, "Russian");
    ifstream in1("circles.txt");
    ifstream in2("walls.txt");
    int num_circles;
    in1 >> num_circles;
    vector<vector<pair<int, int>>> passages(num_circles);
    vector<vector<int>> walls(num_circles - 1);
    fill(passages, walls, in1, in2);
    vector<vector<pair<int, int>>> rooms(num_circles - 1);
    int count_rooms = 0;
    for (int i = 0; i < num_circles - 1; i++)
    {
        for (int j = 0; j < walls[i].size(); j++)
        {
            if (j == walls[i].size() - 1)
            {
                rooms[i].insert(rooms[i].begin(), { walls[i][j] - 360, walls[i][0] });
            }
            else rooms[i].push_back({ walls[i][j], walls[i][j + 1] });
            count_rooms++;
        }
    }
    print_rooms(rooms);
    Graph gr(count_rooms + 2);
    convert_to_graph(rooms, passages, gr, count_rooms);
    gr.printEdgeList();
    gr.ListEdge_to_AdjList();
    gr.printAdjList();
    vector<vector<pair<int, int>>> labyrinth = gr.getAdjList();
    BFS(labyrinth, count_rooms + 1, 0);
    return 0;
}


void fill(vector<vector<pair<int, int>>>& pass, vector<vector<int>>& w, istream& in1, istream& in2)
{
    string line1;
    int it1 = 0;
    while (getline(in1, line1))
    {
        if (line1.empty()) continue;
        istringstream iss(line1);
        pair<int, int> a;
        int j = 0;
        while (iss >> a.first >> a.second)
        {
            pass[it1].push_back(a);
            cout << pass[it1][j].first << " " << pass[it1][j].second << " ";
            j++;
        }
        cout << endl;
        it1++;
    }
    string line2;
    int it2 = 0;
    while (getline(in2, line2))
    {
        if (line2.empty()) continue;
        istringstream iss(line2);
        int a;
        int j = 0;
        while (iss >> a)
        {
            w[it2].push_back(a);
            cout << w[it2][j] << " ";
            j++;
        }
        cout << endl;
        it2++;
    }
}

void print_rooms(vector<vector<pair<int, int>>> rooms)
{
    cout << "Комнаты на кольцах: " << endl;
    for (int i = 0; i < rooms.size(); i++)
    {
        for (int j = 0; j < rooms[i].size(); j++)
        {
            cout << rooms[i][j].first << " " << rooms[i][j].second << " ";
        }
        cout << endl;
    }
}

int bin_search(vector<pair<int, int>> a, pair<int, int> X)
{
    //pair<int, int> X = { 26,40 }; 
    //vector<pair<int, int>> a = { {0,10},{10,14},{14,63},{63,154},{154,159},{159,177},{177,213},{213,228},{218,360} }; 
    int L = 0;
    int R = a.size() - 1;
    int cnt = 0; 
    while (L <= R)
    {
        int M = L + (R - L) / 2;
        if (a[M].first + 360 <= X.first)
        {
            a[M].first += 360;
            a[M].second += 360;
        }
        else if (X.first >= a[R].second)
        {
            X.first -= 360;
            X.second -= 360;
        }
        if (a[M].second <= X.first) L = M + 1;
        else if (a[M].first >= X.second) R = M - 1;
        else if (a[M].first <= X.first && X.second <= a[M].second)
        {
            //cout << "Число итераций бинарного поиска: " << cnt << endl; 
            return M;
        }
        cnt++; 
        if (cnt > 100) break;
    }
}

void convert_to_graph(vector<vector<pair<int, int>>> rings, vector<vector<pair<int, int>>> psg, Graph& gr, int cnt)
{
    int num_of_room = 0;
    for (int i = 0; i < rings.size() + 1; i++)
    {
        if (i == 0)
        {
            for (int k = 0; k < psg[i].size(); k++)
            {
                int index = bin_search(rings[i], psg[i][k]) + 1 + num_of_room;
                gr.add_edge(i, index, 1);
            }
        }
        else
        {
            if (i == rings.size())
            {
                for (int k = 0; k < psg[i].size(); k++)
                {
                    int index = bin_search(rings[i - 1], psg[i][k]) + 1 + num_of_room;
                    gr.add_edge(index, cnt + 1, 1);
                }
            }
            else
            {
                for (int k = 0; k < psg[i].size(); k++)
                {
                    int index_curr_room_curr_ring = bin_search(rings[i - 1], psg[i][k]) + 1 + num_of_room;
                    int index_curr_room_next_ring = bin_search(rings[i], psg[i][k]) + 1 + rings[i - 1].size() + num_of_room;
                    gr.add_edge(index_curr_room_curr_ring, index_curr_room_next_ring, 1);
                }
                num_of_room += rings[i - 1].size();
            }
        }
    }
}

void BFS(vector<vector<pair<int, int>>>& graph, int start, int end)
{
    vector<int> dist(graph.size(), INT_MAX);
    vector<int> way(graph.size(), 0);
    queue<int> q;
    dist[start] = 1;
    q.push(start);
    while (!q.empty())
    {
        int vertex = q.front();
        q.pop();
        for (int i = 0; i < graph[vertex].size(); i++)
        {
            int neighbor = graph[vertex][i].first;
            int weight = graph[vertex][i].second;
            if (dist[neighbor] == INT_MAX)
            {
                dist[neighbor] = dist[vertex] + weight;
                way[neighbor] = vertex;
                q.push(neighbor);
            }
        }
    }
    int curr_vertex = end;
    vector<int> pathway;
    while (curr_vertex != start)
    {
        pathway.push_back(curr_vertex);
        curr_vertex = way[curr_vertex];
    }
    pathway.push_back(start);
    reverse(pathway.begin(), pathway.end());
    cout << "Длина кратчайшего пути: " << dist[end] - 2 << endl;
    cout << "кратчайший путь из лабиринта: " << endl;
    for (int i = 0; i < pathway.size(); i++)
        cout << pathway[i] << " ";
    cout << endl;
}
