//
// Created by Novak on 31/08/2019.
//

#include <eagle/renderer/vulkan/vulkan_converter.h>

namespace eagle {

VkFormat VulkanConverter::to_vk(Format format) {

    VkFormat result = VK_FORMAT_UNDEFINED;
    switch(format){
        case Format::UNDEFINED: result = VK_FORMAT_UNDEFINED; break;
        case Format::R4G4_UNORM_PACK8: result = VK_FORMAT_R4G4_UNORM_PACK8; break;
        case Format::R4G4B4A4_UNORM_PACK16: result = VK_FORMAT_R4G4B4A4_UNORM_PACK16; break;
        case Format::B4G4R4A4_UNORM_PACK16: result = VK_FORMAT_B4G4R4A4_UNORM_PACK16; break;
        case Format::R5G6B5_UNORM_PACK16: result = VK_FORMAT_R5G6B5_UNORM_PACK16; break;
        case Format::B5G6R5_UNORM_PACK16: result = VK_FORMAT_B5G6R5_UNORM_PACK16; break;
        case Format::R5G5B5A1_UNORM_PACK16: result = VK_FORMAT_R5G5B5A1_UNORM_PACK16; break;
        case Format::B5G5R5A1_UNORM_PACK16: result = VK_FORMAT_B5G5R5A1_UNORM_PACK16; break;
        case Format::A1R5G5B5_UNORM_PACK16: result = VK_FORMAT_A1R5G5B5_UNORM_PACK16; break;
        case Format::R8_UNORM: result = VK_FORMAT_R8_UNORM; break;
        case Format::R8_SNORM: result = VK_FORMAT_R8_SNORM; break;
        case Format::R8_USCALED: result = VK_FORMAT_R8_USCALED; break;
        case Format::R8_SSCALED: result = VK_FORMAT_R8_SSCALED; break;
        case Format::R8_UINT: result = VK_FORMAT_R8_UINT; break;
        case Format::R8_SINT: result = VK_FORMAT_R8_SINT; break;
        case Format::R8_SRGB: result = VK_FORMAT_R8_SRGB; break;
        case Format::R8G8_UNORM: result = VK_FORMAT_R8G8_UNORM; break;
        case Format::R8G8_SNORM: result = VK_FORMAT_R8G8_SNORM; break;
        case Format::R8G8_USCALED: result = VK_FORMAT_R8G8_USCALED; break;
        case Format::R8G8_SSCALED: result = VK_FORMAT_R8G8_SSCALED; break;
        case Format::R8G8_UINT: result = VK_FORMAT_R8G8_UINT; break;
        case Format::R8G8_SINT: result = VK_FORMAT_R8G8_SINT; break;
        case Format::R8G8_SRGB: result = VK_FORMAT_R8G8_SRGB; break;
        case Format::R8G8B8_UNORM: result = VK_FORMAT_R8G8B8_UNORM; break;
        case Format::R8G8B8_SNORM: result = VK_FORMAT_R8G8B8_SNORM; break;
        case Format::R8G8B8_USCALED: result = VK_FORMAT_R8G8B8_USCALED; break;
        case Format::R8G8B8_SSCALED: result = VK_FORMAT_R8G8B8_SSCALED; break;
        case Format::R8G8B8_UINT: result = VK_FORMAT_R8G8B8_UINT; break;
        case Format::R8G8B8_SINT: result = VK_FORMAT_R8G8B8_SINT; break;
        case Format::R8G8B8_SRGB: result = VK_FORMAT_R8G8B8_SRGB; break;
        case Format::B8G8R8_UNORM: result = VK_FORMAT_B8G8R8_UNORM; break;
        case Format::B8G8R8_SNORM: result = VK_FORMAT_B8G8R8_SNORM; break;
        case Format::B8G8R8_USCALED: result = VK_FORMAT_B8G8R8_USCALED; break;
        case Format::B8G8R8_SSCALED: result = VK_FORMAT_B8G8R8_SSCALED; break;
        case Format::B8G8R8_UINT: result = VK_FORMAT_B8G8R8_UINT; break;
        case Format::B8G8R8_SINT: result = VK_FORMAT_B8G8R8_SINT; break;
        case Format::B8G8R8_SRGB: result = VK_FORMAT_B8G8R8_SRGB; break;
        case Format::R8G8B8A8_UNORM: result = VK_FORMAT_R8G8B8A8_UNORM; break;
        case Format::R8G8B8A8_SNORM: result = VK_FORMAT_R8G8B8A8_SNORM; break;
        case Format::R8G8B8A8_USCALED: result = VK_FORMAT_R8G8B8A8_USCALED; break;
        case Format::R8G8B8A8_SSCALED: result = VK_FORMAT_R8G8B8A8_SSCALED; break;
        case Format::R8G8B8A8_UINT: result = VK_FORMAT_R8G8B8A8_UINT; break;
        case Format::R8G8B8A8_SINT: result = VK_FORMAT_R8G8B8A8_SINT; break;
        case Format::R8G8B8A8_SRGB: result = VK_FORMAT_R8G8B8A8_SRGB; break;
        case Format::B8G8R8A8_UNORM: result = VK_FORMAT_B8G8R8A8_UNORM; break;
        case Format::B8G8R8A8_SNORM: result = VK_FORMAT_B8G8R8A8_SNORM; break;
        case Format::B8G8R8A8_USCALED: result = VK_FORMAT_B8G8R8A8_USCALED; break;
        case Format::B8G8R8A8_SSCALED: result = VK_FORMAT_B8G8R8A8_SSCALED; break;
        case Format::B8G8R8A8_UINT: result = VK_FORMAT_B8G8R8A8_UINT; break;
        case Format::B8G8R8A8_SINT: result = VK_FORMAT_B8G8R8A8_SINT; break;
        case Format::B8G8R8A8_SRGB: result = VK_FORMAT_B8G8R8A8_SRGB; break;
        case Format::A8B8G8R8_UNORM_PACK32: result = VK_FORMAT_A8B8G8R8_UNORM_PACK32; break;
        case Format::A8B8G8R8_SNORM_PACK32: result = VK_FORMAT_A8B8G8R8_SNORM_PACK32; break;
        case Format::A8B8G8R8_USCALED_PACK32: result = VK_FORMAT_A8B8G8R8_USCALED_PACK32; break;
        case Format::A8B8G8R8_SSCALED_PACK32: result = VK_FORMAT_A8B8G8R8_SSCALED_PACK32; break;
        case Format::A8B8G8R8_UINT_PACK32: result = VK_FORMAT_A8B8G8R8_UINT_PACK32; break;
        case Format::A8B8G8R8_SINT_PACK32: result = VK_FORMAT_A8B8G8R8_SINT_PACK32; break;
        case Format::A8B8G8R8_SRGB_PACK32: result = VK_FORMAT_A8B8G8R8_SRGB_PACK32; break;
        case Format::A2R10G10B10_UNORM_PACK32: result = VK_FORMAT_A2R10G10B10_UNORM_PACK32; break;
        case Format::A2R10G10B10_SNORM_PACK32: result = VK_FORMAT_A2R10G10B10_SNORM_PACK32; break;
        case Format::A2R10G10B10_USCALED_PACK32: result = VK_FORMAT_A2R10G10B10_USCALED_PACK32; break;
        case Format::A2R10G10B10_SSCALED_PACK32: result = VK_FORMAT_A2R10G10B10_SSCALED_PACK32; break;
        case Format::A2R10G10B10_UINT_PACK32: result = VK_FORMAT_A2R10G10B10_UINT_PACK32; break;
        case Format::A2R10G10B10_SINT_PACK32: result = VK_FORMAT_A2R10G10B10_SINT_PACK32; break;
        case Format::A2B10G10R10_UNORM_PACK32: result = VK_FORMAT_A2B10G10R10_UNORM_PACK32; break;
        case Format::A2B10G10R10_SNORM_PACK32: result = VK_FORMAT_A2B10G10R10_SNORM_PACK32; break;
        case Format::A2B10G10R10_USCALED_PACK32: result = VK_FORMAT_A2B10G10R10_USCALED_PACK32; break;
        case Format::A2B10G10R10_SSCALED_PACK32: result = VK_FORMAT_A2B10G10R10_SSCALED_PACK32; break;
        case Format::A2B10G10R10_UINT_PACK32: result = VK_FORMAT_A2B10G10R10_UINT_PACK32; break;
        case Format::A2B10G10R10_SINT_PACK32: result = VK_FORMAT_A2B10G10R10_SINT_PACK32; break;
        case Format::R16_UNORM: result = VK_FORMAT_R16_UNORM; break;
        case Format::R16_SNORM: result = VK_FORMAT_R16_SNORM; break;
        case Format::R16_USCALED: result = VK_FORMAT_R16_USCALED; break;
        case Format::R16_SSCALED: result = VK_FORMAT_R16_SSCALED; break;
        case Format::R16_UINT: result = VK_FORMAT_R16_UINT; break;
        case Format::R16_SINT: result = VK_FORMAT_R16_SINT; break;
        case Format::R16_SFLOAT: result = VK_FORMAT_R16_SFLOAT; break;
        case Format::R16G16_UNORM: result = VK_FORMAT_R16G16_UNORM; break;
        case Format::R16G16_SNORM: result = VK_FORMAT_R16G16_SNORM; break;
        case Format::R16G16_USCALED: result = VK_FORMAT_R16G16_USCALED; break;
        case Format::R16G16_SSCALED: result = VK_FORMAT_R16G16_SSCALED; break;
        case Format::R16G16_UINT: result = VK_FORMAT_R16G16_UINT; break;
        case Format::R16G16_SINT: result = VK_FORMAT_R16G16_SINT; break;
        case Format::R16G16_SFLOAT: result = VK_FORMAT_R16G16_SFLOAT; break;
        case Format::R16G16B16_UNORM: result = VK_FORMAT_R16G16B16_UNORM; break;
        case Format::R16G16B16_SNORM: result = VK_FORMAT_R16G16B16_SNORM; break;
        case Format::R16G16B16_USCALED: result = VK_FORMAT_R16G16B16_USCALED; break;
        case Format::R16G16B16_SSCALED: result = VK_FORMAT_R16G16B16_SSCALED; break;
        case Format::R16G16B16_UINT: result = VK_FORMAT_R16G16B16_UINT; break;
        case Format::R16G16B16_SINT: result = VK_FORMAT_R16G16B16_SINT; break;
        case Format::R16G16B16_SFLOAT: result = VK_FORMAT_R16G16B16_SFLOAT; break;
        case Format::R16G16B16A16_UNORM: result = VK_FORMAT_R16G16B16A16_UNORM; break;
        case Format::R16G16B16A16_SNORM: result = VK_FORMAT_R16G16B16A16_SNORM; break;
        case Format::R16G16B16A16_USCALED: result = VK_FORMAT_R16G16B16A16_USCALED; break;
        case Format::R16G16B16A16_SSCALED: result = VK_FORMAT_R16G16B16A16_SSCALED; break;
        case Format::R16G16B16A16_UINT: result = VK_FORMAT_R16G16B16A16_UINT; break;
        case Format::R16G16B16A16_SINT: result = VK_FORMAT_R16G16B16A16_SINT; break;
        case Format::R16G16B16A16_SFLOAT: result = VK_FORMAT_R16G16B16A16_SFLOAT; break;
        case Format::R32_UINT: result = VK_FORMAT_R32_UINT; break;
        case Format::R32_SINT: result = VK_FORMAT_R32_SINT; break;
        case Format::R32_SFLOAT: result = VK_FORMAT_R32_SFLOAT; break;
        case Format::R32G32_UINT: result = VK_FORMAT_R32G32_UINT; break;
        case Format::R32G32_SINT: result = VK_FORMAT_R32G32_SINT; break;
        case Format::R32G32_SFLOAT: result = VK_FORMAT_R32G32_SFLOAT; break;
        case Format::R32G32B32_UINT: result = VK_FORMAT_R32G32B32_UINT; break;
        case Format::R32G32B32_SINT: result = VK_FORMAT_R32G32B32_SINT; break;
        case Format::R32G32B32_SFLOAT: result = VK_FORMAT_R32G32B32_SFLOAT; break;
        case Format::R32G32B32A32_UINT: result = VK_FORMAT_R32G32B32A32_UINT; break;
        case Format::R32G32B32A32_SINT: result = VK_FORMAT_R32G32B32A32_SINT; break;
        case Format::R32G32B32A32_SFLOAT: result = VK_FORMAT_R32G32B32A32_SFLOAT; break;
        case Format::R64_UINT: result = VK_FORMAT_R64_UINT; break;
        case Format::R64_SINT: result = VK_FORMAT_R64_SINT; break;
        case Format::R64_SFLOAT: result = VK_FORMAT_R64_SFLOAT; break;
        case Format::R64G64_UINT: result = VK_FORMAT_R64G64_UINT; break;
        case Format::R64G64_SINT: result = VK_FORMAT_R64G64_SINT; break;
        case Format::R64G64_SFLOAT: result = VK_FORMAT_R64G64_SFLOAT; break;
        case Format::R64G64B64_UINT: result = VK_FORMAT_R64G64B64_UINT; break;
        case Format::R64G64B64_SINT: result = VK_FORMAT_R64G64B64_SINT; break;
        case Format::R64G64B64_SFLOAT: result = VK_FORMAT_R64G64B64_SFLOAT; break;
        case Format::R64G64B64A64_UINT: result = VK_FORMAT_R64G64B64A64_UINT; break;
        case Format::R64G64B64A64_SINT: result = VK_FORMAT_R64G64B64A64_SINT; break;
        case Format::R64G64B64A64_SFLOAT: result = VK_FORMAT_R64G64B64A64_SFLOAT; break;
        case Format::B10G11R11_UFLOAT_PACK32: result = VK_FORMAT_B10G11R11_UFLOAT_PACK32; break;
        case Format::E5B9G9R9_UFLOAT_PACK32: result = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32; break;
        case Format::D16_UNORM: result = VK_FORMAT_D16_UNORM; break;
        case Format::X8_D24_UNORM_PACK32: result = VK_FORMAT_X8_D24_UNORM_PACK32; break;
        case Format::D32_SFLOAT: result = VK_FORMAT_D32_SFLOAT; break;
        case Format::S8_UINT: result = VK_FORMAT_S8_UINT; break;
        case Format::D16_UNORM_S8_UINT: result = VK_FORMAT_D16_UNORM_S8_UINT; break;
        case Format::D24_UNORM_S8_UINT: result = VK_FORMAT_D24_UNORM_S8_UINT; break;
        case Format::D32_SFLOAT_S8_UINT: result = VK_FORMAT_D32_SFLOAT_S8_UINT; break;
        case Format::BC1_RGB_UNORM_BLOCK: result = VK_FORMAT_BC1_RGB_UNORM_BLOCK; break;
        case Format::BC1_RGB_SRGB_BLOCK: result = VK_FORMAT_BC1_RGB_SRGB_BLOCK; break;
        case Format::BC1_RGBA_UNORM_BLOCK: result = VK_FORMAT_BC1_RGBA_UNORM_BLOCK; break;
        case Format::BC1_RGBA_SRGB_BLOCK: result = VK_FORMAT_BC1_RGBA_SRGB_BLOCK; break;
        case Format::BC2_UNORM_BLOCK: result = VK_FORMAT_BC2_UNORM_BLOCK; break;
        case Format::BC2_SRGB_BLOCK: result = VK_FORMAT_BC2_SRGB_BLOCK; break;
        case Format::BC3_UNORM_BLOCK: result = VK_FORMAT_BC3_UNORM_BLOCK; break;
        case Format::BC3_SRGB_BLOCK: result = VK_FORMAT_BC3_SRGB_BLOCK; break;
        case Format::BC4_UNORM_BLOCK: result = VK_FORMAT_BC4_UNORM_BLOCK; break;
        case Format::BC4_SNORM_BLOCK: result = VK_FORMAT_BC4_SNORM_BLOCK; break;
        case Format::BC5_UNORM_BLOCK: result = VK_FORMAT_BC5_UNORM_BLOCK; break;
        case Format::BC5_SNORM_BLOCK: result = VK_FORMAT_BC5_SNORM_BLOCK; break;
        case Format::BC6H_UFLOAT_BLOCK: result = VK_FORMAT_BC6H_UFLOAT_BLOCK; break;
        case Format::BC6H_SFLOAT_BLOCK: result = VK_FORMAT_BC6H_SFLOAT_BLOCK; break;
        case Format::BC7_UNORM_BLOCK: result = VK_FORMAT_BC7_UNORM_BLOCK; break;
        case Format::BC7_SRGB_BLOCK: result = VK_FORMAT_BC7_SRGB_BLOCK; break;
        case Format::ETC2_R8G8B8_UNORM_BLOCK: result = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK; break;
        case Format::ETC2_R8G8B8_SRGB_BLOCK: result = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK; break;
        case Format::ETC2_R8G8B8A1_UNORM_BLOCK: result = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK; break;
        case Format::ETC2_R8G8B8A1_SRGB_BLOCK: result = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK; break;
        case Format::ETC2_R8G8B8A8_UNORM_BLOCK: result = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK; break;
        case Format::ETC2_R8G8B8A8_SRGB_BLOCK: result = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK; break;
        case Format::EAC_R11_UNORM_BLOCK: result = VK_FORMAT_EAC_R11_UNORM_BLOCK; break;
        case Format::EAC_R11_SNORM_BLOCK: result = VK_FORMAT_EAC_R11_SNORM_BLOCK; break;
        case Format::EAC_R11G11_UNORM_BLOCK: result = VK_FORMAT_EAC_R11G11_UNORM_BLOCK; break;
        case Format::EAC_R11G11_SNORM_BLOCK: result = VK_FORMAT_EAC_R11G11_SNORM_BLOCK; break;
        case Format::ASTC_4x4_UNORM_BLOCK: result = VK_FORMAT_ASTC_4x4_UNORM_BLOCK; break;
        case Format::ASTC_4x4_SRGB_BLOCK: result = VK_FORMAT_ASTC_4x4_SRGB_BLOCK; break;
        case Format::ASTC_5x4_UNORM_BLOCK: result = VK_FORMAT_ASTC_5x4_UNORM_BLOCK; break;
        case Format::ASTC_5x4_SRGB_BLOCK: result = VK_FORMAT_ASTC_5x4_SRGB_BLOCK; break;
        case Format::ASTC_5x5_UNORM_BLOCK: result = VK_FORMAT_ASTC_5x5_UNORM_BLOCK; break;
        case Format::ASTC_5x5_SRGB_BLOCK: result = VK_FORMAT_ASTC_5x5_SRGB_BLOCK; break;
        case Format::ASTC_6x5_UNORM_BLOCK: result = VK_FORMAT_ASTC_6x5_UNORM_BLOCK; break;
        case Format::ASTC_6x5_SRGB_BLOCK: result = VK_FORMAT_ASTC_6x5_SRGB_BLOCK; break;
        case Format::ASTC_6x6_UNORM_BLOCK: result = VK_FORMAT_ASTC_6x6_UNORM_BLOCK; break;
        case Format::ASTC_6x6_SRGB_BLOCK: result = VK_FORMAT_ASTC_6x6_SRGB_BLOCK; break;
        case Format::ASTC_8x5_UNORM_BLOCK: result = VK_FORMAT_ASTC_8x5_UNORM_BLOCK; break;
        case Format::ASTC_8x5_SRGB_BLOCK: result = VK_FORMAT_ASTC_8x5_SRGB_BLOCK; break;
        case Format::ASTC_8x6_UNORM_BLOCK: result = VK_FORMAT_ASTC_8x6_UNORM_BLOCK; break;
        case Format::ASTC_8x6_SRGB_BLOCK: result = VK_FORMAT_ASTC_8x6_SRGB_BLOCK; break;
        case Format::ASTC_8x8_UNORM_BLOCK: result = VK_FORMAT_ASTC_8x8_UNORM_BLOCK; break;
        case Format::ASTC_8x8_SRGB_BLOCK: result = VK_FORMAT_ASTC_8x8_SRGB_BLOCK; break;
        case Format::ASTC_10x5_UNORM_BLOCK: result = VK_FORMAT_ASTC_10x5_UNORM_BLOCK; break;
        case Format::ASTC_10x5_SRGB_BLOCK: result = VK_FORMAT_ASTC_10x5_SRGB_BLOCK; break;
        case Format::ASTC_10x6_UNORM_BLOCK: result = VK_FORMAT_ASTC_10x6_UNORM_BLOCK; break;
        case Format::ASTC_10x6_SRGB_BLOCK: result = VK_FORMAT_ASTC_10x6_SRGB_BLOCK; break;
        case Format::ASTC_10x8_UNORM_BLOCK: result = VK_FORMAT_ASTC_10x8_UNORM_BLOCK; break;
        case Format::ASTC_10x8_SRGB_BLOCK: result = VK_FORMAT_ASTC_10x8_SRGB_BLOCK; break;
        case Format::ASTC_10x10_UNORM_BLOCK: result = VK_FORMAT_ASTC_10x10_UNORM_BLOCK; break;
        case Format::ASTC_10x10_SRGB_BLOCK: result = VK_FORMAT_ASTC_10x10_SRGB_BLOCK; break;
        case Format::ASTC_12x10_UNORM_BLOCK: result = VK_FORMAT_ASTC_12x10_UNORM_BLOCK; break;
        case Format::ASTC_12x10_SRGB_BLOCK: result = VK_FORMAT_ASTC_12x10_SRGB_BLOCK; break;
        case Format::ASTC_12x12_UNORM_BLOCK: result = VK_FORMAT_ASTC_12x12_UNORM_BLOCK; break;
        case Format::ASTC_12x12_SRGB_BLOCK: result = VK_FORMAT_ASTC_12x12_SRGB_BLOCK; break;
        case Format::G8B8G8R8_422_UNORM: result = VK_FORMAT_G8B8G8R8_422_UNORM; break;
        case Format::B8G8R8G8_422_UNORM: result = VK_FORMAT_B8G8R8G8_422_UNORM; break;
        case Format::G8_B8_R8_3PLANE_420_UNORM: result = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM; break;
        case Format::G8_B8R8_2PLANE_420_UNORM: result = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM; break;
        case Format::G8_B8_R8_3PLANE_422_UNORM: result = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM; break;
        case Format::G8_B8R8_2PLANE_422_UNORM: result = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM; break;
        case Format::G8_B8_R8_3PLANE_444_UNORM: result = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM; break;
        case Format::R10X6_UNORM_PACK16: result = VK_FORMAT_R10X6_UNORM_PACK16; break;
        case Format::R10X6G10X6_UNORM_2PACK16: result = VK_FORMAT_R10X6G10X6_UNORM_2PACK16; break;
        case Format::R10X6G10X6B10X6A10X6_UNORM_4PACK16: result = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16; break;
        case Format::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: result = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16; break;
        case Format::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: result = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16; break;
        case Format::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: result = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16; break;
        case Format::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: result = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16; break;
        case Format::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: result = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16; break;
        case Format::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: result = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16; break;
        case Format::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: result = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16; break;
        case Format::R12X4_UNORM_PACK16: result = VK_FORMAT_R12X4_UNORM_PACK16; break;
        case Format::R12X4G12X4_UNORM_2PACK16: result = VK_FORMAT_R12X4G12X4_UNORM_2PACK16; break;
        case Format::R12X4G12X4B12X4A12X4_UNORM_4PACK16: result = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16; break;
        case Format::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: result = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16; break;
        case Format::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: result = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16; break;
        case Format::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: result = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16; break;
        case Format::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: result = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16; break;
        case Format::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: result = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16; break;
        case Format::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: result = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16; break;
        case Format::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: result = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16; break;
        case Format::G16B16G16R16_422_UNORM: result = VK_FORMAT_G16B16G16R16_422_UNORM; break;
        case Format::B16G16R16G16_422_UNORM: result = VK_FORMAT_B16G16R16G16_422_UNORM; break;
        case Format::G16_B16_R16_3PLANE_420_UNORM: result = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM; break;
        case Format::G16_B16R16_2PLANE_420_UNORM: result = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM; break;
        case Format::G16_B16_R16_3PLANE_422_UNORM: result = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM; break;
        case Format::G16_B16R16_2PLANE_422_UNORM: result = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM; break;
        case Format::G16_B16_R16_3PLANE_444_UNORM: result = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM; break;
        case Format::PVRTC1_2BPP_UNORM_BLOCK_IMG: result = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG; break;
        case Format::PVRTC1_4BPP_UNORM_BLOCK_IMG: result = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG; break;
        case Format::PVRTC2_2BPP_UNORM_BLOCK_IMG: result = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG; break;
        case Format::PVRTC2_4BPP_UNORM_BLOCK_IMG: result = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG; break;
        case Format::PVRTC1_2BPP_SRGB_BLOCK_IMG: result = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG; break;
        case Format::PVRTC1_4BPP_SRGB_BLOCK_IMG: result = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG; break;
        case Format::PVRTC2_2BPP_SRGB_BLOCK_IMG: result = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG; break;
        case Format::PVRTC2_4BPP_SRGB_BLOCK_IMG: result = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG; break;
    }
    return result;
}

Format VulkanConverter::to_eg(VkFormat format) {

    Format result = Format::UNDEFINED;

    switch(format){
        case VK_FORMAT_UNDEFINED: result = Format::UNDEFINED; break;
        case VK_FORMAT_R4G4_UNORM_PACK8: result = Format::R4G4_UNORM_PACK8; break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16: result = Format::R4G4B4A4_UNORM_PACK16; break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16: result = Format::B4G4R4A4_UNORM_PACK16; break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16: result = Format::R5G6B5_UNORM_PACK16; break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16: result = Format::B5G6R5_UNORM_PACK16; break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16: result = Format::R5G5B5A1_UNORM_PACK16; break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16: result = Format::B5G5R5A1_UNORM_PACK16; break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16: result = Format::A1R5G5B5_UNORM_PACK16; break;
        case VK_FORMAT_R8_UNORM: result = Format::R8_UNORM; break;
        case VK_FORMAT_R8_SNORM: result = Format::R8_SNORM; break;
        case VK_FORMAT_R8_USCALED: result = Format::R8_USCALED; break;
        case VK_FORMAT_R8_SSCALED: result = Format::R8_SSCALED; break;
        case VK_FORMAT_R8_UINT: result = Format::R8_UINT; break;
        case VK_FORMAT_R8_SINT: result = Format::R8_SINT; break;
        case VK_FORMAT_R8_SRGB: result = Format::R8_SRGB; break;
        case VK_FORMAT_R8G8_UNORM: result = Format::R8G8_UNORM; break;
        case VK_FORMAT_R8G8_SNORM: result = Format::R8G8_SNORM; break;
        case VK_FORMAT_R8G8_USCALED: result = Format::R8G8_USCALED; break;
        case VK_FORMAT_R8G8_SSCALED: result = Format::R8G8_SSCALED; break;
        case VK_FORMAT_R8G8_UINT: result = Format::R8G8_UINT; break;
        case VK_FORMAT_R8G8_SINT: result = Format::R8G8_SINT; break;
        case VK_FORMAT_R8G8_SRGB: result = Format::R8G8_SRGB; break;
        case VK_FORMAT_R8G8B8_UNORM: result = Format::R8G8B8_UNORM; break;
        case VK_FORMAT_R8G8B8_SNORM: result = Format::R8G8B8_SNORM; break;
        case VK_FORMAT_R8G8B8_USCALED: result = Format::R8G8B8_USCALED; break;
        case VK_FORMAT_R8G8B8_SSCALED: result = Format::R8G8B8_SSCALED; break;
        case VK_FORMAT_R8G8B8_UINT: result = Format::R8G8B8_UINT; break;
        case VK_FORMAT_R8G8B8_SINT: result = Format::R8G8B8_SINT; break;
        case VK_FORMAT_R8G8B8_SRGB: result = Format::R8G8B8_SRGB; break;
        case VK_FORMAT_B8G8R8_UNORM: result = Format::B8G8R8_UNORM; break;
        case VK_FORMAT_B8G8R8_SNORM: result = Format::B8G8R8_SNORM; break;
        case VK_FORMAT_B8G8R8_USCALED: result = Format::B8G8R8_USCALED; break;
        case VK_FORMAT_B8G8R8_SSCALED: result = Format::B8G8R8_SSCALED; break;
        case VK_FORMAT_B8G8R8_UINT: result = Format::B8G8R8_UINT; break;
        case VK_FORMAT_B8G8R8_SINT: result = Format::B8G8R8_SINT; break;
        case VK_FORMAT_B8G8R8_SRGB: result = Format::B8G8R8_SRGB; break;
        case VK_FORMAT_R8G8B8A8_UNORM: result = Format::R8G8B8A8_UNORM; break;
        case VK_FORMAT_R8G8B8A8_SNORM: result = Format::R8G8B8A8_SNORM; break;
        case VK_FORMAT_R8G8B8A8_USCALED: result = Format::R8G8B8A8_USCALED; break;
        case VK_FORMAT_R8G8B8A8_SSCALED: result = Format::R8G8B8A8_SSCALED; break;
        case VK_FORMAT_R8G8B8A8_UINT: result = Format::R8G8B8A8_UINT; break;
        case VK_FORMAT_R8G8B8A8_SINT: result = Format::R8G8B8A8_SINT; break;
        case VK_FORMAT_R8G8B8A8_SRGB: result = Format::R8G8B8A8_SRGB; break;
        case VK_FORMAT_B8G8R8A8_UNORM: result = Format::B8G8R8A8_UNORM; break;
        case VK_FORMAT_B8G8R8A8_SNORM: result = Format::B8G8R8A8_SNORM; break;
        case VK_FORMAT_B8G8R8A8_USCALED: result = Format::B8G8R8A8_USCALED; break;
        case VK_FORMAT_B8G8R8A8_SSCALED: result = Format::B8G8R8A8_SSCALED; break;
        case VK_FORMAT_B8G8R8A8_UINT: result = Format::B8G8R8A8_UINT; break;
        case VK_FORMAT_B8G8R8A8_SINT: result = Format::B8G8R8A8_SINT; break;
        case VK_FORMAT_B8G8R8A8_SRGB: result = Format::B8G8R8A8_SRGB; break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32: result = Format::A8B8G8R8_UNORM_PACK32; break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32: result = Format::A8B8G8R8_SNORM_PACK32; break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32: result = Format::A8B8G8R8_USCALED_PACK32; break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: result = Format::A8B8G8R8_SSCALED_PACK32; break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32: result = Format::A8B8G8R8_UINT_PACK32; break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32: result = Format::A8B8G8R8_SINT_PACK32; break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32: result = Format::A8B8G8R8_SRGB_PACK32; break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32: result = Format::A2R10G10B10_UNORM_PACK32; break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32: result = Format::A2R10G10B10_SNORM_PACK32; break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32: result = Format::A2R10G10B10_USCALED_PACK32; break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: result = Format::A2R10G10B10_SSCALED_PACK32; break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32: result = Format::A2R10G10B10_UINT_PACK32; break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32: result = Format::A2R10G10B10_SINT_PACK32; break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32: result = Format::A2B10G10R10_UNORM_PACK32; break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32: result = Format::A2B10G10R10_SNORM_PACK32; break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32: result = Format::A2B10G10R10_USCALED_PACK32; break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: result = Format::A2B10G10R10_SSCALED_PACK32; break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32: result = Format::A2B10G10R10_UINT_PACK32; break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32: result = Format::A2B10G10R10_SINT_PACK32; break;
        case VK_FORMAT_R16_UNORM: result = Format::R16_UNORM; break;
        case VK_FORMAT_R16_SNORM: result = Format::R16_SNORM; break;
        case VK_FORMAT_R16_USCALED: result = Format::R16_USCALED; break;
        case VK_FORMAT_R16_SSCALED: result = Format::R16_SSCALED; break;
        case VK_FORMAT_R16_UINT: result = Format::R16_UINT; break;
        case VK_FORMAT_R16_SINT: result = Format::R16_SINT; break;
        case VK_FORMAT_R16_SFLOAT: result = Format::R16_SFLOAT; break;
        case VK_FORMAT_R16G16_UNORM: result = Format::R16G16_UNORM; break;
        case VK_FORMAT_R16G16_SNORM: result = Format::R16G16_SNORM; break;
        case VK_FORMAT_R16G16_USCALED: result = Format::R16G16_USCALED; break;
        case VK_FORMAT_R16G16_SSCALED: result = Format::R16G16_SSCALED; break;
        case VK_FORMAT_R16G16_UINT: result = Format::R16G16_UINT; break;
        case VK_FORMAT_R16G16_SINT: result = Format::R16G16_SINT; break;
        case VK_FORMAT_R16G16_SFLOAT: result = Format::R16G16_SFLOAT; break;
        case VK_FORMAT_R16G16B16_UNORM: result = Format::R16G16B16_UNORM; break;
        case VK_FORMAT_R16G16B16_SNORM: result = Format::R16G16B16_SNORM; break;
        case VK_FORMAT_R16G16B16_USCALED: result = Format::R16G16B16_USCALED; break;
        case VK_FORMAT_R16G16B16_SSCALED: result = Format::R16G16B16_SSCALED; break;
        case VK_FORMAT_R16G16B16_UINT: result = Format::R16G16B16_UINT; break;
        case VK_FORMAT_R16G16B16_SINT: result = Format::R16G16B16_SINT; break;
        case VK_FORMAT_R16G16B16_SFLOAT: result = Format::R16G16B16_SFLOAT; break;
        case VK_FORMAT_R16G16B16A16_UNORM: result = Format::R16G16B16A16_UNORM; break;
        case VK_FORMAT_R16G16B16A16_SNORM: result = Format::R16G16B16A16_SNORM; break;
        case VK_FORMAT_R16G16B16A16_USCALED: result = Format::R16G16B16A16_USCALED; break;
        case VK_FORMAT_R16G16B16A16_SSCALED: result = Format::R16G16B16A16_SSCALED; break;
        case VK_FORMAT_R16G16B16A16_UINT: result = Format::R16G16B16A16_UINT; break;
        case VK_FORMAT_R16G16B16A16_SINT: result = Format::R16G16B16A16_SINT; break;
        case VK_FORMAT_R16G16B16A16_SFLOAT: result = Format::R16G16B16A16_SFLOAT; break;
        case VK_FORMAT_R32_UINT: result = Format::R32_UINT; break;
        case VK_FORMAT_R32_SINT: result = Format::R32_SINT; break;
        case VK_FORMAT_R32_SFLOAT: result = Format::R32_SFLOAT; break;
        case VK_FORMAT_R32G32_UINT: result = Format::R32G32_UINT; break;
        case VK_FORMAT_R32G32_SINT: result = Format::R32G32_SINT; break;
        case VK_FORMAT_R32G32_SFLOAT: result = Format::R32G32_SFLOAT; break;
        case VK_FORMAT_R32G32B32_UINT: result = Format::R32G32B32_UINT; break;
        case VK_FORMAT_R32G32B32_SINT: result = Format::R32G32B32_SINT; break;
        case VK_FORMAT_R32G32B32_SFLOAT: result = Format::R32G32B32_SFLOAT; break;
        case VK_FORMAT_R32G32B32A32_UINT: result = Format::R32G32B32A32_UINT; break;
        case VK_FORMAT_R32G32B32A32_SINT: result = Format::R32G32B32A32_SINT; break;
        case VK_FORMAT_R32G32B32A32_SFLOAT: result = Format::R32G32B32A32_SFLOAT; break;
        case VK_FORMAT_R64_UINT: result = Format::R64_UINT; break;
        case VK_FORMAT_R64_SINT: result = Format::R64_SINT; break;
        case VK_FORMAT_R64_SFLOAT: result = Format::R64_SFLOAT; break;
        case VK_FORMAT_R64G64_UINT: result = Format::R64G64_UINT; break;
        case VK_FORMAT_R64G64_SINT: result = Format::R64G64_SINT; break;
        case VK_FORMAT_R64G64_SFLOAT: result = Format::R64G64_SFLOAT; break;
        case VK_FORMAT_R64G64B64_UINT: result = Format::R64G64B64_UINT; break;
        case VK_FORMAT_R64G64B64_SINT: result = Format::R64G64B64_SINT; break;
        case VK_FORMAT_R64G64B64_SFLOAT: result = Format::R64G64B64_SFLOAT; break;
        case VK_FORMAT_R64G64B64A64_UINT: result = Format::R64G64B64A64_UINT; break;
        case VK_FORMAT_R64G64B64A64_SINT: result = Format::R64G64B64A64_SINT; break;
        case VK_FORMAT_R64G64B64A64_SFLOAT: result = Format::R64G64B64A64_SFLOAT; break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32: result = Format::B10G11R11_UFLOAT_PACK32; break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: result = Format::E5B9G9R9_UFLOAT_PACK32; break;
        case VK_FORMAT_D16_UNORM: result = Format::D16_UNORM; break;
        case VK_FORMAT_X8_D24_UNORM_PACK32: result = Format::X8_D24_UNORM_PACK32; break;
        case VK_FORMAT_D32_SFLOAT: result = Format::D32_SFLOAT; break;
        case VK_FORMAT_S8_UINT: result = Format::S8_UINT; break;
        case VK_FORMAT_D16_UNORM_S8_UINT: result = Format::D16_UNORM_S8_UINT; break;
        case VK_FORMAT_D24_UNORM_S8_UINT: result = Format::D24_UNORM_S8_UINT; break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT: result = Format::D32_SFLOAT_S8_UINT; break;
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK: result = Format::BC1_RGB_UNORM_BLOCK; break;
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK: result = Format::BC1_RGB_SRGB_BLOCK; break;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: result = Format::BC1_RGBA_UNORM_BLOCK; break;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: result = Format::BC1_RGBA_SRGB_BLOCK; break;
        case VK_FORMAT_BC2_UNORM_BLOCK: result = Format::BC2_UNORM_BLOCK; break;
        case VK_FORMAT_BC2_SRGB_BLOCK: result = Format::BC2_SRGB_BLOCK; break;
        case VK_FORMAT_BC3_UNORM_BLOCK: result = Format::BC3_UNORM_BLOCK; break;
        case VK_FORMAT_BC3_SRGB_BLOCK: result = Format::BC3_SRGB_BLOCK; break;
        case VK_FORMAT_BC4_UNORM_BLOCK: result = Format::BC4_UNORM_BLOCK; break;
        case VK_FORMAT_BC4_SNORM_BLOCK: result = Format::BC4_SNORM_BLOCK; break;
        case VK_FORMAT_BC5_UNORM_BLOCK: result = Format::BC5_UNORM_BLOCK; break;
        case VK_FORMAT_BC5_SNORM_BLOCK: result = Format::BC5_SNORM_BLOCK; break;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK: result = Format::BC6H_UFLOAT_BLOCK; break;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK: result = Format::BC6H_SFLOAT_BLOCK; break;
        case VK_FORMAT_BC7_UNORM_BLOCK: result = Format::BC7_UNORM_BLOCK; break;
        case VK_FORMAT_BC7_SRGB_BLOCK: result = Format::BC7_SRGB_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: result = Format::ETC2_R8G8B8_UNORM_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: result = Format::ETC2_R8G8B8_SRGB_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: result = Format::ETC2_R8G8B8A1_UNORM_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: result = Format::ETC2_R8G8B8A1_SRGB_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: result = Format::ETC2_R8G8B8A8_UNORM_BLOCK; break;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: result = Format::ETC2_R8G8B8A8_SRGB_BLOCK; break;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK: result = Format::EAC_R11_UNORM_BLOCK; break;
        case VK_FORMAT_EAC_R11_SNORM_BLOCK: result = Format::EAC_R11_SNORM_BLOCK; break;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: result = Format::EAC_R11G11_UNORM_BLOCK; break;
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: result = Format::EAC_R11G11_SNORM_BLOCK; break;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: result = Format::ASTC_4x4_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: result = Format::ASTC_4x4_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: result = Format::ASTC_5x4_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: result = Format::ASTC_5x4_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: result = Format::ASTC_5x5_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: result = Format::ASTC_5x5_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: result = Format::ASTC_6x5_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: result = Format::ASTC_6x5_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: result = Format::ASTC_6x6_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: result = Format::ASTC_6x6_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: result = Format::ASTC_8x5_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: result = Format::ASTC_8x5_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: result = Format::ASTC_8x6_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: result = Format::ASTC_8x6_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: result = Format::ASTC_8x8_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: result = Format::ASTC_8x8_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: result = Format::ASTC_10x5_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: result = Format::ASTC_10x5_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: result = Format::ASTC_10x6_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: result = Format::ASTC_10x6_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: result = Format::ASTC_10x8_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: result = Format::ASTC_10x8_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: result = Format::ASTC_10x10_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: result = Format::ASTC_10x10_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: result = Format::ASTC_12x10_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: result = Format::ASTC_12x10_SRGB_BLOCK; break;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: result = Format::ASTC_12x12_UNORM_BLOCK; break;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: result = Format::ASTC_12x12_SRGB_BLOCK; break;
        case VK_FORMAT_G8B8G8R8_422_UNORM: result = Format::G8B8G8R8_422_UNORM; break;
        case VK_FORMAT_B8G8R8G8_422_UNORM: result = Format::B8G8R8G8_422_UNORM; break;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: result = Format::G8_B8_R8_3PLANE_420_UNORM; break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: result = Format::G8_B8R8_2PLANE_420_UNORM; break;
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM: result = Format::G8_B8_R8_3PLANE_422_UNORM; break;
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM: result = Format::G8_B8R8_2PLANE_422_UNORM; break;
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM: result = Format::G8_B8_R8_3PLANE_444_UNORM; break;
        case VK_FORMAT_R10X6_UNORM_PACK16: result = Format::R10X6_UNORM_PACK16; break;
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16: result = Format::R10X6G10X6_UNORM_2PACK16; break;
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: result = Format::R10X6G10X6B10X6A10X6_UNORM_4PACK16; break;
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: result = Format::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16; break;
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: result = Format::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16; break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: result = Format::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16; break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: result = Format::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16; break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: result = Format::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16; break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: result = Format::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16; break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: result = Format::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16; break;
        case VK_FORMAT_R12X4_UNORM_PACK16: result = Format::R12X4_UNORM_PACK16; break;
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16: result = Format::R12X4G12X4_UNORM_2PACK16; break;
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16: result = Format::R12X4G12X4B12X4A12X4_UNORM_4PACK16; break;
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: result = Format::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16; break;
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: result = Format::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16; break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: result = Format::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16; break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: result = Format::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16; break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: result = Format::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16; break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: result = Format::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16; break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: result = Format::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16; break;
        case VK_FORMAT_G16B16G16R16_422_UNORM: result = Format::G16B16G16R16_422_UNORM; break;
        case VK_FORMAT_B16G16R16G16_422_UNORM: result = Format::B16G16R16G16_422_UNORM; break;
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM: result = Format::G16_B16_R16_3PLANE_420_UNORM; break;
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM: result = Format::G16_B16R16_2PLANE_420_UNORM; break;
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM: result = Format::G16_B16_R16_3PLANE_422_UNORM; break;
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM: result = Format::G16_B16R16_2PLANE_422_UNORM; break;
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM: result = Format::G16_B16_R16_3PLANE_444_UNORM; break;
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: result = Format::PVRTC1_2BPP_UNORM_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: result = Format::PVRTC1_4BPP_UNORM_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG: result = Format::PVRTC2_2BPP_UNORM_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG: result = Format::PVRTC2_4BPP_UNORM_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: result = Format::PVRTC1_2BPP_SRGB_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: result = Format::PVRTC1_4BPP_SRGB_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG: result = Format::PVRTC2_2BPP_SRGB_BLOCK_IMG; break;
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG: result = Format::PVRTC2_4BPP_SRGB_BLOCK_IMG; break;
    }
    return result;
}

VkShaderStageFlags VulkanConverter::to_vk(ShaderStage stage) {
    VkShaderStageFlags result;
    switch(stage){
        case ShaderStage::VERTEX: result = VK_SHADER_STAGE_VERTEX_BIT; break;
        case ShaderStage::FRAGMENT:result = VK_SHADER_STAGE_FRAGMENT_BIT; break;
        case ShaderStage::COMPUTE: result = VK_SHADER_STAGE_COMPUTE_BIT; break;
        case ShaderStage::GEOMETRY: result = VK_SHADER_STAGE_GEOMETRY_BIT; break;
        case ShaderStage::TESSALATION_CONTROL: result = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
        case ShaderStage::TESSALATION_EVALUATE: result = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
    }
    return result;
}

ShaderStage VulkanConverter::to_eg(VkShaderStageFlags stage) {
    ShaderStage result;
    switch(stage){
        case VK_SHADER_STAGE_VERTEX_BIT: result = ShaderStage::VERTEX; break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:result = ShaderStage::FRAGMENT; break;
        case VK_SHADER_STAGE_COMPUTE_BIT: result = ShaderStage::COMPUTE; break;
        case VK_SHADER_STAGE_GEOMETRY_BIT: result = ShaderStage::GEOMETRY; break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: result = ShaderStage::TESSALATION_CONTROL; break;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: result = ShaderStage::TESSALATION_EVALUATE; break;
        default: throw std::runtime_error("Invalid VkShaderStageFlags conversion attempt!");
    }
    return result;
}

VkDescriptorType VulkanConverter::to_vk(DescriptorType type) {
    VkDescriptorType result;
    switch(type){
        case DescriptorType::UNIFORM_BUFFER: result = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
        case DescriptorType::SAMPLED_IMAGE: result = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; break;
        case DescriptorType::STORAGE_IMAGE: result = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; break;
        case DescriptorType::STORAGE_BUFFER: result = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
        case DescriptorType::COMBINED_IMAGE_SAMPLER: result = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
    }
    return result;
}

DescriptorType VulkanConverter::to_eg(VkDescriptorType type) {
    DescriptorType result;
    switch(type){
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: result = DescriptorType::COMBINED_IMAGE_SAMPLER; break;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: result = DescriptorType::SAMPLED_IMAGE; break;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: result = DescriptorType::UNIFORM_BUFFER; break;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: result = DescriptorType::STORAGE_IMAGE; break;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: result = DescriptorType::STORAGE_BUFFER; break;
        default: throw std::runtime_error("Invalid VkDescriptorType (not yet supported!)");
    }
    return result;
}

VkFilter VulkanConverter::to_vk(Filter filter) {
    VkFilter result;
    switch(filter){
        case Filter::LINEAR: result = VK_FILTER_LINEAR; break;
        case Filter::NEAREST: result = VK_FILTER_NEAREST; break;
    }
    return result;
}

Filter VulkanConverter::to_eg(VkFilter filter) {
    Filter result;
    switch(filter){
        case VK_FILTER_LINEAR: result = Filter::LINEAR; break;
        case VK_FILTER_NEAREST: result = Filter::NEAREST; break;
        default: throw std::runtime_error("Invalid VkFilter (not yet supported!)");
    }
    return result;
}

VkPrimitiveTopology VulkanConverter::to_vk(PrimitiveTopology topology) {
    VkPrimitiveTopology result;
    switch(topology){
        case PrimitiveTopology::POINT_LIST:result = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case PrimitiveTopology::LINE_LIST:result = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
        case PrimitiveTopology::LINE_STRIP:result = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
        case PrimitiveTopology::TRIANGLE_LIST:result = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case PrimitiveTopology::TRIANGLE_STRIP:result = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        case PrimitiveTopology::TRIANGLE_FAN:result = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
        case PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:result = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY; break;
        case PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:result = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY; break;
        case PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:result = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY; break;
        case PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:result = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; break;
        case PrimitiveTopology::PATCH_LIST:result = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
    }
    return result;
}

PrimitiveTopology VulkanConverter::to_eg(VkPrimitiveTopology topology) {
    PrimitiveTopology result;
    switch(topology){
        case VK_PRIMITIVE_TOPOLOGY_POINT_LIST: result = PrimitiveTopology::POINT_LIST; break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_LIST: result = PrimitiveTopology::LINE_LIST; break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: result = PrimitiveTopology::LINE_STRIP; break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: result = PrimitiveTopology::TRIANGLE_LIST; break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: result = PrimitiveTopology::TRIANGLE_STRIP; break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN: result = PrimitiveTopology::TRIANGLE_FAN; break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY: result = PrimitiveTopology::LINE_LIST_WITH_ADJACENCY; break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY: result = PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY; break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY: result = PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY; break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY: result = PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY; break;
        case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST: result = PrimitiveTopology::PATCH_LIST; break;
        default: throw std::runtime_error("Invalid VkPrimitiveTopology on conversion");
    }
    return result;
}

AttachmentLoadOperator VulkanConverter::to_eg(VkAttachmentLoadOp op) {
    AttachmentLoadOperator result;
    switch(op){
        case VK_ATTACHMENT_LOAD_OP_LOAD: result = AttachmentLoadOperator::LOAD; break;
        case VK_ATTACHMENT_LOAD_OP_CLEAR: result = AttachmentLoadOperator::CLEAR; break;
        case VK_ATTACHMENT_LOAD_OP_DONT_CARE: result = AttachmentLoadOperator::DONT_CARE; break;
        default: throw std::runtime_error("Invalid VkAttachmentLoadOp on conversion");
    }
    return result;
}

VkAttachmentLoadOp VulkanConverter::to_vk(AttachmentLoadOperator op) {
    VkAttachmentLoadOp result;
    switch(op){
        case AttachmentLoadOperator::LOAD: result = VK_ATTACHMENT_LOAD_OP_LOAD; break;
        case AttachmentLoadOperator::CLEAR: result = VK_ATTACHMENT_LOAD_OP_CLEAR; break;
        case AttachmentLoadOperator::DONT_CARE: result = VK_ATTACHMENT_LOAD_OP_DONT_CARE; break;
    }
    return result;
}

AttachmentStoreOperator VulkanConverter::to_eg(VkAttachmentStoreOp op) {
    AttachmentStoreOperator result;
    switch(op){
        case VK_ATTACHMENT_STORE_OP_STORE: result = AttachmentStoreOperator::STORE; break;
        case VK_ATTACHMENT_STORE_OP_DONT_CARE: result = AttachmentStoreOperator::DONT_CARE; break;
//        case VK_ATTACHMENT_STORE_OP_NONE_QCOM: result = AttachmentStoreOperator::NONE_QCOM; break;
        default: throw std::runtime_error("Invalid VkAttachmentStoreOp on conversion");
    }
    return result;
}

VkAttachmentStoreOp VulkanConverter::to_vk(AttachmentStoreOperator op) {
    VkAttachmentStoreOp result;
    switch(op){
        case AttachmentStoreOperator::STORE: result = VK_ATTACHMENT_STORE_OP_STORE; break;
        case AttachmentStoreOperator::DONT_CARE: result = VK_ATTACHMENT_STORE_OP_DONT_CARE; break;
//        case AttachmentStoreOperator::NONE_QCOM: result = VK_ATTACHMENT_STORE_OP_NONE_QCOM; break;
    }
    return result;
}

ImageLayout VulkanConverter::to_eg(VkImageLayout op) {
    ImageLayout result;
    switch(op){
        case VK_IMAGE_LAYOUT_UNDEFINED: result = ImageLayout::UNDEFINED; break;
        case VK_IMAGE_LAYOUT_GENERAL: result = ImageLayout::GENERAL; break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: result = ImageLayout::COLOR_ATTACHMENT_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: result = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: result = ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: result = ImageLayout::SHADER_READ_ONLY_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: result = ImageLayout::TRANSFER_SRC_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: result = ImageLayout::TRANSFER_DST_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED: result = ImageLayout::PREINITIALIZED; break;
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: result = ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: result = ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL; break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: result = ImageLayout::PRESENT_SRC_KHR; break;
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR: result = ImageLayout::SHARED_PRESENT_KHR; break;
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV: result = ImageLayout::SHADING_RATE_OPTIMAL_NV; break;
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT: result = ImageLayout::FRAGMENT_DENSITY_MAP_OPTIMAL_EXT; break;
        default: throw std::runtime_error("Invalid VkImageLayout on conversion");
    }
    return result;
}

VkImageLayout VulkanConverter::to_vk(ImageLayout op) {
    VkImageLayout result;
    switch(op){
        case ImageLayout::UNDEFINED: result = VK_IMAGE_LAYOUT_UNDEFINED; break;
        case ImageLayout::GENERAL: result = VK_IMAGE_LAYOUT_GENERAL; break;
        case ImageLayout::COLOR_ATTACHMENT_OPTIMAL: result = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; break;
        case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL: result = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; break;
        case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL: result = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; break;
        case ImageLayout::SHADER_READ_ONLY_OPTIMAL: result = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; break;
        case ImageLayout::TRANSFER_SRC_OPTIMAL: result = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; break;
        case ImageLayout::TRANSFER_DST_OPTIMAL: result = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; break;
        case ImageLayout::PREINITIALIZED: result = VK_IMAGE_LAYOUT_PREINITIALIZED; break;
        case ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: result = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL; break;
        case ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: result = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL; break;
        case ImageLayout::PRESENT_SRC_KHR: result = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; break;
        case ImageLayout::SHARED_PRESENT_KHR: result = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR; break;
        case ImageLayout::SHADING_RATE_OPTIMAL_NV: result = VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV; break;
        case ImageLayout::FRAGMENT_DENSITY_MAP_OPTIMAL_EXT: result = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT; break;
    }
    return result;
}

VkAttachmentDescription VulkanConverter::to_vk(const RenderAttachmentDescription &egAttachment) {
    VkAttachmentDescription vkAttachment = {};
    vkAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    vkAttachment.loadOp = VulkanConverter::to_vk(egAttachment.loadOp);
    vkAttachment.storeOp = VulkanConverter::to_vk(egAttachment.storeOp);
    vkAttachment.initialLayout = VulkanConverter::to_vk(egAttachment.initialLayout);
    vkAttachment.finalLayout = VulkanConverter::to_vk(egAttachment.finalLayout);
    vkAttachment.stencilLoadOp = VulkanConverter::to_vk(egAttachment.stencilLoadOp);
    vkAttachment.stencilStoreOp = VulkanConverter::to_vk(egAttachment.stencilStoreOp);
    vkAttachment.format = VulkanConverter::to_vk(egAttachment.format);
    return vkAttachment;
}

RenderAttachmentDescription VulkanConverter::to_eg(const VkAttachmentDescription &vkAttachment) {
    RenderAttachmentDescription egAttachment = {};
    egAttachment.loadOp = VulkanConverter::to_eg(vkAttachment.loadOp);
    egAttachment.storeOp = VulkanConverter::to_eg(vkAttachment.storeOp);
    egAttachment.initialLayout = VulkanConverter::to_eg(vkAttachment.initialLayout);
    egAttachment.finalLayout = VulkanConverter::to_eg(vkAttachment.finalLayout);
    egAttachment.stencilLoadOp = VulkanConverter::to_eg(vkAttachment.stencilLoadOp);
    egAttachment.stencilStoreOp = VulkanConverter::to_eg(vkAttachment.stencilStoreOp);
    egAttachment.format = VulkanConverter::to_eg(vkAttachment.format);
    return egAttachment;
}

VkImageTiling VulkanConverter::to_vk(ImageTiling tiling) {
    VkImageTiling result;
    switch(tiling){
        case ImageTiling::OPTIMAL: result = VK_IMAGE_TILING_OPTIMAL; break;
        case ImageTiling::LINEAR: result = VK_IMAGE_TILING_LINEAR; break;
        case ImageTiling::DRM_FORMAT_MODIFIER_EXT: result = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT; break;
    }
    return result;
}

ImageTiling VulkanConverter::to_eg(VkImageTiling tiling) {
    ImageTiling result;
    switch(tiling){
        case VK_IMAGE_TILING_OPTIMAL: result = ImageTiling::OPTIMAL; break;
        case VK_IMAGE_TILING_LINEAR: result = ImageTiling::LINEAR; break;
        case VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT: result = ImageTiling::DRM_FORMAT_MODIFIER_EXT; break;
        default: throw std::runtime_error("Invalid VkImageTiling on conversion");
    }
    return result;
}

VkImageUsageFlagBits VulkanConverter::to_vk(ImageUsage usage) {
    VkImageUsageFlagBits result;
    switch(usage){
        case IMAGE_USAGE_TRANSFER_SRC: result = VK_IMAGE_USAGE_TRANSFER_SRC_BIT; break;
        case IMAGE_USAGE_TRANSFER_DST: result = VK_IMAGE_USAGE_TRANSFER_DST_BIT; break;
        case IMAGE_USAGE_SAMPLED: result = VK_IMAGE_USAGE_SAMPLED_BIT; break;
        case IMAGE_USAGE_STORAGE: result = VK_IMAGE_USAGE_STORAGE_BIT; break;
        case IMAGE_USAGE_COLOR_ATTACHMENT: result = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; break;
        case IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT: result = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
        case IMAGE_USAGE_TRANSIENT_ATTACHMENT: result = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT; break;
        case IMAGE_USAGE_INPUT_ATTACHMENT: result = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; break;
        case IMAGE_USAGE_SHADING_RATE_IMAGE_NV: result = VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV; break;
        case IMAGE_USAGE_FRAGMENT_DENSITY_MAP_EXT: result = VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT; break;
    }
    return result;
}

ImageUsage VulkanConverter::to_eg(VkImageUsageFlagBits usage) {
    ImageUsage result;
    switch(usage){
        case VK_IMAGE_USAGE_TRANSFER_SRC_BIT: result = IMAGE_USAGE_TRANSFER_SRC; break;
        case VK_IMAGE_USAGE_TRANSFER_DST_BIT: result = IMAGE_USAGE_TRANSFER_DST; break;
        case VK_IMAGE_USAGE_SAMPLED_BIT: result = IMAGE_USAGE_SAMPLED; break;
        case VK_IMAGE_USAGE_STORAGE_BIT: result = IMAGE_USAGE_STORAGE; break;
        case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: result = IMAGE_USAGE_COLOR_ATTACHMENT; break;
        case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: result = IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT; break;
        case VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT: result = IMAGE_USAGE_TRANSIENT_ATTACHMENT; break;
        case VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT: result = IMAGE_USAGE_INPUT_ATTACHMENT; break;
        case VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV: result = IMAGE_USAGE_SHADING_RATE_IMAGE_NV; break;
        case VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT: result = IMAGE_USAGE_FRAGMENT_DENSITY_MAP_EXT; break;
        default: throw std::runtime_error("Invalid VkImageUsageFlagBits on conversion");
    }
    return result;
}

ImageAspect VulkanConverter::to_eg(VkImageAspectFlagBits usage) {
    ImageAspect result;
    switch(usage){
        case VK_IMAGE_ASPECT_COLOR_BIT: result = IMAGE_ASPECT_COLOR; break;
        case VK_IMAGE_ASPECT_DEPTH_BIT: result = IMAGE_ASPECT_DEPTH; break;
        case VK_IMAGE_ASPECT_STENCIL_BIT: result = IMAGE_ASPECT_STENCIL; break;
        case VK_IMAGE_ASPECT_METADATA_BIT: result = IMAGE_ASPECT_METADATA; break;
        case VK_IMAGE_ASPECT_PLANE_0_BIT: result = IMAGE_ASPECT_PLANE_0; break;
        case VK_IMAGE_ASPECT_PLANE_1_BIT: result = IMAGE_ASPECT_PLANE_1; break;
        case VK_IMAGE_ASPECT_PLANE_2_BIT: result = IMAGE_ASPECT_PLANE_2; break;
        case VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT: result = IMAGE_ASPECT_MEMORY_PLANE_0_EXT; break;
        case VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT: result = IMAGE_ASPECT_MEMORY_PLANE_1_EXT; break;
        case VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT: result = IMAGE_ASPECT_MEMORY_PLANE_2_EXT; break;
        case VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT: result = IMAGE_ASPECT_MEMORY_PLANE_3_EXT; break;
        default: throw std::runtime_error("Invalid VkImageAspectFlagBits on conversion");
    }
    return result;
}

VkImageAspectFlagBits VulkanConverter::to_vk(ImageAspect usage) {
    VkImageAspectFlagBits result;
    switch(usage){
        case IMAGE_ASPECT_COLOR: result = VK_IMAGE_ASPECT_COLOR_BIT; break;
        case IMAGE_ASPECT_DEPTH: result = VK_IMAGE_ASPECT_DEPTH_BIT; break;
        case IMAGE_ASPECT_STENCIL: result = VK_IMAGE_ASPECT_STENCIL_BIT; break;
        case IMAGE_ASPECT_METADATA: result = VK_IMAGE_ASPECT_METADATA_BIT; break;
        case IMAGE_ASPECT_PLANE_0: result = VK_IMAGE_ASPECT_PLANE_0_BIT; break;
        case IMAGE_ASPECT_PLANE_1: result = VK_IMAGE_ASPECT_PLANE_1_BIT; break;
        case IMAGE_ASPECT_PLANE_2: result = VK_IMAGE_ASPECT_PLANE_2_BIT; break;
        case IMAGE_ASPECT_MEMORY_PLANE_0_EXT: result = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT; break;
        case IMAGE_ASPECT_MEMORY_PLANE_1_EXT: result = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT; break;
        case IMAGE_ASPECT_MEMORY_PLANE_2_EXT: result = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT; break;
        case IMAGE_ASPECT_MEMORY_PLANE_3_EXT: result = VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT; break;
    }
    return result;
}

MemoryProperty VulkanConverter::to_eg(VkMemoryPropertyFlagBits usage) {
    MemoryProperty result;
    switch(usage){
        case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: result = MEMORY_PROPERTY_DEVICE_LOCAL; break;
        case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: result = MEMORY_PROPERTY_HOST_VISIBLE; break;
        case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: result = MEMORY_PROPERTY_HOST_COHERENT; break;
        case VK_MEMORY_PROPERTY_HOST_CACHED_BIT: result = MEMORY_PROPERTY_HOST_CACHED; break;
        case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT: result = MEMORY_PROPERTY_LAZILY_ALLOCATED; break;
        case VK_MEMORY_PROPERTY_PROTECTED_BIT: result = MEMORY_PROPERTY_PROTECTED; break;
        case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD: result = MEMORY_PROPERTY_DEVICE_COHERENT_AMD; break;
        case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD: result = MEMORY_PROPERTY_DEVICE_UNCACHED_AMD; break;
        default: throw std::runtime_error("Invalid VkMemoryPropertyFlagBits on conversion");
    }
    return result;
}

VkMemoryPropertyFlagBits VulkanConverter::to_vk(MemoryProperty usage) {
    VkMemoryPropertyFlagBits result;
    switch(usage){
        case MEMORY_PROPERTY_DEVICE_LOCAL: result = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; break;
        case MEMORY_PROPERTY_HOST_VISIBLE: result = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; break;
        case MEMORY_PROPERTY_HOST_COHERENT: result = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; break;
        case MEMORY_PROPERTY_HOST_CACHED: result = VK_MEMORY_PROPERTY_HOST_CACHED_BIT; break;
        case MEMORY_PROPERTY_LAZILY_ALLOCATED: result = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT; break;
        case MEMORY_PROPERTY_PROTECTED: result = VK_MEMORY_PROPERTY_PROTECTED_BIT; break;
        case MEMORY_PROPERTY_DEVICE_COHERENT_AMD: result = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD; break;
        case MEMORY_PROPERTY_DEVICE_UNCACHED_AMD: result = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD; break;
    }
    return result;
}

PipelineStageFlagsBits VulkanConverter::to_eg(VkPipelineStageFlagBits stage) {
    PipelineStageFlagsBits result;
    switch(stage){
        case VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT: result = PipelineStageFlagsBits::TOP_OF_PIPE_BIT; break;
        case VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT: result = PipelineStageFlagsBits::DRAW_INDIRECT_BIT; break;
        case VK_PIPELINE_STAGE_VERTEX_INPUT_BIT: result = PipelineStageFlagsBits::VERTEX_INPUT_BIT; break;
        case VK_PIPELINE_STAGE_VERTEX_SHADER_BIT: result = PipelineStageFlagsBits::VERTEX_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT: result = PipelineStageFlagsBits::TESSELLATION_CONTROL_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT: result = PipelineStageFlagsBits::TESSELLATION_EVALUATION_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT: result = PipelineStageFlagsBits::GEOMETRY_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT: result = PipelineStageFlagsBits::FRAGMENT_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT: result = PipelineStageFlagsBits::EARLY_FRAGMENT_TESTS_BIT; break;
        case VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT: result = PipelineStageFlagsBits::LATE_FRAGMENT_TESTS_BIT; break;
        case VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT: result = PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT; break;
        case VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT: result = PipelineStageFlagsBits::COMPUTE_SHADER_BIT; break;
        case VK_PIPELINE_STAGE_TRANSFER_BIT: result = PipelineStageFlagsBits::TRANSFER_BIT; break;
        case VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT: result = PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT; break;
        case VK_PIPELINE_STAGE_HOST_BIT: result = PipelineStageFlagsBits::HOST_BIT; break;
        case VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT: result = PipelineStageFlagsBits::ALL_GRAPHICS_BIT; break;
        case VK_PIPELINE_STAGE_ALL_COMMANDS_BIT: result = PipelineStageFlagsBits::ALL_COMMANDS_BIT; break;
        case VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT: result = PipelineStageFlagsBits::TRANSFORM_FEEDBACK_BIT_EXT; break;
        case VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT: result = PipelineStageFlagsBits::CONDITIONAL_RENDERING_BIT_EXT; break;
//        case VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR: result = PipelineStage::RAY_TRACING_SHADER_BIT_KHR; break;
//        case VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR: result = PipelineStage::ACCELERATION_STRUCTURE_BUILD_BIT_KHR; break;
        case VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV: result = PipelineStageFlagsBits::SHADING_RATE_IMAGE_BIT_NV; break;
        case VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV: result = PipelineStageFlagsBits::TASK_SHADER_BIT_NV; break;
        case VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV: result = PipelineStageFlagsBits::MESH_SHADER_BIT_NV; break;
        case VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT: result = PipelineStageFlagsBits::FRAGMENT_DENSITY_PROCESS_BIT_EXT; break;
//        case VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV: result = PipelineStage::COMMAND_PREPROCESS_BIT_NV; break;
        default: throw std::runtime_error("Invalid VkPipelineStageFlagBits on conversion");
    }
    return result;
}

VkPipelineStageFlagBits VulkanConverter::to_vk(PipelineStageFlagsBits stage) {
    VkPipelineStageFlagBits result;
    switch(stage){
        case PipelineStageFlagsBits::TOP_OF_PIPE_BIT: result = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; break;
        case PipelineStageFlagsBits::DRAW_INDIRECT_BIT: result = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT; break;
        case PipelineStageFlagsBits::VERTEX_INPUT_BIT: result = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; break;
        case PipelineStageFlagsBits::VERTEX_SHADER_BIT: result = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; break;
        case PipelineStageFlagsBits::TESSELLATION_CONTROL_SHADER_BIT: result = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; break;
        case PipelineStageFlagsBits::TESSELLATION_EVALUATION_SHADER_BIT: result = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; break;
        case PipelineStageFlagsBits::GEOMETRY_SHADER_BIT: result = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; break;
        case PipelineStageFlagsBits::FRAGMENT_SHADER_BIT: result = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
        case PipelineStageFlagsBits::EARLY_FRAGMENT_TESTS_BIT: result = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; break;
        case PipelineStageFlagsBits::LATE_FRAGMENT_TESTS_BIT: result = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; break;
        case PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT: result = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; break;
        case PipelineStageFlagsBits::COMPUTE_SHADER_BIT: result = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
        case PipelineStageFlagsBits::TRANSFER_BIT: result = VK_PIPELINE_STAGE_TRANSFER_BIT; break;
        case PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT: result = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; break;
        case PipelineStageFlagsBits::HOST_BIT: result = VK_PIPELINE_STAGE_HOST_BIT; break;
        case PipelineStageFlagsBits::ALL_GRAPHICS_BIT: result = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; break;
        case PipelineStageFlagsBits::ALL_COMMANDS_BIT: result = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; break;
        case PipelineStageFlagsBits::TRANSFORM_FEEDBACK_BIT_EXT: result = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT; break;
        case PipelineStageFlagsBits::CONDITIONAL_RENDERING_BIT_EXT: result = VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT; break;
//        case PipelineStage::RAY_TRACING_SHADER_BIT_KHR: result = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR; break;
//        case PipelineStage::ACCELERATION_STRUCTURE_BUILD_BIT_KHR: result = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR; break;
        case PipelineStageFlagsBits::SHADING_RATE_IMAGE_BIT_NV: result = VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV; break;
        case PipelineStageFlagsBits::TASK_SHADER_BIT_NV: result = VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV; break;
        case PipelineStageFlagsBits::MESH_SHADER_BIT_NV: result = VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV; break;
        case PipelineStageFlagsBits::FRAGMENT_DENSITY_PROCESS_BIT_EXT: result = VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT; break;
//        case PipelineStage::COMMAND_PREPROCESS_BIT_NV: result = VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV; break;
        default: throw std::runtime_error("Invalid PipelineStage on conversion");
    }
    return result;
}

VkCommandBufferLevel VulkanConverter::to_vk(CommandBufferLevel level) {
    VkCommandBufferLevel result;
    switch(level){
        case CommandBufferLevel::MASTER: result = VK_COMMAND_BUFFER_LEVEL_PRIMARY; break;
        case CommandBufferLevel::PRIMARY: result = VK_COMMAND_BUFFER_LEVEL_PRIMARY; break;
        case CommandBufferLevel::SECONDARY: result = VK_COMMAND_BUFFER_LEVEL_SECONDARY; break;
    }
    return result;
}

CommandBufferLevel VulkanConverter::to_eg(VkCommandBufferLevel level) {
    CommandBufferLevel result;
    switch(level){
        case VK_COMMAND_BUFFER_LEVEL_PRIMARY: result = CommandBufferLevel::PRIMARY; break;
        case VK_COMMAND_BUFFER_LEVEL_SECONDARY: result = CommandBufferLevel::SECONDARY; break;
        default: throw std::runtime_error("Invalid VkCommandBufferLevel on conversion");
    }
    return result;
}

VkVertexInputRate VulkanConverter::to_vk(VertexInputRate rate) {
    VkVertexInputRate result;
    switch (rate){
        case VertexInputRate::VERTEX: result = VK_VERTEX_INPUT_RATE_VERTEX; break;
        case VertexInputRate::INSTANCE: result = VK_VERTEX_INPUT_RATE_INSTANCE; break;
    }
    return result;
}

VertexInputRate VulkanConverter::to_eg(VkVertexInputRate rate) {
    VertexInputRate result;
    switch (rate){
        case VK_VERTEX_INPUT_RATE_VERTEX: result = VertexInputRate::VERTEX; break;
        case VK_VERTEX_INPUT_RATE_INSTANCE: result = VertexInputRate::INSTANCE; break;
        default: throw std::runtime_error("Invalid VkVertexInputRate on conversion");
    }
    return result;
}

VkCullModeFlags VulkanConverter::to_vk(CullMode mode) {
    VkCullModeFlags result;
    switch (mode){
        case CullMode::NONE: result = VK_CULL_MODE_NONE; break;
        case CullMode::FRONT_BIT: result = VK_CULL_MODE_FRONT_BIT; break;
        case CullMode::BACK_BIT: result = VK_CULL_MODE_BACK_BIT; break;
        case CullMode::FRONT_AND_BACK: result = VK_CULL_MODE_FRONT_AND_BACK; break;
    }
    return result;
}

CullMode VulkanConverter::to_eg(VkCullModeFlagBits mode) {
    CullMode result;
    switch (mode){
        case VK_CULL_MODE_NONE: result = CullMode::NONE; break;
        case VK_CULL_MODE_FRONT_BIT: result = CullMode::FRONT_BIT; break;
        case VK_CULL_MODE_BACK_BIT: result = CullMode::BACK_BIT; break;
        case VK_CULL_MODE_FRONT_AND_BACK: result = CullMode::FRONT_AND_BACK; break;
        default: throw std::runtime_error("Invalid VkCullModeFlagBits on conversion");
    }
    return result;
}

VkAccessFlagBits VulkanConverter::to_vk(AccessFlagBits access) {
    VkAccessFlagBits result;
    switch(access){
        case AccessFlagBits::INDIRECT_COMMAND_READ_BIT: result = VK_ACCESS_INDIRECT_COMMAND_READ_BIT; break;
        case AccessFlagBits::INDEX_READ_BIT: result = VK_ACCESS_INDEX_READ_BIT; break;
        case AccessFlagBits::VERTEX_ATTRIBUTE_READ_BIT: result = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT; break;
        case AccessFlagBits::UNIFORM_READ_BIT: result = VK_ACCESS_UNIFORM_READ_BIT; break;
        case AccessFlagBits::INPUT_ATTACHMENT_READ_BIT: result = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; break;
        case AccessFlagBits::SHADER_READ_BIT: result = VK_ACCESS_SHADER_READ_BIT; break;
        case AccessFlagBits::SHADER_WRITE_BIT: result = VK_ACCESS_SHADER_WRITE_BIT; break;
        case AccessFlagBits::COLOR_ATTACHMENT_READ_BIT: result = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT; break;
        case AccessFlagBits::COLOR_ATTACHMENT_WRITE_BIT: result = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case AccessFlagBits::DEPTH_STENCIL_ATTACHMENT_READ_BIT: result = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT; break;
        case AccessFlagBits::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT: result = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case AccessFlagBits::TRANSFER_READ_BIT: result = VK_ACCESS_TRANSFER_READ_BIT; break;
        case AccessFlagBits::TRANSFER_WRITE_BIT: result = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case AccessFlagBits::HOST_READ_BIT: result = VK_ACCESS_HOST_READ_BIT; break;
        case AccessFlagBits::HOST_WRITE_BIT: result = VK_ACCESS_HOST_WRITE_BIT; break;
        case AccessFlagBits::MEMORY_READ_BIT: result = VK_ACCESS_MEMORY_READ_BIT; break;
        case AccessFlagBits::MEMORY_WRITE_BIT: result = VK_ACCESS_MEMORY_WRITE_BIT; break;
        case AccessFlagBits::TRANSFORM_FEEDBACK_WRITE_BIT_EXT: result = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT; break;
        case AccessFlagBits::TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT: result = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT; break;
        case AccessFlagBits::TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT: result = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT; break;
        case AccessFlagBits::CONDITIONAL_RENDERING_READ_BIT_EXT: result = VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT; break;
        case AccessFlagBits::COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT: result = VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT; break;
        case AccessFlagBits::ACCELERATION_STRUCTURE_READ_BIT_KHR: result = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR; break;
        case AccessFlagBits::ACCELERATION_STRUCTURE_WRITE_BIT_KHR: result = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR; break;
        case AccessFlagBits::SHADING_RATE_IMAGE_READ_BIT_NV: result = VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV; break;
        case AccessFlagBits::FRAGMENT_DENSITY_MAP_READ_BIT_EXT: result = VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT; break;
        case AccessFlagBits::COMMAND_PREPROCESS_READ_BIT_NV: result = VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV; break;
        case AccessFlagBits::COMMAND_PREPROCESS_WRITE_BIT_NV: result = VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV; break;
        case AccessFlagBits::FLAG_BITS_MAX_ENUM: result = VK_ACCESS_FLAG_BITS_MAX_ENUM; break;
    }
    return result;
}

AccessFlagBits VulkanConverter::to_eg(VkAccessFlagBits access) {
    AccessFlagBits result;
    switch(access){
        case VK_ACCESS_INDIRECT_COMMAND_READ_BIT: result = AccessFlagBits::INDIRECT_COMMAND_READ_BIT; break;
        case VK_ACCESS_INDEX_READ_BIT: result = AccessFlagBits::INDEX_READ_BIT; break;
        case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT: result = AccessFlagBits::VERTEX_ATTRIBUTE_READ_BIT; break;
        case VK_ACCESS_UNIFORM_READ_BIT: result = AccessFlagBits::UNIFORM_READ_BIT; break;
        case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT: result = AccessFlagBits::INPUT_ATTACHMENT_READ_BIT; break;
        case VK_ACCESS_SHADER_READ_BIT: result = AccessFlagBits::SHADER_READ_BIT; break;
        case VK_ACCESS_SHADER_WRITE_BIT: result = AccessFlagBits::SHADER_WRITE_BIT; break;
        case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT: result = AccessFlagBits::COLOR_ATTACHMENT_READ_BIT; break;
        case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT: result = AccessFlagBits::COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT: result = AccessFlagBits::DEPTH_STENCIL_ATTACHMENT_READ_BIT; break;
        case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT: result = AccessFlagBits::DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case VK_ACCESS_TRANSFER_READ_BIT: result = AccessFlagBits::TRANSFER_READ_BIT; break;
        case VK_ACCESS_TRANSFER_WRITE_BIT: result = AccessFlagBits::TRANSFER_WRITE_BIT; break;
        case VK_ACCESS_HOST_READ_BIT: result = AccessFlagBits::HOST_READ_BIT; break;
        case VK_ACCESS_HOST_WRITE_BIT: result = AccessFlagBits::HOST_WRITE_BIT; break;
        case VK_ACCESS_MEMORY_READ_BIT: result = AccessFlagBits::MEMORY_READ_BIT; break;
        case VK_ACCESS_MEMORY_WRITE_BIT: result = AccessFlagBits::MEMORY_WRITE_BIT; break;
        case VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT: result = AccessFlagBits::TRANSFORM_FEEDBACK_WRITE_BIT_EXT; break;
        case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT: result = AccessFlagBits::TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT; break;
        case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT: result = AccessFlagBits::TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT; break;
        case VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT: result = AccessFlagBits::CONDITIONAL_RENDERING_READ_BIT_EXT; break;
        case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT: result = AccessFlagBits::COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT; break;
        case VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR: result = AccessFlagBits::ACCELERATION_STRUCTURE_READ_BIT_KHR; break;
        case VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR: result = AccessFlagBits::ACCELERATION_STRUCTURE_WRITE_BIT_KHR; break;
        case VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV: result = AccessFlagBits::SHADING_RATE_IMAGE_READ_BIT_NV; break;
        case VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT: result = AccessFlagBits::FRAGMENT_DENSITY_MAP_READ_BIT_EXT; break;
        case VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV: result = AccessFlagBits::COMMAND_PREPROCESS_READ_BIT_NV; break;
        case VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV: result = AccessFlagBits::COMMAND_PREPROCESS_WRITE_BIT_NV; break;
        case VK_ACCESS_FLAG_BITS_MAX_ENUM: result = AccessFlagBits::FLAG_BITS_MAX_ENUM; break;
    }
    return result;
}

VkDependencyFlagBits VulkanConverter::to_vk(DependencyFlagBits dependency) {
    VkDependencyFlagBits result;
    switch(dependency){
        case BY_REGION_BIT: result = VK_DEPENDENCY_BY_REGION_BIT; break;
        case DEVICE_GROUP_BIT: result = VK_DEPENDENCY_DEVICE_GROUP_BIT; break;
        case VIEW_LOCAL_BIT: result = VK_DEPENDENCY_VIEW_LOCAL_BIT; break;
    }
    return result;
}

DependencyFlagBits VulkanConverter::to_eg(VkDependencyFlagBits dependency) {
    DependencyFlagBits result;
    switch(dependency){
        case VK_DEPENDENCY_BY_REGION_BIT: result = DependencyFlagBits::BY_REGION_BIT; break;
        case VK_DEPENDENCY_DEVICE_GROUP_BIT: result = DependencyFlagBits::DEVICE_GROUP_BIT; break;
        case VK_DEPENDENCY_VIEW_LOCAL_BIT: result = DependencyFlagBits::VIEW_LOCAL_BIT; break;
    }
    return result;
}

}
