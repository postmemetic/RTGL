// Copyright (c) 2026 Brian Schulman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Common.h"
#include "ResolutionState.h"
#include "Generated/ShaderCommonCFramebuf.h"

struct ID3D12GraphicsCommandList;

namespace RTGL1
{

class Framebuffers;

class DLSS45_DX12
{
public:
    struct EvaluateParams
    {
        ID3D12GraphicsCommandList* dx12cmd{ nullptr };
        uint32_t             frameIndex{ 0 };
        Framebuffers*        framebuffers{ nullptr };
        ResolutionState      resolution{};
        RgFloat2D            jitter{ 0, 0 };
        double               timeDelta{ 0.0 };
        bool                 resetHistory{ false };

        FramebufferImageIndex inputColor{ FB_IMAGE_INDEX_FINAL };
        FramebufferImageIndex output{ FB_IMAGE_INDEX_UPSCALED_PONG };
        FramebufferImageIndex motion{ FB_IMAGE_INDEX_MOTION_DLSS };
        FramebufferImageIndex depthLinear{ FB_IMAGE_INDEX_DEPTH_WORLD };
        FramebufferImageIndex specularMotion{ FB_IMAGE_INDEX_MOTION_DLSS };

        float preExposure{ 1.0f };
        bool  useRayReconstruction{ false };
    };

    static auto MakeInstance() -> std::shared_ptr< DLSS45_DX12 >
    {
        return std::shared_ptr< DLSS45_DX12 >( new DLSS45_DX12() );
    }

    bool IsActive() const { return isActive; }
    void SetActive( bool active ) { isActive = active; }

    auto Evaluate( const EvaluateParams& params ) -> std::optional< FramebufferImageIndex >
    {
        ( void )params;
        if( !isActive )
        {
            return std::nullopt;
        }
        return std::nullopt;
    }

private:
    DLSS45_DX12() = default;

private:
    bool isActive{ false };
};

}
