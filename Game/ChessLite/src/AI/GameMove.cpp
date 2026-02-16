#include "AI/GameMove.h"

GameMove::GameMove(const Vector2& from, const Vector2& to) 
	: m_from(from), m_to(to) {
}

const Vector2& GameMove::GetFrom() const {
	return m_from;
}

const Vector2& GameMove::GetTo() const {
	return m_to;
}

float GameMove::GetEvaluation() const {
	return m_evaluation;
}

void GameMove::SetEvaluation(float eval) {
	m_evaluation = eval;
}

void GameMove::AddEvaluation(float addEval) {
	m_evaluation += addEval;
}