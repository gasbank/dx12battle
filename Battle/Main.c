// https://github.com/wolfgangfengel/graphicsdemoskeleton/blob/master/DirectX%2012/02_Triangle/Window.c

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <math.h>
#include <time.h>

#pragma region GUID
#define DEFINE_GUIDW(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

DEFINE_GUIDW(IID_ID3D12Object, 0xc4fec28f, 0x7966, 0x4e95, 0x9f, 0x94, 0xf4, 0x31, 0xcb, 0x56, 0xc3, 0xb8);
DEFINE_GUIDW(IID_ID3D12DeviceChild, 0x905db94b, 0xa00c, 0x4140, 0x9d, 0xf5, 0x2b, 0x64, 0xca, 0x9e, 0xa3, 0x57);
DEFINE_GUIDW(IID_ID3D12RootSignature, 0xc54a6b66, 0x72df, 0x4ee8, 0x8b, 0xe5, 0xa9, 0x46, 0xa1, 0x42, 0x92, 0x14);
DEFINE_GUIDW(IID_ID3D12RootSignatureDeserializer, 0x34AB647B, 0x3CC8, 0x46AC, 0x84, 0x1B, 0xC0, 0x96, 0x56, 0x45, 0xC0,
             0x46);
DEFINE_GUIDW(IID_ID3D12Pageable, 0x63ee58fb, 0x1268, 0x4835, 0x86, 0xda, 0xf0, 0x08, 0xce, 0x62, 0xf0, 0xd6);
DEFINE_GUIDW(IID_ID3D12Heap, 0x6b3b2502, 0x6e51, 0x45b3, 0x90, 0xee, 0x98, 0x84, 0x26, 0x5e, 0x8d, 0xf3);
DEFINE_GUIDW(IID_ID3D12Resource, 0x696442be, 0xa72e, 0x4059, 0xbc, 0x79, 0x5b, 0x5c, 0x98, 0x04, 0x0f, 0xad);
DEFINE_GUIDW(IID_ID3D12CommandAllocator, 0x6102dee4, 0xaf59, 0x4b09, 0xb9, 0x99, 0xb4, 0x4d, 0x73, 0xf0, 0x9b, 0x24);
DEFINE_GUIDW(IID_ID3D12Fence, 0x0a753dcf, 0xc4d8, 0x4b91, 0xad, 0xf6, 0xbe, 0x5a, 0x60, 0xd9, 0x5a, 0x76);
DEFINE_GUIDW(IID_ID3D12PipelineState, 0x765a30f3, 0xf624, 0x4c6f, 0xa8, 0x28, 0xac, 0xe9, 0x48, 0x62, 0x24, 0x45);
DEFINE_GUIDW(IID_ID3D12DescriptorHeap, 0x8efb471d, 0x616c, 0x4f49, 0x90, 0xf7, 0x12, 0x7b, 0xb7, 0x63, 0xfa, 0x51);
DEFINE_GUIDW(IID_ID3D12QueryHeap, 0x0d9658ae, 0xed45, 0x469e, 0xa6, 0x1d, 0x97, 0x0e, 0xc5, 0x83, 0xca, 0xb4);
DEFINE_GUIDW(IID_ID3D12CommandSignature, 0xc36a797c, 0xec80, 0x4f0a, 0x89, 0x85, 0xa7, 0xb2, 0x47, 0x50, 0x82, 0xd1);
DEFINE_GUIDW(IID_ID3D12CommandList, 0x7116d91c, 0xe7e4, 0x47ce, 0xb8, 0xc6, 0xec, 0x81, 0x68, 0xf4, 0x37, 0xe5);
DEFINE_GUIDW(IID_ID3D12GraphicsCommandList, 0x5b160d0f, 0xac1b, 0x4185, 0x8b, 0xa8, 0xb3, 0xae, 0x42, 0xa5, 0xa4, 0x55);
DEFINE_GUIDW(IID_ID3D12CommandQueue, 0x0ec870a6, 0x5d7e, 0x4c22, 0x8c, 0xfc, 0x5b, 0xaa, 0xe0, 0x76, 0x16, 0xed);
DEFINE_GUIDW(IID_ID3D12Device, 0x189819f1, 0x1db6, 0x4b57, 0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7);

#ifdef _DEBUG
// d3d12sdklayers.h
DEFINE_GUIDW(DXGI_DEBUG_D3D12, 0xcf59a98c, 0xa950, 0x4326, 0x91, 0xef, 0x9b, 0xba, 0xa1, 0x7b, 0xfd, 0x95);

DEFINE_GUIDW(IID_ID3D12Debug, 0x344488b7, 0x6846, 0x474b, 0xb9, 0x89, 0xf0, 0x27, 0x44, 0x82, 0x45, 0xe0);
DEFINE_GUIDW(IID_ID3D12DebugDevice, 0x3febd6dd, 0x4973, 0x4787, 0x81, 0x94, 0xe4, 0x5f, 0x9e, 0x28, 0x92, 0x3e);
DEFINE_GUIDW(IID_ID3D12DebugCommandQueue, 0x09e0bf36, 0x54ac, 0x484f, 0x88, 0x47, 0x4b, 0xae, 0xea, 0xb6, 0x05, 0x3a);
DEFINE_GUIDW(IID_ID3D12DebugCommandList, 0x09e0bf36, 0x54ac, 0x484f, 0x88, 0x47, 0x4b, 0xae, 0xea, 0xb6, 0x05, 0x3f);
DEFINE_GUIDW(IID_ID3D12InfoQueue, 0x0742a90b, 0xc387, 0x483f, 0xb9, 0x46, 0x30, 0xa7, 0xe4, 0xe6, 0x14, 0x58);
#endif

// dxgi1_4.h
DEFINE_GUIDW(IID_IDXGISwapChain3, 0x94d99bdb, 0xf1f8, 0x4ab0, 0xb2, 0x36, 0x7d, 0xa0, 0x17, 0x0e, 0xda, 0xb1);
DEFINE_GUIDW(IID_IDXGIOutput4, 0xdc7dca35, 0x2196, 0x414d, 0x9F, 0x53, 0x61, 0x78, 0x84, 0x03, 0x2a, 0x60);
DEFINE_GUIDW(IID_IDXGIFactory4, 0x1bc6ea02, 0xef36, 0x464f, 0xbf, 0x0c, 0x21, 0xca, 0x39, 0xe5, 0x16, 0x8a);
DEFINE_GUIDW(IID_IDXGIAdapter3, 0x645967A4, 0x1392, 0x4310, 0xA7, 0x98, 0x80, 0x53, 0xCE, 0x3E, 0x93, 0xFD);

#pragma endregion

#define WIN_WIDTH 800``
#define WIN_HEIGHT 600
#define WIN_POS_X 200
#define WIN_POS_Y 200
#define FRAME_COUNT 2

inline void ThrowIfFailed(HRESULT hr)
{
#if defined(_DEBUG)
	if (hr != S_OK)
	{
		MessageBoxA(NULL, "Function call failed", "Error", MB_OK | MB_ICONERROR);
	}
#endif
}

ID3D12Device* gDevice;
ID3D12CommandQueue* gCommandQueue;
IDXGISwapChain3* gSwapChain;
UINT gFrameIndex;
ID3D12DescriptorHeap* gRtvHeap;
UINT gRtvDescriptorSize;
ID3D12Resource* gRenderTargets[FRAME_COUNT];
ID3D12CommandAllocator* gCommandAllocator;
ID3D12GraphicsCommandList* gCommandList;
ID3D12RootSignature* gRootSignature;
ID3D12PipelineState* gPipelineState;
ID3D12Resource* gBufVerts;
D3D12_VERTEX_BUFFER_VIEW gDescViewBufVert;
ID3D12Fence* gFence;
static UINT64 gCurrentFence;
HANDLE gFenceEvent;
ID3D12DescriptorHeap* gCbvHeap;
UINT gCbvDescriptorSize;
ID3D12Resource* gUploadCBuffer;
__int64 gCurTime;
__int64 gCountsPerSec;

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

void UpdateScale(float scale, float c0, float c1, float c2, float c3)
{
	UINT8* dataBegin;
	gUploadCBuffer->lpVtbl->Map(gUploadCBuffer, 0, NULL, (void**)&dataBegin);
	float mvpMat[] = {
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		c0, c1, c2, c3
	};
	memcpy(dataBegin, mvpMat, sizeof(mvpMat));
	gUploadCBuffer->lpVtbl->Unmap(gUploadCBuffer, 0, NULL);
}

typedef struct
{
	float position[3];
	float color[4];
} Vertex;

void SetVertexData(Vertex* pV, float x, float y, float z, float cr, float cg, float cb, float ca)
{
	pV->position[0] = x; pV->position[1] = y; pV->position[2] = z;
	pV->color[0] = cr; pV->color[1] = cg; pV->color[2] = cb; pV->color[3] = ca;
}

Vertex* AllocRectangularGrid(int xCount, int yCount, UINT* s)
{
	if (xCount < 1 || yCount < 1)
	{
		*s = 0;
		return NULL;
	}
	
	int vertexCount = xCount * yCount * 4 * 3;
	Vertex* v = malloc(sizeof(Vertex) * vertexCount);
	float dx = 2.0f / xCount;
	float dy = 2.0f / yCount;
	Vertex* pV = v;
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			float xc = (-1.0f + dx / 2 + dx * x);
			float yc = (+1.0f - dy / 2 - dy * y);

			// 동
			SetVertexData(pV, xc, yc, 0, 1, 0, 0, 1);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc + dy / 2, 0, 1, 0, 0, 1);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc - dy / 2, 0, 1, 0, 0, 1);
			pV++;

			// 서
			SetVertexData(pV, xc, yc, 0, 0, 1, 0, 1);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc - dy / 2, 0, 0, 1, 0, 1);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc + dy / 2, 0, 0, 1, 0, 1);
			pV++;

			// 남
			SetVertexData(pV, xc, yc, 0, 0, 0, 1, 1);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc - dy / 2, 0, 0, 0, 1, 1);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc - dy / 2, 0, 0, 0, 1, 1);
			pV++;

			// 북
			SetVertexData(pV, xc, yc, 0, 1, 1, 1, 1);
			pV++;
			SetVertexData(pV, xc - dx / 2, yc + dy / 2, 0, 1, 1, 1, 1);
			pV++;
			SetVertexData(pV, xc + dx / 2, yc + dy / 2, 0, 1, 1, 1, 1);
			pV++;
		}
	}
	*s = sizeof(Vertex) * xCount * yCount * 4 * 3;
	return v;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);
	const HWND hWnd = CreateWindow(L"Battle", L"Battle", WS_POPUP | WS_VISIBLE, WIN_POS_X, WIN_POS_Y, WIN_WIDTH,
	                               WIN_HEIGHT,
	                               0, 0, hInstance, 0);

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
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {0,};
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(
			gDevice->lpVtbl->CreateDescriptorHeap(gDevice, &rtvHeapDesc, &IID_ID3D12DescriptorHeap,
			                                      (LPVOID*)&gRtvHeap));

		gRtvDescriptorSize = gDevice->lpVtbl->GetDescriptorHandleIncrementSize(gDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		// C에서 호출할 때만 있는 버그다...
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			gRtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(gRtvHeap, &rtvHandle);

		for (UINT n = 0; n < FRAME_COUNT; n++)
		{
			ThrowIfFailed(
				gSwapChain->lpVtbl->GetBuffer(gSwapChain, n, &IID_ID3D12Resource, (LPVOID*)&gRenderTargets[n]));
			gDevice->lpVtbl->CreateRenderTargetView(gDevice, gRenderTargets[n], 0, rtvHandle);
			rtvHandle.ptr += gRtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {0,};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(
			gDevice->lpVtbl->CreateDescriptorHeap(gDevice, &cbvHeapDesc, &IID_ID3D12DescriptorHeap,
			                                      (LPVOID*)&gCbvHeap));

		gCbvDescriptorSize = gDevice->lpVtbl->GetDescriptorHandleIncrementSize(gDevice,
		                                                                       D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		const int uploadBufferSize = 256;
		gUploadCBuffer = CreateCommittedResource(uploadBufferSize);

		UpdateScale(0.1f, 1, 1, 1, 1);

		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle;
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			gCbvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(gCbvHeap, &cbvHandle);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = gUploadCBuffer->lpVtbl->GetGPUVirtualAddress(gUploadCBuffer);
		cbvDesc.SizeInBytes = uploadBufferSize;
		gDevice->lpVtbl->CreateConstantBufferView(gDevice, &cbvDesc, cbvHandle);
	}


	gDevice->lpVtbl->CreateCommandAllocator(gDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator,
	                                        (LPVOID*)&gCommandAllocator);


	{
		ID3DBlob* pOutBlob;
		ID3DBlob* pErrorBlob;

		D3D12_DESCRIPTOR_RANGE cbvTableElement = {0,};
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
		ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob,
		                                          &pErrorBlob));
		ThrowIfFailed(gDevice->lpVtbl->CreateRootSignature(gDevice, 0, pOutBlob->lpVtbl->GetBufferPointer(pOutBlob),
		                                                   pOutBlob->lpVtbl->GetBufferSize(pOutBlob),
		                                                   &IID_ID3D12RootSignature,
		                                                   (LPVOID*)&gRootSignature));
	}

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", 0, 0, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, 0));
	ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", 0, 0, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, 0));

	D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {0,};
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


	D3D12_VIEWPORT mViewport = {0.0f, 0.0f, (float)(WIN_WIDTH), (float)(WIN_HEIGHT), 0.0f, 1.0f};
	D3D12_RECT mRectScissor = {0, 0, (LONG)(WIN_WIDTH), (LONG)(WIN_HEIGHT)};

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
	Vertex* triangleVerts = AllocRectangularGrid(8, 6, &vertSize);

	gBufVerts = CreateCommittedResource(vertSize);

	UINT8* dataBegin;
	gBufVerts->lpVtbl->Map(gBufVerts, 0, NULL, (void**)(&dataBegin));
	memcpy(dataBegin, triangleVerts, vertSize);
	gBufVerts->lpVtbl->Unmap(gBufVerts, 0, NULL);

	gDescViewBufVert.BufferLocation = gBufVerts->lpVtbl->GetGPUVirtualAddress(gBufVerts);
	gDescViewBufVert.StrideInBytes = sizeof(Vertex);
	gDescViewBufVert.SizeInBytes = vertSize;

	ThrowIfFailed(gCommandList->lpVtbl->Close(gCommandList));

	ThrowIfFailed(gDevice->lpVtbl->CreateFence(gDevice, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (LPVOID*)&gFence));
	gCurrentFence = 1;

	gFenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

	QueryPerformanceFrequency((LARGE_INTEGER*)&gCountsPerSec);

	double secondsPerCount = 1.0 / (double)gCountsPerSec;

	static BOOL bRunning = TRUE;
	static MSG msg;
	while (bRunning)
	{
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRunning = FALSE;
		}

		ThrowIfFailed(gCommandAllocator->lpVtbl->Reset(gCommandAllocator));
		ThrowIfFailed(gCommandList->lpVtbl->Reset(gCommandList, gCommandAllocator, gPipelineState));

		gCommandList->lpVtbl->SetGraphicsRootSignature(gCommandList, gRootSignature);
		ID3D12DescriptorHeap* descriptorHeaps[] = { gCbvHeap };
		gCommandList->lpVtbl->SetDescriptorHeaps(gCommandList, 1, descriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle;
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_GPU_DESCRIPTOR_HANDLE*))
			gCbvHeap->lpVtbl->GetGPUDescriptorHandleForHeapStart)(gCbvHeap, &cbvHandle);

		gCommandList->lpVtbl->SetGraphicsRootDescriptorTable(gCommandList, 0, cbvHandle);
		
		gCommandList->lpVtbl->RSSetViewports(gCommandList, 1, &mViewport);
		gCommandList->lpVtbl->RSSetScissorRects(gCommandList, 1, &mRectScissor);


		D3D12_RESOURCE_BARRIER barrierRTAsTexture =
		{
			D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			D3D12_RESOURCE_BARRIER_FLAG_NONE,
			{
				{
					gRenderTargets[gFrameIndex],
					D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
					D3D12_RESOURCE_STATE_PRESENT,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				}
			}
		};

		gCommandList->lpVtbl->ResourceBarrier(gCommandList, 1, &barrierRTAsTexture);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			gRtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(gRtvHeap, &rtvHandle);

		rtvHandle.ptr += (SIZE_T)gRtvDescriptorSize * gFrameIndex;

		// Record commands.
		float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
		gCommandList->lpVtbl->ClearRenderTargetView(gCommandList, rtvHandle, clearColor, 0, NULL);

		gCommandList->lpVtbl->OMSetRenderTargets(gCommandList, 1, &rtvHandle, TRUE, NULL);
		gCommandList->lpVtbl->IASetPrimitiveTopology(gCommandList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		gCommandList->lpVtbl->IASetVertexBuffers(gCommandList, 0, 1, &gDescViewBufVert);
		gCommandList->lpVtbl->DrawInstanced(gCommandList, vertSize / sizeof(Vertex), 1, 0, 0);

		D3D12_RESOURCE_BARRIER barrierRTForPresent =
		{
			D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			D3D12_RESOURCE_BARRIER_FLAG_NONE,
			{
				{
					gRenderTargets[gFrameIndex],
					D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PRESENT
				}
			}
		};

		gCommandList->lpVtbl->ResourceBarrier(gCommandList, 1, &barrierRTForPresent);

		ThrowIfFailed(gCommandList->lpVtbl->Close(gCommandList));

		ID3D12CommandList* ppCommandLists[] = {(ID3D12CommandList*)gCommandList};
		gCommandQueue->lpVtbl->ExecuteCommandLists(gCommandQueue, _countof(ppCommandLists), ppCommandLists);

		ThrowIfFailed(gSwapChain->lpVtbl->Present(gSwapChain, 1, 0));

		WaitForPreviousFrame();

		QueryPerformanceCounter((LARGE_INTEGER*)&gCurTime);

		double sec = secondsPerCount * gCurTime;
		
		//UpdateScale(sinf((float)sec), 1, 1, 1, 1);
		UpdateScale(1, 1, 1, 1, 1);
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
	ReleaseCom(gCommandList);
	ReleaseCom(gRootSignature);
	ReleaseCom(gPipelineState);
	ReleaseCom(gBufVerts);
	ReleaseCom(gFence);
	ReleaseCom(gCbvHeap);
	ReleaseCom(gUploadCBuffer);

	free(triangleVerts);
	
	return (int)msg.wParam;
}
