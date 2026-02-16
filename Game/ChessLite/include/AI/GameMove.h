#pragma once
#include <CoreLib/Math/Vector2.h>

class GameMove {
public:
	GameMove() = default;
	GameMove(const Vector2& from, const Vector2& to);
	~GameMove() = default;

	const Vector2& GetFrom() const;
	const Vector2& GetTo() const;
	float GetEvaluation() const;

	void SetEvaluation(float eval);
	void AddEvaluation(float addEval);

private:
	Vector2 m_from{ 0.0f };
	Vector2 m_to{ 0.0f };
	float m_evaluation = 1;
};