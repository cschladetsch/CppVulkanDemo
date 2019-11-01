//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#ifndef VTB_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef int VtbResult;
    
    typedef enum VtbObjectType {
        VtbObjectType_None,
        VtbObjectType_Device,
        VtbObjectType_SwapChain,
        VtbObjectType_FrameBuffer,
        VtbObjectType_Texture,
        VtbObjectType_IndexBuffer,
        VtbObjectType_VertexBuffer,
        VtbObjectType_Mesh,
    } VtbObjectType;
    
    typedef struct VtbObject_Base {
        int magic;
        VtbObjectType type;
    } *VtbObject_Base;

    typedef struct VtbDevice_ {
        VtbObject_Base base;
    } *VtbDevice;

    typedef struct VtbSwapChain_ {
        VtbObject_Base base;
    } *VtbSwapChain;

    typedef struct VtbFrameBuffer_ {
        VtbObject_Base base;
    } *VtbFrameBuffer;

    typedef struct VtbMesh_ {
        VtbObject_Base base;
    } *VtbMesh;

    VtbResult vtbDevice_Create(void *window, VtbDevice *);
    VtbResult vtbDevice_Render(VtbDevice, VtbFrameBuffer);
    VtbResult vtbDevice_Destroy(VtbDevice);
    
    VtbResult vtbSwapChain_Create(VtbDevice, int numFrames, VtbSwapChain *);
    VtbResult vtbSwapChain_NextFrame(VtbSwapChain, VtbFrameBuffer *);
    VtbResult vtbSwapChain_Destroy(VtbSwapChain);

    VtbResult vtbMesh_Load(VtbDevice, const char *, VtbMesh *);
    VtbResult vtbMesh_Destroy(VtbMesh);
    
#ifdef __cplusplus
}
#endif
    
#endif // VTB_H
