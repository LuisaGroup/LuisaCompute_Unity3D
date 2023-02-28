#pragma once

#include <unity3d/i_unity_graphics.h>
#ifdef LC_UNITY3D_ENABLE_DX12
#include <unity3d/i_unity_d3d12.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <comdef.h>
#include <backends/dx/ext_settings.h>
#include <wrl/client.h>

class UnityDeviceConfig : public luisa::compute::DirectXDeviceConfigExt {
private:
    Microsoft::WRL::ComPtr<IDXGIAdapter1> _adapter;
    Microsoft::WRL::ComPtr<IDXGIFactory4> _dxgi_factory;
    ID3D12Device *_device{nullptr};
    IUnityGraphicsD3D12v5 *_unity_graphics{nullptr};
    luisa::vector<UnityGraphicsD3D12ResourceState> _resource_states;

public:
    [[nodiscard]] auto unity_graphics() const noexcept { return _unity_graphics; }
    [[nodiscard]] auto adapter() const noexcept { return _adapter.Get(); }
    [[nodiscard]] auto dxgi_factory() const noexcept { return _dxgi_factory.Get(); }
    [[nodiscard]] auto device() const noexcept { return _device; }
    UnityDeviceConfig(IUnityInterfaces *unity_interface) noexcept;
    ~UnityDeviceConfig() noexcept = default;
    ID3D12Device *GetDevice() noexcept override;
    IDXGIAdapter1 *GetAdapter() noexcept override;
    IDXGIFactory4 *GetDXGIFactory() noexcept override;
    ID3D12CommandQueue *CreateQueue(D3D12_COMMAND_LIST_TYPE type) noexcept override;
    bool ExecuteCommandList(
        ID3D12CommandQueue *queue,
        ID3D12GraphicsCommandList *cmdList) noexcept override;
    void regist_resource(
        ID3D12Resource *resource,
        D3D12_RESOURCE_STATES state) noexcept;
};
#endif