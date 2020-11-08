#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>

#include "constants.h"
#include "cpu/cpu.h"
#include "fs.h"
#include "memory_map.h"
#include "mmu.h"
#include "ppu.h"
#include "ppu_map_proxy.h"
#include "render/gl_rect.h"
#include "render/gl_shader.h"
#include "render/gl_texture.h"
#include "render/sdl.h"

/* clang-format off */
// Order matters here
#include <imgui.h>
#include <imfilebrowser.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
/* clang-format on */

#include "buttons.h"

int main(int argc, char* argv[])
{
    sdl_init();

    Window window;

    if (!sdl_open(&window)) {
        fprintf(stderr, "Failed to open SDL window\n");
        return 1;
    }

    sdl_bind(&window, SDL_SCANCODE_Q, ButtonQuit);
    sdl_bind(&window, SDL_SCANCODE_O, ButtonOpenFile);
    sdl_bind(&window, SDL_SCANCODE_BACKSPACE, ButtonCloseDialog);
    sdl_bind(&window, SDL_SCANCODE_M, ButtonToggleMap);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window.handle, window.context);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    ImGui::FileBrowser file_dialog;
    file_dialog.SetTitle("Choose a ROM");
    bool file_dialog_open {false};

    u8* mem = new u8[0x10000]();

    Cpu cpu(CpuRegisters {
        .io = mem[kIo],
        .if_ = mem[kIf],
        .ie = mem[kIe],
    });
    Ppu ppu(PpuRegisters {
        .if_ = mem[kIf],
        .lcdc = mem[kLcdc],
        .stat = mem[kStat],
        .scx = mem[kScx],
        .scy = mem[kScy],
        .ly = mem[kLy],
        .lyc = mem[kLyc],
        .wy = mem[kWy],
        .wx = mem[kWx],
    });
    PpuMapProxy ppu_proxy(ppu);
    Mmu mmu(cpu, ppu_proxy, mem);

    //cpu.debug = true;

    bool use_bios = true;

    if (use_bios) {
        auto data = FS::read_bytes("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");
        memcpy(mmu.memory, data.data(), data.size());
    }

    cpu.reset(use_bios);
    ppu.reset(use_bios);

    // Sound
    mmu.write8(kNr10, 0x80);
    mmu.write8(kNr11, 0xBF);
    mmu.write8(kNr12, 0xF3);
    mmu.write8(kNr14, 0xBF);
    mmu.write8(kNr21, 0x3F);
    mmu.write8(kNr24, 0xBF);
    mmu.write8(kNr30, 0x7F);
    mmu.write8(kNr31, 0xFF);
    mmu.write8(kNr32, 0x9F);
    mmu.write8(kNr34, 0xBF);
    mmu.write8(kNr41, 0xFF);
    mmu.write8(kNr44, 0xBF);
    mmu.write8(kNr50, 0x77);
    mmu.write8(kNr51, 0xF3);
    mmu.write8(kNr52, 0xF1);

    // CPU registers
    mmu.write8(kIo, 0xCF);
    mmu.write8(kIf, 0xE1);
    mmu.write8(kIe, 0x00);

    // PPU registers
    mmu.write8(kLcdc, 0x91);
    mmu.write8(kStat, 0x00);
    mmu.write8(kScy, 0x00);
    mmu.write8(kScy, 0x00);
    mmu.write8(kLy, 0x00);
    mmu.write8(kLyc, 0x00);
    mmu.write8(kBgPalette, 0xFC);
    mmu.write8(kObj0Palette, 0xFF);
    mmu.write8(kObj1Palette, 0xFF);
    mmu.write8(kWy, 0x00);
    mmu.write8(kWx, 0x00);

    Texture screen;
    texture_init(&screen, kLcdWidth, kLcdHeight, 1);

    Texture map;
    texture_init(&map, kMapWidth, kMapHeight * 2, 4);

    Rect rect;
    rect_init(&rect);
    const auto screen_transform {glm::mat4(1.0f)};
    const auto screen_projection {glm::ortho(0.0f, 1.0f, 0.0f, 1.0f)};
    const auto map_transform {glm::translate(glm::vec3 {0.0f, 0.5f, 0.0f}) * glm::scale(glm::vec3 {0.5f, 0.5f, 1.0f})};
    const auto map_projection {glm::ortho(0.0f, 1.0f, 0.0f, 1.0f)};

    GLuint rect_program = load_shader("assets/shaders/quad.vert.glsl", "assets/shaders/quad.frag.glsl");

    glClearColor(0.0f, 0.2f, 0.6f, 1.0f);

    bool map_visible {false};
    bool paused = false;
    while (true) {
        bool quit = false;
        quit |= sdl_poll(&window);
        quit |= sdl_get_action(&window, ButtonQuit, false);
        if (quit)
            break;

        if (sdl_get_action(&window, ButtonOpenFile, false)) {
            if (!file_dialog_open) {
                file_dialog.Open();
                file_dialog_open = true;
            }
        }

        if (sdl_get_action(&window, ButtonCloseDialog, false)) {
            if (file_dialog_open) {
                file_dialog.Close();
                file_dialog_open = false;
            }
        }

        if (sdl_get_action(&window, ButtonToggleMap, false)) {
            map_visible = !map_visible;
            paused = !paused;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window.handle);
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);

        if (!paused) {
            bool redraw = true;
            int cycles = 0;
            while (cycles < kMaxCyclesPerFrame && cpu.is_enabled()) {
                int instruction_cycles = cpu.tick(mmu);
                ppu.tick(instruction_cycles);
                cycles += instruction_cycles;
            }
            if (redraw) {
                texture_set_pixels(&screen, ppu.get_pixels());
                texture_set_pixels(&map, ppu_proxy.get_map_pixels());
            }
        }

        rect_draw(&rect, screen.handle, rect_program, screen_projection, screen_transform);

        if (file_dialog_open) {
            file_dialog.Display();
            if (file_dialog.HasSelected()) {
                auto selection = file_dialog.GetSelected();
                if (selection.extension() == ".bin" || selection.extension() == ".gb" || selection.extension() == ".dmg") {
                    //mmu_set_rom(&mmu, FS::read_bytes(selection.string()));
                    file_dialog.ClearSelected();
                    file_dialog.Close();
                    file_dialog_open = false;
                    paused = false;
                }
            }
        }

        if (map_visible) {
            ImGui::SetNextWindowSize({0.0f, 0.0f});
            ImGui::Begin("Map", nullptr, ImGuiWindowFlags_NoResize);
            ImVec2 uv_min {0.0f, 0.0f};
            ImVec2 uv_max {1.0f, 1.0f};
            ImGui::Image((ImTextureID)map.handle, ImVec2(400.0f, 400.0f), uv_min, uv_max);
            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        sdl_flip(&window);
    }

    delete[] mem;

    rect_free(&rect);
    texture_free(&map);
    texture_free(&screen);
    glDeleteProgram(rect_program);
    sdl_close(&window);

    sdl_free();

    return 0;
}
