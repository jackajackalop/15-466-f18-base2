#pragma once

#include <glm/glm.hpp>
#include <math.h>

struct Game {
	glm::vec2 body_pos = glm::vec2(0.0f, 0.0f);
	float thighR_angle = 0.0f;
	float thighL_angle = 0.0f;
	float calfR_angle = 0.0f;
	float calfL_angle = 0.0f;
	float calfR_old = 0.0f;
	float calfL_old = 0.0f;

	glm::vec2 ball_velocity = glm::vec2(0.0f,-2.0f);
	glm::vec2 body_velocity = glm::vec2(0.0f, 0.0f);

	void update(float time);

	static constexpr const float FrameWidth = 10.0f;
	static constexpr const float FrameHeight = 8.0f;
	static constexpr const float PaddleWidth = 2.0f;
	static constexpr const float PaddleHeight = 0.4f;
	static constexpr const float BallRadius = 0.5f;
};
