#pragma once
#include <functional>
#include <unordered_map>

#include "ChessTypes.h"

namespace CoreChess {

    /**
    * @brief Function signature used to evaluate a win condition.
    *
    * The function receives a constant reference to the current ChessGame
    * and returns a ChessWinResult describing the outcome.
    */
    using ChessWinConditionFunc = std::function<ChessWinResult(const ChessGame&)>;

    /**
    * @brief Registry responsible for managing win condition functions.
    *
    * This class assigns a unique WinConditionID to each registered
    * win condition function and allows retrieving the corresponding
    * function later via that ID.
    *
    * The registry is implemented as a singleton and is intended to
    * decouple runtime function objects from serialized data. Only
    * the WinConditionID should be serialized.
    */
    class ChessWinConditionRegistry {
    public:
        ~ChessWinConditionRegistry() = default;

        ChessWinConditionRegistry(const ChessWinConditionRegistry&) = delete;
        ChessWinConditionRegistry(ChessWinConditionRegistry&&) = delete;

        ChessWinConditionRegistry& operator=(const ChessWinConditionRegistry&) = delete;
        ChessWinConditionRegistry& operator=(ChessWinConditionRegistry&&) = delete;

        /**
        * @brief Returns the global singleton instance.
        *
        * @return Reference to the ChessWinConditionRegistry instance.
        */
        static ChessWinConditionRegistry& GetInstance();

        /**
        * @brief Adds a new win condition function.
        *
        * A new unique ChessWinConditionID is generated and associated
        * with the provided function.
        *
        * @param func Win condition function to register.
        * @return Generated unique ChessWinConditionID.
        */
        ChessWinConditionID AddWinCondition(ChessWinConditionFunc func);

        /**
        * @brief Retrieves a win condition function by its ID.
        *
        * @param id The ChessWinConditionID.
        * @return The associated WinConditionFunc, or nullptr if not found.
        */
        ChessWinConditionFunc GetWinCondition(ChessWinConditionID id) const;

        /**
        * @brief Executes a registered win condition function.
        *
        * Looks up the win condition associated with the given ChessWinConditionID
        * and invokes it using the provided ChessGame instance.
        *
        * If no function is registered for the specified ID,
        * ChessWinResult::NONE is returned.
        *
        * @param id The unique identifier of the win condition.
        * @param game Constant reference to the current game state.
        * @return The result of the evaluated win condition, or ChessWinResult::NONE if not found.
        */
        ChessWinResult CallWinCondition(ChessWinConditionID id, const ChessGame& game) const;

    private:
        CoreChessIDManager m_idManager;
        std::unordered_map<ChessWinConditionID, ChessWinConditionFunc> m_functions;
    
        ChessWinConditionRegistry() = default;
    };

}
