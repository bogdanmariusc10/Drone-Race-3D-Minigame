#include "lab_m1/tema2/tema2.h"
#include "lab_m1/tema2/trans3D.h"
#include "lab_m1/tema2/obj3D.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
    FOV = 70.0f;
    right = 20.0f;
    left = 0.02f;
    top = 20.0f;
    bottom = 0.02f;

	isAngleMode = true;
    isAcroMode = false;

	droneRotation = 0.0f;
	propellerRotation1 = 0.0f;
	propellerRotation2 = 0.0f;
	propellerRotation3 = 0.0f;
	propellerRotation4 = 0.0f;

    dronePosition = glm::vec3(0.0f, 100.0f, 0.0f);
	dronePreviousPosition = dronePosition;
    droneVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    dronePitch = 0.0f;
    droneYaw = 0.0f;
    droneRoll = 0.0f;
    dronePitchVelocity = 0.0f;
	droneYawVelocity = 0.0f;
	droneRollVelocity = 0.0f;

    gravity = 20.0f;
	rotationMatrix = glm::mat4(1);

    camera = new implemented::Cam();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	minimapCamera = new implemented::Cam();
	minimapCamera->Set(glm::vec3(0, 900, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

    projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, near, far);
    minimapProjectionMatrix = glm::ortho(-350.0f, 350.0f, -350.0f, 350.0f, near, far);


    cameraDistance = 8.0f;
    heightOffset = 9.0f;
    cameraOffset = glm::vec3(0, heightOffset, -cameraDistance);
	timerOffset = glm::vec3(8, 3, -1);

    maxTime = 200.0f;
    remainingTime = maxTime;

    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        vector<VertexFormat> terrainVertices;
        vector<unsigned int> terrainIndices;
        terrainFrequency = 0.1f;
        obj3D::CreateTerrain(terrainFrequency, terrainVertices, terrainIndices);
        CreateMesh("terrain", terrainVertices, terrainIndices);
    }

    {
        const int numRings = 5;
        const int numHouses = 6;
        const int numTrees = 50;
        float minDistance = 40.0f;
        ringColors = vector<glm::vec3>(numRings, glm::vec3(1, 0, 0));
        for (int i = 0; i < numRings; i++)
        {
            prevRingColors.push_back(ringColors[i]);
        }
        ringPassed = vector<bool>(numRings, false);

        treePositions.clear();
        ringPositions.clear();
        housePositions.clear();

        auto isPositionValid = [&](float x, float z, const vector<glm::vec3>& existingPositions)
            {
                for (const glm::vec3& pos : existingPositions)
                {
                    float distance = glm::length(glm::vec2(x - pos.x, z - pos.z));
                    if (distance < minDistance)
                    {
                        return false;
                    }
                }
                return true;
            };

        for (int i = 0; i < numRings; i++)
        {
            float x, z;
            bool positionValid = false;

            while (!positionValid)
            {
                x = (rand() % 100 - 50) * 5;
                z = (rand() % 100 - 50) * 5;

                positionValid = isPositionValid(x, z, ringPositions) && isPositionValid(x, z, housePositions) && isPositionValid(x, z, treePositions);
            }
            ringPositions.push_back(glm::vec3(x, 0, z));
        }

        for (int i = 0; i < numHouses; i++)
        {
            float x, z;
            bool positionValid = false;

            while (!positionValid)
            {
                x = (rand() % 100 - 50) * 5;
                z = (rand() % 100 - 50) * 5;

                positionValid = isPositionValid(x, z, housePositions) && isPositionValid(x, z, ringPositions) && isPositionValid(x, z, treePositions);
            }

            housePositions.push_back(glm::vec3(x, 0, z));
        }

        for (int i = 0; i < numTrees; i++)
        {
            float x, z, y;
            bool positionValid = false;

            while (!positionValid)
            {
                x = (rand() % 100 - 50) * 5;
                z = (rand() % 100 - 50) * 5;

                y = 0;

                positionValid = isPositionValid(x, z, treePositions) && isPositionValid(x, z, ringPositions) && isPositionValid(x, z, housePositions);
            }

            treePositions.push_back(glm::vec3(x, y, z));
        }

        vector<VertexFormat> trunkVertices;
        vector<unsigned int> trunkIndices;
		trunkHeight = 15.0f;
		trunkRadius = 1.0f;
        glm::vec3 trunkColor = glm::vec3(1, 0.5, 0.3);
        obj3D::CreateCylinder(trunkRadius, trunkHeight, trunkVertices, trunkIndices, trunkColor);
        CreateMesh("trunk", trunkVertices, trunkIndices);

        vector<VertexFormat> foliageVertices1;
        vector<unsigned int> foliageIndices1;
        foliageHeight1 = 20.0f;
		foliageRadius1 = 8.0f;
        obj3D::CreateCone(foliageRadius1, foliageHeight1, foliageVertices1, foliageIndices1);
        CreateMesh("foliage1", foliageVertices1, foliageIndices1);

		vector<VertexFormat> foliageVertices2;
		vector<unsigned int> foliageIndices2;
		foliageHeight2 = 20.0f;
		foliageRadius2 = 6.0f;
		obj3D::CreateCone(foliageRadius2, foliageHeight2, foliageVertices2, foliageIndices2);
		CreateMesh("foliage2", foliageVertices2, foliageIndices2);

        for (int i = 0; i < numRings; i++)
        {
            float angle = (rand() % 360);
			ringAngles.push_back(angle);
        }
        vector<VertexFormat> circleVertices;
        vector<unsigned int> circleIndices;
        ringRadius = 10.0f;
        numSegments = 100;
        poleHeight = 20.0f;
        poleRadius = 0.5f;

		obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[0]);
		CreateMesh("ring_1", circleVertices, circleIndices);
		obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[1]);
		CreateMesh("ring_2", circleVertices, circleIndices);
		obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[2]);
		CreateMesh("ring_3", circleVertices, circleIndices);
		obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[3]);
		CreateMesh("ring_4", circleVertices, circleIndices);
		obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[4]);
		CreateMesh("ring_5", circleVertices, circleIndices);

        vector<VertexFormat> houseVertices;
        vector<unsigned int> houseIndices;
        houseWidth = 40.0f;
        houseDepth = 30.0f;
        houseHeight = 30.0f;
        roofHeight = 15.0f;
        obj3D::CreateHouse(houseWidth, houseDepth, houseHeight, roofHeight, houseVertices, houseIndices);
        CreateMesh("house", houseVertices, houseIndices);
    }

    {
        vector<VertexFormat> droneBaseVertices;
        vector<unsigned int> droneBaseIndices;
        float droneLength = 3.0f;
        float droneWidth = 0.25f;
        float droneHeight = 0.1f;
        obj3D::CreateDrone(droneLength, droneWidth, droneHeight, droneBaseVertices, droneBaseIndices, cubePositions);
        CreateMesh("drone", droneBaseVertices, droneBaseIndices);
        
        float propellerLength = 1.2f;
        float propellerWidth = 0.05f;
        float propellerHeight = 0.05f;

        vector<VertexFormat> propellerVertices1;
        vector<unsigned int> propellerIndices1;
	    float propOffLength1 = -1.37f;
		float propOffWidth1 = -4.93f;
        float propOffHeight1 = 0.0f;
		obj3D::CreatePropeller(propellerLength, propellerWidth, propellerHeight, cubePositions[0], propellerVertices1, propellerIndices1, propOffLength1, propOffWidth1, propOffHeight1);
		CreateMesh("propellers_1", propellerVertices1, propellerIndices1);

		vector<VertexFormat> propellerVertices2;
		vector<unsigned int> propellerIndices2;
        float propOffLength2 = 1.37f;
        float propOffWidth2 = -4.93f;
        float propOffHeight2 = 0.0f;
		obj3D::CreatePropeller(propellerLength, propellerWidth, propellerHeight, cubePositions[1], propellerVertices2, propellerIndices2, propOffLength2, propOffWidth2, propOffHeight2);
		CreateMesh("propellers_2", propellerVertices2, propellerIndices2);

		vector<VertexFormat> propellerVertices3;
		vector<unsigned int> propellerIndices3;
        float propOffLength3 = -0.0f;
        float propOffWidth3 = -4.93f;
        float propOffHeight3 = -1.37f;
		obj3D::CreatePropeller(propellerLength, propellerWidth, propellerHeight, cubePositions[2], propellerVertices3, propellerIndices3, propOffLength3, propOffWidth3, propOffHeight3);
		CreateMesh("propellers_3", propellerVertices3, propellerIndices3);
		
		vector<VertexFormat> propellerVertices4;
		vector<unsigned int> propellerIndices4;
        float propOffLength4 = 0.0f;
        float propOffWidth4 = -4.93f;
        float propOffHeight4 = 1.37f;
		obj3D::CreatePropeller(propellerLength, propellerWidth, propellerHeight, cubePositions[3], propellerVertices4, propellerIndices4, propOffLength4, propOffWidth4, propOffHeight4);
		CreateMesh("propellers_4", propellerVertices4, propellerIndices4);
    }

    glm::vec3 minimapPlayerPosition = glm::vec3(dronePosition.x / 700 * 200, dronePosition.z / 700 * 200, 0.0f);
    
	float radius = 20.0f;
	int numSectors = 100;
	int numStacks = 100;
	vector<VertexFormat> playerVertices;
	vector<unsigned int> playerIndices;
    glm::vec3 mixedColor = glm::vec3(1.0f, 1.0f, 1.0f);
	obj3D::CreateSphere(radius, numSectors, numStacks, playerVertices, playerIndices, mixedColor);
	CreateMesh("player", playerVertices, playerIndices);

    for (int i = 0; i < ringPositions.size(); i++)
    {
        vector<VertexFormat> ringVertices;
        vector<unsigned int> ringIndices;
        obj3D::CreateSphere(radius, numSectors, numStacks, ringVertices, ringIndices, ringColors[i]);
        string meshName = "ringMini_" + to_string(i);
        CreateMesh(meshName.c_str(), ringVertices, ringIndices);
    }
}

void Tema2::CreateMesh(const char* name, const vector<VertexFormat>& vertices, const vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    glBindVertexArray(0);

    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << endl;
    }

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}

void Tema2::FrameStart()
{
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

}

void Tema2::FollowDrone()
{
    glm::mat4 rotationYaw = trans3D::RotateOY(glm::radians(droneYaw));
    glm::mat4 rotationPitch = trans3D::RotateOX(glm::radians(dronePitch));
    glm::mat4 rotationRoll = trans3D::RotateOZ(glm::radians(droneRoll));
    glm::mat4 rotationMatrix = rotationYaw * rotationPitch * rotationRoll;
    
    glm::vec3 rotatedCameraOffset = glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));

    glm::vec3 cameraPosition = dronePosition + rotatedCameraOffset;

    glm::vec3 cameraTarget = dronePosition;

    glm::vec3 cameraUp = glm::vec3(rotationMatrix * glm::vec4(0, 10, 0, 0));

    camera->Set(cameraPosition, cameraTarget, cameraUp);
}

void Tema2::EnableTerrainCollision(float deltaTime)
{
    float droneRadius = 3.0f;
    float bounceCoefficient = 0.1f;

    if (dronePosition.y - droneRadius < 0)
    {
        float impactVelocity = -droneVelocity.y;
        droneVelocity.y = bounceCoefficient * impactVelocity;

        dronePosition.y = droneRadius;
    }
    
    droneVelocity *= 0.98f;
    dronePosition += droneVelocity * deltaTime;
}

void Tema2::EnableTreeCollision(float deltaTime)
{
    float droneRadius = 3.0f;
    float bounceCoefficient = 0.01f;

    for (size_t i = 0; i < treePositions.size(); i++)
    {
        glm::vec3 treePos = treePositions[i];
        float distanceXZ = glm::length(glm::vec2(dronePosition.x, dronePosition.z) - glm::vec2(treePos.x, treePos.z));

        if (distanceXZ < droneRadius + trunkRadius && dronePosition.y >= treePos.y && dronePosition.y <= treePos.y + trunkHeight)
        {
            dronePosition.x = treePos.x + (dronePosition.x - treePos.x) * (droneRadius + trunkRadius) / distanceXZ;
            dronePosition.z = treePos.z + (dronePosition.z - treePos.z) * (droneRadius + trunkRadius) / distanceXZ;

            float impactVelocity = -glm::dot(droneVelocity, glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z));
            droneVelocity -= (1 + bounceCoefficient) * impactVelocity * glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z) / distanceXZ;
        }

        float relativeHeight1 = (dronePosition.y - (treePos.y + trunkHeight)) / foliageHeight1;
        if (relativeHeight1 >= 0 && relativeHeight1 <= 1)
        {
            float maxRadiusAtHeight = foliageRadius1 * (1 - relativeHeight1);
            float distanceXZ = glm::length(glm::vec2(dronePosition.x, dronePosition.z) - glm::vec2(treePos.x, treePos.z));
            if (distanceXZ < droneRadius + maxRadiusAtHeight)
            {
                dronePosition.x = treePos.x + (dronePosition.x - treePos.x) * (droneRadius + maxRadiusAtHeight) / distanceXZ;
                dronePosition.z = treePos.z + (dronePosition.z - treePos.z) * (droneRadius + maxRadiusAtHeight) / distanceXZ;

                float impactVelocity = -glm::dot(droneVelocity, glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z));
                droneVelocity -= (1 + bounceCoefficient) * impactVelocity * glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z) / distanceXZ;
            }
        }

		float relativeHeight2 = (dronePosition.y - (treePos.y + trunkHeight)) / foliageHeight2;
        if (relativeHeight2 >= 0 && relativeHeight2 <= 1)
        {
            float maxRadiusAtHeight = foliageRadius2 * (1 - relativeHeight2);
            float distanceXZ = glm::length(glm::vec2(dronePosition.x, dronePosition.z) - glm::vec2(treePos.x, treePos.z));
            if (distanceXZ < droneRadius + maxRadiusAtHeight)
            {
                dronePosition.x = treePos.x + (dronePosition.x - treePos.x) * (droneRadius + maxRadiusAtHeight) / distanceXZ;
                dronePosition.z = treePos.z + (dronePosition.z - treePos.z) * (droneRadius + maxRadiusAtHeight) / distanceXZ;

                float impactVelocity = -glm::dot(droneVelocity, glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z));
                droneVelocity -= (1 + bounceCoefficient) * impactVelocity * glm::vec3(dronePosition.x - treePos.x, 0.0f, dronePosition.z - treePos.z) / distanceXZ;
            }
        }
    }
}

void Tema2::EnableHouseCollision(float deltaTime)
{
    float droneRadius = 4.0f;
    float bounceCoefficient = 0.01f;

    bool isCollidingWithHouse = false;

    for (size_t i = 0; i < housePositions.size(); i++)
    {
        glm::vec3 housePos = housePositions[i];
        float halfWidth = houseWidth / 2.0f;
        float halfDepth = houseDepth / 2.0f;

        glm::vec2 dronePosXZ = glm::vec2(dronePosition.x, dronePosition.z);
        glm::vec2 housePosXZ = glm::vec2(housePos.x, housePos.z);
        float distanceXZ = glm::length(dronePosXZ - housePosXZ);

        if (distanceXZ < droneRadius + halfWidth && dronePosition.y >= housePos.y && dronePosition.y <= housePos.y + houseHeight)
        {
            if (!isCollidingWithHouse)
            {
                isCollidingWithHouse = true;

                glm::vec3 direction = glm::normalize(dronePosition - glm::vec3(housePos.x, dronePosition.y, housePos.z));
                float overlap = (droneRadius + halfWidth) - distanceXZ;
                dronePosition += direction * overlap;

                float impactVelocity = -glm::dot(droneVelocity, direction);
                droneVelocity -= (1 + bounceCoefficient) * impactVelocity * direction;
            }
        }

        float relativeX = (dronePosition.x - housePos.x) / halfWidth;
        float relativeZ = (dronePosition.z - housePos.z) / halfDepth;
        float roofY = housePos.y + houseHeight + (1.0f - max(abs(relativeX), abs(relativeZ))) * roofHeight;

        if (dronePosition.y > housePos.y + houseHeight && dronePosition.y < roofY)
        {
            float distanceXZ = glm::length(glm::vec2(dronePosition.x, dronePosition.z) - glm::vec2(housePos.x, housePos.z));
            if (distanceXZ < droneRadius + halfWidth)
            {
                if (!isCollidingWithHouse)
                {
                    isCollidingWithHouse = true;

                    glm::vec3 directionRoof = glm::normalize(dronePosition - glm::vec3(housePos.x, roofY, housePos.z));
                    float overlapRoof = (droneRadius + halfWidth) - distanceXZ;
                    dronePosition += directionRoof * overlapRoof;

                    float impactVelocityRoof = -glm::dot(droneVelocity, directionRoof);
                    droneVelocity -= (1 + bounceCoefficient) * impactVelocityRoof * directionRoof;
                }
            }
        }
    }

    if (!isCollidingWithHouse)
    {
        dronePosition += droneVelocity * deltaTime;
    }
}

void Tema2::EnableRingCollision(float deltaTime)
{
    float droneRadius = 3.0f;

    for (size_t i = 0; i < ringPositions.size(); i++)
    {
        glm::vec3 ringPos = ringPositions[i];
        float ringThickness = 2.0f;
        float ringHeight = 5.0f;

        float ringInnerRadius = ringRadius - ringThickness / 2.0f;
        float ringOuterRadius = ringRadius + ringThickness / 2.0f;

        float horizontalDistance = glm::length(glm::vec2(dronePosition.x - ringPos.x, dronePosition.z - ringPos.z));
        float verticalOffset = dronePosition.y - (ringPos.y + poleHeight);

        if (horizontalDistance < ringOuterRadius + droneRadius && horizontalDistance > ringInnerRadius - droneRadius)
        {
            if (verticalOffset >= 0 && verticalOffset <= ringHeight)
            {
                glm::vec2 collisionNormal2D = glm::normalize(glm::vec2(dronePosition.x - ringPos.x, dronePosition.z - ringPos.z));
                glm::vec3 collisionNormal = glm::vec3(collisionNormal2D.x, 0.0f, collisionNormal2D.y);

                dronePosition += collisionNormal * (ringOuterRadius - horizontalDistance + droneRadius);
            }
        }

        float poleDistance = glm::length(glm::vec2(dronePosition.x - ringPos.x, dronePosition.z - ringPos.z));
        if (poleDistance < poleRadius + droneRadius && dronePosition.y < poleHeight)
        {
            glm::vec2 collisionNormal2D = glm::normalize(glm::vec2(dronePosition.x - ringPos.x, dronePosition.z - ringPos.z));
            glm::vec3 collisionNormal = glm::vec3(collisionNormal2D.x, 0.0f, collisionNormal2D.y);

            dronePosition += collisionNormal * (poleRadius - poleDistance + droneRadius);
        }
    }
}

void Tema2::Checkpoint()
{
    float droneRadius = 3.0f;
    bool passedThrough = false;

    size_t currentRingIndex = 0;
    for (size_t i = 0; i < ringPositions.size(); i++)
    {
        if (!ringPassed[i]) {
            currentRingIndex = i;
            break;
        }
    }

    for (size_t i = 0; i < ringPositions.size(); i++)
    {
        glm::vec3 ringPos = ringPositions[i];

        float distanceXZ = glm::length(glm::vec2(dronePosition.x, dronePosition.z) - glm::vec2(ringPos.x, ringPos.z));

        float ringMinY = poleHeight;
        float ringMaxY = poleHeight + 2 * ringRadius;

        bool isInsideRing = distanceXZ < (ringRadius + droneRadius);
        bool isWithinVerticalSpace = dronePosition.y >= ringMinY && dronePosition.y <= ringMaxY;

        passedThrough = false;
        if (isInsideRing && isWithinVerticalSpace)
        {
            passedThrough = (dronePreviousPosition.x < ringPos.x - ringRadius && dronePosition.x >= ringPos.x - ringRadius) ||
                (dronePreviousPosition.x > ringPos.x + ringRadius && dronePosition.x <= ringPos.x + ringRadius) ||
                (dronePreviousPosition.z < ringPos.z - ringRadius && dronePosition.z >= ringPos.z - ringRadius) ||
                (dronePreviousPosition.z > ringPos.z + ringRadius && dronePosition.z <= ringPos.z + ringRadius) ||
                (dronePreviousPosition.y < ringMinY && dronePosition.y >= ringMinY) ||
                (dronePreviousPosition.y > ringMaxY && dronePosition.y <= ringMaxY);
        }

        if (i < currentRingIndex)
        {
            prevRingColors[i] = ringColors[i];
            ringColors[i] = glm::vec3(0.0f, 1.0f, 0.0f);
            ringPassed[i] = true;
        }
        else if (i == currentRingIndex)
        {
            prevRingColors[i] = ringColors[i];
            ringColors[i] = glm::vec3(1.0f, 0.0f, 0.0f);
            if (passedThrough && !ringPassed[i])
            {
                prevRingColors[i] = ringColors[i];
                ringColors[i] = glm::vec3(0.0f, 1.0f, 0.0f);
                ringPassed[i] = true;
                currentRingIndex++;
            }
        }
        else
        {
            prevRingColors[i] = ringColors[i];
            ringColors[i] = glm::vec3(1.0f, 1.0f, 0.0f);
        }
        vector<VertexFormat> circleVertices;
        vector<unsigned int> circleIndices;
        obj3D::CreateRing(ringRadius, poleHeight, poleRadius, numSegments, circleVertices, circleIndices, ringColors[i]);

        string meshName = "ring_" + to_string(i);
        CreateMesh(meshName.c_str(), circleVertices, circleIndices);
    }

    dronePreviousPosition = dronePosition;
}

void Tema2::Timer(float deltaTime)
{
    remainingTime -= deltaTime;

    if (remainingTime < 0.0f)
    {
        remainingTime = 0.0f;
    }

    float ratio = remainingTime / maxTime;

    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 color = glm::mix(red, green, ratio);

    float timerWidth = 0.1f;
    float timerHeight = 10.0f * ratio;

    vector<VertexFormat> timerVertices;
    vector<unsigned int> timerIndices;
    obj3D::CreateCylinder(timerWidth, timerHeight, timerVertices, timerIndices, color);
    CreateMesh("timer", timerVertices, timerIndices);
}

void Tema2::RenderMinimap(float deltaTime)
{
	implemented::Cam* originalCamera = camera;
	glm::mat4 originalProjectionMatrix = projectionMatrix;
    camera = minimapCamera;
    projectionMatrix = minimapProjectionMatrix;

    glClear(GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glm::ivec2 miniMap = glm::ivec2(resolution.x - 200, resolution.y - 200);

    glViewport(miniMap.x, miniMap.y, 200, 200);

    glm::mat4 modelMatrix = glm::mat4(1);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);

    glm::mat4 initialRotation = trans3D::RotateOY(glm::radians(45.0f));
    glm::mat4 modelMatrixDrone = glm::mat4(1);
    modelMatrixDrone *= trans3D::Translate(dronePosition.x, dronePosition.y, dronePosition.z);

    glm::mat4 rotationYaw = trans3D::RotateOY(glm::radians(droneYaw));
    glm::mat4 rotationPitch = trans3D::RotateOX(glm::radians(dronePitch));
    glm::mat4 rotationRoll = trans3D::RotateOZ(glm::radians(droneRoll));
    glm::mat4 rotationMatrix = rotationYaw * rotationPitch * rotationRoll;

    modelMatrixDrone *= rotationMatrix;
    modelMatrixDrone *= initialRotation;

    RenderMesh(meshes["player"], shaders["VertexColor"], modelMatrixDrone);

    for (glm::vec3& position : treePositions)
    {
        glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1), position);
        RenderMesh(meshes["trunk"], shaders["VertexColor"], modelMatrix1);

        glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + trunkHeight, position.z));
        RenderMesh(meshes["foliage1"], shaders["VertexColor"], modelMatrix2);

        glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + foliageHeight1 / 2, position.z));
        RenderMesh(meshes["foliage2"], shaders["VertexColor"], modelMatrix3);

        glm::mat4 modelMatrix4 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + foliageHeight1, position.z));
        RenderMesh(meshes["foliage2"], shaders["VertexColor"], modelMatrix4);
    }

    for (int i = 0; i < ringPositions.size(); i++)
    {
		if (ringColors[i] != prevRingColors[i])
        {
            float radius = 20.0f;
            int numSectors = 100;
            int numStacks = 100;
            vector<VertexFormat> ringVertices;
            vector<unsigned int> ringIndices;
            obj3D::CreateSphere(radius, numSectors, numStacks, ringVertices, ringIndices, ringColors[i]);
            string meshName = "ringMini_" + to_string(i);
			CreateMesh(meshName.c_str(), ringVertices, ringIndices);
		}
        glm::vec3 position = ringPositions[i];
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
        string meshName = "ringMini_" + to_string(i);
        RenderMesh(meshes[meshName], shaders["VertexColor"], modelMatrix);
    }

    for (glm::vec3& position : housePositions) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
        RenderMesh(meshes["house"], shaders["VertexColor"], modelMatrix);
    }

    camera = originalCamera;
    projectionMatrix = originalProjectionMatrix;
}

void Tema2::Update(float deltaTimeSeconds)
{
	FollowDrone();
	EnableTerrainCollision(deltaTimeSeconds);
	EnableTreeCollision(deltaTimeSeconds);
	EnableHouseCollision(deltaTimeSeconds);
	EnableRingCollision(deltaTimeSeconds);
	Timer(deltaTimeSeconds);

    float rotationSpeed = 100.0f;
    propellerRotation1 += rotationSpeed * deltaTimeSeconds;
	propellerRotation2 += rotationSpeed * deltaTimeSeconds;
	propellerRotation3 += rotationSpeed * deltaTimeSeconds;
	propellerRotation4 += rotationSpeed * deltaTimeSeconds;

    if (propellerRotation1 >= 360.0f)
    {
        propellerRotation1 -= 360.0f;
    }

	if (propellerRotation2 >= 360.0f)
    {
		propellerRotation2 -= 360.0f;
	}

	if (propellerRotation3 >= 360.0f)
    {
		propellerRotation3 -= 360.0f;
	}

	if (propellerRotation4 >= 360.0f)
    {
		propellerRotation4 -= 360.0f;
	}
	glm::mat4 initialRotation = trans3D::RotateOY(glm::radians(45.0f));
    glm::mat4 modelMatrix = glm::mat4(1);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);
    glm::mat4 modelMatrixDrone = glm::mat4(1);
    modelMatrixDrone *= trans3D::Translate(dronePosition.x, dronePosition.y, dronePosition.z);

    glm::mat4 rotationYaw = trans3D::RotateOY(glm::radians(droneYaw));
    glm::mat4 rotationPitch = trans3D::RotateOX(glm::radians(dronePitch));
    glm::mat4 rotationRoll = trans3D::RotateOZ(glm::radians(droneRoll));
    glm::mat4 rotationMatrix = rotationYaw * rotationPitch * rotationRoll;

    modelMatrixDrone *= rotationMatrix;
    modelMatrixDrone *= initialRotation;

    RenderMesh(meshes["drone"], shaders["VertexColor"], modelMatrixDrone);

    glm::mat4 modelMatrixPropeller1 = glm::mat4(1);
    modelMatrixPropeller1 *= trans3D::Translate(cubePositions[0].x, cubePositions[0].y, cubePositions[0].z);
    modelMatrixPropeller1 *= trans3D::RotateOY(propellerRotation1);
    RenderMesh(meshes["propellers_1"], shaders["VertexColor"], modelMatrixDrone * modelMatrixPropeller1);

	glm::mat4 modelMatrixPropeller2 = glm::mat4(1);
	modelMatrixPropeller2 *= trans3D::Translate(cubePositions[1].x, cubePositions[1].y, cubePositions[1].z);
	modelMatrixPropeller2 *= trans3D::RotateOY(propellerRotation2);
	RenderMesh(meshes["propellers_2"], shaders["VertexColor"], modelMatrixDrone * modelMatrixPropeller2);

	glm::mat4 modelMatrixPropeller3 = glm::mat4(1);
	modelMatrixPropeller3 *= trans3D::Translate(cubePositions[2].x, cubePositions[2].y, cubePositions[2].z);
	modelMatrixPropeller3 *= trans3D::RotateOY(propellerRotation3);
	RenderMesh(meshes["propellers_3"], shaders["VertexColor"], modelMatrixDrone * modelMatrixPropeller3);

	glm::mat4 modelMatrixPropeller4 = glm::mat4(1);
	modelMatrixPropeller4 *= trans3D::Translate(cubePositions[3].x, cubePositions[3].y, cubePositions[3].z);
	modelMatrixPropeller4 *= trans3D::RotateOY(propellerRotation4);
	RenderMesh(meshes["propellers_4"], shaders["VertexColor"], modelMatrixDrone * modelMatrixPropeller4);

	glm::mat4 modelMatrixTimer = glm::mat4(1);
	modelMatrixTimer *= trans3D::Translate(timerOffset.x, timerOffset.y, timerOffset.z);
	modelMatrixTimer *= trans3D::RotateOY(droneRotation + glm::radians(45.0f));
	modelMatrixTimer *= trans3D::RotateOX(glm::radians(270.0f));
    RenderMesh(meshes["timer"], shaders["VertexColor"], modelMatrixDrone * modelMatrixTimer);

    for (glm::vec3& position : treePositions)
    {
        glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1), position);
        RenderMesh(meshes["trunk"], shaders["VertexColor"], modelMatrix1);

        glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + trunkHeight, position.z));
        RenderMesh(meshes["foliage1"], shaders["VertexColor"], modelMatrix2);

        glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + foliageHeight1 / 2, position.z));
        RenderMesh(meshes["foliage2"], shaders["VertexColor"], modelMatrix3);

        glm::mat4 modelMatrix4 = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y + foliageHeight1, position.z));
        RenderMesh(meshes["foliage2"], shaders["VertexColor"], modelMatrix4);
    }

    for (int i = 0; i < ringPositions.size(); i++)
    {
        glm::vec3 position = ringPositions[i];
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
        string meshName = "ring_" + to_string(i);
        RenderMesh(meshes[meshName], shaders["VertexColor"], modelMatrix);
    }

    for (int i = 0; i < ringPositions.size(); i++)
    {
        if (ringPassed[i] == true)
        {
            glm::vec3 position = ringPositions[i];
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
            string meshName = "ring_" + to_string(i);
            RenderMesh(meshes[meshName], shaders["VertexColor"], modelMatrix);
        }
	}

	for (glm::vec3& position : housePositions)
    {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
		RenderMesh(meshes["house"], shaders["VertexColor"], modelMatrix);
	}

    Checkpoint();

    glm::vec3 cameraPosition = camera->position;

    float terrainHeight = 0;

    if (cameraPosition.y < terrainHeight) {
        camera->position.y = terrainHeight;
    }
    RenderMinimap(deltaTimeSeconds);
}

void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    glUseProgram(shader->program);

    GLint modelLocation = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    GLint viewLocation = glGetUniformLocation(shader->GetProgramID(), "View");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    GLint projLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glLineWidth(1.0f);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float thrustSpeed = 15.0f;
    float rotationSpeed = 20.0f;
    droneVelocity.y -= gravity * deltaTime;
    float movementSpeed = 40.0f;

    glm::mat4 rotationYaw = trans3D::RotateOY(glm::radians(droneYaw));
    glm::mat4 rotationPitch = trans3D::RotateOX(glm::radians(dronePitch));
    glm::mat4 rotationRoll = trans3D::RotateOZ(glm::radians(droneRoll));
    rotationMatrix = rotationYaw * rotationPitch * rotationRoll;

    if (window->KeyHold(GLFW_KEY_W)) {
        droneVelocity.y += thrustSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        droneVelocity.y -= thrustSpeed * deltaTime;
    }

    glm::vec3 forward = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));
    glm::vec3 right = glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0));

    if (window->KeyHold(GLFW_KEY_UP))
    {
        if (isAcroMode)
        {
            dronePitchVelocity += rotationSpeed * deltaTime;
        }
        else if (isAngleMode)
        {
            dronePitch += rotationSpeed * deltaTime;
            if (dronePitch < -15.0f) dronePitch = -15.0f;
            if (dronePitch > 15.0f) dronePitch = 15.0f;
        }
    }

    if (window->KeyHold(GLFW_KEY_DOWN))
    {
        if (isAcroMode)
        {
            dronePitchVelocity -= rotationSpeed * deltaTime;
        }
        else if (isAngleMode)
        {
            dronePitch -= rotationSpeed * deltaTime;
            if (dronePitch < -15.0f) dronePitch = -15.0f;
            if (dronePitch > 15.0f) dronePitch = 15.0f;
        }
    }

    if (window->KeyHold(GLFW_KEY_A))
    {
        if (isAcroMode)
        {
            droneYaw += rotationSpeed * deltaTime;
        }
        else if (isAngleMode)
        {
            droneYawVelocity += rotationSpeed * deltaTime;
        }
    }

    if (window->KeyHold(GLFW_KEY_D))
    {
        if (isAcroMode)
        {
            droneYaw -= rotationSpeed * deltaTime;
        }
        else if (isAngleMode)
        {
            droneYawVelocity -= rotationSpeed * deltaTime;
        }
    }

    if (window->KeyHold(GLFW_KEY_LEFT))
    {
        if (isAngleMode)
        {
            droneRoll -= rotationSpeed * deltaTime;
            if (droneRoll < -15.0f) droneRoll = -15.0f;
            if (droneRoll > 15.0f) droneRoll = 15.0f;
        }
        else
        {
            droneRollVelocity -= rotationSpeed * deltaTime;
        }
    }

    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        if (isAngleMode)
        {
            droneRoll += rotationSpeed * deltaTime;
            if (droneRoll < -15.0f) droneRoll = -15.0f;
            if (droneRoll > 15.0f) droneRoll = 15.0f;
        }
        else
        {
            droneRollVelocity += rotationSpeed * deltaTime;
        }
    }

    if (isAcroMode) {
        if (!window->KeyHold(GLFW_KEY_UP) && !window->KeyHold(GLFW_KEY_DOWN))
        {
            dronePitchVelocity *= 0.98f;
        }
        if (!window->KeyHold(GLFW_KEY_A) && !window->KeyHold(GLFW_KEY_D))
        {
            droneYawVelocity *= 0.98f;
        }
        if (!window->KeyHold(GLFW_KEY_LEFT) && !window->KeyHold(GLFW_KEY_RIGHT))
        {
            droneRollVelocity *= 0.98f;
        }

        dronePitch += dronePitchVelocity * deltaTime;
        droneYaw += droneYawVelocity * deltaTime;
        droneRoll += droneRollVelocity * deltaTime;
    }

    if (isAngleMode)
    {
        if (!window->KeyHold(GLFW_KEY_A) && !window->KeyHold(GLFW_KEY_D))
        {
            droneYawVelocity *= 0.98f;
        }
        droneYaw += droneYawVelocity * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_W))
    {
        dronePosition -= forward * movementSpeed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S))
    {
        dronePosition += forward * movementSpeed * deltaTime;
    }
	if (window->KeyHold(GLFW_KEY_UP))
    {
		dronePosition.y -= thrustSpeed * deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_DOWN))
    {
		dronePosition.y += thrustSpeed * deltaTime;
	}

    if (dronePosition.y < 0)
    {
        dronePosition.y = 0;
        droneVelocity.y = 0;
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_M)
    {
        if (isAngleMode)
        {
            isAngleMode = false;
            isAcroMode = true;
            cout << "Switched to Acro Mode" << endl;
        }
        else
        {
            isAcroMode = false;
            isAngleMode = true;
            cout << "Switched to Angle Mode" << endl;
        }
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
