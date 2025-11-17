#include "Object.h"

GPUMaterial Object::GetGPUMaterial() {
	GPUMaterial gpuMat;
	gpuMat.type_ref_pad.x = static_cast<float>(m_Material.m_Type);
	gpuMat.type_ref_pad.y = m_Material.m_RefractionIndex;
	gpuMat.type_ref_pad.w = 0.f;
	gpuMat.type_ref_pad.z = 0.f;
	gpuMat.albedo_fuzz = glm::vec4(m_Material.m_Albedo, m_Material.m_Fuzz);
	return gpuMat;
}