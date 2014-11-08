#include "GPUData.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

using namespace MCSAS;
using namespace Solver;
#include <iostream>
#define uint unsigned int

CGPUData::CGPUData()
{
	chain_address = nullptr; 
	chain_data = nullptr;
	chains_num = 0;

	cudaError_t cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) 
		cout<<"cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?";
}


CGPUData::~CGPUData()
{
	MCSASSResult Res = Clear();
	if (Res != MCSASS_SUCCESS)
		std::cout << "Wrong clear in delete";
}

MCSASSResult 
CGPUData::FromCPUData(const MCSAS::MatrixGenerator::CPUData input)
{
	MCSASSResult Res = MCSASMG_UNKNOWN_ERROR;

	// Сформируем данные на хосте
	KernelGPUData host_data;
	host_data.chains_num = input.Chains.size();
	host_data.chain_address = (uint*)malloc(host_data.chains_num);

	// -Резервируем массив данных host_data.chain_data
	uint mem_size = 0;
	for (uint chain_idx = 0; chain_idx < host_data.chains_num; chain_idx++)
	{
		host_data.chain_address[chain_idx] = mem_size;
		mem_size += sizeof(uint);
		mem_size += sizeof(uint);
		mem_size += sizeof(REAL)* input.Chains[chain_idx].a.size();
		mem_size += sizeof(REAL)* input.Chains[chain_idx].b.size();
		mem_size += sizeof(REAL)* input.Chains[chain_idx].c.size();
		mem_size += sizeof(uint)* input.Chains[chain_idx].LinksColIdx.size();
		mem_size += sizeof(uint)* input.Chains[chain_idx].LinksRowIdx.size();
		mem_size += sizeof(REAL)* input.Chains[chain_idx].LinksVal.size();
	}
	host_data.chain_data = (char*)malloc(mem_size);

	uint mem_idx = 0;
	// -Занесем данные в массив host_data.chain_data
	char* data_idx = host_data.chain_data;
	for (uint chainIdx = 0; chainIdx < host_data.chains_num; chainIdx++)
	{
		/* Структура, из которой будут выведены данные для расчета на GPU
		В структуре цепи хранятся в chain_data по принципу (напомню, sizeof(float) = 4; sizeof(uint) = 4; sizeof(void*) = 4):
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

		uint links_num = input.Chains[chainIdx].LinksVal.size();
		memcpy(data_idx, (char*)links_num, sizeof(uint));
		data_idx += sizeof(uint);

		uint nodes_num = input.Chains[chainIdx].b.size();
		memcpy(data_idx, (char*)nodes_num, sizeof(uint));
		data_idx += sizeof(uint);

		const REAL* vector_a = input.Chains[chainIdx].a.data();
		memcpy(data_idx, (char*)vector_a, sizeof(REAL)*(nodes_num - 1));
		data_idx += sizeof(REAL)*(nodes_num - 1);

		const REAL* vector_b = input.Chains[chainIdx].b.data();
		memcpy(data_idx, (char*)vector_b, sizeof(REAL)*(nodes_num));
		data_idx += sizeof(REAL)*(nodes_num);

		const REAL* vector_c = input.Chains[chainIdx].c.data();
		memcpy(data_idx, (char*)vector_c, sizeof(REAL)*(nodes_num - 1));
		data_idx += sizeof(REAL)*(nodes_num - 1);

		const uint* link_col_idx = input.Chains[chainIdx].LinksColIdx.data();
		memcpy(data_idx, (char*)link_col_idx, sizeof(uint)*(links_num));
		data_idx += sizeof(uint)*(links_num);

		const uint* link_row_idx = input.Chains[chainIdx].LinksColIdx.data();
		memcpy(data_idx, (char*)link_row_idx, sizeof(uint)*(links_num));
		data_idx += sizeof(uint)*(links_num);

		const REAL* link_val = input.Chains[chainIdx].LinksVal.data();
		memcpy(data_idx, (char*)link_val, sizeof(REAL)*(links_num));
		data_idx += sizeof(REAL)*(links_num);
	}

	// -Закинем данные на GPU
	Res = InitialHostDeviceCopy(host_data);

	free(host_data.chain_data);
	free(host_data.chain_address);

	return MCSASS_SUCCESS;
}

MCSASSResult 
MCSAS::Solver::CGPUData::Clear()
{
	if (Chains == nullptr)
		return MCSASS_SUCCESS;

	cudaFree(Chains);

	return MCSASS_SUCCESS;
}

MCSASSResult
CGPUData::InitialHostDeviceCopy(const KernelGPUData Data)
{
	// Обозначим общий массив
	cudaError_t cudaStatus = cudaMalloc((void**)&Chains_dev,  * sizeof(KernelGPUChain));
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaMalloc failed in FromCPUData";
		Res = Clear();
		return MCSASS_CUDAMALLOCFAIL;
	}

	// Создадим внутренние массивы
	for (uint chainIdx = 0; chainIdx)
}
