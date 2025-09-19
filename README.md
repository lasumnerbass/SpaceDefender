# 🚀 Space Defender - Enhanced Edition

[![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![SFML](https://img.shields.io/badge/SFML-2.6%2B-green.svg)](https://www.sfml-dev.org/)
[![macOS](https://img.shields.io/badge/macOS-11.0%2B-lightgrey.svg)](https://www.apple.com/macos/)

A modern, high-performance 2D space shooter game built with C++20 and SFML. Features advanced particle systems, object pooling, audio integration, and a sophisticated build system that seamlessly integrates CMake with Xcode.

![Space Defender Screenshot](https://via.placeholder.com/800x400/000000/FFFFFF?text=Space+Defender+Game+Screenshot)

## ✨ Features

### 🎮 Gameplay
- **Classic Space Shooter**: Pilot your spacecraft through waves of enemies
- **Intuitive Controls**: Arrow keys for movement, spacebar to shoot
- **Progressive Difficulty**: Enemies become more challenging as you advance
- **Score System**: Track your performance and compete for high scores
- **Particle Effects**: Stunning visual effects for explosions and impacts

### 🛠 Technical Features
- **Modern C++20**: Leverages the latest C++ features for optimal performance
- **Object Pooling**: Efficient memory management for game objects
- **Performance Monitoring**: Built-in FPS counter and performance metrics
- **Audio System**: Immersive sound effects and background music
- **Resource Management**: Efficient loading and caching of game assets
- **Cross-Platform**: Works seamlessly across different development environments

### 🔧 Development Features
- **Hybrid Build System**: Both CMake and Xcode support with automatic synchronization
- **Hot Reload**: Development workflow with automatic build system updates
- **Multiple IDEs**: Works with Xcode, VS Code, CLion, and command line
- **Debug Support**: Full debugging capabilities in all supported environments

## 🎯 Quick Start

### Prerequisites

- **macOS 11.0+** (Big Sur or later)
- **Xcode 12+** or **Command Line Tools**
- **SFML 2.6+** (automatically handled via Homebrew)
- **CMake 3.20+**

### One-Command Setup

```bash
# Clone the repository
git clone https://github.com/yourusername/SpaceDefender.git
cd SpaceDefender

# Run the demo to see everything working
./demo_sync.sh
```

This will:
1. Set up the build environment
2. Install dependencies if needed
3. Build the game
4. Launch the game automatically

### Traditional Setup

If you prefer manual setup:

```bash
# 1. Install dependencies (if not already installed)
brew install sfml cmake

# 2. Set up build systems
./sync_build_systems.sh

# 3. Build the game
./build_unified.sh

# 4. Run the game
./build/SpaceDefender
```

## 🎮 How to Play

### Controls
- **Arrow Keys**: Move your spacecraft
- **Spacebar**: Shoot bullets
- **Enter** or **1**: Start a new game from the main menu
- **Escape**: Pause the game
- **Q**: Quit the game

### Objective
- Survive waves of incoming enemies
- Destroy enemies to earn points
- Avoid colliding with enemies or their bullets
- Try to achieve the highest score possible!

### Gameplay Tips
- Keep moving to avoid enemy fire
- Aim for groups of enemies for higher scores
- Watch out for enemy spawn patterns
- Use the particle effects to track enemy positions

## 🏗 Architecture

### Project Structure

```
SpaceDefender/
├── src/                          # Source code
│   ├── main.cpp                  # Entry point
│   ├── Game.cpp                  # Main game logic
│   ├── Player.cpp                # Player spacecraft
│   ├── Enemy.cpp                 # Enemy entities
│   ├── Bullet.cpp                # Projectile system
│   ├── ParticleSystem.cpp        # Visual effects
│   ├── AudioSystem.cpp           # Sound management
│   ├── ResourceManager.cpp       # Asset loading
│   ├── InputManager.cpp          # Input handling
│   └── PerformanceMonitor.cpp    # Performance tracking
├── include/                      # Header files
│   └── [corresponding .hpp files]
├── resources/                    # Game assets
│   └── textures/                 # Sprite images
├── build/                        # CMake build output
├── SpaceDefender.xcodeproj/      # Xcode project (auto-generated)
├── CMakeLists.txt               # CMake configuration
├── sync_build_systems.sh        # Build system synchronization
├── build_unified.sh             # Universal build script
├── demo_sync.sh                 # Demo and setup script
└── README.md                    # This file
```

### Core Systems

#### Game Engine
- **Game Loop**: Fixed timestep with delta time calculations
- **State Management**: Menu, gameplay, pause, and game over states
- **Resource Management**: Efficient loading and caching of textures and sounds

#### Object System
- **Object Pooling**: Reusable game objects to minimize memory allocation
- **Component-Based**: Modular design with separate systems for different functionality
- **Collision Detection**: Efficient AABB collision detection system

#### Performance Optimizations
- **Memory Pools**: Pre-allocated object pools for bullets, enemies, and particles
- **Vertex Array Optimization**: Batch rendering for improved performance
- **Smart Resource Loading**: Lazy loading and automatic resource cleanup

## 🛠 Development

### Build System Features

This project includes an advanced build system synchronization that maintains perfect compatibility between CMake and Xcode:

#### Available Commands

| Command | Description |
|---------|-------------|
| `./demo_sync.sh` | Complete demo showing all features |
| `./sync_build_systems.sh` | One-time sync setup |
| `./sync_build_systems.sh --watch` | Auto-sync during development |
| `./build_unified.sh` | Universal build script |
| `./build.sh` | Traditional CMake build |
| `open SpaceDefender.xcodeproj` | Open in Xcode |

#### Development Workflow

For active development:

```bash
# Start auto-sync (keeps everything synchronized)
./sync_build_systems.sh --watch

# In another terminal, make your changes and build
./build_unified.sh

# Or use Xcode
open SpaceDefender.xcodeproj
# Press Cmd+R to build and run
```

### IDE Support

#### Xcode
- Full project integration with proper scheme configuration
- Working directory automatically set to project root
- Debug builds with complete symbol information
- Integrated terminal builds

#### VS Code
Add to `.vscode/settings.json`:
```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": false
}
```

#### CLion
The project works automatically with CLion's CMake integration.

### Adding New Features

1. **Add Source Files**: Place `.cpp` files in `src/` and `.hpp` files in `include/`
2. **Update Build**: Run `./sync_build_systems.sh` to update both build systems
3. **Test**: Build with `./build_unified.sh` and test in multiple environments

## 🔧 Configuration

### Build Configuration

Edit `CMakeLists.txt` to modify build settings:

```cmake
# Build type (Debug/Release)
set(CMAKE_BUILD_TYPE Release)

# C++ standard
set(CMAKE_CXX_STANDARD 20)

# Compiler flags
add_compile_options(-Wall -Wextra -pedantic)
```

### Game Configuration

Game settings can be modified in the source code:

- **Window size**: Modify `Game.cpp` constructor
- **Spawn rates**: Adjust timing values in `Game.cpp`
- **Player speed**: Modify values in `Player.cpp`
- **Graphics settings**: Update `ResourceManager.cpp`

## 🧪 Testing

### Performance Testing

The game includes built-in performance monitoring:

```cpp
// Enable performance monitoring in debug builds
#ifdef DEBUG
    m_performanceMonitor.setEnabled(true);
#endif
```

### Manual Testing

1. **Basic Functionality**:
   ```bash
   ./build_unified.sh
   ./build/SpaceDefender
   ```

2. **Xcode Integration**:
   ```bash
   open SpaceDefender.xcodeproj
   # Press Cmd+R
   ```

3. **Performance Test**:
   - Run the game and observe FPS counter
   - Check particle system performance
   - Monitor memory usage during gameplay

## 📚 Dependencies

### Runtime Dependencies
- **SFML 2.6+**: Graphics, audio, and window management
- **macOS 11.0+**: Target platform

### Development Dependencies
- **CMake 3.20+**: Build system
- **Xcode 12+**: IDE and compiler
- **Git**: Version control

### Automatic Dependency Management

The build system automatically handles:
- SFML installation via Homebrew
- CMake configuration
- Xcode project generation
- Library linking and path configuration

## 🤝 Contributing

1. **Fork the Repository**
2. **Create a Feature Branch**: `git checkout -b feature/amazing-feature`
3. **Make Changes**: Follow the existing code style
4. **Test Thoroughly**: Ensure builds work in both CMake and Xcode
5. **Update Documentation**: Add relevant documentation
6. **Commit Changes**: `git commit -m 'Add amazing feature'`
7. **Push to Branch**: `git push origin feature/amazing-feature`
8. **Open a Pull Request**

### Code Style Guidelines

- Use modern C++20 features appropriately
- Follow RAII principles
- Use smart pointers for memory management
- Maintain consistent indentation (4 spaces)
- Add comments for complex algorithms
- Keep functions focused and small

## 🐛 Troubleshooting

### Common Issues

#### "SFML not found"
```bash
# Install SFML via Homebrew
brew install sfml

# Force regenerate build files
./sync_build_systems.sh --force
```

#### "Game window doesn't open in Xcode"
```bash
# This is automatically fixed by the sync system
./sync_build_systems.sh --force
```

#### "Build fails"
```bash
# Check sync logs
tail -f .sync_log

# Clean and rebuild
rm -rf build/ build_xcode/
./sync_build_systems.sh
```

#### "Permission denied" errors
```bash
# Make scripts executable
chmod +x *.sh
```

### Debug Information

Enable detailed logging:

```bash
# View sync logs
tail -f .sync_log

# Check build system status
./sync_build_systems.sh --status

# Verbose build
./build_unified.sh --verbose
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **SFML Team**: For the excellent multimedia library
- **Modern C++**: For providing powerful language features
- **Open Source Community**: For inspiration and best practices

## 📞 Support

If you encounter any issues or have questions:

1. Check the [troubleshooting section](#-troubleshooting)
2. Review the [detailed build documentation](README_BUILD_SYNC.md)
3. Check existing [GitHub Issues](../../issues)
4. Create a [new issue](../../issues/new) if needed

---

**Happy Gaming!** 🎮 Built with ❤️ using modern C++20 and SFML.