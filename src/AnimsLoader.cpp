#include "ImageData.h"
#include <CxxUtils/exception.h>

using namespace sfui;

namespace {

class AnimsImageFileLoader final : public iFileLoader<ImageData> {
public:
    bool Probe(const CxxUtils::ifstream& reader) const noexcept final {
        if (reader.size() < std::streamoff(expectedSignature.size())) return false;
        auto signature = reader.peekArray<std::uint8_t, expectedSignature.size()>();
        return signature == expectedSignature;
    }

    FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final {
        return extension == ".anims" ? FileLoader::PriorityLevel::HighlyLikely : FileLoader::PriorityLevel::Unlikely;
    }

    ImageData Load(const std::filesystem::path& filepath, CxxUtils::ifstream&& reader) const final {
        auto frames = std::make_shared<std::vector<sf::Texture>>();
        reader.exceptions(std::ios::failbit | std::ios::badbit); // Make sure exceptions are enabled

        if (reader.size() < std::streamoff(expectedSignature.size())) {
            throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(too small)");
        }

        const auto signature = reader.readArray<std::uint8_t, expectedSignature.size()>();
        if (signature != expectedSignature) {
            throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(invalid magic)");
        }

        reader.skip(sizeof(std::uint16_t)); // width
        reader.skip(sizeof(std::uint16_t)); // height

        while (reader.tell() + std::streamoff(sizeof(std::uint32_t)) < reader.size()) {
            auto frameBufferSize = reader.read<std::uint32_t>();
            if (!frameBufferSize) break;
            if (reader.tell() + frameBufferSize > reader.size()) {
                throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(unexpected end of file)");
            }

            auto frameBuffer = reader.readVector<std::uint8_t>(frameBufferSize);
            sf::Texture texture;
            if (!texture.loadFromMemory(frameBuffer.data(), frameBuffer.size())) {
                throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(corruption)");
            }

            frames->push_back(std::move(texture));
        }

        return frames;
    }

private:
    static constexpr std::array<std::uint8_t, 5> expectedSignature = {'A', 'N', 'I', 'M', 'S'};
};

static FileLoaderRegistrar<AnimsImageFileLoader> animsImageLoaderRegistrar;

}
