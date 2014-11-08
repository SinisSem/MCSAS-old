#ifndef __MATRIXGENERATOR_H__
#define __MATRIXGENERATOR_H__
#include "mcsas.h"

namespace MCSAS
{
	namespace MatrixGenerator
	{
		class CMatrixGenerator
		{
		/*Внутреннее представление данных-----------------------------------------------------------------*/
		private:
			/*Структура, в которой хранится связь - храним только ее ряд, столбец, значение*/
			struct Link
			{
				unsigned int	RowNumber;
				unsigned int	ColNumber;
				double			Value;
			};
			/*Структура, в которой хранятся цепочки со связями*/
			struct Chain
			{
				int					StartNode;						// Индекс первого узла в полной матрице цепей
				int					NodesNumber;					// Количество узлов в цепи
				vector<Link>		Links;							// Вектор нецепочечных связей
				vector<double>		RightVector;					// Правый вектор
				vector<double>		a;								// Нижняя диагональ цепочки
				vector<double>		b;								// Главная диагональ цепочки
				vector<double>		c;								// Верхняя диагональ цепочки
			};
		/*------------------------------------------------------------------------------------------------*/

		public:
										CMatrixGenerator();
										~CMatrixGenerator();

		/*Конфигурация генерируемой сети------------------------------------------------------------------*/
		private:
			MCSASMGTask					m_Task;						/*Конфигурация генерируемой сети*/
			bool						m_HasTask;					/*Было ли задано задание*/
			vector<Chain>				m_Chains;					/*Вектор цепочек - !!!!!!! тут реально и живет внутреннее представление сети*/
		public:
			MCSASMGResult				SetTask(MCSASMGTask);		/*Задает задание*/
			MCSASMGTask					GetTask();					/*Возвращает текущее задание*/
		/*------------------------------------------------------------------------------------------------*/

		/*Генерация внутренних структур-------------------------------------------------------------------*/
		public:
			MCSASMGResult				GenerateInternal();
		private:
			MCSASMGResult				CreateChains();
			MCSASMGResult				CreateRandomLinks();
			MCSASMGResult				CreateEDS();
		/*------------------------------------------------------------------------------------------------*/

		/*Функции-утилиты---------------------------------------------------------------------------------*/
		private:
			double						GetRandomAdmittance();
			double						GetRandomEDSAdmittance();
			double						GetRandomEDS();
			double						GetRandomAddition();
			unsigned int				GetNodesNumber();				// Получить число узлов в результирующей матрице
			static vector<int>			RandomVectorInsert(int, int);
			static vector<int>			RandomPairVectorInsert(int, vector<int>);
		/*------------------------------------------------------------------------------------------------*/

		/*Генерация выходных данных из внутренних структур------------------------------------------------*/
		public:
			COO_matrix					GetCOOMatrix();
			COO_matrix					GetCOOMatrix(unsigned int);
			vector<double>				GetRightVector();
			vector<double>				GetRightVector(unsigned int);
			COO_matrix					GetCOOTriDiagMatrix();
			COO_matrix					GetCOOTriDiagMatrix(unsigned int);
			CPUData						GetCPUChain();
			CPUChain					GetCPUChain(unsigned int);
		/*------------------------------------------------------------------------------------------------*/
		};
	}
}
#endif

