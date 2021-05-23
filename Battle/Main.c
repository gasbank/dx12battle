// https://github.com/wolfgangfengel/graphicsdemoskeleton/blob/master/DirectX%2012/02_Triangle/Window.c

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vertex.h"
#include "PredefinedGuid.h"
#include "ErrorUtil.h"
#include "RenderPass.h"
#include "Rectangle.h"
#include "Matrix.h"

#define WIN_WIDTH (1920)
#define WIN_HEIGHT (1080)
#define WIN_POS_X (0)
#define WIN_POS_Y (0)
#define FRAME_COUNT (2)
#define GRID_COUNT_X (16*2)
#define GRID_COUNT_Y (9*2)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

ID3D12Device* gDevice;
ID3D12CommandQueue* gCommandQueue;
IDXGISwapChain3* gSwapChain;
UINT gFrameIndex;
ID3D12DescriptorHeap* gRtvHeap;
UINT gRtvDescriptorSize;
ID3D12Resource* gRenderTargets[FRAME_COUNT];
ID3D12CommandAllocator* gCommandAllocator;
ID3D12CommandAllocator* gCommandAllocator2;
ID3D12GraphicsCommandList* gCommandList;
ID3D12GraphicsCommandList* gCommandList2;
ID3D12RootSignature* gRootSignature;
ID3D12PipelineState* gPipelineState;
ID3D12PipelineState* gPipelineState2;
ID3D12Resource* gBufVerts;
D3D12_VERTEX_BUFFER_VIEW gDescViewBufVert;
ID3D12Fence* gFence;
static UINT64 gCurrentFence;
HANDLE gFenceEvent;
ID3D12DescriptorHeap* gCbvHeap;
UINT gCbvDescriptorSize;
ID3D12Resource* gUploadCBuffer;

D3D12_VIEWPORT gViewport = { 0.0f, 0.0f, (float)(WIN_WIDTH), (float)(WIN_HEIGHT), 0.0f, 1.0f };
D3D12_RECT gRectScissor = { 0, 0, (LONG)(WIN_WIDTH), (LONG)(WIN_HEIGHT) };

__int64 gCurTime;
__int64 gCountsPerSec;

// 상수 버퍼의 최대 크기는 64 KB (65,536바이트)다.
typedef struct _CBPEROBJECT
{
	float gWorldViewProj[GRID_COUNT_X * GRID_COUNT_Y][16]; // 64-byte
	float c[GRID_COUNT_X * GRID_COUNT_Y][4]; // 768-byte
	float pad0[4 * 4 * 3 + 16]; // 192-byte
} CBPEROBJECT;

const size_t CBPEROBJECT_SIZE = sizeof(CBPEROBJECT);
const size_t CBPEROBJECT_SIZE_REMAINDER = sizeof(CBPEROBJECT) % 256;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"Battle";
	wcex.hIconSm = 0;

	return RegisterClassExW(&wcex);
}

void ReleaseCom(void* comPtr)
{
	if (comPtr == NULL) return;

	IUnknown* p = (IUnknown*)comPtr;
	p->lpVtbl->Release(p);
}

void WaitForPreviousFrame()
{
	const UINT64 fence = gCurrentFence;
	ThrowIfFailed(gCommandQueue->lpVtbl->Signal(gCommandQueue, gFence, fence));
	gCurrentFence++;

	// Wait until the previous frame is finished.
	if (gFence->lpVtbl->GetCompletedValue(gFence) < fence)
	{
		ThrowIfFailed(gFence->lpVtbl->SetEventOnCompletion(gFence, fence, gFenceEvent));
		WaitForSingleObject(gFenceEvent, INFINITE);
	}

	gFrameIndex = gSwapChain->lpVtbl->GetCurrentBackBufferIndex(gSwapChain);
}

ID3D12Resource* CreateCommittedResource(size_t size)
{
	D3D12_HEAP_PROPERTIES heapProperties =
	{
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		1,
		1
	};

	D3D12_RESOURCE_DESC VertexBufferDesc =
	{
		D3D12_RESOURCE_DIMENSION_BUFFER,
		0,
		size,
		1,
		1,
		1,
		DXGI_FORMAT_UNKNOWN,
		{1, 0},
		D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		D3D12_RESOURCE_FLAG_NONE
	};

	ID3D12Resource* pRes;

	ThrowIfFailed(gDevice->lpVtbl->CreateCommittedResource(gDevice,
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&VertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, // Clear value
		&IID_ID3D12Resource,
		(LPVOID*)&pRes));

	return pRes;
}

void InitConstantBuffer()
{
	UINT8* dataBegin;
	gUploadCBuffer->lpVtbl->Map(gUploadCBuffer, 0, NULL, (void**)&dataBegin);
	CBPEROBJECT cbPerObject;
	float mvpMat[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	memcpy(cbPerObject.gWorldViewProj, mvpMat, sizeof(mvpMat));
	memset(cbPerObject.c, 0, sizeof(cbPerObject.c));

	memcpy(dataBegin, &cbPerObject, sizeof(cbPerObject));
	gUploadCBuffer->lpVtbl->Unmap(gUploadCBuffer, 0, NULL);
}

void UpdateScale(float scale, float sec, float rot)
{
	UINT8* dataBegin;
	gUploadCBuffer->lpVtbl->Map(gUploadCBuffer, 0, NULL, (void**)&dataBegin);
	CBPEROBJECT cbPerObject;


	float dx = 2.0f / GRID_COUNT_X;
	float dy = 2.0f / GRID_COUNT_Y;

	int gWorldViewProjIndex = 0;

	for (int i = 0; i < GRID_COUNT_Y; i++)
	{
		for (int j = 0; j < GRID_COUNT_X; j++)
		{
			float st = max(0.8f, fabsf(sinf((float)sec + 1.1f * i + 2.2f * j)));

			float rotMat[16];
			if (gWorldViewProjIndex % 2 == 0)
			{
				MatRotY(rotMat, rot + i + j);
			}
			else
			{
				MatRotX(rotMat, rot + i + j);
			}


			float t1Mat[16];
			MatTranslate(t1Mat, -1.0f + dx / 2 + dx * j, 1.0f - dy / 2 - dy * i, 0);
			float t2Mat[16];
			MatTranslate(t2Mat, -(-1.0f + dx / 2 + dx * j), -(1.0f - dy / 2 - dy * i), 0);

			float s[16];
			MatScale(s, st);

			float m1[16];
			float m2[16];
			float m3[16];

			MatMult(m1, t1Mat, rotMat);
			MatMult(m2, m1, s);
			MatMult(m3, m2, t2Mat);

			float ss = sinf(((float)sec + i * cosf(sec / 10.0f) - j * sinf(sec / 20.0f)) * 3);
			float d = 150.0f;
			for (int j = 0; j < 4; j++)
			{
				cbPerObject.c[gWorldViewProjIndex][j] = ss / 255.0f * d;
			}

			memcpy(cbPerObject.gWorldViewProj[gWorldViewProjIndex], m3, sizeof(m3));
			gWorldViewProjIndex++;
		}
	}

	memcpy(dataBegin, &cbPerObject, sizeof(cbPerObject));
	gUploadCBuffer->lpVtbl->Unmap(gUploadCBuffer, 0, NULL);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);
	const HWND hWnd = CreateWindow(L"Battle", L"Battle", WS_POPUP | WS_VISIBLE, WIN_POS_X, WIN_POS_Y, WIN_WIDTH,
		WIN_HEIGHT, 0, 0, hInstance, 0);

	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	{
		ID3D12Debug* debugController;

		if (SUCCEEDED(D3D12GetDebugInterface(&IID_ID3D12Debug, (void**)&debugController)))
		{
			debugController->lpVtbl->EnableDebugLayer(debugController);

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		ReleaseCom(debugController);
	}
#endif

	IDXGIFactory4* pFactory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, &IID_IDXGIFactory4, (LPVOID*)&pFactory));

	ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, (LPVOID*)&gDevice));

	static D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(
		gDevice->lpVtbl->CreateCommandQueue(gDevice, &queueDesc, &IID_ID3D12CommandQueue, (LPVOID*)&gCommandQueue));

	static DXGI_SWAP_CHAIN_DESC1 descSwapChain;
	descSwapChain.BufferCount = FRAME_COUNT;
	descSwapChain.Width = WIN_WIDTH;
	descSwapChain.Height = WIN_HEIGHT;
	descSwapChain.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	descSwapChain.SampleDesc.Count = 1;

	IDXGISwapChain1* pSwapChain;
	pFactory->lpVtbl->CreateSwapChainForHwnd(pFactory,
		(IUnknown*)gCommandQueue,
		hWnd,
		&descSwapChain,
		NULL,
		NULL,
		&pSwapChain);

	ThrowIfFailed(pFactory->lpVtbl->MakeWindowAssociation(pFactory, hWnd, DXGI_MWA_NO_ALT_ENTER));

	pSwapChain->lpVtbl->QueryInterface(pSwapChain, &IID_IDXGISwapChain3, (LPVOID*)&gSwapChain);

	gFrameIndex = gSwapChain->lpVtbl->GetCurrentBackBufferIndex(gSwapChain);

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { 0, };
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(
			gDevice->lpVtbl->CreateDescriptorHeap(gDevice,
				&rtvHeapDesc, &IID_ID3D12DescriptorHeap,
				(LPVOID*)&gRtvHeap));

		gRtvDescriptorSize = gDevice->lpVtbl->GetDescriptorHandleIncrementSize(
			gDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		// C에서 호출할 때만 있는 버그다...
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			gRtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(gRtvHeap, &rtvHandle);

		for (UINT n = 0; n < FRAME_COUNT; n++)
		{
			ThrowIfFailed(
				gSwapChain->lpVtbl->GetBuffer(gSwapChain,
					n, &IID_ID3D12Resource, (LPVOID*)&gRenderTargets[n]));

			gDevice->lpVtbl->CreateRenderTargetView(gDevice,
				gRenderTargets[n], 0, rtvHandle);

			rtvHandle.ptr += gRtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = { 0, };
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(
			gDevice->lpVtbl->CreateDescriptorHeap(gDevice,
				&cbvHeapDesc, &IID_ID3D12DescriptorHeap, (LPVOID*)&gCbvHeap));

		gCbvDescriptorSize = gDevice->lpVtbl->GetDescriptorHandleIncrementSize(gDevice,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		const int uploadBufferSize = sizeof(CBPEROBJECT);
		gUploadCBuffer = CreateCommittedResource(uploadBufferSize);

		UpdateScale(1, 0, 0);

		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle;
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			gCbvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(gCbvHeap, &cbvHandle);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = gUploadCBuffer->lpVtbl->GetGPUVirtualAddress(gUploadCBuffer);
		cbvDesc.SizeInBytes = uploadBufferSize;
		gDevice->lpVtbl->CreateConstantBufferView(gDevice, &cbvDesc, cbvHandle);
	}


	gDevice->lpVtbl->CreateCommandAllocator(gDevice,
		D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (LPVOID*)&gCommandAllocator);
	gDevice->lpVtbl->CreateCommandAllocator(gDevice,
		D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (LPVOID*)&gCommandAllocator2);

	{
		ID3DBlob* pOutBlob;
		ID3DBlob* pErrorBlob;

		D3D12_DESCRIPTOR_RANGE cbvTableElement = { 0, };
		cbvTableElement.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvTableElement.NumDescriptors = 1;
		cbvTableElement.BaseShaderRegister = 0;

		D3D12_ROOT_DESCRIPTOR_TABLE rootDescriptorTable;
		rootDescriptorTable.NumDescriptorRanges = 1;
		rootDescriptorTable.pDescriptorRanges = &cbvTableElement;

		D3D12_ROOT_PARAMETER slotRootParameter[1];
		slotRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		slotRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameter[0].DescriptorTable = rootDescriptorTable;

		D3D12_ROOT_SIGNATURE_DESC descRootSignature =
		{
			1,
			slotRootParameter,
			0,
			NULL,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		};
		ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob));

		ThrowIfFailed(gDevice->lpVtbl->CreateRootSignature(gDevice,
			0, pOutBlob->lpVtbl->GetBufferPointer(pOutBlob),
			pOutBlob->lpVtbl->GetBufferSize(pOutBlob),
			&IID_ID3D12RootSignature, (LPVOID*)&gRootSignature));
	}

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	// #define 매크로를 셰이더 파일에서 정의한 것처럼 해 준다. 편리!
	D3D_SHADER_MACRO shaderMacros[] = {
		{ "GRID_COUNT_X", STR(GRID_COUNT_X)},
		{ "GRID_COUNT_Y", STR(GRID_COUNT_Y)},
		{ 0, 0 },
	};

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", shaderMacros, 0, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, 0));
	ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", shaderMacros, 0, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, 0));

	D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"RINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	UINT numElements = _countof(layout);

	static D3D12_RASTERIZER_DESC rasterizer =
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		FALSE,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		TRUE,
		FALSE,
		FALSE,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	static D3D12_BLEND_DESC blendState = {
		FALSE, FALSE,
		{
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0, };
	psoDesc.InputLayout.pInputElementDescs = layout;
	psoDesc.InputLayout.NumElements = numElements;
	psoDesc.pRootSignature = gRootSignature;
	psoDesc.VS.BytecodeLength = vertexShader->lpVtbl->GetBufferSize(vertexShader);
	psoDesc.VS.pShaderBytecode = vertexShader->lpVtbl->GetBufferPointer(vertexShader);
	psoDesc.PS.BytecodeLength = pixelShader->lpVtbl->GetBufferSize(pixelShader);
	psoDesc.PS.pShaderBytecode = pixelShader->lpVtbl->GetBufferPointer(pixelShader);
	psoDesc.RasterizerState = rasterizer;
	psoDesc.BlendState = blendState;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ThrowIfFailed(
		gDevice->lpVtbl->CreateGraphicsPipelineState(gDevice, &psoDesc, &IID_ID3D12PipelineState,
			(LPVOID*)&gPipelineState));

	ThrowIfFailed(gDevice->lpVtbl->CreateCommandList(gDevice, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, gCommandAllocator,
		gPipelineState, &IID_ID3D12GraphicsCommandList,
		(LPVOID*)&gCommandList));


	// Define the geometry for a triangle.
	// Vertex triangleVerts[] =
	// {
	// 	{{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	// 	{{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	// 	{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	//
	// 	{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	// 	{{1.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	// 	{{0.5f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	// };
	//

	UINT vertSize;
	Vertex* triangleVerts = AllocRectangularGridDoubleSided(GRID_COUNT_X, GRID_COUNT_Y, &vertSize);

	gBufVerts = CreateCommittedResource(vertSize);

	UINT8* dataBegin;
	gBufVerts->lpVtbl->Map(gBufVerts, 0, NULL, (void**)(&dataBegin));
	memcpy(dataBegin, triangleVerts, vertSize);
	gBufVerts->lpVtbl->Unmap(gBufVerts, 0, NULL);

	gDescViewBufVert.BufferLocation = gBufVerts->lpVtbl->GetGPUVirtualAddress(gBufVerts);
	gDescViewBufVert.StrideInBytes = sizeof(Vertex);
	gDescViewBufVert.SizeInBytes = vertSize;

	ThrowIfFailed(gCommandList->lpVtbl->Close(gCommandList));
	//ThrowIfFailed(gCommandList2->lpVtbl->Close(gCommandList2));

	ThrowIfFailed(gDevice->lpVtbl->CreateFence(gDevice, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (LPVOID*)&gFence));
	gCurrentFence = 1;

	gFenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

	QueryPerformanceFrequency((LARGE_INTEGER*)&gCountsPerSec);

	double secondsPerCount = 1.0 / (double)gCountsPerSec;

	static BOOL bRunning = TRUE;
	static MSG msg;

	double dsecAccum = 0;
	int dsecCount = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&gCurTime);

	while (bRunning)
	{
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRunning = FALSE;
		}

		Render(gCommandAllocator, gCommandList, gPipelineState, gRootSignature, gCbvHeap, gRenderTargets[gFrameIndex],
			&gViewport, &gRectScissor, gRtvHeap, gFrameIndex, gRtvDescriptorSize, &gDescViewBufVert, vertSize);

		ID3D12CommandList* ppCommandLists[] = { (ID3D12CommandList*)gCommandList };
		gCommandQueue->lpVtbl->ExecuteCommandLists(gCommandQueue, _countof(ppCommandLists), ppCommandLists);


		ThrowIfFailed(gSwapChain->lpVtbl->Present(gSwapChain, 1, 0));

		WaitForPreviousFrame();

		__int64 curTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

		double dsec = secondsPerCount * (curTime - gCurTime);

		dsecAccum += dsec;
		dsecCount++;
		if (dsecAccum > 1.0f)
		{
			char m[80];
			//wsprintf(m, L"Hello: %.2f", dsecAccum / dsecCount);
			//swprintf(
			sprintf_s(m, 80, "Hello: %f %d WTF\n", dsecAccum / dsecCount, 1985);
			OutputDebugStringA(m);
			dsecAccum = 0;
			dsecCount = 0;
		}

		gCurTime = curTime;

		double sec = secondsPerCount * gCurTime;

		float s = sinf((float)sec);
		float d = 30.0f;

		//UpdateScale(sinf((float)sec), 1, 1, 1, 1);
		UpdateScale(1.0f, (float)sec, (float)sec);
	}

	WaitForPreviousFrame();

	CloseHandle(gFenceEvent);


	ReleaseCom(pFactory);
	ReleaseCom(pSwapChain);
	ReleaseCom(gSwapChain);
	ReleaseCom(gDevice);
	ReleaseCom(gCommandQueue);
	ReleaseCom(gRtvHeap);
	for (UINT n = 0; n < FRAME_COUNT; n++)
	{
		ReleaseCom(gRenderTargets[n]);
	}
	ReleaseCom(gCommandAllocator);
	ReleaseCom(gCommandAllocator2);
	ReleaseCom(gCommandList);
	ReleaseCom(gCommandList2);
	ReleaseCom(gRootSignature);
	ReleaseCom(gPipelineState);
	ReleaseCom(gPipelineState2);
	ReleaseCom(gBufVerts);
	ReleaseCom(gFence);
	ReleaseCom(gCbvHeap);
	ReleaseCom(gUploadCBuffer);

	free(triangleVerts);

	return (int)msg.wParam;
	}
