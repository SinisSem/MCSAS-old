#ifndef __mcsas_h__
#define __mcsas_h__

#include <vector>
using namespace std;

#define	REAL	float

namespace MCSAS
{
	/*����� ����-------------------------------------------------------------------------------------------------------------------------------*/
	// ���������, �� ������� ����� �������� ������ ������ ��� ������� �� GPU 
	struct KernelGPUChain
	{
		unsigned int		LinksNum;				// ����� ������
		unsigned int*		LinksColIdx;			// ������ �������� �������� ������  (������ = LinksNum)
		unsigned int*		LinksRowIdx;			// ������ �������� ����� ������  (������ = LinksNum)
		REAL*				LinksVal;				// ������ �������� ������ (������ = LinksNum)

		unsigned int		NodesNum;				// ���������� ����� ������� (�� �����, �����)
		REAL*				a;						// ������ ���������  (������ = (NodesNum-1))
		REAL*				b;						// ������� ��������� (������ =  NodesNum)
		REAL*				�;						// ������� ��������� (������ = (NodesNum-1))
	};

	/* ���������, �� ������� ����� �������� ������ ��� ������� �� GPU
		� ��������� ���� �������� � chain_data �� �������� (�������, sizeof(float) = 4; sizeof(uint) = 4; sizeof(void*) = 4):
		-��� ����� 

		#	|	���������� ����					|	�� ��� ��������	|	��� ��� �����
		---------------------------------------------------------------------------------------------------------
		1.	|	sizeof(uint)					|	links_num		|	������� ������ � ������ ����
		2.	|	sizeof(uint)					|	nodes_num		|	������� ����� (����� - �����) � ������ ����
		3.	|	sizeof(REAL) * (nodes_num-1)	|	vector_a		|	������ ������� ���������
		4.	|	sizeof(REAL) * nodes_num		|	vector_b		|	������ ������� ���������
		5.	|	sizeof(REAL) * (nodes_num-1)	|	vector_c		|	������ ������ ���������
		6.	|	sizeof(uint) * links_num		|	link_col_idx	|	������ �������� �������� ������
		7.	|	sizeof(uint) * links_num		|	link_row_idx	|	������ �������� ����� ������
		8.	|	sizeof(REAL) * links_num		|	link_val		|	������ �������� ������*/
	struct KernelGPUData					
	{
		unsigned int		chains_num;		// ������� ����� � �������			(sizeof(unsigned int) = 4)//!!!!!!!!!!!!!!!!!!�� ������, ���� �� ��� ������� - ����� �������)))
		unsigned int		mem_size;
		unsigned int*		chain_address;	// ������ ������� ����� (�������� ����� � ������� )
		char*				chain_data;		// ������ ������
	};
	/*����� ����\------------------------------------------------------------------------------------------------------------------------------*/

	/*Solver-----------------------------------------------------------------------------------------------------------------------------------*/
	namespace Solver
	{
		class CGPUData;
		/*��������� ���� ������, ������������ ���������*/
		enum MCSASSResult
		{
			MCSASS_SUCCESS = 0,				/*������������, ���� ������� ��������� �������*/
			MCSASS_CUDAMALLOCFAIL,			/*������������, ���� ������ ����� cudaMalloc*/
			MCSASMG_UNKNOWN_ERROR = 9999	/*����������������, ��� ����������� ������*/
		};
	}
	/*Solver\----------------------------------------------------------------------------------------------------------------------------------*/

	/*MatrixGenerator--------------------------------------------------------------------------------------------------------------------------*/
	namespace MatrixGenerator
	{ 
		/*��������� ���� ������, ������������ ��������� matrix_generator*/
		enum MCSASMGResult
		{
			MCSASMG_SUCCESS = 0,				/*������������, ���� ������� ��������� �������*/
			MCSASMG_UNSUFFICIENT_TASK,			/*������������ �������*/
			MCSASMG_UNKNOWN_ERROR = 9999		/*����������������, ��� ����������� ������*/
		};

		/*���������, � ������� ���������� ������� ��� ����������*/
		struct MCSASMGTask
		{
			vector<unsigned int>		ChainNodesNumber;				// ������� ����� ����� � ������ �������
			unsigned int				RandomNetAdmittancesNumber;		// ���������� ��������� ������
			double						BaseAdmittance;					// ������� ������������
			double						AdmittancesDispersion;			// ��������� ������������� ������������ ������� (������������ �������)
			double						BaseMainDiagonalAddition;		// ������� ������� � ������� ��������� (������������ �����)
			double						AdditionDispersion;				// ��������� ������� � ������� ��������� (������������ �������)
			double						AdditionsNumber;				// ���������� ����� � �������� � ������� ��������� (������������ ����� �����)
			double						EDSBase;						// ������� ���
			double						EDSDispersion;					// ��������� ���
			double						EDSNumber;						// ����� ����� � ��� (������������ ����� �����)
			double						EDSAdmittanceBase;				// ������� ������������ ���
			double						EDSAdmittanceDispersion;		// ��������� ������������� ���
			bool						CheckConductivity;				// ��������� �� �������� ������ ��������� ��������� �����
		};

		// ��������� ��� COO ������� (����� ��� ������ � ���� � ����������� �������� MATLAB'��)
		struct COO_matrix
		{
			vector<double>			Vals;														// ������ ��������
			vector<unsigned int>	Cols;														// ������ ��������
			vector<unsigned int>	Rows;														// ������ �����

			unsigned int			GetRows();													// �������� (��������� �� ���������) ����� �����
			unsigned int			GetCols();													// �������� (��������� �� ���������) ����� ��������

		};

		class MCSASMGUtility
		{
		public:
			static MCSASMGTask			DefaultTask();											// ������ �������� �������� ������ �������
			static MCSASMGResult		WriteMatrixMarketFile(COO_matrix, string);				// ���������� matrix market file �������� �������
			static MCSASMGResult		WriteMatrixMarketFile(vector<double>, string);			// ���������� matrix market file ��������� �������
			static double				RandomDouble(double, double);							// ���������� ���������� ���� double � �������� ����� � ������������
			static int					RandomInt(int, int);									// ���������� ���������� ���� int � �������� �� 
		};

		struct CPUChain
		{

			vector<unsigned int>	LinksColIdx;			// ������ �������� �������� ������  (������ = LinksNum)
			vector<unsigned int>	LinksRowIdx;			// ������ �������� ����� ������  (������ = LinksNum)
			vector<REAL>			LinksVal;				// ������ �������� ������ (������ = LinksNum)

			vector<REAL>			a;						// ������ ���������  (������ = (NodesNum-1))
			vector<REAL>			b;						// ������� ��������� (������ =  NodesNum)
			vector<REAL>			c;						// ������� ��������� (������ = (NodesNum-1))
		};

		struct CPUData
		{
			vector<CPUChain>		Chains;						// ������ ����� (������ = ChainNum)
		};
	};
	/*MatrixGenerator\-------------------------------------------------------------------------------------------------------------------------*/
}

#endif