#ifndef SFUI_FILE_LOADER_H_INCLUDED
#define SFUI_FILE_LOADER_H_INCLUDED

#include <memory>
#include <string_view>
#include <libvfs/FileReader.h>

namespace sfui {

/**
 * @enum LoaderPriorityLevel
 * @brief Defines priority levels for file loaders.
 */
enum class LoaderPriorityLevel {
    Unlikely,       /**< Loader is unlikely to be used */
    Likely,         /**< Loader is likely to be used */
    HighlyLikely    /**< Loader is highly likely to be used */
};

/**
 * @class iFileLoader
 * @brief Interface for file loaders.
 *
 * @tparam T The type of data to be loaded.
 */
template<typename T>
class iFileLoader {
public:
    using Type = T;

    /**
     * @brief Virtual destructor for the iFileLoader interface.
     */
    virtual ~iFileLoader() = default;

    /**
     * @brief Gets the priority of the loader based on the file extension.
     *
     * @param extension The file extension to check.
     * @return The priority level of the loader.
     */
    virtual LoaderPriorityLevel GetPriority(std::string_view extension) const noexcept = 0;

    /**
     * @brief Probes the file reader to check if it can be loaded by this loader.
     *
     * @param reader The file reader to probe.
     * @return True if the reader can be loaded, false otherwise.
     */
    virtual bool Probe(const LibVFS::FileReader& reader) const = 0;

    /**
     * @brief Loads data from the file reader.
     *
     * @param filepath The path to referenced file reader.
     * @param reader The file reader to load from.
     * @return The loaded data of type T.
     */
    virtual T Load(std::string_view filepath, LibVFS::FileReader&& reader) const = 0;
    T Load(std::string_view filepath) const { return Load(filepath, std::move(LibVFS::FileReader(filepath))); }
};

/**
 * @brief Registers a custom file loader for a specific resource type.
 *
 * This function registers a loader class that implements the `iFileLoader` interface
 * for a particular resource type, enabling the system to handle file loading for that type.
 *
 * @tparam Loader The class implementing the `iFileLoader` interface for the specific resource type.
 *                It must inherit from `iFileLoader<Type>`, where `Type` is the resource type
 *                the loader handles (e.g., `ImageData`, `AudioData`, etc.).
 * @tparam Args   Variadic template for the arguments to be passed to the loader's constructor.
 *
 * @param args Arguments forwarded to the constructor of the `Loader` class.
 *
 * @note The `Loader` class must be derived from `iFileLoader<Type>`.
 *
 * Example usage:
 * @code
 * class PngFileLoader final : public iFileLoader<ImageData> { ... };
 * class WavFileLoader final : public iFileLoader<AudioData> { ... };
 *
 * int main()
 * {
 *     LoaderRegister<PngFileLoader>();
 *     LoaderRegister<WavFileLoader>();
 * ...
 * @endcode
 */
template<typename Loader, typename... Args>
static inline void LoaderRegister(Args&&... args)
{
    using Type = typename Loader::Type;
    static_assert(std::is_base_of_v<iFileLoader<Type>, Loader>);
    extern void RegisterFileLoader(std::unique_ptr<iFileLoader<Type>>);
    RegisterFileLoader(std::make_unique<Loader>(std::forward<Args>(args)...));
}

}

#endif // SFUI_FILE_LOADER_H_INCLUDED
