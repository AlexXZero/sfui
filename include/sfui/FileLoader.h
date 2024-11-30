#ifndef SFUI_FILE_LOADER_H_INCLUDED
#define SFUI_FILE_LOADER_H_INCLUDED

#include <memory>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <string_view>
#include <libvfs/FileReader.h>
#include <CxxUtils/logger.h>
#include <CxxUtils/demangle.h>
#include <CxxUtils/exception.h>

namespace sfui {

namespace FileLoader {

/**
 * @enum PriorityLevel
 * @brief Specifies the priority of file loaders.
 */
enum class PriorityLevel {
    Unlikely,       /**< Loader is unlikely to be used */
    Likely,         /**< Loader is likely to be used */
    HighlyLikely    /**< Loader is very likely to be used */
};

}

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
     * @brief Returns the loader's priority based on the file extension.
     *
     * @param extension The file extension, converted to lowercase, to check.
     * @return The loader's priority level.
     */
    virtual FileLoader::PriorityLevel GetPriority(std::string_view extension) const noexcept = 0;

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
     * @param filepath The file's path.
     * @param reader The file reader to load data from.
     * @return The loaded data of type T.
     */
    virtual T Load(const std::filesystem::path& filepath, LibVFS::FileReader&& reader) const = 0;
};

/**
 * @class FileLoaderRegistry
 * @brief Manages file loaders and their registration.
 *
 * Registers file loaders for different resource types and selects the most appropriate one based on file characteristics.
 */
class FileLoaderRegistry {
public:
    /**
     * @brief Registers a custom file loader for a resource type.
     *
     * This function allows the system to handle file loading for a specific resource type by registering a loader.
     *
     * @tparam Loader The loader class that implements `iFileLoader<Type>`.
     * @tparam Args   Arguments for the loader's constructor.
     *
     * @param args Arguments passed to the loader's constructor.
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
     *     FileLoaderRegistry::Register<PngFileLoader>();
     *     FileLoaderRegistry::Register<WavFileLoader>();
     *     ...
     * }
     * @endcode
     */
    template<typename Loader, typename... Args>
    static inline void Register(Args&&... args) {
        using LoaderType = typename Loader::Type;
        static_assert(std::is_base_of_v<iFileLoader<LoaderType>, Loader>);
        CxxUtils::LogInfo("Registering FileLoader: ")
            << CxxUtils::demangle(typeid(Loader).name())
            << "<" << CxxUtils::demangle(typeid(LoaderType).name()) << ">";
        Instance<LoaderType>().emplace_back(std::make_unique<Loader>(std::forward<Args>(args)...));
    }

    /**
     * @brief Finds the most suitable loader for a resource.
     *
     * Searches for a loader based on the file's extension and reader compatibility, prioritizing highly likely loaders first.
     *
     * @tparam T The type of resource to load (e.g., ImageData, AudioData).
     * @param filepath The path to the file.
     * @param reader The file reader associated with the file.
     * @return The appropriate loader for the resource.
     * @throws CxxUtils::Exception If no suitable loader is found.
     */
    template<typename T>
    static const iFileLoader<T>& GetLoader(const std::filesystem::path& filepath, const LibVFS::FileReader& reader) {
        const auto& loaders = Instance<T>();
        auto extension = filepath.extension().string();

        // Convert the file extension to lowercase to ensure case-insensitive comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

        // Check highly likely loaders first
        for (const auto& loader : loaders) {
            FileLoader::PriorityLevel priority = loader->GetPriority(extension);
            if (priority == FileLoader::PriorityLevel::HighlyLikely && loader->Probe(reader)) {
                return *loader;
            }
        }
        CxxUtils::LogInfo("No highly likely loader found for file: ") << filepath.string();

        // Check likely applicable loaders
        for (const auto& loader : loaders) {
            FileLoader::PriorityLevel priority = loader->GetPriority(extension);
            if (priority == FileLoader::PriorityLevel::Likely && loader->Probe(reader)) {
                return *loader;
            }
        }
        CxxUtils::LogWarning("No likely loader found for file: ") << filepath.string();

        // Check other loaders
        for (const auto& loader : loaders) {
            FileLoader::PriorityLevel priority = loader->GetPriority(extension);
            if (priority != FileLoader::PriorityLevel::HighlyLikely && priority != FileLoader::PriorityLevel::Likely && loader->Probe(reader)) {
                return *loader;
            }
        }
        CxxUtils::LogError("No applicable loader found for file: ") << filepath.string();

        throw CxxUtils::Exception("No suitable loader found for file: %s", filepath.c_str());
    }

private:
    template<typename T>
    using FileLoaderRegistryImp = std::vector<std::unique_ptr<iFileLoader<T>>>;

    template<typename T>
    static FileLoaderRegistryImp<T>& Instance() {
        static FileLoaderRegistryImp<T> instance;
        return instance;
    }
};

/**
 * @brief Helper for static registration of file loaders.
 *
 * Automatically registers a loader when the program starts.
 *
 * Example usage:
 * @code
 * static FileLoaderRegistrar<PngFileLoader> pngRegistrar;
 * static FileLoaderRegistrar<WavFileLoader> wavRegistrar;
 * @endcode
 */
template<typename Loader>
struct FileLoaderRegistrar {
    FileLoaderRegistrar() { FileLoaderRegistry::Register<Loader>(); }
};

/**
 * @class iFileLoadable
 * @brief Interface for managing objects loadable from file.
 *
 * @tparam T The type of resource to load (e.g., ImageData, AudioData).
 */
template<typename T>
class iFileLoadable {
public:
    /**
     * @brief Virtual destructor for the iFileLoadable interface.
     */
    virtual ~iFileLoadable() = default;

    /**
     * @brief Loads a resource from the specified file.
     *
     * This function finds the best loader for the file based on its extension
     * and compatibility with the reader. Then use it for loading the resource.
     *
     * @param filepath The path to the file.
     * @return The loaded resource of type T.
     * @throws CxxUtils::Exception If no suitable loader is found.
     */
    static T Load(const std::filesystem::path& filepath) {
        LibVFS::FileReader reader(filepath);
        const auto& loader = FileLoaderRegistry::GetLoader<T>(filepath, reader);
        return loader.Load(filepath, std::move(reader));
    }
};

}

#endif // SFUI_FILE_LOADER_H_INCLUDED
