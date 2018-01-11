#pragma once
#include "../VFrame/VPostEffect.h"
#include <array>

class ScoreboardEffect :	public VPostEffectBase
{
public:
	ScoreboardEffect();
	~ScoreboardEffect() = default;

	virtual void Apply(const sf::RenderTexture& input, sf::RenderTarget& output);
	void SetPointSize(float size);
	void SetPointAmount(unsigned int amount);
	void SetBlur(bool value, float amount);

private:
	typedef std::array<sf::RenderTexture, 2> RenderArray;

	void prepareTextures(sf::Vector2u size);
	void downsample(const sf::RenderTexture& input, sf::RenderTexture& output);
	void mask(const sf::RenderTexture& source, sf::RenderTarget& target);
	void blurMultipass(RenderArray& renderTextures);
	void blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);

	unsigned int pointAmount;
	bool blurEffect;
	float blurAmount;

	sf::Shader downSample;
	sf::Shader maskSample;
	sf::Shader gaussianBlur;
	sf::Shader fullPass;

	sf::RenderTexture mDownSampleTexture;
	RenderArray	mBlurPassTextures;
};

