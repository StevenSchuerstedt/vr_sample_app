#pragma once

#include <iostream>
#include "VR_Library.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "glm/glm/mat4x4.hpp"

// TODO: Verweisen Sie hier auf zusätzliche Header, die Ihr Programm erfordert.

class vr_sub_renderer;

int width = 1024;
int height = 512;


vr_library* vr_lib;
vr_sub_renderer* sub_render;

GLuint cube_vao;

glm::mat4 mvp;

glm::mat4 hmd_pos;
glm::mat4 cont_pos;

GLFWwindow* window;

Shader* sample_shader = nullptr;
Shader* render_model_shader = nullptr;

class vr_sub_renderer {

public:
    vr_sub_renderer(){}
	void setup_render_targets();

    

    struct FramebufferDesc
    {
        GLuint m_nDepthBufferId;
        GLuint m_nRenderTextureId;
        GLuint m_nRenderFramebufferId;
        GLuint m_nResolveFramebufferId;
        GLuint m_nResolveTextureId;
    };


    struct ControllerRenderModel {
        void init_controller(controller* cont);
        void draw_controller();

        static GLuint shader_rendermodel;
        static GLuint m_rendermodelMatrixLocation;

        bool init = false;
    private:
        GLuint m_glVertBuffer;
        GLuint m_glIndexBuffer;
        GLuint m_glVertArray;
        GLuint m_glCubeArray;
        GLuint m_glTexture;
        GLsizei m_unVertexCount;
        std::string m_sModelName;
    };
    ControllerRenderModel rHand;


    FramebufferDesc frame_left_eye;
    FramebufferDesc frame_right_eye;

    vr_library::framebuffer_info fb_info;

    glm::mat4 convertSteamVRMatrixToMat4(const vr::HmdMatrix34_t& matPose);
    glm::mat4 convertSteamVRMatrixToMat4(const vr::HmdMatrix44_t& matPose);
    vr::HmdMatrix34_t convertMat4toSteamVRMatrix(const glm::mat4& matPose);

    glm::mat4 getCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);

private:
    void createFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc);

};