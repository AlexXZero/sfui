# SFUI - Simple and Fast UI library

SFUI is a C++ library designed to simplify the creation of graphical user interfaces (GUI) in applications using the SFML library for rendering and JSON for defining dynamic UI layouts. With SFUI, you can adjust the UI of your application without recompiling, making it adaptable and easy to maintain.

## About SFUI

SFUI offers two primary ways to create a user interface:

1. **JSON-based UI Configuration**: Similar to HTML, SFUI allows you to define the layout, positioning, and behaviour of GUI elements in a JSON file. This approach lets you modify the UI structure dynamically without the need to recompile the application.

2. **C++ API for Programmatic UI Creation**: Inspired by Qt, SFUI provides C++ classes that allow you to create and configure UI elements directly in your code. This option is useful for applications requiring dynamic UI generation or customization at runtime.

### Key Features

- **Modular UI Definition**: Use JSON configurations to manage UI layouts, making updates and adjustments easy.
- **Flexible Resource Loading**: Includes generic loaders and parsers, which can be customized to handle different formats for fonts, images, and other resources.
- **Event Handling**: Provides classes for handling mouse and keyboard events, enabling interactive and responsive UI components.
- **Rendering Powered by SFML**: Leverages the powerful 2D rendering capabilities of SFML, ensuring smooth and efficient UI rendering.

## Dependencies
This library uses SFML as backend for rendering UI and nlohmann/json for parsing
UI configuration.

## Getting Started

Follow these instructions to set up the project locally and get it running.

### Prerequisites

Ensure you have the following tools installed:

- **CMake:** A cross-platform build system.

### Installation

1. Create a build directory and navigate into it:
    ```sh
    mkdir build
    cd build
    ```

2. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

3. Build the project:
    ```sh
    make
    ```

### Usage

For an example of how to use the library, refer to the [example](example) directory and its [README.md](example/README.md) file.

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.
