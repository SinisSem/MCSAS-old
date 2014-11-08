#pragma once
#include "mcsas.h"
namespace MCSAS
{
	namespace Solver
	{
		class CGPUData :
			public KernelGPUData
		{
		public:
			CGPUData();
			~CGPUData();

		public:
			MCSASSResult	FromCPUData(MCSAS::MatrixGenerator::CPUData);			// ������� ������� �� CPU ����� � STD ���������
			MCSASSResult	Clear();												// ����������� ������
		private:
			MCSASSResult	InitialHostDeviceCopy(KernelGPUData);
		};
	}
}

