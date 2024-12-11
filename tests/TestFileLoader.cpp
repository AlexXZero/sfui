#include "UTest.hpp"
#include <sfui/FileLoader.h>

namespace sfui {

struct TestImageData final : public iFileLoadable<TestImageData> {};
struct TestAudioData final : public iFileLoadable<TestAudioData> {};
struct TestFontData final : public iFileLoadable<TestFontData> {};

}

namespace {

class PngFileLoader final : public sfui::iFileLoader<sfui::TestImageData> {
public:
    bool Probe(const CxxUtils::ifstream& reader) const final { return true; }
    sfui::FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::FileLoader::PriorityLevel::Likely; }
    sfui::TestImageData Load(const std::filesystem::path& filepath, CxxUtils::ifstream&& reader) const final { return {}; } // fake reader
};

class WavFileLoader final : public sfui::iFileLoader<sfui::TestAudioData> {
public:
    bool Probe(const CxxUtils::ifstream& reader) const final { return true; }
    sfui::FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::FileLoader::PriorityLevel::Likely; }
    sfui::TestAudioData Load(const std::filesystem::path& filepath, CxxUtils::ifstream&& reader) const final { return {}; } // fake reader
};

class TtfFileLoader final : public sfui::iFileLoader<sfui::TestFontData> {
public:
    bool Probe(const CxxUtils::ifstream& reader) const final { return true; }
    sfui::FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::FileLoader::PriorityLevel::Likely; }
    sfui::TestFontData Load(const std::filesystem::path& filepath, CxxUtils::ifstream&& reader) const final { return {}; } // fake reader
};

// Static loader registration
static sfui::FileLoaderRegistrar<PngFileLoader> pngRegistrar;
static sfui::FileLoaderRegistrar<WavFileLoader> wavRegistrar;

}

UTEST(RuntimeLoadersRegistration)
{
    // Given:
    sfui::FileLoaderRegistry::Register<TtfFileLoader>();

    // Then:
    UTEST_ASSERT_TRUE(true);
}
