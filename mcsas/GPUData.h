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
			MCSASSResult	FromCPUData(MCSAS::MatrixGenerator::CPUData);			// Создать матрицу из CPU файла с STD векторами
			MCSASSResult	Clear();												// Освобождает память
		private:
			MCSASSResult	InitialHostDeviceCopy(KernelGPUData);
		};
	}
}

