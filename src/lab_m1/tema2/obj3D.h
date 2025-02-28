#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
#include "lab_m1/tema2/trans3D.h"

namespace obj3D
{
	void CreateCylinder(float radius, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 color);
	void CreateCone(float radius, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices);
	void CreateTerrain(float terrainFrequency, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices);
	void CreateDrone(float length, float width, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, std::vector<glm::vec3>& cubePositions);
	void CreatePropeller(float length, float width, float height, const glm::vec3& cubePosition, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float propOffLength, float propOffWidth, float propOffHeight);
	void CreateRing(float circleRadius, float poleHeight, float poleRadius, int numSegments, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 circleColor);
	void CreateHouse(float width, float depth, float height, float roofHeight, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices);
	void CreateSphere(float radius, int numSectors, int numStacks, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 color);
}