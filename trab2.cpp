#include <vector>
#include <cstdio>
#include "gurobi_c++.h"
#include <sstream>

#define NIL 0

using namespace std;

int prizedKpaths(int n, int m, vector<vector<int>> &passages, vector<int> &prizes, int P, int k, int target)
{
	int result = NIL;
    GRBVar* x = 0;
    GRBVar* y = 0;
    prizes.insert(prizes.begin(), 0);

    for (int i = 1; i <= n; i++){
        if (i != target){
            vector<int> nova_aresta(3, 0);
            nova_aresta[0] = 0;
            nova_aresta[1] = i;
            passages.push_back(nova_aresta);
        }
    }

    vector<vector<int>> incidencia(m + n - 1, vector<int>(n + 1, 0));
	for (int j = 0; j < m + n - 1; j++)
	{
		incidencia[j][passages[j][0]] = 1;
		incidencia[j][passages[j][1]] = -1;
	}

	try 
    {
		// Set gurobi environment
		GRBEnv env = GRBEnv(true); // creates empty environment
		env.set(GRB_IntParam_OutputFlag, 0); // comment this line to show optimization data in the std output
		env.start();

		// Write your model here //

        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "Prize Hunt");

        //Inicialização da variável binária de decisão x
        x = model.addVars(n + 1, GRB_BINARY);
        for(int i = 0; i <= n; i++){
            ostringstream vname;
            vname << "x" << i;
            x[i].set(GRB_DoubleAttr_Obj, 0);
            x[i].set(GRB_StringAttr_VarName, vname.str());
        }
        
        //Inicialização da variável binária de decisão y
        y = model.addVars(m+n-1, GRB_BINARY);
        for (int j = 0; j < m + n - 1; j ++){
            ostringstream vname;
            vname << "y" << j;
            y[j].set(GRB_DoubleAttr_Obj, 0);
            y[j].set(GRB_StringAttr_VarName, vname.str());
        }

        //Definição da função objetivo a ser maximizada
        GRBLinExpr function = 0;
        for (int i = 0; i <= n; i++){
            function += x[i] * prizes[i];
        }
        for (int j = 0; j < m+n - 1; j++){
            function -= y[j] * passages[j][2];
        }
        model.setObjective(function, GRB_MAXIMIZE);
    

        // RESTRIÇÕES

        //Restrição equação 2 relatório
        GRBLinExpr min_prem = 0;
        for (int i = 1; i <= n; i++){
            if (prizes[i] > 0){
                min_prem += x[i];
            }
        }
        model.addConstr(min_prem >= P, "Numero minimo de premios coletados");


        //Restrição equação 3 relatório
        GRBLinExpr saida_cacadores = 0;
        GRBLinExpr entrada_cacadores = 0;
        for (int j = 0; j < n + m -1; j ++){
            if (incidencia[j][0] == 1){
                saida_cacadores += y[j];
            }
            if (incidencia[j][target] == -1){
                entrada_cacadores -= y[j];
            }
        }
        model.addConstr(saida_cacadores == k, "k arestas utilizadas do vertice inicial");
        model.addConstr(entrada_cacadores == -k, "k arestas utilizadas do vertice target");

        //Restrição equação 4 relatório
        for (int i = 1; i <= n; i++){
            GRBLinExpr limite = 0;
            for (int j = 0; j < n+m-1; j++){
                if (incidencia[j][i] < 0){
                    limite += y[j]; 
                }
            }
            model.addConstr(limite >= x[i], "Verifica se o vértice pode ser pego ou não");
        }

        //Restrição equação 5 relatório
        for (int i = 1; i <= n; i++){
            GRBLinExpr limite = 0;
            if (i != target){
                for (int j = 0; j < m+n-1; j++){
                    if (incidencia[j][i] == 1){
                        limite += y[j];
                    } else if(incidencia[j][i] == -1){
                        limite -= y[j];
                    }
                }
                model.addConstr(limite == 0, "Grau de entrada confirmada igual ao de saida confirmada");
            }
        }

        // Use barrier to solve root relaxation
        model.set(GRB_IntParam_Method, GRB_METHOD_BARRIER);
        model.optimize();
        result = model.get(GRB_DoubleAttr_ObjVal);
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

    delete[] x;

    delete[] y;

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
