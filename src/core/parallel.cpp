/**
 * Parallel evaluation implementation (Mức 3 -- nhánh B)
 *
 * Mô tả:
 *   File này CHỈ phải hoàn thành nếu sinh viên chọn nhánh B.
 *   Sinh viên không chọn nhánh B có thể giữ stub này -- Engine không gọi
 *   tới nó trừ khi flag `--parallel` được bật ở command-line.
 */

#include "parallel.h"

/* ---------- Importing ---------- */

#include "logic.h"

// Sinh viên thêm headers tuỳ cách triển khai:
//   #include <algorithm>
//   #include <execution>     // cho std::execution::par
//   #include <future>        // cho std::async
//   #include <thread>

namespace core::par {

/**
 * Mô tả: Đánh giá 1 nước đi -- pure heuristic.
 *
 * TODO (nhánh B):
 *   - Chọn một heuristic deterministic. Ví dụ:
 *       new_state = applyTurn(state, move, goal);
 *       if checkWin(...)  → +SCORE_INF
 *       if checkLose(...) → -SCORE_INF
 *       else              → countLines(symbol) - countLines(opponent)
 */
int evaluateMove(const GameState& /*state*/, Move /*move*/, char /*symbol*/, int /*goal*/) {
    // TODO (nhánh B)
    throw NotImplementedException();
}

/**
 * Mô tả: Đánh giá tuần tự -- baseline để so sánh.
 *
 * TODO (nhánh B):
 *   auto moves = core::enumerateValidMoves(state.board);
 *   std::vector<std::pair<Move,int>> result;
 *   result.reserve(moves.size());
 *   for (auto m : moves)
 *       result.emplace_back(m, evaluateMove(state, m, symbol, goal));
 *   return result;
 *
 *   Hoặc dùng std::transform để bộc lộ FP.
 */
std::vector<std::pair<Move, int>>
evaluateAllMoves_seq(const GameState& /*state*/, char /*symbol*/, int /*goal*/) {
    // TODO (nhánh B)
    throw NotImplementedException();
}

/**
 * Mô tả: Đánh giá song song.
 *
 * TODO (nhánh B):
 *   Lựa chọn 1 trong các hướng:
 *
 *   ── Hướng 1 (đơn giản, C++17): std::execution::par ──
 *       auto moves = core::enumerateValidMoves(state.board);
 *       std::vector<std::pair<Move,int>> result(moves.size());
 *       std::transform(std::execution::par,
 *                      moves.begin(), moves.end(), result.begin(),
 *                      [&](Move m){ return std::pair{m, evaluateMove(state,m,symbol,goal)}; });
 *       return result;
 *
 *   ── Hướng 2 (linh hoạt hơn, kiểm soát threads): std::async ──
 *       auto moves = core::enumerateValidMoves(state.board);
 *       std::vector<std::future<std::pair<Move,int>>> futures;
 *       for (auto m : moves)
 *           futures.push_back(std::async(std::launch::async, [&,m]{
 *               return std::pair{m, evaluateMove(state, m, symbol, goal)};
 *           }));
 *       std::vector<std::pair<Move,int>> result;
 *       for (auto& f : futures) result.push_back(f.get());
 *       return result;
 *
 *   Yêu cầu: kết quả phải bằng evaluateAllMoves_seq cho cùng input.
 */
std::vector<std::pair<Move, int>>
evaluateAllMoves_par(const GameState& /*state*/, char /*symbol*/, int /*goal*/,
                      int /*num_threads*/) {
    // TODO (nhánh B)
    throw NotImplementedException();
}

}  // namespace core::par
