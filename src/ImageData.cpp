#include "ImageData.h"
#include <CxxUtils/exception.h>
#include <algorithm>

using namespace sfui;

namespace {

class DefaultImageFileLoader final : public iFileLoader<ImageData> {
public:
    bool Probe(const CxxUtils::ifstream& reader) const noexcept final {
        return true;
    }

    FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final {
        const auto supportedFormats = {".bmp", ".png", ".tga", ".jpg", ".gif", ".psd", ".hdr", ".pic", ".pnm"};
        return std::any_of(supportedFormats.begin(), supportedFormats.end(),
                           [extension](std::string_view ext) { return ext == extension; })
                   ? FileLoader::PriorityLevel::Likely
                   : FileLoader::PriorityLevel::Unlikely;
    }

    ImageData Load(const std::filesystem::path& filepath, CxxUtils::ifstream&& reader) const final {
        auto frames = std::make_shared<std::vector<sf::Texture>>();

        reader.exceptions(std::ios::failbit | std::ios::badbit); // Make sure exceptions are enabled
        const auto fileSize = reader.size();
        const auto fileData = reader.readVector<std::uint8_t>(fileSize);

        // Use SFML to load the texture
        sf::Texture texture;
        if (!texture.loadFromMemory(fileData.data(), fileSize)) {
            throw CxxUtils::Exception("Failed to load image: " + filepath.string());
        }

        frames->push_back(std::move(texture));
        return frames;
    }
};

static FileLoaderRegistrar<DefaultImageFileLoader> defaultImageLoaderRegistrar;

}
