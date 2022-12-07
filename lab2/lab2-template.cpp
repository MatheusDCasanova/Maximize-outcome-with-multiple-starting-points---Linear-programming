#include <vector>
#include <cstdio>
#include "gurobi_c++.h"

#define NIL 0

using namespace std;

int prizedKpaths(int n, int m, vector<vector<int>> &passages, vector<int> &prizes, int P, int k, int target)
{
	int result = NIL;

	try
	{
		// Set gurobi environment
		GRBEnv env = GRBEnv(true); // creates empty environment
		env.set(GRB_IntParam_OutputFlag, 0); // comment this line to show optimization data in the std output
		env.start();

		// Write your model here //

	}
	catch (GRBException e)
	{
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...)
	{
		cout << "Exception during optimization." << endl;
	}

	return result;
}

//////////////////////////////////////////////////////
// NÃO MOFIFICAR A PARTIR DAQUI
/* Função que lida com leitura e escrita de valores */
int main()
{
	int n, m, t, k, P;
	cin >> n >> m;

	vector<vector<int>> passages(m, vector<int>(3, NIL));
	vector<int> prizes(n, NIL);

	int u, v, w;
	for (int i = 0; i < m; i++)
	{
		cin >> u >> v >> w;
		passages[i][0] = u;
		passages[i][1] = v;
		passages[i][2] = w;
	}

	int p;
	for (int i = 0; i < n; i++)
	{
		cin >> p;
		prizes[i] = p;
	}
	cin >> P >> k >> t;

	cout << prizedKpaths(n, m, passages, prizes, P, k, t) << endl;

	return 0;
}
