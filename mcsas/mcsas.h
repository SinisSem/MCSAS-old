#ifndef __mcsas_h__
#define __mcsas_h__

#include <vector>
using namespace std;

#define	REAL	float

namespace MCSAS
{
	/*Общие типы-------------------------------------------------------------------------------------------------------------------------------*/
	// Структура, из которой будут выведены данные связей для расчета на GPU 
	struct KernelGPUChain
	{
		unsigned int		LinksNum;				// Число связей
		unsigned int*		LinksColIdx;			// Массив индексов столбцов связей  (размер = LinksNum)
		unsigned int*		LinksRowIdx;			// Массив индексов строк связей  (размер = LinksNum)
		REAL*				LinksVal;				// Массив значений связей (размер = LinksNum)

		unsigned int		NodesNum;				// Количество узлов матрицы (по факту, строк)
		REAL*				a;						// Нижняя диагональ  (размер = (NodesNum-1))
		REAL*				b;						// Главная диагональ (размер =  NodesNum)
		REAL*				с;						// Верхняя диагональ (размер = (NodesNum-1))
	};

	/* Структура, из которой будут выведены данные для расчета на GPU
		В структуре цепи хранятся в chain_data по принципу (напомню, sizeof(float) = 4; sizeof(uint) = 4; sizeof(void*) = 4):
		-Под нужды 

		#	|	Количество байт					|	На что выделено	|	Что это такое
		---------------------------------------------------------------------------------------------------------
		1.	|	sizeof(uint)					|	links_num		|	Сколько связей у данной цепи
		2.	|	sizeof(uint)					|	nodes_num		|	Сколько узлов (читай - строк) у данной цепи
		3.	|	sizeof(REAL) * (nodes_num-1)	|	vector_a		|	Вектор верхней диагонали
		4.	|	sizeof(REAL) * nodes_num		|	vector_b		|	Вектор главной диагонали
		5.	|	sizeof(REAL) * (nodes_num-1)	|	vector_c		|	Вектор нижней диагонали
		6.	|	sizeof(uint) * links_num		|	link_col_idx	|	Вектор индексов столбцов связей
		7.	|	sizeof(uint) * links_num		|	link_row_idx	|	Вектор индексов строк связей
		8.	|	sizeof(REAL) * links_num		|	link_val		|	Вектор значений связей*/
	struct KernelGPUData					
	{
		unsigned int		chains_num;		// Сколько цепей в задании			(sizeof(unsigned int) = 4)//!!!!!!!!!!!!!!!!!!не уверен, надо ли его хранить - время покажет)))
		unsigned int		mem_size;
		unsigned int*		chain_address;	// Массив адресов цепей (смещения цепей в массиве )
		char*				chain_data;		// Массив данных
	};
	/*Общие типы\------------------------------------------------------------------------------------------------------------------------------*/

	/*Solver-----------------------------------------------------------------------------------------------------------------------------------*/
	namespace Solver
	{
		class CGPUData;
		/*Возможные коды ошибок, возвращаемые сольвером*/
		enum MCSASSResult
		{
			MCSASS_SUCCESS = 0,				/*Возвращается, если функция выполнена успешно*/
			MCSASS_CUDAMALLOCFAIL,			/*Возвращается, если ошибку выдал cudaMalloc*/
			MCSASMG_UNKNOWN_ERROR = 9999	/*Необрабатываемая, или неизвестная ошибка*/
		};
	}
	/*Solver\----------------------------------------------------------------------------------------------------------------------------------*/

	/*MatrixGenerator--------------------------------------------------------------------------------------------------------------------------*/
	namespace MatrixGenerator
	{ 
		/*Возможные коды ошибок, возвращаемые функциями matrix_generator*/
		enum MCSASMGResult
		{
			MCSASMG_SUCCESS = 0,				/*Возвращается, если функция выполнена успешно*/
			MCSASMG_UNSUFFICIENT_TASK,			/*Недопустимое задание*/
			MCSASMG_UNKNOWN_ERROR = 9999		/*Необрабатываемая, или неизвестная ошибка*/
		};

		/*Структура, в которой передается задание для генератора*/
		struct MCSASMGTask
		{
			vector<unsigned int>		ChainNodesNumber;				// Сколько узлов будет в каждой цепочке
			unsigned int				RandomNetAdmittancesNumber;		// Количество случайных связей
			double						BaseAdmittance;					// Базовая проводимость
			double						AdmittancesDispersion;			// Дисперсия проводимостей относительно базовой (относительно базовой)
			double						BaseMainDiagonalAddition;		// Базовая добавка к главной диагонали (проводимости шунта)
			double						AdditionDispersion;				// Дисперсия добавок к главной диагонали (относительно базовой)
			double						AdditionsNumber;				// Количество узлов с добавкой к главной диагонали (относительно числа узлов)
			double						EDSBase;						// Базовая ЭДС
			double						EDSDispersion;					// Дисперсия ЭЛС
			double						EDSNumber;						// Число узлов с ЭДС (относительно числа узлов)
			double						EDSAdmittanceBase;				// Базовая проводимость ЭДС
			double						EDSAdmittanceDispersion;		// Дисперсия проводимостей ЭДС
			bool						CheckConductivity;				// Выполнять ли проверку полной связности итогового графа
		};

		// Структура для COO матрицы (нужна для вывода в файл и последующей проверки MATLAB'ом)
		struct COO_matrix
		{
			vector<double>			Vals;														// Вектор значений
			vector<unsigned int>	Cols;														// Вектор столбцов
			vector<unsigned int>	Rows;														// Вектор строк

			unsigned int			GetRows();													// Реальное (проверено из элементов) число рядов
			unsigned int			GetCols();													// Реальное (проверено из элементов) число столбцов

		};

		class MCSASMGUtility
		{
		public:
			static MCSASMGTask			DefaultTask();											// Выдает заведомо заданный пример задания
			static MCSASMGResult		WriteMatrixMarketFile(COO_matrix, string);				// Записывает matrix market file заданной матрицы
			static MCSASMGResult		WriteMatrixMarketFile(vector<double>, string);			// Записывает matrix market file заданного вектора
			static double				RandomDouble(double, double);							// Возвращает переменную типа double с заданной базой и отклоненеием
			static int					RandomInt(int, int);									// Возвращает переменную типа int в пределах от 
		};

		struct CPUChain
		{

			vector<unsigned int>	LinksColIdx;			// Массив индексов столбцов связей  (размер = LinksNum)
			vector<unsigned int>	LinksRowIdx;			// Массив индексов строк связей  (размер = LinksNum)
			vector<REAL>			LinksVal;				// Массив значений связей (размер = LinksNum)

			vector<REAL>			a;						// Нижняя диагональ  (размер = (NodesNum-1))
			vector<REAL>			b;						// Главная диагональ (размер =  NodesNum)
			vector<REAL>			c;						// Верхняя диагональ (размер = (NodesNum-1))
		};

		struct CPUData
		{
			vector<CPUChain>		Chains;						// Массив цепей (размер = ChainNum)
		};
	};
	/*MatrixGenerator\-------------------------------------------------------------------------------------------------------------------------*/
}

#endif