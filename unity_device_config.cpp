#include <unity3d/unity_device_config.h>
#include <core/stl/optional.h>
#ifdef LC_UNITY3D_ENABLE_DX12
#ifdef NDEBUG
#define ThrowIfFailed(x) (x)
#else
#define ThrowIfFailed(x)     \
    {                        \
        HRESULT hr_ = (x);   \
        assert(hr_ == S_OK); \
    }
#endif
using namespace Microsoft::WRL;
luisa::string local_path;
luisa::string plugin_path() {
    char buffer[MAX_PATH] = {0};
    GetDllDirectoryA(MAX_PATH, buffer);
    luisa::string str(buffer);
    if (!str.empty() && *(str.end() - 1) != '\\' && *(str.end() - 1) != '\\') {
        str += '/';
    }
    return str;
}
class EnginePathIniter {
public:
    EnginePathIniter() {
        local_path = plugin_path();
    }
};
static EnginePathIniter enginePathIniter;

UnityDeviceConfig::UnityDeviceConfig(IUnityInterfaces *unity_interface) noexcept {
    uint32_t dxgiFactoryFlags = 0;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(_dxgi_factory.GetAddressOf())));
    _unity_graphics = unity_interface->Get<IUnityGraphicsD3D12v5>();
    _device = _unity_graphics->GetDevice();
    for (auto adapterIndex = 0u; _dxgi_factory->EnumAdapters1(adapterIndex, _adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; adapterIndex++) {
        DXGI_ADAPTER_DESC1 desc;
        _adapter->GetDesc1(&desc);
        auto uid = _device->GetAdapterLuid();
        if (memcmp(&desc.AdapterLuid, &uid, sizeof(LUID)) == 0) {
            break;
        }
        _adapter = nullptr;
    }
}
ID3D12Device *UnityDeviceConfig::GetDevice() noexcept {
    return _device;
}
IDXGIAdapter1 *UnityDeviceConfig::GetAdapter() noexcept {
    return _adapter.Get();
}
IDXGIFactory4 *UnityDeviceConfig::GetDXGIFactory() noexcept {
    return _dxgi_factory.Get();
}
// queue is nullable
ID3D12CommandQueue *UnityDeviceConfig::CreateQueue(D3D12_COMMAND_LIST_TYPE type) noexcept {
    if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
        return _unity_graphics->GetCommandQueue();
    return nullptr;
}
bool UnityDeviceConfig::ExecuteCommandList(
    ID3D12CommandQueue *queue,
    ID3D12GraphicsCommandList *cmdList) noexcept {
    if (queue != _unity_graphics->GetCommandQueue()) return false;
    ID3D12CommandList *cmdListBase = cmdList;
    _unity_graphics->ExecuteCommandList(cmdList, _resource_states.size(), _resource_states.data());
    _resource_states.clear();
    return true;
}
void UnityDeviceConfig::regist_resource(
    ID3D12Resource *resource,
    D3D12_RESOURCE_STATES state) noexcept {
    _resource_states.emplace_back(UnityGraphicsD3D12ResourceState{
        resource,
        state,
        state});
}
static luisa::unique_ptr<UnityDeviceConfig> config;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces *unity_interface) {
    config = luisa::make_unique<UnityDeviceConfig>(unity_interface);
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload() {
    config = nullptr;
}
static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
    // TODO: load render events
}
extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}
#endif