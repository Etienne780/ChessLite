#include "ChessWinConditionRegistry.h"

namespace CoreChess {


    ChessWinConditionRegistry& ChessWinConditionRegistry::GetInstance() {
        static ChessWinConditionRegistry instance;
        return instance;
    }

    ChessWinConditionID ChessWinConditionRegistry::AddWinCondition(ChessWinConditionFunc func) {
        ChessWinConditionID newID(m_idManager.GetNewUniqueIdentifier());
        m_functions[newID] = std::move(func);
        return newID;
    }

    ChessWinConditionFunc ChessWinConditionRegistry::GetWinCondition(ChessWinConditionID id) const {
        auto it = m_functions.find(id);
        if (it != m_functions.end())
            return it->second;

        return nullptr;
    }

    ChessWinResult ChessWinConditionRegistry::CallWinCondition(
        ChessWinConditionID id, 
        const ChessGame& game) const 
    {
        auto func = GetWinCondition(id);
        if (func == nullptr)
            return ChessWinResult::NONE;
        return func(game);
    }

}