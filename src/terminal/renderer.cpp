#include "renderer.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "../utils/config.h"
#include "../core/types.h"

/* ---------- Definitions ---------- */

TerminalRenderer::TerminalRenderer() : I_Renderer() {}

TerminalRenderer::~TerminalRenderer() = default;

void TerminalRenderer::init(const RunConfig &config)
{
    this->clearScreen();
    if (config.interactive)
    {
        std::cout << "========================================\n";
        std::cout << "             TIC-TAC-TOE                \n";
        std::cout << "========================================\n";
    }
}

void TerminalRenderer::clearScreen()
{
#if defined(_WIN32)
    system("cls");
#else
    system("clear");
#endif
}

void TerminalRenderer::showSelectMenu(SelectType selectType, int context)
{
    if (selectType != SelectType::PLAYER_UI)
    {
        this->clearScreen();

        if (selectType != SelectType::TITLE_UI)
        {
            std::cout << "========================================\n";
            std::cout << "             TIC-TAC-TOE                \n";
            std::cout << "========================================\n\n";
        }
    }

    switch (selectType)
    {
    case SelectType::TITLE_UI:
        std::cout << std::format(">----- Tic-tac-toe [Console v{}] -----<\n\n", "25022870");
        break;
    case SelectType::SIZE_UI:
        std::cout << std::format("Size Input (NxN, 3 <= N <= {})", BOARD_N_MAX) << ":\n> ";
        break;
    case SelectType::GOAL_UI:
        std::cout << std::format("Goal Input (3 <= goal <= {})", context) << ":\n> ";
        break;
    case SelectType::GAME_MODE_UI:
        std::cout << "Select Game Mode:\n(1) PvP | (2) PvE | (3) EvE\n> ";
        break;
    case SelectType::BOT_LEVEL_UI:
        std::cout << "Select Bot Level:\n(1) EASY | (2) MEDIUM | (3) HARD\n> ";
        break;
    case SelectType::PLAYER_UI:
        std::cout << "Your move (row col):\n> ";
        break;
    case SelectType::MUL_BOT_LEVEL_UI:
        std::cout << "Select Bots Level (bot1_level bot2_level):\n> ";
        break;
    default:
        break;
    }
}

void TerminalRenderer::showInvalidSelect(SelectType selectType, int context)
{
    std::cout << "\n[!] Invalid select. Please try again!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void TerminalRenderer::showValidSelect(SelectType selectType, int context)
{
    std::cout << "\n[+] Valid select received!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void TerminalRenderer::displayBoard(const Board &board)
{
    int size = board.size;

    std::cout << "   ";
    for (int j = 0; j < size; j++)
    {
        std::cout << std::format("{:2} ", j);
    }
    std::cout << std::endl
              << "  ";
    for (int j = 0; j < size * 3; j++)
    {
        std::cout << "-";
    }
    std::cout << std::endl;

    for (int i = 0; i < size; i++)
    {
        std::cout << std::format("{:2}|", i);
        for (int j = 0; j < size; j++)
        {
            std::cout << std::format(" {} ", board.at(i, j));
        }
        std::cout << std::endl;
    }
}

void TerminalRenderer::showPlayer(int player, bool is_bot)
{
    std::string p = std::to_string(player + 1);
    if (is_bot)
    {
        std::cout << "Bot (Player " << p << ") is thinking..." << std::endl;
    }
    else
    {
        std::cout << "Player " << p << "'s turn" << std::endl;
    }
}

void TerminalRenderer::showMove(int row, int col)
{
    std::cout << "Move placed at (" << row << ',' << col << ')' << std::endl;
}

void TerminalRenderer::showInvalidMove()
{
    std::cout << "The move is invalid" << std::endl;
}

void TerminalRenderer::showResult(int winner, bool is_bot, const WinLine *winLine)
{
    if (winner == -1)
    {
        std::cout << "It's a draw!" << std::endl;
    }
    else if (is_bot)
    {
        std::cout << "Bot " << std::to_string(winner + 1) << " win!" << std::endl;
    }
    else
    {
        std::cout << "Player " << std::to_string(winner + 1) << " wins!" << std::endl;
    }
}

void TerminalRenderer::printResult(const GameResult &gameResult)
{
    std::cout << gameResult.winner << " " << gameResult.turns << std::endl;
}

void TerminalRenderer::close()
{
    // Cleanup if needed
}