#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/camera.h"
#include <vector>


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void FrameStart() override;
        void FollowDrone();
        void EnableTerrainCollision(float deltaTime);
        void EnableTreeCollision(float deltaTime);
        void EnableHouseCollision(float deltaTime);
        void EnableRingCollision(float deltaTime);
        void Checkpoint();
        void Timer(float deltaTime);
        void RenderMinimap(float deltaTime);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::Cam *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

		implemented::Cam* minimapCamera;
		glm::mat4 minimapProjectionMatrix;

        GLfloat FOV;
        GLfloat right, left, bottom, top;
        GLfloat near = 0.1f;
        GLfloat far = 1000.0f;

        float droneRotation;
        float propellerRotation1;
		float propellerRotation2;
		float propellerRotation3;
		float propellerRotation4;
        std::vector<glm::vec3> treePositions;
        float terrainFrequency;
        std::vector<glm::vec3> cubePositions;
		std::vector<glm::vec3> ringPositions;
		std::vector<glm::vec3> housePositions;

        float trunkHeight, trunkRadius;
		float foliageHeight1, foliageRadius1;
		float foliageHeight2, foliageRadius2;
        float gravity;
        glm::vec3 dronePosition;
        glm::vec3 droneVelocity;
        float dronePitch;
        float droneYaw;
        float droneRoll;
		glm::mat4 rotationMatrix;
		glm::vec3 cameraPosition;
        glm::vec3 cameraVelocity;
		float cameraPitch;
		float cameraYaw;
		float cameraRoll;
        float cameraDistance;
        float heightOffset;
        glm::vec3 cameraOffset;
        glm::vec3 timerOffset;
        std::vector<float> ringAngles;
        float houseWidth;
        float houseDepth;
        float houseHeight;
        float roofHeight;

        std::vector<glm::vec3> ringColors;
        std::vector<glm::vec3> prevRingColors;
        std::vector<bool> ringPassed;
        float ringRadius;
        int numSegments;
        float thickness;
        float height;
        float poleHeight;
        float poleRadius;

        glm::vec3 dronePreviousPosition;
        bool isAngleMode;
        bool isAcroMode;
        float dronePitchVelocity;
		float droneYawVelocity;
		float droneRollVelocity;

        float maxTime;
        float remainingTime;

        glm::vec3 playerCorner;
        float playerWidth, playerHeight;
    };
}   // namespace m1
