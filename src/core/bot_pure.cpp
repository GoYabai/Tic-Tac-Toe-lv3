/**
 * Bot (FP-style) implementation
 */

#include "bot_pure.h"

/* ---------- Importing ---------- */

#include "logic.h"
#include "pipeline.h"

namespace bot
{

    /* ---------- Easy ---------- */

    /**
     * Mô tả: Bot Easy -- random pick.
     *
     * TODO (mức 1 -- task 2, "Higher-order function cho Bot LV1"):
     *
     *   Yêu cầu KHÔNG dùng for-loop tích luỹ. Có 2 cách triển khai gợi ý:
     *
     *   ── Cách 1: dùng `fp::pipe(...)` từ pipeline.h ──
     *       auto moves = core::enumerateValidMoves(state.board);
     *       if (moves.empty()) return INVALID_MOVE;
     *       std::uniform_int_distribution<int> dist(0, (int)moves.size() - 1);
     *       return moves[dist(rng)];
     *
     *   ── Cách 2: full pipeline + std::ranges ──
     *       using namespace std::ranges;
     *       auto cells   = core::enumerateCells(state.board);
     *       auto valid   = cells | views::filter([&](Move m){
     *                                  return core::isValidMove(state.board, m);
     *                              });
     *       auto vec     = std::vector<Move>(valid.begin(), valid.end());
     *       ...
     *
     *   Lưu ý:
     *     - KHÔNG được dùng `generator` toàn cục -- tham số `rng` bắt buộc.
     *     - Phải xử lý trường hợp board đầy → trả về INVALID_MOVE.
     */
    Move easy(const GameState &state, Rng &rng)
    {
        // TODO (Mức 1 -- task 2)
        auto moves = core::enumerateValidMoves(state.board);

        if (moves.empty())
        {
            return INVALID_MOVE;
        }
        std::uniform_int_distribution<int> dist(0, (int)moves.size() - 1);
        return moves[dist(rng)];
    }

    /* ---------- Medium ---------- */

    /**
     * Mô tả: Bot Medium -- heuristic 3 bước.
     *
     * TODO (Mức 2 -- Bot, sinh viên tự quyết áp dụng FP vào bot nào):
     *   Khuyến khích bộc lộ rõ:
     *     - Composition: ráp 3 bước lại thành 1 pipeline.
     *     - Higher-order: viết hàm `tryFirstNonEmpty(strategies)` trả về Move
     *       đầu tiên không phải INVALID_MOVE.
     *     - Closure / currying: cố định `state` & `goal` cho các predicate.
     *
     *   Có thể fallback `easy(state, rng)` nếu chưa làm.
     */
    Move medium(const GameState &state, int goal, Rng &rng)
    {
        // TODO (Mức 2 -- Bot)
        char mySymbol = core::symbolOf(state.currentPlayer);
        char oppSymbol = core::opponentOf(mySymbol);

        auto makeWinChecker = [state, goal](char symbol)
        {
            return [state, goal, symbol](Move m)
            {
                Board nextBoard = core::applyMove(state.board, m, symbol);
                return core::checkWin(nextBoard, symbol, goal, EndRule::OPEN_TWO);
            };
        };
        auto isWinningMove = makeWinChecker(mySymbol);
        auto isBlockingMove = makeWinChecker(oppSymbol);

        auto getWinMoves = fp::pipe(
            core::enumerateValidMoves,
            fp::filter(isWinningMove));

        auto getBlockMoves = fp::pipe(
            core::enumerateValidMoves,
            fp::filter(isBlockingMove));

        std::vector<Move> winMoves = getWinMoves(state.board);
        if (!winMoves.empty())
            return winMoves.front();

        std::vector<Move> blockMoves = getBlockMoves(state.board);
        if (!blockMoves.empty())
            return blockMoves.front();

        return easy(state, rng);
    }

    /* ---------- Hard ---------- */

    /**
     * Mô tả: Bot Hard -- minimax / heuristic mạnh.
     *
     * TODO:
     *   - Mức 2: ưu tiên cài bằng FP (currying, composition).
     *   - Mức 3 nhánh B: TÁCH `evaluateAllMoves` thành pure function rời
     *     trong `core/parallel.h` để chạy được song song.
     */
    Move hard(const GameState &state, int goal, Rng &rng)
    {
        char botSym = core::symbolOf(state.currentPlayer);
        char oppSym = core::opponentOf(botSym);
        int size = state.board.size;

        auto hasNeighbor = [](const Board &b, Move m)
        {
            for (int dr = -2; dr <= 2; ++dr)
            {
                for (int dc = -2; dc <= 2; ++dc)
                {
                    if (dr == 0 && dc == 0)
                        continue;
                    int nr = m.row + dr;
                    int nc = m.col + dc;
                    if (b.inRange(nr, nc) && b.at(nr, nc) != EMPTY_CELL)
                        return true;
                }
            }
            return false;
        };

        auto evaluateBoard = [goal, botSym, oppSym](const Board &b)
        {
            int botScore = 0, playerScore = 0;
            int dx[4] = {0, 1, 1, 1};
            int dy[4] = {1, 0, 1, -1};

            auto getScore = [goal](int count, int blocks)
            {
                if (blocks == 2)
                    return 0;
                int score = 0;
                if (count >= goal)
                    score = 10000;
                else if (count == goal - 1)
                    score = 1000;
                else if (count == goal - 2)
                    score = 100;
                else if (count == goal - 3)
                    score = 10;
                if (blocks == 1)
                    score /= 5;
                return score;
            };

            for (int i = 0; i < b.size; ++i)
            {
                for (int j = 0; j < b.size; ++j)
                {
                    if (b.at(i, j) == EMPTY_CELL)
                        continue;
                    char currentSym = b.at(i, j);
                    for (int d = 0; d < 4; ++d)
                    {
                        int prev_i = i - dx[d], prev_j = j - dy[d];
                        if (b.inRange(prev_i, prev_j) && b.at(prev_i, prev_j) == currentSym)
                            continue;

                        int count = 1, blocks = 0;
                        if (b.inRange(prev_i, prev_j) && b.at(prev_i, prev_j) != EMPTY_CELL)
                            blocks++;

                        int next_i = i + dx[d], next_j = j + dy[d];
                        while (b.inRange(next_i, next_j) && b.at(next_i, next_j) == currentSym)
                        {
                            count++;
                            next_i += dx[d];
                            next_j += dy[d];
                        }
                        if (b.inRange(next_i, next_j) && b.at(next_i, next_j) != EMPTY_CELL)
                            blocks++;

                        int score = getScore(count, blocks);
                        if (currentSym == botSym)
                            botScore += score;
                        else
                            playerScore += score;
                    }
                }
            }
            return botScore - (playerScore * 2);
        };

        std::function<int(const Board &, int, int, int, bool)> minimax;
        minimax = [&](const Board &currentBoard, int depth, int alpha, int beta, bool isMax) -> int
        {
            if (core::checkWin(currentBoard, botSym, goal, EndRule::OPEN_TWO))
                return 100000 - depth;
            if (core::checkWin(currentBoard, oppSym, goal, EndRule::OPEN_TWO))
                return depth - 100000;
            if (core::checkDraw(currentBoard))
                return 0;
            if (depth >= 3)
                return evaluateBoard(currentBoard);

            auto cands = fp::pipe(
                core::enumerateValidMoves,
                fp::filter([&](Move m)
                           { return hasNeighbor(currentBoard, m); }))(currentBoard);

            if (isMax)
            {
                int best = -1000000;
                for (Move m : cands)
                {
                    Board nextB = core::applyMove(currentBoard, m, botSym);
                    int score = minimax(nextB, depth + 1, alpha, beta, false);
                    best = std::max(best, score);
                    alpha = std::max(alpha, best);
                    if (beta <= alpha)
                        break;
                }
                return best;
            }
            else
            {
                int best = 1000000;
                for (Move m : cands)
                {
                    Board nextB = core::applyMove(currentBoard, m, oppSym);
                    int score = minimax(nextB, depth + 1, alpha, beta, true);
                    best = std::min(best, score);
                    beta = std::min(beta, best);
                    if (beta <= alpha)
                        break;
                }
                return best;
            }
        };

        auto candidates = fp::pipe(
            core::enumerateValidMoves,
            fp::filter([&](Move m)
                       { return hasNeighbor(state.board, m); }))(state.board);

        if (candidates.empty())
            return {size / 2, size / 2};

        int best_score = -1000000;
        Move best_move = candidates[0];
        int alpha = -1000000, beta = 1000000;

        for (Move m : candidates)
        {
            Board nextB = core::applyMove(state.board, m, botSym);
            int score = minimax(nextB, 1, alpha, beta, false);

            if (score > best_score)
            {
                best_score = score;
                best_move = m;
            }
            alpha = std::max(alpha, best_score);
        }

        return best_move;
    }

    /* ---------- Factory ---------- */

    /**
     * Mô tả: Trả về BotFn ứng với level.
     *
     * TODO (mức 1 -- task 2, đi kèm easy()):
     *   - Mỗi nhánh trả về một LAMBDA (đóng gói goal vào closure):
     *
     *     case EASY:
     *         return [](const GameState& s, Rng& r) { return easy(s, r); };
     *
     *     case MEDIUM:
     *         return [goal](const GameState& s, Rng& r) {
     *             return medium(s, goal, r);
     *         };
     *
     *     case HARD:
     *         return [goal](const GameState& s, Rng& r) {
     *             return hard(s, goal, r);
     *         };
     *
     *     default:
     *         return [](const GameState&, Rng&) { return INVALID_MOVE; };
     *
     * Đây là cũng là một ví dụ HOF (factory trả về function).
     */
    BotFn makeBot(BotLevel level, int goal)
    {
        switch (level)
        {
        case BotLevel::EASY:
            return [](const GameState &s, Rng &r)
            {
                return easy(s, r);
            };

        case BotLevel::MEDIUM:
            return [goal](const GameState &s, Rng &r)
            {
                return medium(s, goal, r);
            };

        case BotLevel::HARD:
            return [goal](const GameState &s, Rng &r)
            {
                return hard(s, goal, r);
            };

        default:
            return [](const GameState &, Rng &)
            {
                return INVALID_MOVE;
            };
        }
    }

} // namespace bot
