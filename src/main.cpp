#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

int main()
{
	std::cout << "Hello World\n";
	glm::vec3 temp(1.0f);

	std::cout << glm::to_string(temp) << "\n";

	return 0;
}