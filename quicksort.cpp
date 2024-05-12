#include "inc/SDL.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <atomic>

#undef main

constexpr float UPDATE_FREQUENCY = 1;
constexpr float TARGET_FRAME_TIME = 1000.0f / 30.0f;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;

std::atomic<bool> sortingFinished(false);

struct SortArgs {
    int* arr;
    int low;
    int high;
};

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high];  
    int i = (low - 1);  

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;  
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void sort_algorithm(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        sort_algorithm(arr, low, pi - 1);
        sort_algorithm(arr, pi + 1, high);
    }
}

int sort(void* args) {
    auto timer_start = std::chrono::high_resolution_clock::now();

    SortArgs* sortArgs = static_cast<SortArgs*>(args);
    sort_algorithm(sortArgs->arr, sortArgs->low, sortArgs->high);

    auto timer_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start);
    std::cout << "Sorting Finished in: " << duration.count() << "ms" << std::endl << std::endl;
    sortingFinished = true;

    return 0;
}

int main(int argc, char* argv[]) {
    SDL_Surface* surface = SDL_LoadBMP("image.bmp");
    if (surface == nullptr) {
        std::cerr << "Failed to load image! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    const int WIDTH = surface->w;
    const int HEIGHT = surface->h;

    const int pixels = WIDTH * HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Image Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        std::cerr << "Failed to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_RenderClear(renderer);

    SDL_Event event;
    bool running = true;
    int x = 0, y = 0;
    int counter = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 testTime = SDL_GetTicks();

    //std::cout << "Creating Array..." << std::endl;

    std::vector<int> arr(pixels);
    for (int i = 0; i < pixels; i++){
        arr[i] = i;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(arr.begin(), arr.end(), std::default_random_engine(seed));

    std::cout << "Press 'Enter' while in the render window to Start:" << std::endl;
    bool key_pressed = false;
    while (!key_pressed) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                key_pressed = true;
                break;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                key_pressed = true;
                break;
            }
        }
    }
    std::cout << "Sorting..." << std::endl;

    SortArgs args = {arr.data(), 0, pixels - 1};
    SDL_Thread* thread = SDL_CreateThread(sort, "sort", &args);

    float scaleX = static_cast<float>(WINDOW_WIDTH) / WIDTH;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / HEIGHT;
    int dstWidth = std::max(static_cast<int>(scaleX), 1);
    int dstHeight = std::max(static_cast<int>(scaleY), 1);
    int dstX = x * scaleX;
    int dstY = y * scaleY;
    int windowPixels = WINDOW_HEIGHT * WINDOW_WIDTH;

    while (running) {
        int currentPixel = (x + (WIDTH * y));
        int currentScalePixel = (dstX + (WINDOW_WIDTH * dstY));

        if (currentPixel == arr[currentPixel]){
            SDL_Rect srcRect = { x, y, 1, 1 };
            SDL_Rect dstRect = { dstX, dstY, dstWidth, dstHeight };
            SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);

            counter++;
        }

        dstX++;
        if (dstX >= WINDOW_WIDTH) {
            dstX = 0;
            dstY++;
            if (dstY >= WINDOW_HEIGHT) {
                Uint32 frameTime = SDL_GetTicks() - startTime;

                if (frameTime > TARGET_FRAME_TIME && counter > UPDATE_FREQUENCY){
                    if (SDL_PollEvent(&event) != 0 && event.type == SDL_QUIT) {
                        running = false;
                        break;
                    }

                    //std::cout << frameTime << "ms" << std::endl;
                    //std::cout << counter << "/" << windowPixels << "Pixels" << std::endl;
                    SDL_RenderPresent(renderer);
                    startTime = SDL_GetTicks();
                }

                if (sortingFinished == true && windowPixels <= counter){
                    running = false;
                } else {
                    SDL_RenderClear(renderer);
                    counter = 0;
                    dstX = 0;
                    dstY = 0;
                }
            }
        }

        x = dstX/scaleX;
        y = dstY/scaleY;
    }
    SDL_RenderPresent(renderer);

    std::cout << "Press 'Enter' while in the render window to Exit:";
    key_pressed = false;
    while (!key_pressed) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                key_pressed = true;
                break;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                key_pressed = true;
                break;
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

//g++ quicksort.cpp -o QSmain.exe -Iinc -Llib -lSDL2 -lopengl32