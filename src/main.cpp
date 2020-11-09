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

    cpu.debug = false;

    bool skip_bios = true;

    std::vector<u8> bios_data;
    std::vector<u8> rom_data = FS::read_bytes("C:/Dev/Korlow/roms/cpu_instrs/cpu_instrs.gb");
    std::vector<u8> rom_start(0x100);

    cpu.reset(skip_bios);
    ppu.reset(skip_bios);

    if (!skip_bios) {
        if (rom_data.size()) {
            // Copy from 0x100-N to memory
            std::memcpy(&mem[0x100], rom_data.data() + 0x100, rom_data.size() - 0x100);

            // Copy from 0-0x100 to temporary area
            // This gets swapped to actual memory when BIOS exits
            std::copy_n(rom_data.begin(), 0x100, rom_start.begin());
            mmu.set_rom_start(rom_start.data());
        }

        // Load BIOS
        bios_data = FS::read_bytes("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");
        std::memcpy(mmu.memory, bios_data.data(), bios_data.size());
    }
    else {
        // Copy the entire ROM to memory
        std::memcpy(mem, rom_data.data(), rom_data.size());

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
    }

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
    bool paused {false};
    bool file_dialog_open {false};

    if (rom_data.empty()) {
        paused = true;
        file_dialog_open = true;
        file_dialog.Open();
    }

    int total_instructions = 0;

    // This is the base timer speed. It updates once every 16 cycles.
    u32 timer_counter = 0;
    u8& timer_clock = mem[kTima];

    // This updates once every 256 cycles.
    u32 divider_counter = 0;
    u8& divider_clock = mem[kDiv];

    while (true) {
        bool quit = false;
        quit |= sdl_poll(&window);
        quit |= sdl_get_action(&window, ButtonQuit, false);
        if (quit)
            break;

        if (sdl_get_action(&window, ButtonOpenFile, false)) {
            if (!file_dialog_open) {
                paused = true;
                file_dialog.Open();
                file_dialog_open = true;
            }
        }

        if (sdl_get_action(&window, ButtonCloseDialog, false)) {
            if (file_dialog_open) {
                paused = false;
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
            auto cpu_start = SDL_GetTicks();
            while (cycles < kMaxCyclesPerFrame && cpu.is_enabled() && (SDL_GetTicks() - cpu_start) < 16) {
                int instruction_cycles = cpu.tick(mmu);
                ppu.tick(instruction_cycles);
                cycles += instruction_cycles;
                total_instructions++;

                divider_counter++;
                if (divider_counter == 16) {
                    divider_counter = 0;
                    divider_clock++;
                }
                timer_counter++;
                int divider = 1;
                int tac = mem[kTac];
                if (tac == 0)
                    divider = 64;
                else if (tac == 1)
                    divider = 4;
                else if (tac == 2)
                    divider = 8;
                else if (tac == 3)
                    divider = 16;
                if ((++timer_counter % divider) == 0) {
                    timer_counter = 0;
                    if (++timer_clock == 0) {
                        timer_clock = mem[kTma];
                        cpu.registers.if_ |= 0x4;
                    }
                }
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
                    cpu.reset(skip_bios);
                    mmu.reset(skip_bios);
                    ppu.reset(skip_bios);

                    // Load ROM
                    if (std::filesystem::file_size(selection) > 0x10000) {
                        fprintf(stderr, "Rom too large: %lld/%d\n", std::filesystem::file_size(selection), 0x10000);
                    }
                    else {
                        rom_data = FS::read_bytes(selection.string());

                        // Get first 0x100 bytes, which will replace the bootrom
                        std::copy_n(rom_data.begin(), 0x100, rom_start.begin());
                        mmu.set_rom_start(rom_start.data());

                        printf("Setting rom %s\n", selection.string().c_str());

                        // If we want to see the boot screen, set it up
                        if (!skip_bios) {
                            if (!bios_data.size()) {
                                bios_data = FS::read_bytes("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");
                            }
                            std::memcpy(mem, bios_data.data(), 0x100);
                            std::memcpy(&mem[0x100], rom_data.data() + 0x100, rom_data.size() - 0x100);
                            mmu.set_rom_start(rom_start.data());
                        }
                        // Otherwise just copy the whole thing
                        else {
                            std::memcpy(mem, rom_data.data(), rom_data.size());
                        }
                    }

                    file_dialog.ClearSelected();
                    file_dialog.Close();
                    file_dialog_open = false;
                }
            }
        }

        if (!file_dialog.IsOpened()) {
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            ImGui::Checkbox("Paused", &paused);
            ImGui::Checkbox("Debug", &cpu.debug);
            ImGui::End();
        }

        if (map_visible) {
            ImGui::SetNextWindowSize({0.0f, 0.0f});
            ImGui::Begin("Map", nullptr, ImGuiWindowFlags_NoResize);
            ImVec2 uv_min {0.0f, 0.0f};
            ImVec2 uv_max {1.0f, 1.0f};
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(map.handle)), ImVec2(400.0f, 400.0f), uv_min, uv_max);
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
