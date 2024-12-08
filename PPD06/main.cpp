#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

int n, m;
vector<int> adj[100001];

bool foundSolution = false;
bool foundSolutionParalelised = false;
int availableThreads = std::thread::hardware_concurrency();

bool checkCondition(vector<int>& vertexList, int candidate, int startingNode)
{
	for (auto& vertex : vertexList)
	{
		if (vertex == candidate && !(vertex == candidate && candidate == startingNode && vertexList.size() == n))
			return false;
	}
	return true;
}

void findHamiltonianCycle(vector<int> vertexList, int node, int startingNode)
{
	if (foundSolution)
		return;

	if (node == startingNode && vertexList.size() == n + 1)
	{
		cout << "Found solution: \n";
		for (auto elem : vertexList)
			cout << elem << " ";
		cout << "\n";
		foundSolution = true;
		return;
	}

	for (auto candidate : adj[node])
	{
		if (checkCondition(vertexList, candidate, startingNode))
		{
			vertexList.push_back(candidate);
			findHamiltonianCycle(vertexList, candidate, startingNode);
			vertexList.pop_back();
		}
	}
}

void findHamiltonianCycleParalelised(vector<int> vertexList, int node, int startingNode, int nrThreads)
{
	if (foundSolutionParalelised)
		return;

	if (node == startingNode && vertexList.size() == n + 1)
	{
		cout << "Found solution: \n";
		for (auto elem : vertexList)
			cout << elem << " ";
		cout << "\n";
		foundSolutionParalelised = true;
		return;
	}

	for (auto candidate : adj[node])
	{
		if (checkCondition(vertexList, candidate, startingNode))
		{
			if (nrThreads > 1)
			{
				vector<int> vertexListCopy{ vertexList };
				vertexListCopy.push_back(candidate);
				thread t(findHamiltonianCycleParalelised, vertexListCopy, candidate, startingNode, nrThreads / 2);
				t.join();
			}
			else
			{
				vertexList.push_back(candidate);
				findHamiltonianCycleParalelised(vertexList, candidate, startingNode, nrThreads);
				vertexList.pop_back();
			}
		}
	}
}

int main(void)
{
	ifstream file("graph.txt");
	file >> n >> m;

	for (int i = 0; i < m; i++)
	{
		int x, y;
		file >> x >> y;
		adj[x].push_back(y);
	}
	
	auto begin = std::chrono::high_resolution_clock::now();
	findHamiltonianCycle({ 1 }, 1, 1);
	auto end = std::chrono::high_resolution_clock::now();
	cout << "Sequential: " << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << "\n";
	
	begin = std::chrono::high_resolution_clock::now();
	findHamiltonianCycleParalelised({ 1 }, 1, 1, availableThreads);
	end = std::chrono::high_resolution_clock::now();
	cout << "Paralelised: " << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << "\n";
	
	cout << "Hello world!\n";
	return 0;
}