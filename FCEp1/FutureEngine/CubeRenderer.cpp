#include "CubeRenderer.h"
#include "RenderTargetCube.h"
#include "SceneGraph.h"
#include "SolarisRenderer.h"

CubeRenderer::CubeRenderer(int size) {

	m_Target = new RenderTargetCube(size);


}

CubeRenderer::CubeRenderer() {

    m_Target = nullptr;

}

void CubeRenderer::Render(SolarisRenderer* renderer, glm::vec3 position, float range) {


    auto originalCam = renderer->GetCamera(); // Save current camera
    auto captureCam = new NodeCamera;

    captureCam->SetFOV(90.0f);         // Cubemap face needs 90° FOV
    captureCam->SetFarZ(range);        // Set maximum view distance
    captureCam->SetPosition(position); // Place camera at capture origin

    renderer->SetCamera(captureCam);

    // Directions for each cube face (OpenGL convention)
    const glm::vec3 targets[6] = {
        glm::vec3(1, 0, 0),   // +X
        glm::vec3(-1, 0, 0),  // -X
        glm::vec3(0, 1, 0),   // +Y
        glm::vec3(0, -1, 0),  // -Y
        glm::vec3(0, 0, 1),   // +Z
        glm::vec3(0, 0, -1)   // -Z
    };

    const glm::vec3 ups[6] = {
        glm::vec3(0, 1, 0),  // +X
        glm::vec3(0, 1, 0),  // -X
        glm::vec3(0, 0, 1),   // +Y
        glm::vec3(0, 0, -1),  // -Y
        glm::vec3(0, 1, 0),  // +Z
        glm::vec3(0, 1, 0)   // -Z
    };

    for (int i = 0; i < 6; ++i) {
        // Set up camera for current face
        captureCam->LookAt(position + targets[i], ups[i]);

        // Bind render target and clear
        m_Target->BindFace(i);
        m_Target->ClearFace(i, glm::vec3(0, 1, 1)); // Sky-blue clear color

        // Render the scene from this face's POV
        renderer->RenderScene(); // <- You must implement or call this

        // Unbind this face
        m_Target->Unbind();
    }

    renderer->SetCamera(originalCam);
    delete captureCam;


}

void CubeRenderer::RenderDepth(SolarisRenderer* renderer, glm::vec3 position, float range) {


    auto originalCam = renderer->GetCamera(); // Save current camera
    auto captureCam = new NodeCamera;

    captureCam->SetFOV(90.0f);         // Cubemap face needs 90° FOV
    captureCam->SetFarZ(range);        // Set maximum view distance
    captureCam->SetPosition(position); // Place camera at capture origin

    renderer->SetCamera(captureCam);

    // Directions for each cube face (OpenGL convention)
    const glm::vec3 targets[6] = {
        glm::vec3(1, 0, 0),   // +X
        glm::vec3(-1, 0, 0),  // -X
        glm::vec3(0, -1, 0),   // +Y
        glm::vec3(0, -1, 0),  // -Y
        glm::vec3(0, 0, 1),   // +Z
        glm::vec3(0, 0, -1)   // -Z
    };

    const glm::vec3 ups[6] = {
        glm::vec3(0, 1, 0),  // +X
        glm::vec3(0, 1, 0),  // -X
        glm::vec3(0, 0, 1),   // +Y
        glm::vec3(0, 0, 1),  // -Y
        glm::vec3(0, 1, 0),  // +Z
        glm::vec3(0, 1, 0)   // -Z
    };

    for (int i = 0; i < 6; ++i) {
        // Set up camera for current face
        captureCam->LookAt(position + targets[i], ups[i]);

        // Bind render target and clear
        m_Target->BindFace(i);
        m_Target->ClearFace(i, glm::vec3(1, 1, 1)); // Sky-blue clear color

        // Render the scene from this face's POV
        renderer->RenderSceneDepth(); // <- You must implement or call this

        // Unbind this face
        m_Target->Unbind();
    }

    renderer->SetCamera(originalCam);
    delete captureCam;

}