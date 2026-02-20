#pragma once
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/OTNFile.h>

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

template<>
inline void OTN::ToOTNDataType<GameMove>(OTN::OTNObjectBuilder& obj, GameMove& move) {
	obj.SetObjectName("GameMove");
	obj.AddNames("eval", "from_x", "from_y", "to_x", "to_y");

	if (obj.IsBuildingOTNObject()) {
		Vector2 from = move.GetFrom();
		Vector2 to = move.GetTo();
		obj.AddData(move.GetEvaluation(), from.x, from.y, to.x, to.y);
	}
	else {
		Vector2 from, to;
		float eval;

		obj.AddData(eval, from.x, from.y, to.x, to.y);

		move = GameMove(from, to);
		move.SetEvaluation(eval);
	}
}