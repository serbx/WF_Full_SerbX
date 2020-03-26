#pragma once

class Direct3D12VertexManager;

class Direct3D12Renderer final : public Renderer
{
private:
	struct FrameFence
	{
		ComPtr<ID3D12Fence> fence;
		HANDLE fence_event;
		uint64_t fence_value;
	};

private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGISwapChain3> m_swap_chain;
	ComPtr<ID3D12CommandQueue> m_command_queue;
	std::vector<ComPtr<ID3D12CommandAllocator>> m_command_allocators;
	std::vector<ComPtr<ID3D12GraphicsCommandList>> m_command_lists;
	ComPtr<ID3D12RootSignature> m_root_signature;
	ComPtr<ID3D12DescriptorHeap> m_desc_heap_rtv;
	ComPtr<ID3D12DescriptorHeap> m_desc_heap_srv;
	ComPtr<ID3D12PipelineState> m_pipeline_state;
	std::vector<ComPtr<ID3D12Resource>> m_render_views;

	FrameFence m_completion_fence;
	std::vector<FrameFence> m_frame_fences;
	uint64_t m_fence_value;

	uint32_t m_frame_index, m_frame_count;
	uint32_t m_desc_size_rtv, m_desc_size_srv;
	HANDLE m_fence_event;


	float width, height;

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_srv_desc_cpu_handle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE m_srv_desc_gpu_handle;
	std::unordered_map<uint32_t, std::unique_ptr<Direct3D12VertexManager>> m_fonts;
	bool m_initialized;
	bool CreateCommandList();
	bool CreateRenderTargets();
	bool CreateFrameFences();
	bool CreateRootSignature();
	bool CreatePipelineStateObjects();

public:
	Direct3D12Renderer(ComPtr<IDXGISwapChain3>&& swap_chain, ComPtr<ID3D12CommandQueue>&& command_queue);
	virtual ~Direct3D12Renderer();

	virtual HRESULT Init();

	virtual bool Begin(const int fps = 25);
	virtual void End();
	virtual void Present();

	virtual HRESULT LoadFont(const Font& font);
	virtual void FreeFont(const Font& font);

	virtual void XM_CALLCONV AddFilledRect(const DirectX::XMVECTOR &rect);

	virtual HRESULT AddText(const Font& font, float x, float y, float scale, const std::string& strText, DWORD flag = 0);
private:
	friend class Direct3D12VertexManager;
	void WaitForFence();
	void WaitForCompletion();
	void SignalQueue();
};
