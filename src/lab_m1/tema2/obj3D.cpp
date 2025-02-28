#include "obj3D.h"

#include <vector>
#include <cmath>

#include "core/engine.h"
#include "utils/gl_utils.h"

void obj3D::CreateCylinder(float radius, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 color)
{
    const int numSegments = 20;
    float angleStep = 2 * M_PI / numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = i * angleStep;
        vertices.emplace_back(glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle)), color);
    }

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = i * angleStep;
        vertices.emplace_back(glm::vec3(radius * cos(angle), height, radius * sin(angle)), color);
    }

    vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), color);
    vertices.emplace_back(glm::vec3(0.0f, height, 0.0f), color);

    int centerBottom = numSegments * 2;
    for (int i = 0; i < numSegments; ++i)
    {
        indices.push_back(centerBottom);
        indices.push_back(i);
        indices.push_back((i + 1) % numSegments);
    }

    int centerTop = centerBottom + 1;
    for (int i = 0; i < numSegments; ++i)
    {
        indices.push_back(centerTop);
        indices.push_back(numSegments + i);
        indices.push_back(numSegments + (i + 1) % numSegments);
    }

    for (int i = 0; i < numSegments; ++i)
    {
        int next = (i + 1) % numSegments;

        indices.push_back(i);
        indices.push_back(numSegments + i);
        indices.push_back(numSegments + next);

        indices.push_back(i);
        indices.push_back(numSegments + next);
        indices.push_back(next);
    }
}

void obj3D::CreateCone(float radius, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices)
{
    const int numSegments = 20;
    float angleStep = 2 * M_PI / numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = i * angleStep;
        vertices.emplace_back(glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle)), glm::vec3(0.0f, 0.4f, 0.0f));
    }

    vertices.emplace_back(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f));

    int center = numSegments;
    for (int i = 0; i < numSegments; ++i)
    {
        indices.push_back(center);
        indices.push_back(i);
        indices.push_back((i + 1) % numSegments);
    }

    for (int i = 0; i < numSegments; ++i)
    {
        int next = (i + 1) % numSegments;

        indices.push_back(i);
        indices.push_back(next);
        indices.push_back(center);
    }
}

void obj3D::CreateTerrain(float terrainFrequency, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices)
{
    const int gridSize = 700;
    const float cellSize = 1.0f;

    float terrainWidth = gridSize * cellSize;
    float terrainDepth = gridSize * cellSize;
    float offsetX = -terrainWidth / 2.0f;
    float offsetZ = -terrainDepth / 2.0f;

    float flatHeight = 0.0f;

    for (int z = 0; z <= gridSize; ++z)
    {
        for (int x = 0; x <= gridSize; ++x)
        {
            float height = flatHeight;

            glm::vec3 position(x * cellSize + offsetX, height, z * cellSize + offsetZ);

            float greenIntensity = glm::clamp(height / 100.0f, 0.2f, 1.0f);

            vertices.push_back(VertexFormat(position, glm::vec3(0.0f, greenIntensity, 0.0f)));
        }
    }

    for (int z = 0; z < gridSize; ++z)
    {
        for (int x = 0; x < gridSize; ++x)
        {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void obj3D::CreateDrone(float length, float width, float height, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, std::vector<glm::vec3>& cubePositions)
{
    float terrainHeight = 5;

    float halfLength = length / 2.0f;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    glm::vec3 p1(-halfLength, terrainHeight - halfHeight, halfWidth);
    glm::vec3 p2(halfLength, terrainHeight - halfHeight, halfWidth);
    glm::vec3 p3(halfLength, terrainHeight + halfHeight, halfWidth);
    glm::vec3 p4(-halfLength, terrainHeight + halfHeight, halfWidth);
    glm::vec3 p5(-halfLength, terrainHeight - halfHeight, -halfWidth);
    glm::vec3 p6(halfLength, terrainHeight - halfHeight, -halfWidth);
    glm::vec3 p7(halfLength, terrainHeight + halfHeight, -halfWidth);
    glm::vec3 p8(-halfLength, terrainHeight + halfHeight, -halfWidth);

    glm::vec3 p9(-halfWidth, terrainHeight - halfHeight, halfLength);
    glm::vec3 p10(halfWidth, terrainHeight - halfHeight, halfLength);
    glm::vec3 p11(halfWidth, terrainHeight + halfHeight, halfLength);
    glm::vec3 p12(-halfWidth, terrainHeight + halfHeight, halfLength);
    glm::vec3 p13(-halfWidth, terrainHeight - halfHeight, -halfLength);
    glm::vec3 p14(halfWidth, terrainHeight - halfHeight, -halfLength);
    glm::vec3 p15(halfWidth, terrainHeight + halfHeight, -halfLength);
    glm::vec3 p16(-halfWidth, terrainHeight + halfHeight, -halfLength);

    glm::vec3 mixedColor = glm::vec3(1.0f, 1.0f, 1.0f);

    vertices.push_back(VertexFormat(p1, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p2, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p3, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p4, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p5, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p6, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p7, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p8, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));

    vertices.push_back(VertexFormat(p9, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p10, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p11, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p12, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p13, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p14, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p15, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.push_back(VertexFormat(p16, mixedColor, glm::vec3(0.0f, 0.0f, 1.0f)));

    glm::vec3 cubeSize(0.125f, 0.125f, 0.125f);
    glm::vec3 cubeColor = mixedColor;

    cubePositions = {
        p7 - glm::vec3(0.125f, -0.025f, -halfWidth),
        p4 - glm::vec3(-0.125f, -0.025f, halfWidth),
        p11 - glm::vec3(0.125f, -0.025f, 0.125f),
        p16 - glm::vec3(-0.125f, -0.025f, -0.125f),
    };

    for (const glm::vec3& position : cubePositions)
    {
        glm::vec3 cubeVertices[8] = {
            position + glm::vec3(-cubeSize.x, -cubeSize.y, cubeSize.z),
            position + glm::vec3(cubeSize.x, -cubeSize.y, cubeSize.z),
            position + glm::vec3(cubeSize.x, cubeSize.y, cubeSize.z),
            position + glm::vec3(-cubeSize.x, cubeSize.y, cubeSize.z),
            position + glm::vec3(-cubeSize.x, -cubeSize.y, -cubeSize.z),
            position + glm::vec3(cubeSize.x, -cubeSize.y, -cubeSize.z),
            position + glm::vec3(cubeSize.x, cubeSize.y, -cubeSize.z),
            position + glm::vec3(-cubeSize.x, cubeSize.y, -cubeSize.z)
        };

        for (int i = 0; i < 8; ++i)
        {
            vertices.push_back(VertexFormat(cubeVertices[i], cubeColor, glm::vec3(0.0f, 0.0f, 1.0f)));
        }

        unsigned int cubeIndices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 1, 5, 5, 4, 0,
            1, 2, 6, 6, 5, 1,
            2, 3, 7, 7, 6, 2,
            3, 0, 4, 4, 7, 3
        };

        unsigned int offset = vertices.size() - 8;
        for (int i = 0; i < 36; ++i)
        {
            indices.push_back(cubeIndices[i] + offset);
        }
    }

    unsigned int cuboidIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        1, 2, 6, 6, 5, 1,
        2, 3, 7, 7, 6, 2,
        3, 0, 4, 4, 7, 3,
    };

    unsigned int cuboid2Indices[] = {
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        8, 9, 13, 13, 12, 8,
        9, 10, 14, 14, 13, 9,
        10, 11, 15, 15, 14, 10,
        11, 8, 12, 12, 15, 11,
    };

    for (int i = 0; i < 36; ++i)
    {
        indices.push_back(cuboidIndices[i]);
    }

    for (int i = 0; i < 36; ++i)
    {
        indices.push_back(cuboid2Indices[i]);
    }
}

void obj3D::CreatePropeller(float length, float width, float height, const glm::vec3& cubePosition, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, float propOffLength, float propOffWidth, float propOffHeight)
{
    glm::vec3 propellerPosition = cubePosition + glm::vec3(propOffLength, propOffWidth, propOffHeight);
    
    float halfLength = length / 2.0f;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    glm::vec3 p1(propellerPosition.x - halfLength, propellerPosition.y - halfHeight, propellerPosition.z + halfWidth);
    glm::vec3 p2(propellerPosition.x + halfLength, propellerPosition.y - halfHeight, propellerPosition.z + halfWidth);
    glm::vec3 p3(propellerPosition.x + halfLength, propellerPosition.y + halfHeight, propellerPosition.z + halfWidth);
    glm::vec3 p4(propellerPosition.x - halfLength, propellerPosition.y + halfHeight, propellerPosition.z + halfWidth);
    glm::vec3 p5(propellerPosition.x - halfLength, propellerPosition.y - halfHeight, propellerPosition.z - halfWidth);
    glm::vec3 p6(propellerPosition.x + halfLength, propellerPosition.y - halfHeight, propellerPosition.z - halfWidth);
    glm::vec3 p7(propellerPosition.x + halfLength, propellerPosition.y + halfHeight, propellerPosition.z - halfWidth);
    glm::vec3 p8(propellerPosition.x - halfLength, propellerPosition.y + halfHeight, propellerPosition.z - halfWidth);

    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

    vertices.push_back(VertexFormat(p1, color));
    vertices.push_back(VertexFormat(p2, color));
    vertices.push_back(VertexFormat(p3, color));
    vertices.push_back(VertexFormat(p4, color));
    vertices.push_back(VertexFormat(p5, color));
    vertices.push_back(VertexFormat(p6, color));
    vertices.push_back(VertexFormat(p7, color));
    vertices.push_back(VertexFormat(p8, color));

    unsigned int propellerIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        1, 2, 6, 6, 5, 1,
        2, 3, 7, 7, 6, 2,
        3, 0, 4, 4, 7, 3 
    };

    unsigned int offset = vertices.size() - 8;
    for (int i = 0; i < 36; ++i)
    {
        indices.push_back(propellerIndices[i] + offset);
    }
}

void obj3D::CreateRing(float circleRadius, float poleHeight, float poleThickness, int numSegments, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 circleColor)
{
    int baseIndex = vertices.size();
    
    float ringOffset = poleHeight * 3 / 2;
    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = (i * 2.0f * M_PI) / numSegments;

        float yOuter = circleRadius * cos(angle);
        float zOuter = circleRadius * sin(angle);

        float yInner = (circleRadius - poleThickness) * cos(angle);
        float zInner = (circleRadius - poleThickness) * sin(angle);

        vertices.emplace_back(glm::vec3(0.0f, ringOffset + yOuter, zOuter), circleColor);
        vertices.emplace_back(glm::vec3(0.0f, ringOffset + yInner, zInner), circleColor);

        if (i > 0)
        {
            int outer1 = baseIndex + 2 * (i - 1);
            int inner1 = baseIndex + 2 * (i - 1) + 1;
            int outer2 = baseIndex + 2 * i;
            int inner2 = baseIndex + 2 * i + 1;

            indices.push_back(outer1);
            indices.push_back(outer2);
            indices.push_back(inner2);

            indices.push_back(outer1);
            indices.push_back(inner2);
            indices.push_back(inner1);
        }
    }

    int poleBaseIndex = vertices.size();

    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = (i * 2.0f * M_PI) / numSegments;
        float x = poleThickness * cos(angle);
        float z = poleThickness * sin(angle); 

        vertices.emplace_back(glm::vec3(x, 0.0f, z), glm::vec3(0, 0, 0));
        vertices.emplace_back(glm::vec3(x, poleHeight, z), glm::vec3(0, 0, 0));
    }

    for (int i = 0; i < numSegments; ++i)
    {
        int lower1 = poleBaseIndex + 2 * i;
        int upper1 = poleBaseIndex + 2 * i + 1;
        int lower2 = poleBaseIndex + 2 * (i + 1);
        int upper2 = poleBaseIndex + 2 * (i + 1) + 1;

        indices.push_back(lower1);
        indices.push_back(upper1);
        indices.push_back(lower2);

        indices.push_back(upper1);
        indices.push_back(upper2);
        indices.push_back(lower2);
    }
}

void obj3D::CreateHouse(float width, float depth, float height, float roofHeight, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices)
{
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    glm::vec3 houseColor = glm::vec3(1.0f, 0.647f, 0.0f);

    vertices.emplace_back(glm::vec3(-halfWidth, 0, -halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(halfWidth, 0, -halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(halfWidth, 0, halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(-halfWidth, 0, halfDepth), houseColor);

    vertices.emplace_back(glm::vec3(-halfWidth, height, -halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(halfWidth, height, -halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(halfWidth, height, halfDepth), houseColor);
    vertices.emplace_back(glm::vec3(-halfWidth, height, halfDepth), houseColor);

    unsigned int cuboidIndices[] =
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        2, 3, 7, 2, 7, 6,
        1, 2, 6, 1, 6, 5,
        0, 3, 7, 0, 7, 4
    };

    indices.insert(indices.end(), std::begin(cuboidIndices), std::end(cuboidIndices));

    glm::vec3 roofColor = glm::vec3(0.6f, 0.3f, 0.0f);

    vertices.emplace_back(glm::vec3(-halfWidth, height, -halfDepth), roofColor);
    vertices.emplace_back(glm::vec3(halfWidth, height, -halfDepth), roofColor);
    vertices.emplace_back(glm::vec3(-halfWidth, height, halfDepth), roofColor);
    vertices.emplace_back(glm::vec3(halfWidth, height, halfDepth), roofColor);
    vertices.emplace_back(glm::vec3(0, height + roofHeight, 0), roofColor);

    unsigned int roofIndices[] = {
        8, 9, 12,
        10, 11, 12,
        8, 10, 12,
        9, 11, 12
    };

    indices.insert(indices.end(), std::begin(roofIndices), std::end(roofIndices));
}

void obj3D::CreateSphere(float radius, int numSectors, int numStacks, std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices, glm::vec3 color)
{
	float sectorStep = 2 * M_PI / numSectors;
	float stackStep = M_PI / numStacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= numStacks; ++i)
    {
		stackAngle = M_PI / 2 - i * stackStep;
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= numSectors; ++j)
        {
			sectorAngle = j * sectorStep;

			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);

			glm::vec3 position(x, y, z);

			vertices.push_back(VertexFormat(position, color));
		}
	}

	int k1, k2;
	for (int i = 0; i < numStacks; ++i)
    {
		k1 = i * (numSectors + 1);
		k2 = k1 + numSectors + 1;

		for (int j = 0; j < numSectors; ++j, ++k1, ++k2)
        {
			if (i != 0)
            {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (numStacks - 1))
            {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}
