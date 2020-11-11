#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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

void update_tiles_texture(Texture* texture, u8* mem, u8* palette);

struct Rom {
    std::filesystem::path path;
    std::vector<u8> data;
};

struct Cartridge {
    Rom rom;
    Rom bios;
};

void cartridge_load_bios(Cartridge* cart, const std::filesystem::path& file_path)
{
    assert(std::filesystem::file_size(file_path) == 0x100);

    cart->bios.path = std::filesystem::absolute(file_path);
    cart->bios.data = FS::read_bytes(file_path.string());

    fprintf(stdout, "Loaded BIOS '%s'\n", file_path.string().c_str());
}

void cartridge_load_rom(Cartridge* cart, const std::filesystem::path& file_path)
{
    static constexpr int kMaxRomSize {0x10000};

    const auto rom_size {std::filesystem::file_size(file_path)};

    if (rom_size > kMaxRomSize) {
        throw std::runtime_error("Rom too large: " + std::to_string(rom_size) + "/" + std::to_string(kMaxRomSize));
    }

    cart->rom.path = std::filesystem::absolute(file_path);
    cart->rom.data = FS::read_bytes(file_path.string());

    fprintf(stdout, "Loaded ROM '%s'\n", file_path.string().c_str());
}

void mmu_set_cartridge(Mmu* mmu, Cartridge* cart, bool skip_bios)
{
    assert(cart->rom.data.size());
    assert(cart->bios.data.size());

    if (skip_bios) {
        std::copy(cart->rom.data.begin(), cart->rom.data.end(), mmu->memory);
    }
    else {
        std::copy_n(cart->rom.data.begin() + 0x100, cart->rom.data.size() - 0x100, mmu->memory + 0x100);
        std::copy(cart->bios.data.begin(), cart->bios.data.end(), mmu->memory);
        mmu->set_rom_start(cart->rom.data.data());
    }
}

std::string get_time_as_string()
{
    time_t time_now = time(0);
    struct tm new_time;
    localtime_s(&new_time, &time_now);
    char buffer[80];
    strftime(buffer, 80, "%Y%m%d%H%M%S", &new_time);
    return std::string {buffer};
}

void dump_vram(const std::string& path, u8* memory)
{
    FILE* s {fopen(path.c_str(), "w+")};

    if (!s) {
        throw std::runtime_error("Failed to create VRAM dump " + path);
    }

    fwrite(&memory[0x8000], 0x2000, 1, s);
    fclose(s);
}

void run()
{
    sdl_init();

    if (!std::filesystem::exists("./bios.gb")) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                 "Missing BIOS",
                                 // clang-format off
                                 "Please place your legally dumped Game Boy boot ROM (BIOS) \n\
next to the executable and name it bios.gb",
                                 // clang-format on
                                 nullptr);
        sdl_free();
        return;
    }

    Window window;

    if (!sdl_open(&window)) {
        fprintf(stderr, "Failed to open SDL window\n");
        sdl_free();
        return;
    }

    sdl_bind(&window, SDL_SCANCODE_Q, ButtonQuit);
    sdl_bind(&window, SDL_SCANCODE_O, ButtonOpenFile);
    sdl_bind(&window, SDL_SCANCODE_BACKSPACE, ButtonCloseDialog);
    sdl_bind(&window, SDL_SCANCODE_M, ButtonToggleMap);
    sdl_bind(&window, SDL_SCANCODE_D, ButtonDumpVRAM);

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

    cpu.reset(skip_bios);
    ppu.reset(skip_bios);

    if (skip_bios) {
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

    Cartridge cart;
    cartridge_load_bios(&cart, {"./bios.gb"});

    Texture screen;
    texture_init(&screen, kLcdWidth, kLcdHeight, 1);

    Texture map;
    texture_init(&map, kMapWidth, kMapHeight * 2, 4);

    Texture tiles_texture;
    texture_init(&tiles_texture, 24 * 8, 16 * 8, 4);

    Rect rect;
    rect_init(&rect);
    const auto screen_transform {glm::mat4(1.0f)};
    const auto screen_projection {glm::ortho(0.0f, 1.0f, 0.0f, 1.0f)};
    const auto map_transform {glm::translate(glm::vec3 {0.0f, 0.5f, 0.0f}) * glm::scale(glm::vec3 {0.5f, 0.5f, 1.0f})};
    const auto map_projection {glm::ortho(0.0f, 1.0f, 0.0f, 1.0f)};

    GLuint rect_program = load_shader("assets/shaders/quad.vert.glsl", "assets/shaders/quad.frag.glsl");

    glClearColor(0.0f, 0.2f, 0.6f, 1.0f);

    bool map_visible {false};
    bool tiles_visible {false};
    bool paused {true};
    bool file_dialog_open {true};

    file_dialog.Open();

    int total_instructions = 0;

    // This is the base timer speed. It updates once every 16 cycles.
    u32 timer_counter = 0;
    u8& timer_clock = mem[kTima];

    // This updates once every 256 cycles.
    u32 divider_counter = 0;
    u8& divider_clock = mem[kDiv];

    struct Message {
        std::string str;
        std::chrono::time_point<std::chrono::steady_clock> expire;
    };

    std::vector<Message> messages;

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
        }

        if (sdl_get_action(&window, ButtonDumpVRAM, false)) {
            if (!cart.rom.data.empty()) {
                const auto dump_path = cart.rom.path.string() + "." + get_time_as_string() + ".vram_dump";
                dump_vram(dump_path, mmu.memory);
                messages.push_back({"Dumped VRAM to '" + dump_path + "'\n", std::chrono::steady_clock::now() + std::chrono::seconds(4)});
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window.handle);
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);

        if (!messages.empty()) {
            const auto now = std::chrono::steady_clock::now();

            messages.erase(
                std::remove_if(
                    messages.begin(),
                    messages.end(),
                    [&now](const Message& message) {
                        return now > message.expire;
                    }),
                messages.end());

            if (!messages.empty()) {
                ImGui::SetNextWindowPos({500, 40});
                ImGui::SetNextWindowSize({260, 0});
                ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
                for (auto const& message : messages) {
                    ImGui::TextWrapped("%s", message.str.c_str());
                }
                ImGui::End();
            }
        }

        if (!paused) {
            bool redraw = false;
            int cycles = 0;
            auto cpu_start = SDL_GetTicks();
            while (cycles < kMaxCyclesPerFrame && cpu.is_enabled() && (SDL_GetTicks() - cpu_start) < 16) {
                int instruction_cycles = cpu.tick(mmu);

                int sync_ticks = 0;
                while (sync_ticks != instruction_cycles) {
                    ppu.tick(redraw);
                    sync_ticks++;
                }

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

                    try {
                        cartridge_load_rom(&cart, selection);
                        mmu_set_cartridge(&mmu, &cart, skip_bios);
                    }
                    catch (const std::runtime_error& e) {
                        fprintf(stderr, "ROM failed to load: %s\n", e.what());
                    }

                    file_dialog.ClearSelected();
                    file_dialog.Close();
                    file_dialog_open = false;
                }
            }
        }

        if (!file_dialog.IsOpened()) {
            ImGui::Begin("Debug");
            ImGui::Checkbox("Paused", &paused);
            ImGui::Checkbox("Debug", &cpu.debug);

            if (tiles_visible) {
                if (ImGui::Button("Hide tiles"))
                    tiles_visible = false;
                ImGui::SameLine();
                if (ImGui::Button("Refresh"))
                    update_tiles_texture(&tiles_texture, mem, ppu.bg_palette);
            }
            else {
                if (ImGui::Button("Show tiles")) {
                    tiles_visible = true;
                    update_tiles_texture(&tiles_texture, mem, ppu.bg_palette);
                }
            }

            if (map_visible) {
                if (ImGui::Button("Hide map"))
                    map_visible = false;
            }
            else {
                if (ImGui::Button("Show map")) {
                    map_visible = true;
                    texture_set_pixels(&map, ppu_proxy.get_map_pixels());
                }
            }

            ImGui::End();
        }

        if (map_visible) {
            ImGui::SetNextWindowSize({0.0f, 0.0f});
            ImGui::Begin("Map", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImVec2 uv_min {0.0f, 0.0f};
            ImVec2 uv_max {1.0f, 1.0f};
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(map.handle)), ImVec2(400.0f, 400.0f), uv_min, uv_max);
            ImGui::End();
        }

        if (tiles_visible) {
            ImGui::SetNextWindowSize({0.0f, 0.0f});
            ImGui::Begin("Tiles", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImVec2 uv_min {0.0f, 0.0f};
            ImVec2 uv_max {1.0f, 1.0f};
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tiles_texture.handle)), ImVec2(400.0f, 400.0f), uv_min, uv_max);
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
}

void update_tiles_texture(Texture* texture, u8* mem, u8* palette)
{
    u8* map = &mem[0x9800];
    u8* tiles = &mem[0x8000];

    const int tiles_w = 24;
    const int tiles_h = 16;
    const int tex_w = tiles_w * 8;
    const int tex_h = tiles_h * 8;
    const int n_pixels = tex_w * tex_h;

    std::array<u32, n_pixels> pixels {0};

    for (int y = 0; y < tex_h; y++) {
        for (int x = 0; x < tex_w; x++) {
            int tile_x = x / 8;
            int tile_y = y / 8;
            int tile_idx = tile_y * tiles_w + tile_x;
            int row_addr = (tile_idx * 16) + ((y % 8) * 2);
            u8* row = &tiles[row_addr];
            u8 mask = 0x80 >> (x % 8);
            u8 pal_idx = !!(row[0] & mask) | !!(row[1] & mask) << 1;
            u8 pc = palette[pal_idx];
            u32 c = 0xFF000000 | (pc << 16) | (pc << 8) | pc;
            pixels[y * tex_w + x] = c;
        }
    }

    texture_set_pixels(texture, reinterpret_cast<const u8*>(pixels.data()));
}

int main(int argc, char* argv[])
{
    try {
        run();
    }
    catch (const std::runtime_error& e) {
        fprintf(stderr, "Caught exception: %s\n", e.what());
    }
    catch (...) {
        fprintf(stderr, "Caught exception\n");
    }

    return 0;
}
