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

private:
    std::shared_ptr<std::vector<sf::Texture>> m_data;
};

}

#endif // SFUI_IMAGE_DATA_H_INCLUDED
