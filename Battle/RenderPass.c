#include "RenderPass.h"
#include "ErrorUtil.h"
#include "Vertex.h"

void Render(ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList* commandList,
            ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, ID3D12DescriptorHeap* cbvHeap,
            ID3D12Resource* renderTarget, D3D12_VIEWPORT* viewport, D3D12_RECT* rectScissor,
            ID3D12DescriptorHeap* rtvHeap, UINT frameIndex, UINT rtvDescriptorSize, D3D12_VERTEX_BUFFER_VIEW* descViewBufVert, UINT vertSize)
{
	ThrowIfFailed(commandAllocator->lpVtbl->Reset(commandAllocator));
	ThrowIfFailed(commandList->lpVtbl->Reset(commandList, commandAllocator, pipelineState));


	commandList->lpVtbl->SetGraphicsRootSignature(commandList, rootSignature);
	ID3D12DescriptorHeap* descriptorHeaps[] = {cbvHeap};
	commandList->lpVtbl->SetDescriptorHeaps(commandList, 1, descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle;
	((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_GPU_DESCRIPTOR_HANDLE*))
		cbvHeap->lpVtbl->GetGPUDescriptorHandleForHeapStart)(cbvHeap, &cbvHandle);

	commandList->lpVtbl->SetGraphicsRootDescriptorTable(commandList, 0, cbvHandle);

	commandList->lpVtbl->RSSetViewports(commandList, 1, viewport);
	commandList->lpVtbl->RSSetScissorRects(commandList, 1, rectScissor);


	D3D12_RESOURCE_BARRIER barrierRTAsTexture =
	{
		D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		D3D12_RESOURCE_BARRIER_FLAG_NONE,
		{
			{
				renderTarget,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			}
		}
	};

	commandList->lpVtbl->ResourceBarrier(commandList, 1, &barrierRTAsTexture);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	((void(__stdcall*)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
		rtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(rtvHeap, &rtvHandle);

	rtvHandle.ptr += (SIZE_T)rtvDescriptorSize * frameIndex;

	// Record commands.
	float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	commandList->lpVtbl->ClearRenderTargetView(commandList, rtvHandle, clearColor, 0, NULL);

	commandList->lpVtbl->OMSetRenderTargets(commandList, 1, &rtvHandle, TRUE, NULL);
	commandList->lpVtbl->IASetPrimitiveTopology(commandList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->lpVtbl->IASetVertexBuffers(commandList, 0, 1, descViewBufVert);
	commandList->lpVtbl->DrawInstanced(commandList, vertSize / sizeof(Vertex), 1, 0, 0);

	D3D12_RESOURCE_BARRIER barrierRTForPresent =
	{
		D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		D3D12_RESOURCE_BARRIER_FLAG_NONE,
		{
			{
				renderTarget,
				D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			}
		}
	};

	commandList->lpVtbl->ResourceBarrier(commandList, 1, &barrierRTForPresent);

	ThrowIfFailed(commandList->lpVtbl->Close(commandList));
}
