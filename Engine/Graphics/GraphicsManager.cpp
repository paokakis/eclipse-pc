#include "GraphicsManager.hpp"
#include "../Tools/Log.hpp"
#include "../../sources/libpng/png.h"
#include <string>
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "soil\src\SOIL.h"
#include "../Game/common/MapCoordinates.h"
#include <random>


#define DEFAULT_RENDER_WIDTH	(1280)
#define DEFAULT_RENDER_HEIGHT	(720)
//#define DEPTH_BUFFER
//#define ON_TEXTURE_RENDERING
#define PERSPECTIVE
#define FULL_SCREEN				(false)

#ifndef PERSPECTIVE
#define ORTHOGRAPHIC
#endif // !PERSPECTIVE

#define BLOOM
#define SHADOW_MAPPING

#ifndef DEPTH_BUFFER
#define MSAA_SAMPLES			(4)
//#define DEPTH_BUFFER_AA // MSAA
#ifndef DEPTH_BUFFER_AA
#define AA_SECOND // MSAA simpler
#endif // !DEPTH_BUFFER_AA
#endif // !DEPTH_BUFFER

#define DEPTH_BITS				(32)
#define GAMMA					(0.8)
#define BLOOM_EXP				(5.0)

static const char* VERTEX_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"in vec2 aPosition;\n"
"in vec2 aTexture;\n"
"out vec2 vTexture;\n"
"void main() {\n"
" vTexture = aTexture;\n"
" gl_Position = vec4(aPosition, 1.0, 1.0 );\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"out vec4 colorSample;\n"
"uniform float gamma;\n"
#ifdef DEPTH_BUFFER
"uniform sampler2D uTexture;\n"
#elif defined(DEPTH_BUFFER_AA)
"uniform sampler2DMS uTexture;\n"
"uniform int uMSAA;\n"
#endif
"in vec2 vTexture;\n"
"void main() {\n"
#ifdef DEPTH_BUFFER
" colorSample = pow(texture2D(uTexture, vTexture), vec4(1.0 / gamma));\n"//texture2D(uTexture, vTexture);\n"
#elif defined(DEPTH_BUFFER_AA)
" vec4 col;\n"
" ivec2 txSize = ivec2(textureSize(uTexture)* vTexture);\n"
" for (int i = 0; i < uMSAA; ++i)\n"
"  col += texelFetch(uTexture, txSize, i);\n"
" vec4 msaaCol = col / uMSAA;\n"
" colorSample = pow(msaaCol, vec4(1.0 / gamma));\n"
#endif
"}\n";

static const char* VERTEX_BLUR =
R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 1.0f);
    TexCoords = texCoords;
}
)glsl";

static const char* FRAGMENT_BLUR =
R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
     vec3 result = texture(image, TexCoords).rgb * weight[0];
     if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
             result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         }
     }
     FragColor = vec4(result, 1.0);
}
)glsl";

static const char* BLOOM_VERTEX_FINAL = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 1.0f);
    TexCoords = texCoords;
} )glsl";

static const char* BLOOM_FRAGMENT_FINAL =
R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;
uniform float gamma;

vec3 Uncharted2Tonemap(vec3 x)
{
    float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{             
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    // tone mapping
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	vec3 result = Uncharted2Tonemap(hdrColor) * exposure;
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0f);
} )glsl";

static const char* SHADOW_VERTEX = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
})glsl";

static const char* SHADOW_FRAGMENT = R"glsl(
#version 330 core

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
})glsl";

static bool initialized = false;
GraphicsManager* GraphicsManager::mRef = NULL;

void callback_readPng(png_structp pStruct, png_bytep pData, png_size_t pSize) 
{
	Resource* resource = ((Resource*)png_get_io_ptr(pStruct));
	if (resource->read(pData, pSize) != STATUS_OK) 
	{
		resource->close();
	}
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const char *message,
	void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::string errorMSG;

	errorMSG += ("Debug message ( %d ): %s\n", id, message);

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             errorMSG += ("\nSource: API\n"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   errorMSG += ("\nSource: Window System\n"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: errorMSG += ("\nSource: Shader Compiler\n"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     errorMSG += ("\nSource: Third Party\n"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     errorMSG += ("\nSource: Application\n"); break;
		case GL_DEBUG_SOURCE_OTHER:           errorMSG += ("\nSource: Other\n"); break;
	}

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               errorMSG += ("Type: Error\n"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorMSG += ("Type: Deprecated Behaviour\n"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  errorMSG += ("Type: Undefined Behaviour\n"); break;
		case GL_DEBUG_TYPE_PORTABILITY:         errorMSG += ("Type: Portability\n"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         errorMSG += ("Type: Performance\n"); break;
		case GL_DEBUG_TYPE_MARKER:              errorMSG += ("Type: Marker\n"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          errorMSG += ("Type: Push Group\n"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           errorMSG += ("Type: Pop Group\n"); break;
		case GL_DEBUG_TYPE_OTHER:               errorMSG += ("Type: Other\n"); break;
	}

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         errorMSG += ("Severity: high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       errorMSG += ("Severity: medium"); break;
		case GL_DEBUG_SEVERITY_LOW:          errorMSG += ("Severity: low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: errorMSG += ("Severity: notification"); break;
	}

	Log::error(errorMSG.c_str());
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}

GraphicsManager::GraphicsManager(TimeManager& timeMan) :
	mDisplay(NULL), mTimeManager(timeMan),
	mTextures(), mShaders(), mVertexBuffers(), mComponents(),
	mScreenFrameBuffer(0), mRenderDepthBuffer(0),
	mRenderFrameBuffer(0), mRenderVertexBuffer(0),
	mRenderTexture(0), mRenderShaderProgram(0),
	aPosition(0), aTexture(0),
	uProjection(0), uTexture(0),
	mFullScreen(FULL_SCREEN), mGamma(GAMMA),
	mRenderWidth(DEFAULT_RENDER_WIDTH), mRenderHeight(DEFAULT_RENDER_HEIGHT), MSAA(MSAA_SAMPLES),
	mEnableBloom(true), mBloomExposure(BLOOM_EXP)
{
	Log::info("Creating GraphicsManager.");
	
	mRef = this;
}

GraphicsManager::~GraphicsManager() 
{
	Log::info("Destroying GraphicsManager.");

	if (mCamera != NULL) delete mCamera;
}

void GraphicsManager::registerComponent(GraphicsComponent* pComponent, GLuint slot)
{
	mComponents.insert(std::pair<GLuint, GraphicsComponent*>(slot, pComponent));
	if (initialized) pComponent->load();
}

status GraphicsManager::removeComponent(GLuint slot)
{
	mComponents.erase(slot);

	return STATUS_OK;
}

status GraphicsManager::start() 
{
	Log::info("Starting GraphicsManager %d - %d.", mRenderWidth, mRenderHeight);

	// GLFW init
	if (!glfwInit()) 
	{
		Log::error("Error initialising GLFW");
		goto _ERROR_;
	}
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_DEPTH_BITS, DEPTH_BITS);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	// Anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, MSAA);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // comment this line in a release build! 
#endif

	if (!mFullScreen)
	{
		mDisplay = glfwCreateWindow(mRenderWidth, mRenderHeight, GAME_TITLE, NULL, NULL);
	}
	else
	{
		mDisplay = glfwCreateWindow(mRenderWidth, mRenderHeight, GAME_TITLE, glfwGetPrimaryMonitor(), NULL);
	}
	
	if (!mDisplay) 
	{
		Log::error("Error creating window");
		goto _ERROR_;
	}

	// GLFW setup
	glfwMakeContextCurrent(mDisplay);
	glfwGetFramebufferSize(mDisplay, &mRenderWidth, &mRenderHeight);
	glfwSwapInterval(1); // swap back buffer and front buffer every frame (VSYNC)
	glfwSetInputMode(mDisplay, GLFW_STICKY_KEYS, GL_TRUE); // <-- TODO check this

	glfwSetCursorPosCallback(mDisplay, Mouse::mousePosCallback);		// set the callback function to handle mouse position
	glfwSetMouseButtonCallback(mDisplay, Mouse::mouseButtonsCallback);	// set the callback function to handle mouse button events
	glfwSetScrollCallback(mDisplay, Mouse::mouseScrollCallback);		// set the callback function to handle mouse scroll events
	glfwSetKeyCallback(mDisplay, Keyboard::keyCallback);				// set the callback function to handle keyboard events
	glfwSetCharCallback(mDisplay, Keyboard::character_callback);		// set the callback function to handle character events

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int xPos = (vidMode->width - mRenderWidth) / 2;
	int yPos = (vidMode->height - mRenderHeight) / 2;
	glfwSetWindowPos(mDisplay, xPos, yPos);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		Log::error("Error initialising GLEW!");
		goto _ERROR_;
	}
	// due to a bug in GLEW the glewInit call always generates an OpenGL error; clear the flag(s) by calling glGetError();
	glGetError();
#ifdef _DEBUG
	// enable OpenGL debug context if context allows for debug context
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	// Defines and initializes offscreen surface.
	if (initializeRenderBuffer() != STATUS_OK)
		goto _ERROR_;
	
	// gl stuff
	glViewport(0, 0, mRenderWidth, mRenderHeight);
	// transparency
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.1f, DEPTH);
	glDepthMask(GL_TRUE);
	glEnable(GL_MULTISAMPLE);			// Enables multisampling
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
	glEnable(GL_POINT_SMOOTH); 
	glEnable(GL_LINE_SMOOTH); 
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	mCamera = new Camera2(mTimeManager, mRenderWidth, mRenderHeight, DEPTH);
	mCamera->initialize();

	// Loads graphics components.
	for (gComponentVectorIterator componentIt = mComponents.begin(); componentIt != mComponents.end(); ++componentIt)
	{
		if (componentIt->second->load() != STATUS_OK)
			return STATUS_KO;
	}

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	Log::info("GL Vendor : %s", vendor);
	Log::info("GL Renderer : %s", renderer);
	Log::info("GL Version (string) : %s", version);
	Log::info("GL Version (integer) : %d.%d", major, minor);
	Log::info("GLSL Version : %s\n", glslVersion);

	initialized = true;

	return STATUS_OK;
_ERROR_:
	Log::error("Error while starting GraphicsManager");
	stop();
	return STATUS_KO;
}

status GraphicsManager::update()
{
	mCamera->update();

	static const float clearColor = 0.0f;
	static gComponentVectorIterator componentIt;
#ifdef DEPTH_BUFFER_AA
	glViewport(0, 0, mRenderWidth, mRenderHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);

	glClearColor(clearColor, clearColor, clearColor, clearColor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Render graphic components.
	for (componentIt = mComponents.begin(); componentIt != mComponents.end(); ++componentIt)
	{
		(*componentIt).second->update();
		(*componentIt).second->draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mScreenFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glViewport(0, 0, mRenderWidth, mRenderHeight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mRenderTexture);
	glUseProgram(mRenderShaderProgram);
	glUniform1i(uTexture, 0);
	glUniform1i(uMSAA, MSAA);
	glUniform1f(uGamma, mGamma);
	// Indicates to OpenGL how position and uv coordinates are stored.
	glBindBuffer(GL_ARRAY_BUFFER, mRenderVertexBuffer);
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, // Attribute Index
		2, // Number of components (x and y)
		GL_FLOAT, // Data type
		GL_FALSE, // Normalized
		sizeof(RenderVertex), // Stride
		(GLvoid*)0); // Offset
	glEnableVertexAttribArray(aTexture);
	glVertexAttribPointer(aTexture, // Attribute Index
		2, // Number of components (u and v)
		GL_FLOAT, // Data type
		GL_FALSE, // Normalized
		sizeof(RenderVertex), // Stride
		(GLvoid*)(sizeof(GLfloat) * 2)); // Offset
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

#elif defined(AA_SECOND)
	//drawShadows();
	// 1. Draw scene as normal in multisampled buffers

	glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (componentIt = mComponents.begin(); componentIt != mComponents.end(); ++componentIt)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		(*componentIt).second->update();
		(*componentIt).second->draw();
		(*componentIt).second->draw3D();
		//(*componentIt).second->drawShadows(depthMap, lightSpaceMatrix);

		(*componentIt).second->drawSilhuette();
	}
#if defined(BLOOM)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mRenderFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
	glBlitFramebuffer(0, 0, mRenderWidth, mRenderHeight, 0, 0, mRenderWidth, mRenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	drawBloom();
#else
	// 2. Now blit buffer(s) to default framebuffers
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mRenderFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, mRenderWidth, mRenderHeight, 0, 0, mRenderWidth, mRenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif
	
#endif
	// Shows the result to the user.
	glfwSwapBuffers(mDisplay);
	// update width and height on window change
	glfwGetFramebufferSize(mDisplay, &mRenderWidth, &mRenderHeight);
	// wake up sleeping tasks (synchronize)
	Scheduler::getInstance()->notifyTasks();

	return STATUS_OK;
}

void GraphicsManager::drawBloom()
{
	// 2. Blur bright fragments w/ two-pass Gaussian Blur 
	GLboolean horizontal = true, first_iteration = true;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingpongFBO[horizontal]);
	glBlitFramebuffer(0, 0, mRenderWidth, mRenderHeight, 0, 0, mRenderWidth, mRenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glUseProgram(mBlurProgram);
	for (GLuint i = 0; i < mBloomAmmount; ++i)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(mBlurProgram, "horizontal"), horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		RenderQuad();
		horizontal = !horizontal;
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Now render quad with scene's visuals as its texture image
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mBloomFinalProgram);
	glUniform1i(uTexture0, 0);
	glUniform1i(uTexture1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	glUniform1i(glGetUniformLocation(mBloomFinalProgram, "bloom"), mEnableBloom);
	glUniform1f(glGetUniformLocation(mBloomFinalProgram, "exposure"), mBloomExposure);
	glUniform1f(glGetUniformLocation(mBloomFinalProgram, "gamma"), mGamma);
	RenderQuad();
	glUseProgram(0);
}

void GraphicsManager::drawShadows()
{	
	GLfloat near_plane = 1.0f, far_plane = DEPTH;
	//lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
	lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
	lightView = glm::lookAt(SUN_POS, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// - now render scene from light's point of view
	glUseProgram(simpleDepthShader);
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "lightSpaceMatrix"), 1, GL_FALSE, &(lightSpaceMatrix[0][0]));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (auto componentIt = mComponents.begin(); componentIt != mComponents.end(); ++componentIt)
	{
		(*componentIt).second->drawDepth(simpleDepthShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glViewport(0, 0, mRenderWidth, mRenderHeight);
}

void GraphicsManager::stop() 
{
	Log::info("Stopping GraphicsManager.");

	// Releases textures.
	textureMapIterator textureIt;
	for (textureIt = mTextures.begin(); textureIt != mTextures.end(); ++textureIt) 
	{
		glDeleteTextures(1, &textureIt->second->texture);
		delete (*textureIt).first;
		delete (*textureIt).second;
	}
	mTextures.clear();
	// Releases shaders.
	gluintVectorIterator shaderIt;
	for (shaderIt = mShaders.begin(); shaderIt < mShaders.end(); ++shaderIt) 
	{
		glDeleteProgram(*shaderIt);
	}
	mShaders.clear();
	std::map<std::pair<const char*, const char*>, GLuint>::iterator shaderCacheIt;
	for (shaderCacheIt = mShadersCache.begin(); shaderCacheIt != mShadersCache.end(); ++shaderCacheIt)
	{
		glDeleteProgram(shaderCacheIt->second);
	}
	// Releases vertex buffers.
	gluintVectorIterator vertexBufferIt;
	for (vertexBufferIt = mVertexBuffers.begin(); vertexBufferIt < mVertexBuffers.end(); ++vertexBufferIt) 
	{
		glDeleteBuffers(1, &(*vertexBufferIt));
	}
	mVertexBuffers.clear();
	// Releases index buffers.
	gluintVectorIterator indexBufferIt;
	for (indexBufferIt = mIndexBuffers.begin(); indexBufferIt < mIndexBuffers.end(); ++indexBufferIt)
	{
		glDeleteBuffers(1, &(*indexBufferIt));
	}
	mIndexBuffers.clear();	
	mVertexArrays.clear();
	if (mRenderFrameBuffer != 0)
	{
		glDeleteFramebuffers(1, &mRenderFrameBuffer);
		mRenderFrameBuffer = 0;
	}
	if (mRenderTexture != 0)
	{
		glDeleteTextures(1, &mRenderTexture);
		mRenderTexture = 0;
	}
	// Destroys OpenGL context.
	if (mDisplay != NULL)
	{
		glfwDestroyWindow(mDisplay);
		mDisplay = NULL;
	}
}

GLuint GraphicsManager::loadShader(const char* pVertexShader, const char* pFragmentShader) 
{
	std::map<std::pair<const char*, const char*>, GLuint>::iterator iter;
	for (iter = mShadersCache.begin(); iter != mShadersCache.end(); ++iter)
	{
		if ((strcmp(iter->first.first, pVertexShader) == 0) && (strcmp(iter->first.second, pFragmentShader) == 0))
		{
			Log::info("Shader with id %u found in cache", iter->second);

			return iter->second;
		}
	}

	GLint result; char log[256];
	GLuint vertexShader, fragmentShader, shaderProgram;
	// Builds the vertex shader.
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &pVertexShader, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(vertexShader, sizeof(log), 0, log);
		Log::error("Vertex shader error: %s", log);
		goto __ERROR__;
	}
	// Builds the fragment shader.
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pFragmentShader, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(log), 0, log);
		Log::error("Fragment shader error: %s", log);
		goto __ERROR__;
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (result == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(log), 0, log);
		Log::error("Shader program error: %s", log);
		goto __ERROR__;
	}

	mShadersCache.insert(std::make_pair(std::make_pair(pVertexShader, pFragmentShader), shaderProgram));

	return shaderProgram;
__ERROR__:
	Log::error("Error loading shader.");
	if (vertexShader > 0) glDeleteShader(vertexShader);
	if (fragmentShader > 0) glDeleteShader(fragmentShader);
	return 0;
}

GLuint GraphicsManager::loadAllShaders(const char* pVertexShader, const char* pFragmentShader, const char* pGeometryShader)
{
	GLint result; char log[256];
	GLuint vertexShader, fragmentShader, geometryShader, shaderProgram;
	// Builds the vertex shader.
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &pVertexShader, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(vertexShader, sizeof(log), 0, log);
		Log::error("Vertex shader error: %s", log);
		goto __ERROR__;
	}
	// Builds the fragment shader.
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pFragmentShader, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(log), 0, log);
		Log::error("Fragment shader error: %s", log);
		goto __ERROR__;
	}
	// Builds the geometry shader.
	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &pGeometryShader, NULL);
	glCompileShader(geometryShader);
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(geometryShader, sizeof(log), 0, log);
		Log::error("Geometry shader error: %s", log);
		goto __ERROR__;
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
	if (result == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(log), 0, log);
		Log::error("Shader program error: %s", log);
		goto __ERROR__;
	}
	mShaders.push_back(shaderProgram);
	return shaderProgram;
__ERROR__:
	Log::error("Error loading shader.");
	if (vertexShader > 0) glDeleteShader(vertexShader);
	if (fragmentShader > 0) glDeleteShader(fragmentShader);
	if (geometryShader > 0) glDeleteShader(geometryShader);
	return 0;
}

TextureProperties* const GraphicsManager::loadTextureDiffuse(const std::string path, bool invert)
{
	Log::info("Loading texture %s", path.c_str());

	// Looks for the texture in cache first.
	textureMapIterator textureIt;
	for (textureIt = mTextures.begin(); textureIt != mTextures.end(); ++textureIt)
	{
		if (strcmp(textureIt->second->textureResource->getPath(), path.c_str()) == 0)
		{
			Log::info("Found %s in cache", textureIt->second->textureResource->getPath());
			return textureIt->second;
		}
	}

	Resource* pResource = new Resource(path.c_str());
	TextureProperties* textureProperties = new TextureProperties; 
	GLuint texture;
	GLint width, height;

	if (invert) texture = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
	else texture = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MULTIPLY_ALPHA);

	if (texture > 0) 
	{
		int mipLevel = 0;
		glBindTexture(GL_TEXTURE_2D, texture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &height);
	}
	else goto __ERROR__;

	// Caches the loaded texture.
	textureProperties->texture = texture;
	textureProperties->textureResource = pResource;
	textureProperties->width = width;
	textureProperties->height = height;

	mTextures.insert(std::make_pair(pResource, textureProperties));

	Log::info("Success in creating and caching texture %u", texture);
	pResource->close();

	return const_cast<TextureProperties* const>(textureProperties);

__ERROR__:
	Log::error("Error loading texture into OpenGL.");
	pResource->close();
	delete pResource;
	return NULL;
}

TextureProperties* const GraphicsManager::loadTextureNormal(const std::string path)
{
	Log::info("Loading Normal texture %s", path.c_str());

	// Looks for the texture in cache first.
	textureMapIterator textureIt;
	for (textureIt = mTextures.begin(); textureIt != mTextures.end(); ++textureIt)
	{
		if (strcmp(textureIt->second->textureResource->getPath(), path.c_str()) == 0)
		{
			Log::info("Found %s in cache", textureIt->second->textureResource->getPath());
			return textureIt->second;
		}
	}

	Resource* pResource = new Resource(path.c_str());
	TextureProperties* textureProperties = new TextureProperties;

	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	// Caches the loaded texture.
	textureProperties->texture = textureID;
	textureProperties->textureResource = pResource;
	textureProperties->width = width;
	textureProperties->height = height;

	mTextures.insert(std::make_pair(pResource, textureProperties));

	Log::info("Success in creating and caching Normal texture %u", textureID);
	pResource->close();

	return const_cast<TextureProperties* const>(textureProperties);
}

TextureProperties* const GraphicsManager::loadCubeMap(std::vector<std::string> faces)
{
	Log::info("Loading Cube map");

	Resource* pResource = NULL;
	TextureProperties* textureProperties = NULL;
	textureMapIterator textureIt;
	GLint error;

	if (faces.size() < 6) goto __ERROR__;

	// Looks for the texture in cache first.
	for (textureIt = mTextures.begin(); textureIt != mTextures.end(); ++textureIt)
	{		
		if (strcmp(textureIt->second->textureResource->getPath(), faces[0].c_str()) == 0)
		{
			Log::info("Found %s in cache", textureIt->second->textureResource->getPath());
			return textureIt->second;
		}
	}

	pResource = new Resource(faces[0].c_str());
	textureProperties = new TextureProperties;
	GLuint textureID;
	int width, height;
	unsigned char* image;

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); ++i)
	{
		image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		Log::error("Error loading texture %s", glewGetErrorString(error));
		goto __ERROR__;
	}

	// Caches the loaded texture.
	textureProperties->texture = textureID;
	textureProperties->textureResource = pResource;
	textureProperties->width = width;
	textureProperties->height = height;

	mTextures.insert(std::make_pair(pResource, textureProperties));

	return textureProperties;

__ERROR__:
	if (pResource) delete pResource;
	if (textureProperties) delete textureProperties;

	Log::error("Error loading cubemap texture");

	return NULL;
}

GLuint GraphicsManager::loadShaderFile(const char* vertex, const char* fragment)
{
	Log::info("Loading shader files %s %s", vertex, fragment);

	std::map<std::pair<const char*, const char*>, GLuint>::iterator shaderIter;
	for (shaderIter = mFileShadersCache.begin(); shaderIter != mFileShadersCache.end(); ++shaderIter)
	{
		if (strcmp(shaderIter->first.first, vertex) == 0 || strcmp(shaderIter->first.second, fragment) == 0)
		{
			Log::info("Found shader %s in cache", shaderIter->first);
			return shaderIter->second;
		}
	}

	std::string vertexFile;
	std::string fragmentFile;
	// read entire file into string
	if (std::ifstream is{ vertex, std::ios::binary | std::ios::ate }) 
	{
		auto size = is.tellg();
		std::string str(size, '\0'); // construct string to stream size
		is.seekg(0);
		is.read(&str[0], size);
		vertexFile = str;
	}
	else 
	{
		goto __ERROR__;
	}

	// read entire file into string
	if (std::ifstream is{ fragment, std::ios::binary | std::ios::ate }) 
	{
		auto size = is.tellg();
		std::string str(size, '\0'); // construct string to stream size
		is.seekg(0);
		is.read(&str[0], size);
		fragmentFile = str;
	}
	else 
	{
		goto __ERROR__;
	}

	GLuint shaderProg = loadShader(vertexFile.c_str(), fragmentFile.c_str());
	mFileShadersCache.insert(std::make_pair(std::make_pair(vertex, fragment), shaderProg));

	return shaderProg;

__ERROR__:
	Log::error("Error reading shader files  %s  %s", vertex, fragment);

	return NULL;
}

status GraphicsManager::removeTexture(const std::string path)
{
	Log::info("Removing texture %s", path.c_str());
	// Looks for the texture in cache first.
	textureMapIterator textureIt;
	for (textureIt = mTextures.begin(); textureIt != mTextures.end(); ++textureIt)
	{
		if (strcmp(textureIt->second->textureResource->getPath(), path.c_str()) == 0)
		{
			Log::info("Remove texture %s success", path.c_str());
			delete ((*textureIt).first);
			delete ((*textureIt).second);
			mTextures.erase(textureIt);

			return STATUS_OK;
		}
	}

	Log::warn("Removing texture %s not found", path.c_str());

	return STATUS_KO;
}

std::vector<Assimp3D> GraphicsManager::load3dObject(const std::string path)
{
	Log::info("Loading 3D object %s", path.c_str());

	m3DcomponentsIterator iter;
	for (iter = m3Dcomponents.begin(); iter != m3Dcomponents.end(); ++iter)
	{
		if (strcmp(iter->first.c_str(), path.c_str()) == 0)
		{
			Log::info("Found %s in cache", iter->first.c_str());

			return *iter->second;
		}
	}

	std::vector<Assimp3D>* obj3D = new std::vector<Assimp3D>(loadAssimpObj(path.c_str()));
	m3Dcomponents.insert(std::make_pair(path, obj3D));

	Log::info("Success in creating and caching 3D object %s", path.c_str());

	return *obj3D;
}

std::vector<Assimp3D> GraphicsManager::loadStandard3dObject(Obj_type type)
{
	std::vector<Assimp3D>* retobj = NULL;
	switch (type)
	{
	case OBJECT_CUBE:
		retobj = new std::vector<Assimp3D>(loadAssimpCube());
		break;
	default:
		break;
	}

	return *retobj;
}

status GraphicsManager::remove3dObject(const std::string path)
{
	Log::info("Removing texture %s", path.c_str());

	m3DcomponentsIterator iter;
	for (iter = m3Dcomponents.begin(); iter != m3Dcomponents.end(); ++iter)
	{
		if (strcmp(iter->first.c_str(), path.c_str()) == 0)
		{
			Log::info("Removing 3D object %s", iter->first.c_str());
			delete (*iter).second;

			m3Dcomponents.erase(iter);

			return STATUS_OK;
		}
	}

	Log::warn("Removing 3D object %s, not found", path.c_str());

	return STATUS_KO;
}

GLuint GraphicsManager::loadVertexBuffer(const void* pVertexBuffer, int32_t pVertexBufferSize, VBO_Type_t vboType)
{
	Log::info("Loading Vertex buffer");

	GLuint vertexBuffer;
	// Upload specified memory buffer into OpenGL.
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	switch (vboType)
	{
	case VBO_STATIC:
		glBufferData(GL_ARRAY_BUFFER, pVertexBufferSize, pVertexBuffer, GL_STATIC_DRAW);
		break;
	case VBO_DYNAMIC:
		glBufferData(GL_ARRAY_BUFFER, pVertexBufferSize, pVertexBuffer, GL_DYNAMIC_DRAW);
		break;
	default:
		glBufferData(GL_ARRAY_BUFFER, pVertexBufferSize, pVertexBuffer, GL_STATIC_DRAW);
		break;
	}
	// Unbinds the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR)
		goto _ERROR_;
	mVertexBuffers.push_back(vertexBuffer);

	return vertexBuffer;
_ERROR_:
	Log::error("Error loading vertex buffer.");
	if (vertexBuffer > 0) glDeleteBuffers(1, &vertexBuffer);

	return 0;
}

GLuint GraphicsManager::loadVertexArray()
{
	Log::info("Loading VAO");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Unbinds the buffer.
	glBindVertexArray(0);
	if (glGetError() != GL_NO_ERROR)
		goto _ERROR_;
	mVertexArrays.push_back(vao);

	return vao;
_ERROR_:
	Log::error("Error loading VAO.");
	if (vao > 0) glGenVertexArrays(1, &vao);

	return 0;
}

GLuint GraphicsManager::loadIndexBuffer(const void* pIndexBuffer, int32_t pIndexBufferSize)
{
	Log::info("Loading index buffer");

	GLuint indexBuffer;
	// Upload specified memory buffer into OpenGL.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIndexBufferSize, pIndexBuffer, GL_STATIC_DRAW);
	// Unbinds the buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR)
		goto _ERROR_;
	mIndexBuffers.push_back(indexBuffer);

	return indexBuffer;
_ERROR_:
	Log::error("Error loading index buffer.");
	if (indexBuffer > 0) glDeleteBuffers(1, &indexBuffer);

	return 0;
}

GLuint generateMultiSampleTexture(GLuint samples, int32_t screenWidth, int32_t screenHeight)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, screenWidth, screenHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	return texture;
}

status GraphicsManager::initializeRenderBuffer()
{
	Log::info("Loading offscreen buffer");

	const RenderVertex vertices[] = {
		{ -1.0f, -1.0f, 0.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	};
	float screenRatio = float(mRenderHeight) / float(mRenderWidth);
	//mRenderWidth = DEFAULT_RENDER_WIDTH;
	//mRenderHeight = int32_t(float(mRenderWidth) * screenRatio);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mScreenFrameBuffer);
	/*Create a texture for off-screen rendering, like we have seen previously. In
	glTexImage2D(), pass a NULL value as the last parameter to create only the
	surface without initializing its content:*/
#ifdef DEPTH_BUFFER
	glGenTextures(1, &mRenderTexture);
	glBindTexture(GL_TEXTURE_2D, mRenderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mRenderWidth, mRenderHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	/*Then, create an off-screen framebuffer with glGenFramebuffers().
	Attach the previous texture to it with glBindFramebuffer().*/
	glGenFramebuffers(1, &mRenderFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTexture, 0);
	// depth buffer
	glGenRenderbuffers(1, &mRenderDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mRenderWidth, mRenderHeight);
	// Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderDepthBuffer);
#elif defined(DEPTH_BUFFER_AA)
	glGenFramebuffers(1, &mRenderFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
	// Create a multisampled color attachment texture
	mRenderTexture = generateMultiSampleTexture(MSAA, mRenderWidth, mRenderHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mRenderTexture, 0);
	// Create a renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &mRenderDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderDepthBuffer);
	if (DEPTH_BITS == 32)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH32F_STENCIL8, mRenderWidth, mRenderHeight);
	}
	else
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderDepthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::error("AA Framebuffer is not complete!");
		goto ERROR;
	}
	// Terminate by restoring the device state
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
#if defined(BLOOM)
	
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// - Create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	
	glGenTextures(2, colorBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mRenderWidth, mRenderHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// - Create and attach depth buffer (renderbuffer)
	
	glGenRenderbuffers(1, &bloomRboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, bloomRboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mRenderWidth, mRenderHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bloomRboDepth);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint BLOOMattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, BLOOMattachments);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("Framebuffer not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Ping pong framebuffer for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mRenderWidth, mRenderHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// Also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Log::error("Framebuffer not complete!");
	}

	mBlurProgram = loadShader(VERTEX_BLUR, FRAGMENT_BLUR);
	if (mBlurProgram == 0)
		goto __ERROR__;

	mBloomFinalProgram = loadShader(BLOOM_VERTEX_FINAL, BLOOM_FRAGMENT_FINAL);
	if (mBlurProgram == 0)
		goto __ERROR__;

	uTexture0 = glGetUniformLocation(mBloomFinalProgram, "scene");
	uTexture1 = glGetUniformLocation(mBloomFinalProgram, "bloomBlur");
#endif	
#ifdef AA_SECOND
	// Framebuffers

	glGenFramebuffers(1, &mRenderFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
	// Create a multisampled color attachment texture
	GLuint textureColorBufferMultiSampled = generateMultiSampleTexture(MSAA, mRenderWidth, mRenderHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
	// Create a renderbuffer object for depth and stencil attachments

	glGenRenderbuffers(1, &mRenderDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderDepthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH32F_STENCIL8, mRenderWidth, mRenderHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderDepthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
#ifdef SHADOW_MAPPING
	// Configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	simpleDepthShader = loadShader(SHADOW_VERTEX, SHADOW_FRAGMENT);
	if (simpleDepthShader == 0)
		goto __ERROR__;
#endif
	/*Create the shader program used to render texture to screen and retrieve its
	attributes and uniforms:*/
	mRenderVertexBuffer = loadVertexBuffer(vertices, sizeof(vertices));
	if (mRenderVertexBuffer == 0)
		goto __ERROR__;
	mRenderShaderProgram = loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mRenderShaderProgram == 0)
		goto __ERROR__;
	aPosition = glGetAttribLocation(mRenderShaderProgram, "aPosition");
	aTexture = glGetAttribLocation(mRenderShaderProgram, "aTexture");
	uTexture = glGetUniformLocation(mRenderShaderProgram, "uTexture");
	uGamma = glGetUniformLocation(mRenderShaderProgram, "gamma");

#if defined(DEPTH_BUFFER_AA)
	uMSAA = glGetUniformLocation(mRenderShaderProgram, "uMSAA");
#endif

	return STATUS_OK;
__ERROR__:
	Log::error("Error while loading offscreen buffer");

	return STATUS_KO;
}