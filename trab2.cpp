#include <vector>
#include <cstdio>
#include "gurobi_c++.h"
#include "stdlib.h"
#include <sstream>

#define NIL 0

using namespace std;

int prizedKpaths(int n, int m, vector<vector<int>> &passages, vector<int> &prizes, int P, int k, int target)
{
	int result = NIL;
    GRBVar* x = 0;
    GRBVar** y = 0;
    prizes.insert(prizes.begin(), 0);

    for (int i = 1; i <= n; i++){
        if (i != target){
            vector<int> nova_aresta(3, 0);
            nova_aresta[0] = 0;
            nova_aresta[1] = i;
            passages.insert(passages.begin(), nova_aresta);
        }
    }

    vector<vector<int>> incidencia(m + n - 1, vector<int>(n + 1, 0));
	for (int j = 0; j < m + n - 1; j++)
	{
		incidencia[j][passages[j][0]] = 1;
		incidencia[j][passages[j][1]] = -1;
	}

    //cout << "Processada a entrada\n";
	try 
    {
		// Set gurobi environment
		GRBEnv env = GRBEnv(true); // creates empty environment
		env.set(GRB_IntParam_OutputFlag, 0); // comment this line to show optimization data in the std output
		env.start();

		// Write your model here //

        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "Prize Hunt");

        x = model.addVars(n + 1, GRB_BINARY);
        //model.update();

        for(int i = 0; i <= n; i++){
            ostringstream vname;
            vname << "x" << i;
            x[i].set(GRB_DoubleAttr_Obj, 0);
            x[i].set(GRB_StringAttr_VarName, vname.str());
        }

        //cout << "Defini o x\n";

        y = new GRBVar* [m + n -1];
        for (int j = 0; j < m + n - 1; j ++){
            y[j] = model.addVars(k, GRB_BINARY);
            //model.update();
            for (int d = 0; d < k; d++){
                ostringstream vname;
                vname << "y" << j << d;
                y[j][d].set(GRB_DoubleAttr_Obj, 0);
                y[j][d].set(GRB_StringAttr_VarName, vname.str());
            }
        }

        //cout << "Defini o y\n";

        GRBLinExpr function = 0;

        for (int i = 0; i <= n; i++){
            function += x[i] * prizes[i];
        }
        for (int j = 0; j < m+n - 1; j++){
            for (int d = 0; d < k; d++){
                function = function - y[j][d] * passages[j][2];
            }
        }
        //cout << "Defini a funcao\n";
        model.setObjective(function, GRB_MAXIMIZE);
    
        // Restricoes
        //restricao 1: Para todo j somatorio em k de yjk <= 1
        for (int j = 0; j < m + n - 1; j++){
            GRBLinExpr lim_aresta = 0;
            for (int d = 0; d < k; d++){
                lim_aresta += y[j][d];
            }
            model.addConstr(lim_aresta <= 1, "Passagem única por aresta");
        }

        //cout << "Defini a restricao 1\n";

        //restricao 2: somatorio em x de 1 ate n tem que ser maior que P
        GRBLinExpr min_prem = 0;
        for (int i = 1; i <= n; i++){
            min_prem += x[i];
        }
        model.addConstr(min_prem >= P, "Numero minimo de premios coletados");

        //cout << "Defini a restricao 2\n";

        //restricao 3: 
        //Para todo vertice i de 1 ate n com i diferente de t, para todo cacador, o fluxo tem q ser 0
        for (int i = 1; i <= n; i++){
            if (i != target){
                // para todo vertice vertice
                for (int d = 0; d < k; d++){
                    //para todo caçador
                    GRBLinExpr grau_zero = 0;
                    for (int j = 0; j < m+n-1; j++){
                        grau_zero += incidencia[j][i] * y[j][d];
                    }
                    model.addConstr(grau_zero == 0, "Grau de entrada confirmada igual ao de saida confirmada");
                }
            }
        }
        //cout << "Defini a restricao 3\n";

        //restricao 4:
        //Para cada caçador, ele deve sair de uma e apenas uma aresta do vertice inicial
        for (int d = 0; d < k; d++){
            GRBLinExpr saida_unica = 0;
            GRBLinExpr entrada_unica = 0;
            for (int j = 0; j < n + m -1; j ++){
                saida_unica += incidencia[j][0] * y[j][d];
                entrada_unica += incidencia[j][target] * y[j][d];
            }
            model.addConstr(saida_unica == 1, "Caçador sai por apenas uma aresta do vertice inicial");
            model.addConstr(entrada_unica == -1, "Caçador sai por apenas uma aresta do vertice inicial");
        }
        //cout << "Defini a restricao 4\n";

        //restricao 5:
        //Garantir que o vértice soh eh percorrido se existem arestas que passam por ele
        for (int i = 0; i <= n; i++){
            GRBLinExpr pode_pegar = 0;
            for (int j = 0; j < n+m-1; j++){
                for (int d = 0; d < k; d++){
                    pode_pegar += y[j][d] * abs(incidencia[j][i]);
                }
            }
            model.addConstr(pode_pegar >= x[i], "Verifica se o vértice pode ser pego ou não");
        }

        //cout << "Defini a restricao 5\n";

        //model.update();
        // Use barrier to solve root relaxation
        model.set(GRB_IntParam_Method, GRB_METHOD_BARRIER);
        //model.update();
        model.optimize();
        //cout << "Otimizei\n";
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
    for (int j = 0; j < m+n - 1; j++){
        delete[] y[j];
    }

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
