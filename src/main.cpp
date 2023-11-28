#include "ray.hpp"
#include <iostream>
#include <glm/gtc/random.hpp>
#include <chrono>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>


#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "vendor/stb_image_write.h"
#include "vendor/BS_thread_pool.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_sdl2.h"
#include "vendor/imgui/imgui_impl_sdlrenderer2.h"
#include "scene.hpp"
#include "camera.hpp"
#include "plane.hpp"
#include "renderer.hpp"


using namespace glm;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;


BS::thread_pool pool(8);
Scene *scene = new Scene();
Camera *camera = new Camera(70.0f, 0.1f, 100.0f);


void SetupScene() {
    auto *purple = new Material(vec3(0.8f, 0.2f, 0.8f));
    auto *green = new Material(vec3(0.2f, 0.8f, 0.2f));
    auto *red = new Material(vec3(0.8f, 0.2f, 0.2f));
    auto *blue = new Material(vec3(0.2f, 0.2f, 0.8f));
    auto *white = new Material(vec3(1.0f, 1.0f, 1.0f));


    vec3 leftWallNormal = vec3(1, 0, 0);
    scene->objects.push_back(new Plane(vec3(-5, 0, 0), leftWallNormal, red));
    vec3 backWallNormal = vec3(0, 0, -1);
    scene->objects.push_back(new Plane(vec3(0, 0, 0), backWallNormal, white));
    vec3 rightWallNormal = vec3(-1, 0, 0);
    scene->objects.push_back(new Plane(vec3(5, 0, 0), rightWallNormal, blue));
    vec3 topWallNormal = vec3(0, -1, 0);
    scene->objects.push_back(new Plane(vec3(0, 5, 0), topWallNormal, white));
    vec3 bottomWallNormal = vec3(0, 1, 0);
    scene->objects.push_back(new Plane(vec3(0, -5, 0), bottomWallNormal, white));


    scene->objects.push_back(new Sphere(vec3(2, -1.9, -4), 1.0f, green));
    scene->objects.push_back(new Sphere(vec3(-2, 0, -4), 1.0f, purple));


    camera->OnResize(WIDTH, HEIGHT);


}

unsigned char *data = new unsigned char[WIDTH * HEIGHT * 3];

SDL_Texture *texture = nullptr;

void Draw() {



    if(scene->shouldRender) {
        scene->shouldRender = false;
        for (uint32_t y = 0; y < HEIGHT; y++) {
            for (uint32_t x = 0; x < WIDTH; x++) {

                pool.submit([x, y]() -> void {
                    vec3 finalColor = RTRenderer::DoPixel(x, y, scene, camera);
                    data[(y * WIDTH + x) * 3] = (unsigned char) (finalColor.x * 255);
                    data[(y * WIDTH + x) * 3 + 1] = (unsigned char) (finalColor.y * 255);
                    data[(y * WIDTH + x) * 3 + 2] = (unsigned char) (finalColor.z * 255);
                });


            }
        }
        pool.wait_for_tasks();

    }


    SDL_UpdateTexture(texture, nullptr, data, WIDTH * 3);



    ImGui::Begin("Raytracer");
    ImVec2 windowSize = ImGui::GetWindowSize();

    windowSize.x -= 20;
    windowSize.y -= 40;

    ImGui::Image(texture, windowSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();


    ImGui::Begin("Settings");
        ImGui::SliderInt("Samples Per Pixel", &scene->samplesPerPixel, 1, 200);
        ImGui::SliderInt("Max Depth", &scene->maxDepth, 1, 10);
    ImGui::End();


    //spheres and their materials
    ImGui::Begin("Scene");
    if(ImGui::Button("Render")) {
        scene->shouldRender = true;
    }
    ImGui::Separator();
    for(int i = 0; i < scene->objects.size(); i++) {
        if(dynamic_cast<Sphere*>(scene->objects[i])) {
            ImGui::PushID(i);
            Sphere* sphere = dynamic_cast<Sphere*>(scene->objects[i]);
            ImGui::Text("Sphere %d", i);
            ImGui::SliderFloat3("Position", &sphere->center.x, -5.0f, 5.0f);
            ImGui::SliderFloat("Radius", &sphere->radius, 0.1f, 5.0f);
            ImGui::ColorEdit3("Color", &sphere->material->albedo.x);
            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::End();




}


int main() {
    std::cout << "Hello, World!" << std::endl;


    SetupScene();


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1080, 720,
                              SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    //create texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
            ImGui_ImplSDL2_ProcessEvent(&event);
        }


        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        //begin dockspace
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


        Draw();


        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);


    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();





//    auto *data = new unsigned char[WIDTH * HEIGHT * 3];
//
//
//    //start time
//    auto start = std::chrono::high_resolution_clock::now();
//
//
//    for (uint32_t y = 0; y < HEIGHT; y++) {
//        for (uint32_t x = 0; x < WIDTH; x++) {
//
//            pool.submit([x, y, data]() -> void {
//                vec3 finalColor = RTRenderer::DoPixel(x, y, scene, camera);
//
//
//                data[(y * WIDTH + x) * 3] = (unsigned char) (finalColor.x * 255);
//                data[(y * WIDTH + x) * 3 + 1] = (unsigned char) (finalColor.y * 255);
//                data[(y * WIDTH + x) * 3 + 2] = (unsigned char) (finalColor.z * 255);
//            });
//
//
//        }
//    }
//
//
//    pool.wait_for_tasks();
//
//
//    //find how long it took in miliseconds
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//    std::cout << "Time taken: " << duration << "ms" << std::endl;
//
//
//    stbi_flip_vertically_on_write(1);
//    stbi_write_png("test.png", WIDTH, HEIGHT, 3, data, WIDTH * 3);

    return 0;
}
