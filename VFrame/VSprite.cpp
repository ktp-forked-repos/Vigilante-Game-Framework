#include "VSprite.h"
#include "VGlobal.h"

void VSprite::setSize(unsigned int texWidth, unsigned int texHeight, bool animated, int width, int height, int offsetX, int offsetY)
{
	Size = sf::Vector2f(sf::Vector2u(width, height));

	if (Size.x <= 0)
	{
		Size.x = static_cast<float>(animated ? texHeight : texWidth);
		Size.x = static_cast<float>(Size.x > texWidth ? texWidth : Size.x);
	}
	if (Size.y <= 0)
	{
		Size.y = static_cast<float>(animated ? Size.x : texHeight);
		Size.y = static_cast<float>(Size.y > texHeight ? texHeight : Size.y);
	}

	FrameSize = sf::Vector2u(Size);
	Animation = VAnimationManager(texWidth, texHeight, FrameSize.x, FrameSize.y, offsetX, offsetY);
	Origin = sf::Vector2f(FrameSize) / 2.0f;

	Radius = Size.x < Size.y ? Size.x / 2 : Size.y / 2;

	vertexArray[0].position = sf::Vector2f();
	vertexArray[1].position = sf::Vector2f(Size.x, 0.0f);
	vertexArray[2].position = sf::Vector2f(Size.x, Size.y);
	vertexArray[3].position = sf::Vector2f(0.0f, Size.y);
}

void VSprite::updateTransform()
{
	if (transformable.getPosition() != Position + Origin - Offset) 
		transformable.setPosition(Position + Origin - Offset);
	if (transformable.getRotation() != Angle) 
		transformable.setRotation(Angle);
	if (transformable.getScale() != Scale)	
		transformable.setScale(Scale);
	if (transformable.getOrigin() != Origin) 
		transformable.setOrigin(Origin);

	if (vertexArray[0].color != Tint)
	{
		vertexArray[0].color = Tint;
		vertexArray[1].color = Tint;
		vertexArray[2].color = Tint;
		vertexArray[3].color = Tint;
	}
}

void VSprite::updateFrame()
{
	sf::FloatRect rect;
	rect.left		= FlipX ? Animation.GetU() + FrameSize.x : Animation.GetU();
	rect.top		= FlipY ? Animation.GetV() + FrameSize.y : Animation.GetV();
	rect.width		= FlipX ? -(float)FrameSize.x : (float)FrameSize.x;
	rect.height		= FlipY ? -(float)FrameSize.y : (float)FrameSize.y;
	vertexArray[0].texCoords = sf::Vector2f(rect.left,				rect.top);
	vertexArray[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
	vertexArray[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
	vertexArray[3].texCoords = sf::Vector2f(rect.left,				rect.top + rect.height);
}

VSprite* VSprite::LoadGraphic(sf::String filename, bool animated, int width, int height, const sf::IntRect& area)
{
	RenderState.texture = &VGlobal::p()->Content->LoadTexture(filename);

	setSize(
		area.width == 0 ? RenderState.texture->getSize().x : area.width, 
		area.height == 0 ? RenderState.texture->getSize().y : area.height,
		animated, width, height, area.left, area.top
		);

	updateFrame();

	return this;
}

VSprite* VSprite::LoadGraphicFromTexture(sf::Texture& texture, bool animated, int width, int height, const sf::IntRect& area)
{
	RenderState.texture = &texture;

	setSize(
		area.width == 0 ? RenderState.texture->getSize().x : area.width,
		area.height == 0 ? RenderState.texture->getSize().y : area.height,
		animated, width, height, area.left, area.top
		);

	updateFrame();
	return this;
}

VSprite* VSprite::MakeGraphic(int width, int height, sf::Color color, float outline, sf::Color outlineColor)
{
	if (disposible && RenderState.texture)
	{
		delete RenderState.texture;
		RenderState.texture = nullptr;
		disposible = false;
	}

	sf::Image image;
	image.create(width, height, color);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (x < outline || y < outline || x > width - outline || y > height - outline)
			{
				image.setPixel(x, y, outlineColor);
			}
		}
	}

	sf::Texture* texture = new sf::Texture();
	texture->loadFromImage(image);
	disposible = true;

	RenderState.texture = texture;
	setSize(width, height, false, width, height);

	return this;
}

VSprite* VSprite::MakeGraphicCircle(int radius, sf::Color color, float outline, sf::Color outlineColor)
{
	if (disposible && RenderState.texture)
	{
		delete RenderState.texture;
		RenderState.texture = nullptr;
		disposible = false;
	}

	unsigned int diameter = radius * 2;

	sf::Image image;
	image.create(diameter, diameter, sf::Color::Transparent);

	for (unsigned int y = 0; y < diameter; y++)
	{
		for (unsigned int x = 0; x < diameter; x++)
		{
			int dx = radius - x;
			int dy = radius - y;
			int length = (dx * dx) + (dy * dy);

			if (length <= (radius * radius))
			{
				if (outline <= 0 || length <= ((radius - outline) * (radius - outline)))
					image.setPixel(x, y, color);
				else
					image.setPixel(x, y, outlineColor);
			}
		}
	}

	sf::Texture* texture = new sf::Texture();
	texture->loadFromImage(image);
	disposible = true;

	RenderState.texture = texture;
	setSize(radius * 2, radius * 2, false, radius * 2, radius * 2);

	return this;
}

void VSprite::Destroy()
{
	VSUPERCLASS::Destroy();

	if (disposible && RenderState.texture)
	{
		delete RenderState.texture;
		RenderState.texture = nullptr;
		disposible = false;
	}
}

void VSprite::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	Animation.Update(dt);
	
	bool updateTexture = false;
	if (Animation.GetLastFrame() != Animation.GetCurrentFrame())
		updateTexture = true;

	if ((FlipX && vertexArray[2].texCoords.x - vertexArray[0].texCoords.x > 0) || (!FlipX && vertexArray[2].texCoords.x - vertexArray[0].texCoords.x < 0))
		updateTexture = true;

	if ((FlipY && vertexArray[2].texCoords.y - vertexArray[0].texCoords.y > 0) || (!FlipY && vertexArray[2].texCoords.y - vertexArray[0].texCoords.y < 0))
		updateTexture = true;

	if (updateTexture)
	{
		updateFrame();
	}
}

void VSprite::Draw(sf::RenderTarget& RenderTarget)
{
	VSUPERCLASS::Draw(RenderTarget);

	sf::View renderTargetView = RenderTarget.getView();
	sf::View scrollView = RenderTarget.getDefaultView();
	RenderState.transform = transformable.getTransform();

	if (TestInView(renderTargetView, scrollView, this, transformable.getTransform().transformRect(vertexArray.getBounds())))
	{
		RenderTarget.setView(scrollView);
		RenderTarget.draw(vertexArray, RenderState);
		RenderTarget.setView(renderTargetView);
	}
}