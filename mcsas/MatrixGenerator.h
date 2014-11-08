#ifndef __MATRIXGENERATOR_H__
#define __MATRIXGENERATOR_H__
#include "mcsas.h"

namespace MCSAS
{
	namespace MatrixGenerator
	{
		class CMatrixGenerator
		{
		/*���������� ������������� ������-----------------------------------------------------------------*/
		private:
			/*���������, � ������� �������� ����� - ������ ������ �� ���, �������, ��������*/
			struct Link
			{
				unsigned int	RowNumber;
				unsigned int	ColNumber;
				double			Value;
			};
			/*���������, � ������� �������� ������� �� �������*/
			struct Chain
			{
				int					StartNode;						// ������ ������� ���� � ������ ������� �����
				int					NodesNumber;					// ���������� ����� � ����
				vector<Link>		Links;							// ������ ������������ ������
				vector<double>		RightVector;					// ������ ������
				vector<double>		a;								// ������ ��������� �������
				vector<double>		b;								// ������� ��������� �������
				vector<double>		c;								// ������� ��������� �������
			};
		/*------------------------------------------------------------------------------------------------*/

		public:
										CMatrixGenerator();
										~CMatrixGenerator();

		/*������������ ������������ ����------------------------------------------------------------------*/
		private:
			MCSASMGTask					m_Task;						/*������������ ������������ ����*/
			bool						m_HasTask;					/*���� �� ������ �������*/
			vector<Chain>				m_Chains;					/*������ ������� - !!!!!!! ��� ������� � ����� ���������� ������������� ����*/
		public:
			MCSASMGResult				SetTask(MCSASMGTask);		/*������ �������*/
			MCSASMGTask					GetTask();					/*���������� ������� �������*/
		/*------------------------------------------------------------------------------------------------*/

		/*��������� ���������� ��������-------------------------------------------------------------------*/
		public:
			MCSASMGResult				GenerateInternal();
		private:
			MCSASMGResult				CreateChains();
			MCSASMGResult				CreateRandomLinks();
			MCSASMGResult				CreateEDS();
		/*------------------------------------------------------------------------------------------------*/

		/*�������-�������---------------------------------------------------------------------------------*/
		private:
			double						GetRandomAdmittance();
			double						GetRandomEDSAdmittance();
			double						GetRandomEDS();
			double						GetRandomAddition();
			unsigned int				GetNodesNumber();				// �������� ����� ����� � �������������� �������
			static vector<int>			RandomVectorInsert(int, int);
			static vector<int>			RandomPairVectorInsert(int, vector<int>);
		/*------------------------------------------------------------------------------------------------*/

		/*��������� �������� ������ �� ���������� ��������------------------------------------------------*/
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

