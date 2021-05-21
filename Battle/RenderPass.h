#ifndef __RENDER_PASS_H_
#define __RENDER_PASS_H_

#include <d3d12.h>

void Render(ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList* commandList,
    ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, ID3D12DescriptorHeap* cbvHeap,
    ID3D12Resource* renderTarget, D3D12_VIEWPORT* viewport, D3D12_RECT* rectScissor,
    ID3D12DescriptorHeap* rtvHeap, UINT frameIndex, UINT rtvDescriptorSize, D3D12_VERTEX_BUFFER_VIEW* descViewBufVert, UINT vertSize);

#endif