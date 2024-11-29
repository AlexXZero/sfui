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
    bool Probe(const LibVFS::FileReader& reader) const final { return true; }
    sfui::LoaderPriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::LoaderPriorityLevel::Likely; }
    sfui::TestImageData Load(std::string_view filepath, LibVFS::FileReader&& reader) const final { return {}; } // fake reader
};

class WavFileLoader final : public sfui::iFileLoader<sfui::TestAudioData> {
public:
    bool Probe(const LibVFS::FileReader& reader) const final { return true; }
    sfui::LoaderPriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::LoaderPriorityLevel::Likely; }
    sfui::TestAudioData Load(std::string_view filepath, LibVFS::FileReader&& reader) const final { return {}; } // fake reader
};

class TtfFileLoader final : public sfui::iFileLoader<sfui::TestFontData> {
public:
    bool Probe(const LibVFS::FileReader& reader) const final { return true; }
    sfui::LoaderPriorityLevel GetPriority(std::string_view extension) const noexcept final { return sfui::LoaderPriorityLevel::Likely; }
    sfui::TestFontData Load(std::string_view filepath, LibVFS::FileReader&& reader) const final { return {}; } // fake reader
};

}

UTEST(LoadersRegistration)
{
    // Given:
    sfui::LoaderRegister<PngFileLoader>();
    sfui::LoaderRegister<WavFileLoader>();
    sfui::LoaderRegister<TtfFileLoader>();

    // Then:
    UTEST_ASSERT_TRUE(true);
}
