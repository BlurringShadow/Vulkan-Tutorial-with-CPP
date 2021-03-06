﻿#include "vulkan_sample.h"
#include "glm_camera.h"

using namespace vulkan;
using namespace std::chrono;

static glm_camera glm_camera{};

vulkan_sample sample;

static float x_degree{};
static float y_degree{};
static float z_degree{};
static constexpr vec2 center{sample.width / 2, sample.height / 2};

void update_camera()
{
    static constexpr mat4 model{1};
    static const auto& proj = []
    {
        auto&& p = glm_camera.get_proj_mat();
        p[1][1] *= -1;
        return p;
    }();

    glm_camera.quaternion = qua{vec3{x_degree, y_degree, z_degree}};

    sample.flush_transform_to_memory();

    sample.set_transform({proj * glm_camera.get_view_mat() * model});
}

void perform_camera_update(GLFWwindow* window, const int key, const float speed_up_ratio)
{
    switch(key)
    {
    case GLFW_KEY_W: { glm_camera.pos.z += speed_up_ratio; }
    break;
    case GLFW_KEY_S: { glm_camera.pos.z -= speed_up_ratio; }
    break;
    case GLFW_KEY_A: { glm_camera.pos.x += speed_up_ratio; }
    break;
    case GLFW_KEY_D: { glm_camera.pos.x -= speed_up_ratio; }
    break;
    case GLFW_KEY_Q: { glm_camera.pos.y -= speed_up_ratio; }
    break;
    case GLFW_KEY_E: { glm_camera.pos.y += speed_up_ratio; }
    break;
    case GLFW_KEY_KP_ADD: { z_degree += speed_up_ratio; }
    break;
    case GLFW_KEY_KP_SUBTRACT: { z_degree -= speed_up_ratio; }
    break;
    case GLFW_KEY_HOME:
    {
        glm_camera.pos = {0, -1, -5};
        z_degree = x_degree = y_degree = 0;
    }
    break;
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    default: break;
    }

    update_camera();
}

void key_callback(GLFWwindow* window, const int key, const int, const int action, const int)
{
    static auto speed_up_ratio = 1.0f;
    switch(action)
    {
    case GLFW_REPEAT: speed_up_ratio += 0.1f;
    case GLFW_PRESS: perform_camera_update(window, key, speed_up_ratio);
        break;
    case GLFW_RELEASE: speed_up_ratio = 0.1f;
        break;
    default: break;
    }
}

void cursor_callback(GLFWwindow* window, const double x, const double y)
{
    y_degree += static_cast<float>(x - center.x) * 0.001f;
    x_degree += static_cast<float>(y - center.y) * 0.001f;
    glfwSetCursorPos(window, center.x, center.y);
    update_camera();
}

int main()
{
    try
    {
        sample.initialize();

        {
            const auto& extent = sample.swapchain_create_info().imageExtent;
            glm_camera.aspect_ratio = extent.width / static_cast<decltype(glm_camera.aspect_ratio)>(extent.height);
        }

        glfwSetKeyCallback(sample.get_window(), key_callback);
        glfwSetCursorPosCallback(sample.get_window(), cursor_callback);
        glfwSetInputMode(sample.get_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(sample.get_window(), center.x, center.y);

        key_callback(sample.get_window(),GLFW_KEY_HOME, 0,GLFW_PRESS, 0);

        while(sample.render());
    } catch(const std::exception& e) { std::cerr << e.what(); }
    return 0;
}
