# Flipper Zero BLE Spam Module (not working)

This project implements a Bluetooth Low Energy (BLE) spam module for the Flipper Zero device. The module is designed to send custom names to nearby Bluetooth devices, allowing for testing and experimentation with BLE advertising.

## Project Structure

- **application.fam**: Configuration or application manifest file specific to the Flipper Zero platform.
- **ble_spam.c**: Main implementation of the BLE spam module, including initialization and spam management functions.
- **ble_spam.h**: Header file declaring functions and constants for the BLE spam module.
- **icons/ble_10px.png**: Icon file for the user interface of the application.
- **scenes/ble_spam_scene.c**: Implements scene logic for the BLE spam module, handling user interactions and displaying spam status.
- **scenes/ble_spam_scene.h**: Header file declaring functions and structures for the BLE spam scene.
- **scenes/ble_spam_scene_config.h**: Configuration settings for the BLE spam scene, such as layout or display parameters.
- **helpers/ble_payload.c**: Helper functions for creating and managing BLE payloads.
- **helpers/ble_payload.h**: Header file declaring functions and constants for BLE payload management.
- **views/ble_spam_view.c**: Implements view logic for displaying the BLE spam module's interface.
- **views/ble_spam_view.h**: Header file declaring functions and structures for the BLE spam view.
- **ble_spam_app.c**: Entry point for the application, initializing the BLE spam module and starting the main loop.
- **ble_spam_app.h**: Header file declaring functions and constants for the application entry point.
- **ble_spam_i.h**: Internal definitions or interfaces used across the BLE spam module.
- **protocols/custom_names.c**: Implements functionality for managing and sending custom names to Bluetooth devices.

## Setup Instructions

1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Ensure you have the necessary development environment set up for Flipper Zero.
4. Compile the project using the appropriate build tools for the Flipper Zero platform.
5. Upload the compiled application to your Flipper Zero device.

## Usage

Once the application is running on your Flipper Zero, you can start sending custom names to nearby Bluetooth devices. The interface will allow you to configure the names and manage the spam functionality.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.# flipper_custom_ble
