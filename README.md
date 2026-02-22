# RTGL1

RTGL1 is a library that aims to simplify the process of porting 3D applications to *real-time path tracing*, via hardware accelerated ray tracing, denoising algorithms (A-SVGF) and sampling algorithms (ReSTIR, ReSTIR GI) to improve the image quality by reusing spatio-temporal data.

## Build
1. Requirements:
    * 64-bit CPU
    * GPU with a ray tracing support
    * [Git](https://github.com/git-for-windows/git/releases)
    * [CMake](https://cmake.org/download/)
    * [Vulkan SDK](https://vulkan.lunarg.com/)
    * [Python 3](https://www.python.org/downloads/) (for building the shaders)
    * Windows SDK >= 10.0.22621.0
 
1. Clone the repository with all submodules
    * `git clone  --recurse-submodules https://github.com/sultim-t/RayTracedGL1.git`

1. Configure with CMake
    * on Windows, with Visual Studio: 
        * open the folder as CMake project
    * otherwise:
        * specify windowing systems to build the library with, by enabling some of the CMake options:
            * `RG_WITH_SURFACE_WIN32`
            * `RG_WITH_SURFACE_METAL`
            * `RG_WITH_SURFACE_WAYLAND`
            * `RG_WITH_SURFACE_XCB`
            * `RG_WITH_SURFACE_XLIB`  
        * configure
        ```
        mkdir Build
        cd Build
        cmake ..
        ```
        * but make sure that projects that use RTGL1 can find the compiled dynamic library, as it usually assumed that it's in `Build/x64-Debug` or `Build/x64-Release`

1. Build
    * `cmake --build .`

1. Build shaders
    * Run `Source/Shaders/GenerateShaders.py` with Python3, it will generate SPIR-V files to `Build` folder

### Notes:
* RTGL1 requires a set of blue noise images on start-up: `RgInstanceCreateInfo::pBlueNoiseFilePath`. A ready-to-use resource can be found here: `Tools/BlueNoise_LDR_RGBA_128.ktx2`


## RtglExample (test application)

`RtglExample` is the sample executable built from `Tests/RtglTest.cpp`.  
It demonstrates RTGL initialization, camera + mesh upload, light upload, and frame rendering.

### Build and target
- The target is enabled when `RG_WITH_EXAMPLES=True` (default in this repository CMake settings).
- Output binary:
  - Debug: `Build/x64-Debug/RtglExample.exe`
  - Release/RelWithDebInfo: `Build/x64-Release/RtglExample.exe`

### Run methods
1. Visual Studio 2022 (CMake mode)
   - Open repository folder.
   - Select configuration: `x64-Debug` or `x64-Release`.
   - Select launch target: `RtglExample.exe`.
   - Press Run/Play.

1. Terminal
   - Default scene argument:
     - `.\Build\x64-Debug\RtglExample.exe`
   - Explicit scene argument:
     - `.\Build\x64-Debug\RtglExample.exe C:\path\to\scene.gltf`
     - `.\Build\x64-Debug\RtglExample.exe .\Build\_external_\Sponza\glTF\Sponza.gltf`

### Override folder and required runtime files
`RtglExample` is compiled with `ASSET_DIRECTORY="../"` in this repo, so the default override folder is the parent of the executable directory.

For `Build/x64-Debug/RtglExample.exe`, override folder resolves to `Build/`.

Recommended files in that override folder:
- `BlueNoise_LDR_RGBA_128.ktx2`
- `WaterNormal_n.ktx2`
- `DirtMask.ktx2`
- `SceneBuildWarning.ktx2`

Ready-to-use versions are in `Tools/` and can be copied to `Build/`.

### How scene loading works
`RtglExample` uses two scene paths:

1. Static scene import path (map-based, handled by RTGL scene importer)
   - `RgStartFrameInfo.pMapName` is hardcoded to `"untitled"` in the sample.
   - Expected file format:
     - `<override>/scenes/<map>/<map>.gltf`
   - With defaults, expected file is:
     - `Build/scenes/untitled/untitled.gltf`

2. Dynamic test model path (handled directly in `RtglTest.cpp` via tinygltf)
   - CLI argument `argv[1]` if present.
   - Otherwise defaults to:
     - `_external_/Sponza/glTF/Sponza.gltf`
   - This path is resolved under the override folder (`Build/_external_/Sponza/glTF/Sponza.gltf` with defaults).

Notes:
- This repository does not ship a full test scene package in-tree.
- If the file is missing, importer logs:
  - `Can't find a file, no static scene will be present: ...`
- Then scene import reports:
  - `New scene is empty`
- Static geometry is still rebuilt.

### What you see when scenes are missing
If static and dynamic scene files are absent, `RtglExample` still uploads fallback test geometry:
- a test cube
- a translucent rasterized quad/plane

This is expected behavior of the harness, and is useful only for sanity checks.

### Lighting in RtglExample
`RtglExample` uploads a directional "sun" every frame, and also configures sky rendering:
- sun intensity default is high (`ctl_SunIntensity`)
- sky defaults to cubemap mode in the sample, but no external sky cubemap is uploaded in current test code
- sky color multiplier (`ctl_SkyIntensity`) still contributes

If your view appears dark:
- verify camera direction and position
- raise sky/sun intensity with runtime controls (below)

### Runtime controls (keyboard)
Camera:
- `W/A/S/D`: move
- `Q/E`: down/up
- Arrow keys: look around

Lighting and visual parameters:
- `I` + keypad `+/-`: sun intensity
- `O` + keypad `+/-`: sky intensity
- `Y` + keypad `+/-`: test light intensity/count variables
- `R` + keypad `+/-`: roughness
- `M` + keypad `+/-`: metallicity
- `Tab`: toggle sky mode

Upscaling / frame generation / denoising:
- `U`: cycle upscaler mode (`FSR2`, `DLSS2`, `DLSS3 (no FG)`, `DLSS3 + FG`)
- `T`: cycle render resolution mode (`Ultra Performance`, `Performance`, `Balanced`, `Quality`, `Native AA`)
- `G`: cycle frame generation variant (`2x` / `3x`)
- `H`: cycle DLSS preset for the active render resolution mode (`Auto`, `F`, `J`, `K`, `L`, `M`)
- `J`: toggle denoiser (`ON` / `OFF`)

Other:
- `P`: fullscreen/windowed toggle
- `Z`: toggle cube animation

### Common troubleshooting
1. `Can't find a file, no static scene will be present: ../scenes\untitled\untitled.gltf`
   - Cause: static scene file missing.
   - Fix: create `Build/scenes/untitled/untitled.gltf`, or change map name in sample code.

1. `New scene is empty`
   - Cause: static importer loaded no scene.
   - Fix: same as above, and verify glTF + referenced buffers/textures are present.

1. `Can't load GLTF...` (from tinygltf path in the sample)
   - Cause: CLI/default dynamic glTF path not found or invalid.
   - Fix: pass a valid absolute glTF path on command line.

1. `SUPPRESS_TEXLAYERS shader macro is active...`
   - Cause: build is compiled with texture-layer suppression.
   - Effect: layer1/2/3 texcoords and multi-layer albedo settings are forced off.
   - Fix options:
     - keep sample layer flags disabled
     - or rebuild with `SUPPRESS_TEXLAYERS=0` in generated shader/common definitions

### Recommended test-scene setup
For stable visual validation:
1. Put a complete glTF scene (with textures and `.bin` files) in `Build/_external_/YourScene/...`.
1. Launch `RtglExample` with absolute glTF path as argument.
1. Also place a map scene at `Build/scenes/untitled/untitled.gltf` if you want to validate static import/export workflow.
1. Keep `RTGL1.json` in override folder (`Build/`) with `developerMode=true` for diagnostics.

## Tools

### Shader development

RTGL1 supports shader hot-reloading (a target application sets `RgStartFrameInfo::requestShaderReload=true` in runtime).

But to ease the process of *building* the shaders, instead of running `GenerateShaders.py` from a terminal manually, you can install [Visual Studio Code](https://code.visualstudio.com/) and [Script Runner extension](https://marketplace.visualstudio.com/items?itemName=easterapps.script-runner) to it. Open `Sources/Shaders` folder, add such config to VS Code's `.json` settings file (TODO: VS Code workspace).
```
"script-runner.definitions": { "commands": [ { "identifier": "shaderBuild", "description": "Build shaders", "command": "cls; python .\\GenerateShaders.py -ps", "working_directory": "${workspaceFolder}", }, { "identifier": "shaderGenAndBuild", "description": "Build shaders with generating common files", "command": "cls; python .\\GenerateShaders.py -ps -g", "working_directory": "${workspaceFolder}", } ], },
```
Then assign hotkeys to `shaderBuild` and `shaderGenAndBuild` commands in `File->Preferences->Keyboard Shortcuts`.

### Textures
Some games don't have PBR materials, but to add them, RTGL1 provides 'texture overriding' functionality: application requests to upload an original texture and specifies its name, then RTGL1 tries to find files with such name (appending some suffixes, e.g. `_n` for normal maps, or none for albedo maps) and loads them instead of original ones. These files are in `.ktx2` format with a specific compression and contain image data. 

To generate such textures: 
1. [Compressonator CLI](https://gpuopen.com/compressonator/) and `Python3` are required
1. Create a folder, put `Tools/CreateKTX2.py`, create folder named `Raw` and `Compressed`.
1. The script:
   1. scans files (with `INPUT_EXTENSIONS`) in `Raw` folder
   1. generates corresponding `.ktx2` file to `Compressed` folder, preserving the hierarchy

On RTGL1 initialization, `RgInstanceCreateInfo::pOverridenTexturesFolderPath` should contain a path to the `Compressed` folder. 



## Developer config (RTGL1.json)
RTGL1 reads an optional developer config file named `RTGL1.json` from the **override folder** passed to `RgInstanceCreateInfo::pOverrideFolderPath`.
This lets you enable validation layers, debug UI, and other diagnostics without changing code.

**Location**
- File name: `RTGL1.json`
- Directory: whatever your app passes as `pOverrideFolderPath`
- Typical layout: place `RTGL1.json` in the same folder you pass as the override folder (often alongside the RTGL DLL or your mod/config folder).
- **gzdoom-rt:** `pOverrideFolderPath` is set to `rt/` (relative to the executable), so place `RTGL1.json` in `gzdoom-rt/bin/rt/`.

**Supported keys (all optional)**
- `developerMode` (bool): enables RTGL’s dev UI (ImGui debug window) and developer tooling.
- `vulkanValidation` (bool): enables Vulkan validation layers.
- `dx12Validation` (bool): enables DX12 validation (used for DXGI/interop paths).
- `dlssValidation` (bool): enables extra DLSS logging/validation.
- `dlssForceDefaultPreset` (bool): forces DLSS to use the default preset.
- `fsrValidation` (bool): enables FSR debug validation.
- `fsr3async` (bool): enables FSR3 async workloads (if supported).
- `fpsMonitor` (bool): enables `VK_LAYER_LUNARG_monitor` FPS layer (requires it installed on the system).
- `dxgiToVkSwapchainSwitchHack` (bool): enables a swapchain switch workaround (defaults to true).

**Example**
```json
{
  "developerMode": true,
  "vulkanValidation": false,
  "fpsMonitor": false,
  "dlssValidation": false,
  "dlssForceDefaultPreset": false,
  "fsrValidation": false,
  "fsr3async": false,
  "dx12Validation": false,
  "dxgiToVkSwapchainSwitchHack": true
}
```

Notes:
- Unknown keys are ignored.
- If `developerMode` is enabled, the debug window shows a live FPS readout.
# Projects
* https://github.com/sultim-t/Serious-Engine-RT/releases
* https://github.com/sultim-t/prboom-plus-rt
* https://github.com/sultim-t/vkquake-rt
* https://github.com/sultim-t/xash-rt
