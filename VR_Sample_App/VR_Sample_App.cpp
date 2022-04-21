// VR_Library.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "VR_Sample_App.h"
#include <ext/matrix_transform.hpp>
#include <ext/matrix_clip_space.hpp>

bool enable_vr = true;


void vr_sub_renderer::setup_render_targets() {
    
    fb_info = vr_lib->get_framebuffer_info();

    createFrameBuffer(fb_info.render_width, fb_info.render_height, frame_left_eye);
    createFrameBuffer(fb_info.render_width, fb_info.render_height, frame_right_eye);

    //TODO: create framebuffer for overlay
}

void vr_sub_renderer::ControllerRenderModel::init_controller(controller* cont) {

    //generate controller data with opengl

    vr::RenderModel_t vrModel = *cont->pModel;
    
    // create and bind a VAO to hold state for this model
    glGenVertexArrays(1, &m_glVertArray);
    glBindVertexArray(m_glVertArray);

    // Populate a vertex buffer
    glGenBuffers(1, &m_glVertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_glVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vr::RenderModel_Vertex_t) * vrModel.unVertexCount, vrModel.rVertexData, GL_STATIC_DRAW);

    // Identify the components in the vertex buffer

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void*)offsetof(vr::RenderModel_Vertex_t, vPosition));

    //normals are not needed so far
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, vNormal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void*)offsetof(vr::RenderModel_Vertex_t, rfTextureCoord));

    // Create and populate the index buffer
    glGenBuffers(1, &m_glIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * vrModel.unTriangleCount * 3, vrModel.rIndexData, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // create and populate the texture

    glGenTextures(1, &m_glTexture);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cont->pTexture->unWidth, cont->pTexture->unHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, cont->pTexture->rubTextureMapData);

    // If this renders black ask McJohn what's wrong.
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLfloat fLargest;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_unVertexCount = vrModel.unTriangleCount * 3;

    //set texture pos
    render_model_shader->use();
    render_model_shader->setInt("tex", 0);

    init = true;

}

void vr_sub_renderer::ControllerRenderModel::draw_controller()
{
    glBindVertexArray(m_glVertArray);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);

    glDrawElements(GL_TRIANGLES, m_unVertexCount, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
}


void vr_sub_renderer::createFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc)
{
    glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

    glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

    glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

    glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

    glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
    glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // check FBO status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
    }
    else
    {
        std::cout << "Framebuffer creation FAILED." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 vr_sub_renderer::convertSteamVRMatrixToMat4(const vr::HmdMatrix34_t& matPose)
{
    glm::mat4 matrixObj(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrixObj;
}

glm::mat4 vr_sub_renderer::convertSteamVRMatrixToMat4(const vr::HmdMatrix44_t& matPose)
{
    glm::mat4 matrixObj(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], matPose.m[3][0],
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], matPose.m[3][1],
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], matPose.m[3][2],
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], matPose.m[3][3]
    );
    return matrixObj;
}

vr::HmdMatrix34_t vr_sub_renderer::convertMat4toSteamVRMatrix(const glm::mat4& matPose)
{


    vr::HmdMatrix34_t matrixObj = {
        matPose[0][0], matPose[1][0],matPose[2][0],matPose[3][0],
        matPose[0][1],matPose[1][1],matPose[2][1],matPose[3][1],
        matPose[0][2],matPose[1][2],matPose[2][2],matPose[3][2]
    };

    return matrixObj;
}

glm::mat4 vr_sub_renderer::getCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
    
    glm::mat4 matMVP;
    if (nEye == vr::Eye_Left)
    {
        matMVP = convertSteamVRMatrixToMat4(vr_lib->m_mat4ProjectionLeft) * convertSteamVRMatrixToMat4(vr_lib->m_mat4eyePosLeft);
    }
    else if (nEye == vr::Eye_Right)
    {
        matMVP = convertSteamVRMatrixToMat4(vr_lib->m_mat4ProjectionRight) * convertSteamVRMatrixToMat4(vr_lib->m_mat4eyePosRight);
    }

    return matMVP;
}


void init() {

	glfwInit();

	window = glfwCreateWindow(width, height, "VR_Sample_App", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

    render_model_shader = new Shader("../../../../VR_Sample_App/renderModel.vert", "../../../../VR_Sample_App/renderModel.frag");
	sample_shader = new Shader("../../../../VR_Sample_App/vertex.vert", "../../../../VR_Sample_App/fragment.frag");
    sample_shader->use();
    if (enable_vr) {
        vr_lib = new vr_library();
        vr_lib->init();

        sub_render = new vr_sub_renderer();
        sub_render->setup_render_targets();

        //eye needs to be eye^1 (inverse), vr_lib cannot do that atm because it doenst want to depend on glm
        vr_lib->m_mat4eyePosLeft = sub_render->convertMat4toSteamVRMatrix(glm::inverse(sub_render->convertSteamVRMatrixToMat4(vr_lib->m_mat4eyePosLeft)));
        vr_lib->m_mat4eyePosRight = sub_render->convertMat4toSteamVRMatrix(glm::inverse(sub_render->convertSteamVRMatrixToMat4(vr_lib->m_mat4eyePosRight)));
    }
    

}

void setup_cube() {

    static GLfloat cube_vertices[] = {
    //x       y      z    r     g     b     u    v      normalx normaly normalz
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };


	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	//vertex Attributes
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);


	//load layout location of position

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	//color

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(sizeof(GL_FLOAT) * 3));
	glEnableVertexAttribArray(1);

	//texcoords

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(sizeof(GL_FLOAT) * 6));
	glEnableVertexAttribArray(2);


	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)(sizeof(GL_FLOAT) * 8));
	glEnableVertexAttribArray(3);


}

void setup_camera() {

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.4f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 10.0f);

    glm::mat4 model(1.0f);
    sample_shader->use();
    sample_shader->setMat4("model", model);

    sample_shader->setVec3("view_pos", glm::vec3(0.4f, 1.0f, 1.0f));

    mvp = proj * view * model;

    sample_shader->setMat4("mvp", mvp);
}


void setup_light() {

    sample_shader->setVec3("light_pos", glm::vec3(1.0f, 2.0f, 1.0f));

}

void render_scene() {


    //render scene
    sample_shader->use();
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

   
}


void print_matrix(glm::mat4 m) {

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void render() {

	glClearColor(0.2f, 0.2f, 0.17f, 1.0f);


   

	while (!glfwWindowShouldClose(window)) {
		
		glClear(GL_COLOR_BUFFER_BIT);
 
        if (enable_vr) {
            //update all states of vr library (hmd, controller..) and put in event vector
            vr_lib->update();
            
            //consume events, move ownership
            for (const auto& events : vr_lib->vr_events) {
                switch (events->type) {
                case vr_event_type::HMD_POSITION: {
                    //do stuff
                    hmd_position* hmd_events;
                    hmd_events = static_cast<hmd_position*>(events.get());
                    hmd_pos = glm::inverse(sub_render->convertSteamVRMatrixToMat4(hmd_events->HMD_position));
                    
                    break;
                } 
                case vr_event_type::CONTROLLER_PRESS:// int y = ((controller_press*)events).
                {
                    //handle button events
                    auto press_event = static_cast<controller_press*>(events.get());
                    if (press_event->menuButton_pressed)
                        vr_lib->toggle_overlay(true);
                    else
                        vr_lib->toggle_overlay(false);
                    break;
                }
                

                case vr_event_type::CONTROLLER_ACTIVATED: {
                    //generate vao with controller data

                    //render controller with pose
                    auto controller_info = static_cast<controller_activated*>(events.get());

                    if(!sub_render->rHand.init)
                        sub_render->rHand.init_controller(controller_info->p_controller);

                    cont_pos = sub_render->convertSteamVRMatrixToMat4(controller_info->p_controller->m_rmat4Pose);

                    break;
                }

                    
                }

            }
            //clear all events after they have been processed
            vr_lib->vr_events.clear();

            glViewport(0, 0, sub_render->fb_info.render_width, sub_render->fb_info.render_height);

            //render scene left eye
            glBindFramebuffer(GL_FRAMEBUFFER, sub_render->frame_left_eye.m_nRenderFramebufferId);
            glClear(GL_COLOR_BUFFER_BIT);
            sample_shader->use();
            sample_shader->setMat4("mvp", sub_render->getCurrentViewProjectionMatrix(vr::Eye_Left) * hmd_pos);

            render_scene();


            if (sub_render->rHand.init) {
                render_model_shader->use();
                render_model_shader->setMat4("mvp", sub_render->getCurrentViewProjectionMatrix(vr::Eye_Left) * hmd_pos * cont_pos);
                sub_render->rHand.draw_controller();
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, sub_render->frame_left_eye.m_nRenderFramebufferId);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sub_render->frame_left_eye.m_nResolveFramebufferId);

            glBlitFramebuffer(0, 0, sub_render->fb_info.render_width, sub_render->fb_info.render_height, 0, 0, sub_render->fb_info.render_width, sub_render->fb_info.render_height,
                GL_COLOR_BUFFER_BIT,
                GL_LINEAR);

            //render scene right eye
            glBindFramebuffer(GL_FRAMEBUFFER, sub_render->frame_right_eye.m_nRenderFramebufferId);
            glClear(GL_COLOR_BUFFER_BIT);
            sample_shader->use();
            sample_shader->setMat4("mvp", sub_render->getCurrentViewProjectionMatrix(vr::Eye_Right) * hmd_pos);

            render_scene();

            if (sub_render->rHand.init) {
                render_model_shader->use();
                render_model_shader->setMat4("mvp", sub_render->getCurrentViewProjectionMatrix(vr::Eye_Right) * hmd_pos * cont_pos);
                sub_render->rHand.draw_controller();
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, sub_render->frame_right_eye.m_nRenderFramebufferId);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sub_render->frame_right_eye.m_nResolveFramebufferId);

            glBlitFramebuffer(0, 0, sub_render->fb_info.render_width, sub_render->fb_info.render_height, 0, 0, sub_render->fb_info.render_width, sub_render->fb_info.render_height,
                GL_COLOR_BUFFER_BIT,
                GL_LINEAR);

            //submit frames to hmd
            vr_lib->submit_frame(sub_render->frame_left_eye.m_nResolveTextureId, sub_render->frame_right_eye.m_nResolveTextureId);

            //render_overlay_frame();

            vr_lib->submit_frame_overlay(sub_render->frame_left_eye.m_nResolveTextureId);

        }
        else {

            render_scene();
        }
        
        
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}

int main()
{
	init();

	setup_cube();

    setup_camera();

    setup_light();

	render();

	std::cout << "Hello from VR_Sample_App." << std::endl;
	return 0;
}