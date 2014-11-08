#include "mcsas.h"
#include <fstream>
using namespace MCSAS;
using namespace MatrixGenerator;

unsigned int
COO_matrix::GetRows()
{
	int rows = 0;
	for (int elementNumber = 0; elementNumber < Rows.size(); elementNumber++)
	{
		if (Rows[elementNumber] > rows)
			rows = Rows[elementNumber];
	}
	rows++;
	return rows;
}

unsigned int
COO_matrix::GetCols()
{
	int cols = 0;
	for (int elementNumber = 0; elementNumber < Cols.size(); elementNumber++)
	{
		if (Cols[elementNumber] > cols)
			cols = Cols[elementNumber];
	}
	cols++;
	return cols;
}

MCSASMGTask		
MCSASMGUtility::DefaultTask()
{
	MCSASMGTask Def;
	vector<unsigned int> Chains;
	// 2  x 128
	Chains.push_back(128);
	Chains.push_back(128);
	// 2  x 64
	Chains.push_back(64);
	Chains.push_back(64);
	// 20 x 16
	for (int i = 0; i < 20; i++)
		Chains.push_back(16);
	// 50 x 8
	for (int i = 0; i < 50; i++)
		Chains.push_back(8);

	Def.ChainNodesNumber = Chains;
	Def.AdmittancesDispersion = 0.3;
	Def.AdditionDispersion = 1;
	Def.BaseAdmittance = 1;
	Def.BaseMainDiagonalAddition = 0.5;
	Def.RandomNetAdmittancesNumber = 50;
	Def.AdditionsNumber = 0.3;
	Def.EDSAdmittanceBase = 1;
	Def.EDSAdmittanceDispersion = 0;
	Def.EDSDispersion = 0;
	Def.EDSBase = 1;
	Def.EDSNumber = 0.1;
	Def.CheckConductivity = false;

	return Def;
}

MCSASMGResult
MCSASMGUtility::WriteMatrixMarketFile(vector<double> Vector, const string filename)
{
	std::ofstream output(filename.c_str());

	if (!output)
		return MCSASMG_UNKNOWN_ERROR;

	output.scientific;
	output.precision(10);
	output << "%%MatrixMarket matrix coordinate real general\n";

	// Rows - Cols - Entires
	output << "\t" << Vector.size() << "\t" << 1 << "\t" << Vector.size() << "\n";

	for (int i = 0; i < Vector.size(); i++)
	{
		output << (i + 1) << " ";
		output << (1) << " ";
		output << (Vector[i]);
		output << "\n";
	}

	return MCSASMG_SUCCESS;
}

MCSASMGResult
MCSASMGUtility::WriteMatrixMarketFile(COO_matrix Matrix, const string filename)
{
	std::ofstream output(filename.c_str());

	if (!output)
		return MCSASMG_UNKNOWN_ERROR;

	output.scientific;
	output.precision(10);
	output << "%%MatrixMarket matrix coordinate real general\n";

	// Rows - Cols - Entires
	output << "\t" << Matrix.GetRows() << "\t" << Matrix.GetCols() << "\t" << Matrix.Vals.size() << "\n";

	for (int i = 0; i < Matrix.Vals.size(); i++)
	{
		output << (Matrix.Rows[i] + 1) << " ";
		output << (Matrix.Cols[i] + 1) << " ";
		output << (Matrix.Vals[i]);
		output << "\n";
	}

	return MCSASMG_SUCCESS;
}

double
MCSASMGUtility::RandomDouble(double Base, double Dispersion)
{
	double x = ((double)rand() / (double)(RAND_MAX)) - 0.5;			// Случайное число [-0.5,0.5]
	double delta = Base * Dispersion;
	return (Base + (delta + delta) * x);
}

int
MCSASMGUtility::RandomInt(int From, int To)
{
	if (To < From)
	{
		int temp = To;
		To = From;
		From = temp;
	}
	if (To == From)
		return To;

	double Dispersion = To - From;
	Dispersion *= ((double)rand() / (double)(RAND_MAX));
	int Random = From + (int)(Dispersion);
}