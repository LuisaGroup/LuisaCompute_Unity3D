_config_project({
	project_name = "lc-unity3d",
	project_kind = "shared"
})
local add_includedirs = _get_add_includedirs()
local add_defines = _get_add_defines()
add_deps("lc-runtime")
add_files("**.cpp")
add_syslinks("DXGI")
if DxBackend then
	add_defines("LC_UNITY3D_ENABLE_DX12", {
		public = true
	})
end
