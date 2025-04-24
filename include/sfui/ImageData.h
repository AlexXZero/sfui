#ifndef SFUI_IMAGE_DATA_H_INCLUDED
#define SFUI_IMAGE_DATA_H_INCLUDED

#include "FileLoader.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cassert>


namespace sfui {

class ImageData final : public iFileLoadable<ImageData> {
public:
    ImageData() = default;
    ImageData(std::shared_ptr<std::vector<sf::Texture>> data) noexcept : m_data(data) { assert(m_data != nullptr); }
    ~ImageData() = default;
    std::size_t FramesCount() const noexcept { return m_data ? m_data->size() : 0; }
    const sf::Texture& Frame(std::size_t frameIndex) const noexcept { assert(FramesCount() > frameIndex); return (*m_data)[frameIndex]; }
    sf::Vector2u Size() const noexcept { return Frame(0).getSize(); }
    std::shared_ptr<std::vector<sf::Texture>> Data() { return m_data; }
    void SetSmooth(bool smooth) { for (auto& texture : *m_data) texture.setSmooth(smooth); }

private:
    std::shared_ptr<std::vector<sf::Texture>> m_data;
};

// Non repeated animations
class FiniteAnimation : /*public sf::Transformable, */public sf::Drawable {
public:
    FiniteAnimation(ImageData data, std::size_t fps = 60)
        : m_data(data), m_fps(fps), m_shape(sf::Vector2f(m_data.Size())), m_startRenderingTime(std::chrono::high_resolution_clock::now()) {}
    FiniteAnimation(const std::filesystem::path& path, std::size_t fps = 60)
        : FiniteAnimation(ImageData::Load(path), fps) {}
    ~FiniteAnimation() override = default;
    sf::Vector2u GetNativeSize() const { return m_data.Size(); }

    void SetSize(const sf::Vector2f& size) { m_shape.setSize(size); }
    void SetTextureRect(const sf::IntRect& rect) { m_shape.setTextureRect(rect); }
    void SetFillColor(const sf::Color& color) { m_shape.setFillColor(color); }
    void SetPosition(float x, float y) { m_shape.setPosition(x, y); }
    void SetPosition(const sf::Vector2f& position) { m_shape.setPosition(position); }
    void SetRotation(float angle) { m_shape.setRotation(angle); }
    void SetScale(float factorX, float factorY) { m_shape.setScale(factorX, factorY); }
    void SetScale(float factorXY) { m_shape.setScale(factorXY, factorXY); }
    void SetScale(const sf::Vector2f& factors) { m_shape.setScale(factors); }
    void SetOrigin(float x, float y) { m_shape.setOrigin(x, y); }
    void SetOrigin(const sf::Vector2f& origin) { m_shape.setOrigin(origin); }
    const sf::Vector2f& GetSize() const { return m_shape.getSize(); }
    const sf::IntRect& GetTextureRect() const { return m_shape.getTextureRect(); }
    const sf::Color& GetFillColor() const { return m_shape.getFillColor(); }
    const sf::Vector2f& GetPosition() const { return m_shape.getPosition(); }
    float GetRotation() const { return m_shape.getRotation(); }
    const sf::Vector2f& GetScale() const { return m_shape.getScale(); }
    const sf::Vector2f& GetOrigin() const { return m_shape.getOrigin(); }
    void Move(float offsetX, float offsetY) { m_shape.move(offsetX, offsetY); }
    void Move(const sf::Vector2f& offset) { m_shape.move(offset); }
    void Rotate(float angle) { m_shape.rotate(angle); }
    void Scale(float factorX, float factorY) { m_shape.scale(factorX, factorY); }
    void Scale(const sf::Vector2f &factor) { m_shape.scale(factor); }

    bool IsFinished() const {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startRenderingTime).count();
        auto frameIndex = (elapsedTime * m_fps / 1000);
        return frameIndex >= m_data.FramesCount();
    }
    void ResetPlay() const {
        m_startRenderingTime = std::chrono::high_resolution_clock::now();
    }

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startRenderingTime).count();
        auto frameIndex = (elapsedTime * m_fps / 1000);
        if (frameIndex < m_data.FramesCount()) {
            m_shape.setTexture(&m_data.Frame(frameIndex));
            target.draw(m_shape, states); // m_shape.draw(target, states);
            //states.transform *= getTransform();
            //states.texture = &m_data.Frame(frameIndex);
            //target.draw(m_vertices, states);
        }
    }

private:
    ImageData m_data;
    std::size_t m_fps;
    mutable sf::RectangleShape m_shape;
    mutable std::chrono::high_resolution_clock::time_point m_startRenderingTime;
};

class RepeatingAnimation : public FiniteAnimation {
public:
    RepeatingAnimation(ImageData data, std::size_t fps = 60)
            : FiniteAnimation(data, fps) {}
    RepeatingAnimation(const std::filesystem::path& path, std::size_t fps = 60)
            : FiniteAnimation(path, fps) {}
    ~RepeatingAnimation() final = default;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const final {
        if (IsFinished()) {
            ResetPlay();
        }
        FiniteAnimation::draw(target, states);
    }
};

}

#endif // SFUI_IMAGE_DATA_H_INCLUDED
