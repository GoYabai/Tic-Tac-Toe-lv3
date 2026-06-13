#include "interaction.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "../core/logic.h"

TerminalInteraction::TerminalInteraction() {}

TerminalInteraction::~TerminalInteraction() {}

void TerminalInteraction::init(const RunConfig &config)
{
    is_interactive_ = config.interactive;
    initInteraction(config);
}

void TerminalInteraction::initInteraction(const RunConfig &config)
{
    cin_backup = nullptr;

    if (!config.interactive && !config.input_file.empty())
    {
        global_file_in.open(config.input_file);
        if (global_file_in.is_open())
        {
            cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(global_file_in.rdbuf());
            if (is_interactive_)
            {
                std::cout << "[Interaction] redirected cin to: ..." << std::endl;
            }
        }
        else
        {
            std::cerr << "[Interaction] ERROR: failed to open input file, using console.\n";
        }
    }
}

void TerminalInteraction::closeInteraction()
{
    if (cin_backup)
    {
        std::cin.rdbuf(cin_backup);
        if (is_interactive_)
        {
            std::cout << "[Interaction] fallback using 'std::cin' input stream.\n";
        }
    }

    if (global_file_in.is_open())
    {
        global_file_in.close();
    }
}

bool TerminalInteraction::validateInput(const std::string &input)
{
    if (input.empty())
    {
        return false;
    }
    for (char c : input)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }
    return true;
}

bool TerminalInteraction::getInput(int *val)
{
    std::string input;
    std::cin >> input;
    if (!validateInput(input))
    {
        return false;
    }

    try
    {
        *val = std::stoi(input);
    }
    catch (...)
    {
        std::cerr << "[Interaction] Input number is out of range!\n";
        return false;
    }
    return true;
}

void TerminalInteraction::pause(int timeout)
{
    if (timeout == 0)
    {
        std::cout << std::endl;
        std::cin.ignore(10000, '\n');
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
    else if (timeout > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}

bool TerminalInteraction::selectSize(int *size)
{
    if (getInput(size) == false)
    {
        return false;
    }
    if (*size < 3 || *size > 15)
    {
        std::cerr << "[Interaction] Input number is out of range!\n";
        return false;
    }
    return true;
}

bool TerminalInteraction::selectGoal(int *goal, const int size)
{
    if (getInput(goal) == false)
    {
        return false;
    }
    if (*goal < 3 || *goal > size)
    {
        std::cerr << "[Interaction] Input number is out of range!\n";
        return false;
    }
    return true;
}

bool TerminalInteraction::selectGameMode(GameMode *mode)
{
    int input;
    if (getInput(&input) == false)
    {
        return false;
    }
    if (input < 1 || input > 3)
    {
        std::cerr << "[Interaction] Input number is out of range!\n";
        return false;
    }
    *mode = static_cast<GameMode>(input - 1);
    return true;
}

bool TerminalInteraction::selectBotLevel(BotLevel *levels, const int index)
{
    if (index < 0 || index > 1)
    {
        return false;
    }
    int input;
    if (getInput(&input) == false)
    {
        return false;
    }
    if (input < 1 || input > 3)
    {
        std::cerr << "[Interaction] Input number is out of range!\n";
        return false;
    }
    levels[index] = static_cast<BotLevel>(input - 1);
    return true;
}

bool TerminalInteraction::getPlayerMove(int *row, int *col)
{
    if (!getInput(row))
    {
        return false;
    }
    if (!getInput(col))
    {
        return false;
    }
    return true;
}

void TerminalInteraction::close()
{
    closeInteraction();
}