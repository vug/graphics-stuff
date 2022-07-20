#include "CameraController.h"

#include <imgui.h>

#include <cmath>

namespace ws
{
  Camera2DController::Camera2DController(Camera2D &cam2D) : camera(cam2D) {}

  void Camera2DController::update(float viewWidth, float viewHeight)
  {
    // Pan
    static glm::vec2 camPos0{};
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      camPos0 = camera.position;
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
      ImVec2 mouseDrag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
      // for consistent pan experience at every zoom-level
      // move the camera full width/height when mouse moved whole side
      camera.position.x = camPos0.x - mouseDrag.x * (camera.width / viewWidth);
      camera.position.y = camPos0.y + mouseDrag.y * (camera.height / viewHeight);
    }
    // Zoom
    const float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0)
    {
      // for consistent zoom experience at every zoom-level should have "same amount" of visible zoom in/out
      // therefore change the width & height exponentially.
      const float factor = std::powf(2.f, -wheel / 4.0f);
      camera.width = camera.width * factor;
      camera.height = camera.height * factor;
    }
  }
}