#ifndef SFUI_FILE_LOADER_H_INCLUDED
#define SFUI_FILE_LOADER_H_INCLUDED

#include <memory>
#include <vector>
#include <filesystem>
#include <string_view>
#include <libvfs/FileReader.h>
#include <CxxUtils/logger.h>
#include <CxxUtils/exception.h>

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
};

/**
 * @class iFileLoadable
 * @brief Interface for managing file loadable objects and their loaders.
 *
 * This class provides functionality to register and utilize file loaders for specific
 * resource types. It determines the appropriate loader for a given file based on its
 * priority and compatibility with the file.
 *
 * @tparam T The type of resource that can be loaded (e.g., ImageData, AudioData).
 */
template<typename T>
class iFileLoadable {
public:
    /**
     * @brief Virtual destructor for the iFileLoadable interface.
     */
    virtual ~iFileLoadable() = default;

    /**
     * @brief Loads a resource of type T from the specified file path.
     *
     * This function iterates through registered loaders in priority order to find
     * the most suitable loader for the given file. It first checks loaders marked
     * as `HighlyLikely`, followed by `Likely`, and finally considers others.
     *
     * @param filepath The path to the file to be loaded.
     * @return The loaded resource of type T.
     * @throws CxxUtils::Exception If no suitable loader is found for the file.
     *
     * @note This function assumes that at least one loader is registered for the type T.
     */
    static T Load(const std::filesystem::path& filepath) {
        auto extension = filepath.extension();
        LibVFS::FileReader reader(filepath);

        // Check highly likely loaders
        for (const auto& loader : m_loaders) {
            LoaderPriorityLevel priority = loader->GetPriority(extension);
            if (priority == LoaderPriorityLevel::HighlyLikely && loader->Probe(reader)) {
                return loader->Load(filepath, std::move(reader));
            }
        }
        CxxUtils::LogInfo("No highly likely loader found for file: ") << filepath;

        // Check likely applicable loaders
        for (const auto& loader : m_loaders) {
            LoaderPriorityLevel priority = loader->GetPriority(extension);
            if (priority == LoaderPriorityLevel::Likely && loader->Probe(reader)) {
                return loader->Load(filepath, std::move(reader));
            }
        }
        CxxUtils::LogWarning("No likely loader found for file: ") << filepath;

        // Check the rest of the possible loaders
        for (const auto& loader : m_loaders) {
            LoaderPriorityLevel priority = loader->GetPriority(extension);
            if (priority != LoaderPriorityLevel::HighlyLikely && priority != LoaderPriorityLevel::Likely && loader->Probe(reader)) {
                return loader->Load(filepath, std::move(reader));
            }
        }
        CxxUtils::LogError("No applicable loader found for file: ") << filepath;

        throw CxxUtils::Exception("No applicable loader found for file: %s", filepath.c_str());
    }

    /**
     * @brief Registers a file loader for the resource type T.
     *
     * This function allows adding custom loaders that implement the `iFileLoader` interface.
     * The loaders are stored in an internal list and used during the `Load` process.
     *
     * @param loader A unique pointer to the loader to be registered.
     *
     * Example usage:
     * @code
     * iFileLoadable<ImageData>::RegisterFileLoader(std::make_unique<MyImageLoader>());
     * @endcode
     */
    static void RegisterFileLoader(std::unique_ptr<iFileLoader<T>> loader) {
        m_loaders.emplace_back(std::move(loader));
    }

private:
    /**
     * @brief A list of registered loaders for the resource type T.
     *
     * The loaders are stored in a static inline vector and queried during the
     * loading process to determine the best match for a given file.
     */
    static inline std::vector<std::unique_ptr<iFileLoader<T>>> m_loaders;
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
    Type::RegisterFileLoader(std::make_unique<Loader>(std::forward<Args>(args)...));
}

}

#endif // SFUI_FILE_LOADER_H_INCLUDED
