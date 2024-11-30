#include "ImageData.h"
#include <CxxUtils/exception.h>
#include <array>

using namespace sfui;

namespace {

class AnimsImageFileLoader final : public iFileLoader<ImageData> {
public:
    bool Probe(const LibVFS::FileReader& reader) const final {
        if (reader.GetSize() < expectedSignature.size()) return false;
        std::array<std::uint8_t, 5> signature;
        reader.Peek(signature.data(), signature.size());
        return signature == expectedSignature;
    }

    FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final {
        return extension == ".anims" ? FileLoader::PriorityLevel::HighlyLikely : FileLoader::PriorityLevel::Unlikely;
    }

    ImageData Load(const std::filesystem::path& filepath, LibVFS::FileReader&& reader) const final {
        auto frames = std::make_shared<std::vector<sf::Texture>>();

        if (reader.GetSize() < expectedSignature.size()) {
            throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(too small)");
        }

        const auto signature = reader.ReadArray<std::uint8_t, expectedSignature.size()>();
        if (signature != expectedSignature) {
            throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(invalid magic)");
        }

        reader.Skip(sizeof(std::uint16_t)); // width
        reader.Skip(sizeof(std::uint16_t)); // height

        while (reader.Tell() + sizeof(std::uint32_t) < reader.GetSize()) {
            auto frameBufferSize = reader.Read<std::uint32_t>();
            if (!frameBufferSize) break;
            if (reader.Tell() + frameBufferSize > reader.GetSize()) {
                throw CxxUtils::Exception("Failed to load image: " + filepath.string() + ": Bad ANIMS file(unexpected end of file)");
            }

            auto frameBuffer = reader.ReadVector<std::uint8_t>(frameBufferSize);
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
