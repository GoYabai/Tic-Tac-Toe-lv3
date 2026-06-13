/**
 * Tic-tac-toe Game in C++ -- Level 3 (Functional Programming)
 *
 * Mô tả:
 *   File `main.cpp` là "composition root" của chương trình:
 *   nơi DUY NHẤT chúng ta tạo ra Logger, RNG, Renderer, Interaction…
 *   rồi INJECT chúng xuống Engine.
 *
 *   Sau khi sinh viên hoàn thành mức 1 task 3, KHÔNG có biến toàn cục
 *   nào liên quan tới Logger / RNG nữa -- mọi phụ thuộc đều được khai báo
 *   tường minh ở đây.
 *
 * level 3.0 (FP paradigm -- pure core / imperative shell)
 * base from level 2 (OOP) v0.5 13/05/2026
 */

/* ---------- Importing ---------- */

#include <iostream>
#include <memory>

#include "interface/i_interaction.h"
#include "interface/i_renderer.h"
#include "shell/engine.h"
#include "shell/logger.h"
#include "shell/rng.h"
#include "terminal/interaction.h"
#include "terminal/renderer.h"
#include "utils/config.h"

// SDL -- chỉ include nếu sinh viên muốn giữ phần GUI từ level 2.
// #include "sdl/interaction.h"
// #include "sdl/renderer.h"

/**
 * Mô tả: Entry point.
 */
int main(int argc, char* argv[]) {
    // ── 1. Parse config ────────────────────────────────────────────────
    RunConfig config = parseArgs(argc, argv);

    if (config.is_help) {
        std::cout << configHelpStr();
        return 0;
    }

    // ── 2. Tạo Logger (KHÔNG global, được inject xuống Engine) ─────────
    Logger logger;
    logger.init(config.judge_mode, config.to_file, config.log_file, config.verbose_flag);
    logger.log("Logger initialized!");
    logger.log(config.toString(), Logger::Level::DEBUG);

    // ── 3. Tạo RNG (KHÔNG global, được inject xuống Engine/Bot) ────────
    Rng rng = makeRng(config.seed);
    logger.log("RNG initialized with seed = " + std::to_string(config.seed));

    // ── 4. Tạo Renderer & Interaction theo chế độ ──────────────────────
    std::unique_ptr<I_Renderer> iRenderer;
    std::unique_ptr<I_Interaction> iInteraction;

    if (!config.gui_flag || config.judge_mode) {
        iRenderer = std::make_unique<TerminalRenderer>();
        iInteraction = std::make_unique<TerminalInteraction>();
        logger.log("Terminal renderer & interaction initialized!");
    } else {
        // Sinh viên muốn dùng SDL: bỏ comment khối dưới + bỏ comment include phía trên.
        //
        // iRenderer    = std::make_unique<SDLRenderer>();
        // iInteraction = std::make_unique<SDLInteraction>();
        // logger.log("SDL renderer & interaction initialized!");
        logger.log("SDL chưa được kích hoạt trong starter level 3 -- fallback Terminal.",
                   Logger::Level::WARNING);
        iRenderer = std::make_unique<TerminalRenderer>();
        iInteraction = std::make_unique<TerminalInteraction>();
    }

    // ── 5. Tạo Engine -- INJECT mọi dependency ──────────────────────────
    Engine engine(&config, iRenderer.get(), iInteraction.get(), &logger, &rng);

    try {
        engine.init();
        engine.startGame();
        GameResult result = engine.playGame();
        engine.endGame(result);
    } catch (const QuitException&) {
        logger.log("Quit signal received. Cleaning up...", Logger::Level::WARNING);
    } catch (const NotImplementedException& e) {
        logger.log(std::string("Some functions are not implemented yet: ") + e.what(),
                   Logger::Level::WARNING);
    }

    // ── 6. Cleanup (RAII tự lo phần còn lại) ───────────────────────────
    engine.close();
    logger.log("Engine destroyed!");
    logger.log("Logger closing . . .");
    logger.close();

    return 0;
}

/* ---------- Compile ---------- */
/**
 * Open terminal at folder 'level-3':
 *
 * 1 - Prepare build folder:
 *     mkdir build && cd build
 *
 * 2 - Config project:
 *     cmake ..
 *
 * 3 - Compile:
 *     cmake --build .
 */

/*
level-3/
│
├── CMakeLists.txt
│
├── src/
│   ├── main.cpp                   <-- composition root: tạo & inject dependencies
│   │
│   ├── core/                      <-- functional core: PURE, không IO/Logger/RNG global
│   │   ├── types.h
│   │   ├── pipeline.h             (cung cấp sẵn -- pipe/compose/curry/identity)
│   │   ├── logic.{h,cpp}          (mức 1 -- task 1: FP-hoá Logic::*)
│   │   ├── bot_pure.{h,cpp}       (mức 1 -- task 2: HOF cho Bot LV1; mức 2: bot khác)
│   │   ├── parallel.{h,cpp}       (mức 3 -- nhánh B: tuỳ chọn)
│   │   └── recursion_combinator.cpp (demo Y -- mức 3 nhánh A, tham khảo)
│   │
│   ├── shell/                     <-- imperative shell: side-effect cô lập
│   │   ├── logger.{h,cpp}         (instance-based; mức 1 -- task 3)
│   │   ├── rng.h                  (instance-based; mức 1 -- task 2&3)
│   │   └── engine.{h,cpp}         (mức 2 -- không mutate board)
│   │
│   ├── interface/                 <-- abstract interface (giữ từ level 2)
│   │   ├── i_renderer.{h,cpp}
│   │   └── i_interaction.{h,cpp}
│   │
│   ├── terminal/                  <-- impl Terminal (chép từ level 2, đổi chữ ký Board)
│   │   ├── renderer.{h,cpp}
│   │   └── interaction.{h,cpp}
│   │
│   ├── sdl/                       <-- (TUỲ CHỌN) impl SDL -- không bắt buộc làm
│   │   └── ...
│   │
│   └── utils/
│       ├── config.{h,cpp}
│       ├── helper.h               <-- measureExecutionTime nhận callback (HOF)
│       └── helper.tpp
*/
