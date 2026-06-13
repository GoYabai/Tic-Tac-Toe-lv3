/**
 * Engine implementation
 */

#include "engine.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <sstream>
#include <thread>

#include "../core/bot_pure.h"
#include "../core/logic.h"
#include "../utils/helper.h"

/* ---------- Definitions ---------- */

Engine::Engine(const RunConfig *config,
               I_Renderer *iRenderer,
               I_Interaction *iInteraction,
               Logger *logger,
               Rng *rng)
    : config_(config),
      iRenderer_(iRenderer),
      iInteraction_(iInteraction),
      logger_(logger),
      rng_(rng) {}

Engine::~Engine() = default;

/* ---------- init ---------- */

void Engine::init()
{
    if (logger_)
        logger_->log("Engine initializing . . .");

    if (iRenderer_)
        iRenderer_->init(*config_);
    if (iInteraction_)
        iInteraction_->init(*config_);

    if (logger_)
        logger_->log("Engine initialized!");
}

bool Engine::sanity_check()
{
    bool ok = true;
    if (!iRenderer_)
    {
        if (logger_)
            logger_->log("Renderer not implemented!", Logger::Level::WARNING);
        ok = false;
    }
    if (!iInteraction_)
    {
        if (logger_)
            logger_->log("Interaction not implemented!", Logger::Level::WARNING);
        ok = false;
    }
    if (!logger_)
    {
        // không log được, nhưng vẫn fail safety check
        ok = false;
    }
    if (!rng_)
    {
        if (logger_)
            logger_->log("RNG not provided!", Logger::Level::ERROR);
        ok = false;
    }
    return ok;
}

/* ---------- startGame ---------- */

/**
 * Mô tả: Lấy input cấu hình từ user (size, goal, mode, bot levels).
 *
 * TODO (mức 1 -- task 3, đã được Logger DI):
 *   - Cài đặt giống level 2, NHƯNG dùng `logger_->log(...)` thay vì
 *     `Logger::log(...)`.
 *   - Sau khi xong: khởi tạo `gameState_` bằng `core::initBoard(...)`.
 */
void Engine::startGame()
{
    if (logger_)
        logger_->log("[Engine] Starting game . . .");

    if (!sanity_check())
    {
        if (logger_)
            logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return;
    }

    // TODO (mức 1 -- task 3):
    //   1. Hiển thị title (showSelectMenu(SelectType::TITLE_UI)).
    //   2. Lấy size, goal, mode, bot levels -- pattern do-while giống level 2.
    //   3. Khởi tạo gameState_:
    //        gameState_.board         = core::initBoard(gameSetup_.size);
    //        gameState_.currentPlayer = 0;
    //        gameState_.turn          = 0;
    //        gameState_.winner        = -1;
    //        gameState_.isFinished    = false;
    //
    //   QUAN TRỌNG: thay mọi `Logger::log(...)` bằng `logger_->log(...)`.
    if (config_->interactive)
    {
        iRenderer_->clearScreen();
        iRenderer_->showSelectMenu(SelectType::TITLE_UI);
        iInteraction_->pause();
    }
    bool isSelected;

    do
    {
        if (config_->interactive)
            iRenderer_->showSelectMenu(SelectType::SIZE_UI);
        isSelected = iInteraction_->selectSize(&gameSetup_.size);

        if (config_->interactive && !isSelected)
            iRenderer_->showInvalidSelect(SelectType::SIZE_UI, gameSetup_.size);
    } while (!isSelected);

    if (config_->interactive)
        iRenderer_->showValidSelect(SelectType::SIZE_UI, gameSetup_.size);

    if (logger_)
        logger_->log(std::format("user input 'size' = {}", gameSetup_.size), Logger::Level::DEBUG);
    do
    {
        if (config_->interactive)
            iRenderer_->showSelectMenu(SelectType::GOAL_UI, gameSetup_.size);
        isSelected = iInteraction_->selectGoal(&gameSetup_.goal, gameSetup_.size);

        if (config_->interactive && !isSelected)
            iRenderer_->showInvalidSelect(SelectType::GOAL_UI, gameSetup_.goal);
    } while (!isSelected);

    if (config_->interactive)
        iRenderer_->showValidSelect(SelectType::GOAL_UI, gameSetup_.goal);

    if (logger_)
        logger_->log(std::format("user input 'goal' = {}", gameSetup_.goal), Logger::Level::DEBUG);

    do
    {
        if (config_->interactive)
            iRenderer_->showSelectMenu(SelectType::GAME_MODE_UI);
        isSelected = iInteraction_->selectGameMode(&gameSetup_.mode);

        if (config_->interactive && !isSelected)
            iRenderer_->showInvalidSelect(SelectType::GAME_MODE_UI, (int)gameSetup_.mode);
    } while (!isSelected);

    if (config_->interactive)
        iRenderer_->showValidSelect(SelectType::GAME_MODE_UI, (int)gameSetup_.mode);

    if (logger_)
        logger_->log(std::format("user input 'game mode' = {}", modeToString((int)gameSetup_.mode)), Logger::Level::DEBUG);

    if (gameSetup_.mode == GameMode::PVE)
    {
        do
        {
            if (config_->interactive)
                iRenderer_->showSelectMenu(SelectType::BOT_LEVEL_UI);
            isSelected = iInteraction_->selectBotLevel(gameSetup_.levels.data(), 1);

            if (config_->interactive && !isSelected)
                iRenderer_->showInvalidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup_.levels[1]);
        } while (!isSelected);

        if (config_->interactive)
            iRenderer_->showValidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup_.levels[1]);

        if (logger_)
            logger_->log(std::format("user input 'bot level' = {}", botToString((int)gameSetup_.levels[1])), Logger::Level::DEBUG);
    }

    if (gameSetup_.mode == GameMode::EVE)
    {
        do
        {
            if (config_->interactive)
                iRenderer_->showSelectMenu(SelectType::MUL_BOT_LEVEL_UI, 0);
            isSelected = iInteraction_->selectBotLevel(gameSetup_.levels.data(), 0);

            if (config_->interactive && !isSelected)
                iRenderer_->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup_.levels[0]);

        } while (!isSelected);

        if (config_->interactive)
            iRenderer_->showValidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup_.levels[0]);

        if (logger_)
            logger_->log(std::format("user input 'bot level[0]' = {}", botToString((int)gameSetup_.levels[0])), Logger::Level::DEBUG);

        do
        {
            if (config_->interactive)
                iRenderer_->showSelectMenu(SelectType::MUL_BOT_LEVEL_UI, 1);
            isSelected = iInteraction_->selectBotLevel(gameSetup_.levels.data(), 1);

            if (config_->interactive && !isSelected)
                iRenderer_->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup_.levels[1]);

        } while (!isSelected);

        if (config_->interactive)
            iRenderer_->showValidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup_.levels[1]);

        if (logger_)
            logger_->log(std::format("user input 'bot level[1]' = {}", botToString((int)gameSetup_.levels[1])), Logger::Level::DEBUG);
    }

    gameState_.board = core::initBoard(gameSetup_.size);
    gameState_.currentPlayer = 0;
    gameState_.turn = 0;
    gameState_.winner = -1;
    gameState_.isFinished = false;

    if (logger_)
        logger_->log("Board initialized!");
    if (logger_)
        logger_->log("[Engine] Game started!");
}

/* ---------- playGame ---------- */

/**
 * Mô tả: Game loop CHÍNH.
 *
 * TODO:
 *   ── Mức 1 (task 3): chỉ cần thay Logger global → logger_-> ──
 *      Có thể giữ tinh thần while-loop của level 2.
 *
 *   ── Mức 2 (Engine): KHÔNG mutate board ──
 *      Thay vì:
 *          Logic::makeMove(gameSetup.board, row, col, symbols[player]);
 *      dùng:
 *          gameState_ = core::applyTurn(gameState_, {row, col}, gameSetup_.goal);
 *
 *      Loop dừng khi `gameState_.isFinished == true`.
 *
 *      Khuyến khích: viết `playGame` thành một đệ quy đuôi:
 *          GameResult loop(GameState s) {
 *              if (s.isFinished) return result_of(s);
 *              auto move  = nextMove(s);
 *              auto next  = core::applyTurn(s, move, goal);
 *              return loop(next);   // tail call
 *          }
 *      → minh hoạ rõ "game loop là một fold".
 *
 * Trả về: GameResult.
 */
GameResult Engine::playGame()
{
    if (logger_)
        logger_->log("[Engine] Playing game . . .");

    GameResult gameResult{-1, false, 0};

    if (!sanity_check())
    {
        if (logger_)
            logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return gameResult;
    }

    // TODO (mức 1+2): cài đặt game loop theo gợi ý ở trên.
    //
    // Gợi ý mức 1: cụ thể hoá bot bằng FP factory:
    //   bot::BotFn bot0 = (mode == EVE)
    //                       ? bot::makeBot(gameSetup_.levels[0], gameSetup_.goal)
    //                       : bot::BotFn{};
    //   bot::BotFn bot1 = (mode != PVP)
    //                       ? bot::makeBot(gameSetup_.levels[1], gameSetup_.goal)
    //                       : bot::BotFn{};
    //
    // Gợi ý đo thời gian (mức 1+3B):
    //   auto move = measureExecutionTime(
    //       "bot->getMove",
    //       [&]{ return bot1(gameState_, *rng_); },
    //       config_->verbose_flag,
    //       [this](const std::string& m){ logger_->log(m, Logger::Level::DEBUG); }
    //   );

    bot::BotFn bot0 = (gameSetup_.mode == GameMode::EVE)
                          ? bot::makeBot(gameSetup_.levels[0], gameSetup_.goal)
                          : bot::BotFn{};
    bot::BotFn bot1 = (gameSetup_.mode != GameMode::PVP)
                          ? bot::makeBot(gameSetup_.levels[1], gameSetup_.goal)
                          : bot::BotFn{};

    std::array<bot::BotFn, 2> bots = {bot0, bot1};
    std::array<bool, 2> is_bot = {gameSetup_.mode == GameMode::EVE,
                                  gameSetup_.mode != GameMode::PVP};
    auto loop = [this, &is_bot, &bots](auto &self, const GameState &s) -> GameState
    {
        if (s.isFinished)
        {
            return s;
        }

        if (logger_)
            logger_->log(std::format("[Engine] starting turn #{}", s.turn), Logger::Level::DEBUG);
        if (config_->interactive)
        {
            iRenderer_->clearScreen();
            iRenderer_->displayBoard(s.board);
            iRenderer_->showPlayer(s.currentPlayer, is_bot[s.currentPlayer]);
        };

        Move move = INVALID_MOVE;
        if (is_bot[s.currentPlayer])
        {
            move = measureExecutionTime(
                std::format("bot#{}->getMove", s.currentPlayer),
                [&]()
                { return bots[s.currentPlayer](s, *rng_); },
                config_->verbose_flag,
                [this](const std::string &m)
                { if (logger_) logger_->log(m, Logger::Level::DEBUG); });
        }
        else
        {
            bool is_valid = false;
            do
            {
                int row, col;
                if (config_->interactive)
                    iRenderer_->showSelectMenu(SelectType::PLAYER_UI);

                iInteraction_->getPlayerMove(&row, &col);
                move = {row, col};

                is_valid = core::isValidMove(s.board, move);

                if (!is_valid && config_->interactive)
                    iRenderer_->showInvalidMove();
            } while (!is_valid);
        }

        if (config_->interactive)
            iRenderer_->showMove(move.row, move.col);
        if (logger_)
            logger_->log(std::format("player {} make move to ({}, {})", s.currentPlayer + 1, move.row, move.col), Logger::Level::DEBUG);

        if (is_bot[s.currentPlayer] && config_->interactive)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }

        GameState nextState = core::applyTurn(s, move, gameSetup_.goal);

        if (logger_)
            logger_->log("[Engine] turn done!", Logger::Level::DEBUG);
        return self(self, nextState);
    };

    gameState_ = loop(loop, gameState_);

    gameResult.winner = gameState_.winner;
    gameResult.turns = gameState_.turn;
    if (gameState_.winner != -1)
    {
        gameResult.isBot = is_bot[gameState_.winner];
    }

    if (logger_)
        logger_->log("[Engine] Game done!");

    return gameResult;
}
/* ---------- endGame ---------- */

/**
 * Mô tả: Hiển thị kết quả game.
 *
 * TODO (mức 1 -- task 3):
 *   - Giống level 2 nhưng dùng logger_->log(...).
 *   - Truyền `gameState_.board` (immutable) vào renderer thay vì mảng cũ.
 */
void Engine::endGame(const GameResult &gameResult)
{
    if (logger_)
        logger_->log("[Engine] Ending game . . .");

    if (!sanity_check())
    {
        if (logger_)
            logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return;
    }

    if (config_->interactive)
    {
        iRenderer_->clearScreen();
        iRenderer_->displayBoard(gameState_.board);
        char symbol = (gameResult.winner == 0) ? 'X' : 'O';

        auto winLine = core::getWinLine(gameState_.board, symbol, gameSetup_.goal, EndRule::OPEN_TWO);
        if (winLine)
        {
            std::string s = "[WinLine] cells: ";
            for (auto [r, c] : winLine->cells)
                s += std::format("({}, {}) ", r, c);

            if (logger_)
                logger_->log(s, Logger::Level::DEBUG);
        }
        else
        {
            if (logger_)
                logger_->log("[WinLine] none", Logger::Level::DEBUG);
        }

        iRenderer_->showResult(gameResult.winner, gameResult.isBot, winLine ? &(*winLine) : nullptr);
        iInteraction_->pause();
    }
    else if (config_->judge_mode)
    {
        iRenderer_->printResult(gameResult);
    }

    std::stringstream ss;
    ss << "after " << gameResult.turns << " turns";
    if (logger_)
        logger_->log(ss.str(), Logger::Level::DEBUG);

    ss.str(std::string());
    ss.clear();

    ss << "game end with result: ";
    if (gameResult.winner == -1)
        ss << "draw";
    else
        ss << "player " << gameResult.winner + 1
           << " (" << (gameResult.isBot ? "bot" : "human") << ") "
           << "win!";
    if (logger_)
        logger_->log(ss.str(), Logger::Level::DEBUG);

    if (logger_)
        logger_->log("[Engine] Game ended!");
}

/* ---------- close ---------- */

void Engine::close()
{
    if (logger_)
        logger_->log("Engine closing . . .");

    if (iRenderer_)
        iRenderer_->close();
    if (iInteraction_)
        iInteraction_->close();

    if (logger_)
        logger_->log("Engine closed!");
}
