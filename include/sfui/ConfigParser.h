#ifndef SFUI_CONFIG_PARSER_H_INCLUDED
#define SFUI_CONFIG_PARSER_H_INCLUDED

#include <CxxUtils/json5e.h>
#include <nlohmann/json.hpp>
#include <string_view>
#include <filesystem>
#include <optional>

namespace sfui {

class ConfigView;

/**
 * @brief Converts configuration nodes into strongly typed values.
 *
 * ConfigParser is the primary customization point for configuration parsing.
 * Specializations can be provided for custom types to define how they are
 * constructed from configuration nodes.
 *
 * @tparam T Type produced by the parser.
 */
template<typename T> struct ConfigParser {
    [[nodiscard]] static T parse(ConfigView config);
};

/**
 * @brief Lightweight non-owning view over a JSON configuration node.
 *
 * ConfigView provides typed access helpers for reading configuration values
 * without exposing JSON parsing logic throughout the codebase.
 *
 * Values are converted using ConfigParser specializations.
 *
 * The view does not own the underlying JSON value and must not outlive it.
 */
class ConfigView {
public:
    /**
     * @brief Lightweight non-owning iterable view over a configuration array.
     *
     * ArrayView provides iteration over configuration nodes stored inside
     * JSON arrays.
     *
     * The view does not own the underlying JSON data and must not outlive it.
     */
    class ArrayView {
    public:
        class Iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ConfigView;
            Iterator(nlohmann::json::const_iterator iterator) : m_iterator(iterator) {}
            [[nodiscard]] ConfigView operator*() const { return ConfigView(*m_iterator); }
            Iterator& operator++() { ++m_iterator; return *this; }
            [[nodiscard]] bool operator==(const Iterator& it) const { return m_iterator == it.m_iterator; }
            [[nodiscard]] bool operator!=(const Iterator& it) const { return m_iterator != it.m_iterator; }

        private:
            nlohmann::json::const_iterator m_iterator;
        };

    public:
        explicit ArrayView(const nlohmann::json& value) : m_value(value) {}
        [[nodiscard]] Iterator begin() const { return Iterator(m_value.begin()); }
        [[nodiscard]] Iterator end() const { return Iterator(m_value.end()); }

    private:
        const nlohmann::json& m_value;
    };

public:
    /**
     * @brief Creates a non-owning view over a JSON value.
     *
     * @param config JSON value referenced by the view.
     */
    explicit ConfigView(const nlohmann::json& config) noexcept
        : m_config(config) {}

    /**
     * @brief Checks whether the specified key exists.
     *
     * @param key Configuration key.
     *
     * @return true if the key exists.
     */
    [[nodiscard]] bool contains(std::string_view key) const {
        return m_config.contains(key);
    }

    /**
     * @brief Returns the underlying JSON value.
     *
     * This method is intended primarily for low-level integrations and
     * internal parser implementations.
     */
    [[nodiscard]] const nlohmann::json& raw() const noexcept {
        return m_config;
    }

    /**
     * @brief Returns an optional configuration value.
     *
     * If the key is missing, std::nullopt is returned.
     *
     * @tparam T Parsed value type.
     * @param key Configuration key.
     *
     * @throws std::exception Propagated from ConfigParser<T>.
     */
    template<typename T = ConfigView>
    [[nodiscard]] std::optional<T> optional(std::string_view key) const {
        if (auto it = m_config.find(key); it != m_config.end()) {
            return ConfigParser<T>::parse(ConfigView(*it));
        }

        return std::nullopt;
    }

    /**
     * @brief Returns a required configuration value.
     *
     * @tparam T Parsed value type.
     * @param key Configuration key.
     *
     * @throws std::out_of_range If the key does not exist.
     * @throws std::exception Propagated from ConfigParser<T>.
     */
    template<typename T = ConfigView>
    [[nodiscard]] T required(std::string_view key) const {
        if (auto it = m_config.find(key); it != m_config.end()) {
            return ConfigParser<T>::parse(ConfigView(*it));
        }

        throw std::out_of_range("Key not found: " + std::string(key));
    }

    /**
     * @brief Returns a configuration value or a fallback value.
     *
     * @tparam T Parsed value type.
     * @param key Configuration key.
     * @param defaultValue Value returned when the key is missing.
     *
     * @throws std::exception Propagated from ConfigParser<T>.
     */
    template<typename T>
    [[nodiscard]] T valueOr(std::string_view key, T defaultValue) const {
        if (auto it = m_config.find(key); it != m_config.end()) {
            return ConfigParser<T>::parse(ConfigView(*it));
        }

        return defaultValue;
    }

    /**
     * @brief Parses the current configuration node as the specified type.
     *
     * Unlike required() or optional(), this method parses the current node
     * itself instead of accessing a child key.
     *
     * @tparam T Parsed value type.
     *
     * @throws std::exception Propagated from ConfigParser<T>.
     */
    template<typename T>
    [[nodiscard]] T as() const {
        return ConfigParser<T>::parse(*this);
    }

    /**
     * @brief Returns an iterable view over a configuration array.
     *
     * Missing keys are treated as empty arrays.
     *
     * @param key Configuration key.
     *
     * @throws std::runtime_error If the value exists but is not an array.
     */
    [[nodiscard]] ArrayView array(std::string_view key) const {
        static const nlohmann::json cEmptyArray = nlohmann::json::array();
        return valueOr<ArrayView>(key, ArrayView(cEmptyArray));
    }

private:
    const nlohmann::json& m_config;
};

/**
 * @brief Owns parsed configuration data.
 *
 * Config provides lifetime management for parsed JSON configuration trees
 * and produces lightweight ConfigView instances for reading values.
 */
class Config {
public:
    /**
     * @brief Creates configuration storage from an existing JSON tree.
     *
     * @param config Parsed configuration tree.
     */
    explicit Config(nlohmann::json&& config) noexcept
        : m_config(std::move(config)) {}

    /**
     * @brief Loads and parses a configuration file.
     *
     * @param configFile Configuration file path.
     */
    explicit Config(const std::filesystem::path& configFile)
        : m_config(CxxUtils::json5e::parse(configFile)) {}

    /**
     * @brief Returns a view over the root configuration node.
     */
    [[nodiscard]] ConfigView view() const noexcept {
        return ConfigView(m_config);
    }

private:
    nlohmann::json m_config;
};

/**
 * @brief Parses a configuration node into the target type.
 *
 * By default, parsing is delegated to nlohmann::json::get().
 *
 * @param config Configuration node view.
 */
template<typename T>
T ConfigParser<T>::parse(ConfigView config) {
    return config.raw().get<T>();
}

/**
 * @brief Parses configuration nodes as ConfigView instances.
 */
template<> struct ConfigParser<ConfigView> {
    [[nodiscard]] static ConfigView parse(ConfigView config) {
        return config;
    }
};

/**
 * @brief Parses configuration nodes as ConfigListView instances.
 */
using ConfigListView = ConfigView::ArrayView;
template<> struct ConfigParser<ConfigListView> {
    [[nodiscard]] static ConfigListView parse(ConfigView config) {
        const auto& value = config.raw();
        if (!value.is_array()) {
            throw std::runtime_error("Expected array");
        }
        return ConfigListView(value);
    }
};

}

#endif // SFUI_CONFIG_PARSER_H_INCLUDED
