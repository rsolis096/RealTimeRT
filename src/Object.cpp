#include "Object.h"

void Object::SetMaterial(Material material)
{
	m_Material = material;
	m_Material.CreateGPUMaterial();
	m_MatId = float(Material::gpuMats.size() - 1);

}
