# CMake-Xcode Build System Synchronization

This project includes an advanced build system synchronization script that ensures seamless compatibility between CMake and Xcode across all development environments.

## 🚀 Quick Start

### One-Time Setup
```bash
# Run the synchronization script to set everything up
./sync_build_systems.sh
```

### Build and Run
```bash
# Use the unified build script (recommended)
./build_unified.sh

# Or build with CMake directly
cd build && make

# Or build with Xcode
xcodebuild -project SpaceDefender.xcodeproj -scheme SpaceDefender build

# Run the game
./build/SpaceDefender
```

## 📋 Available Commands

| Command | Description |
|---------|-------------|
| `./sync_build_systems.sh` | One-time sync of build systems |
| `./sync_build_systems.sh --watch` | Monitor files and auto-sync changes |
| `./sync_build_systems.sh --daemon` | Run as background daemon |
| `./sync_build_systems.sh --force` | Force regeneration of all build files |
| `./build_unified.sh` | Universal build script |
| `./build.sh` | Original CMake build script |
| `open SpaceDefender.xcodeproj` | Open project in Xcode |

## 🔄 Automatic Synchronization Features

### What Gets Synchronized

1. **CMakeLists.txt Changes** → Automatically regenerates Xcode project
2. **Xcode Project Changes** → Updates CMake build configuration
3. **Source File Changes** → Triggers rebuilds in both systems
4. **Build Settings** → Ensures consistent configuration across platforms

### Supported Environments

- ✅ **Terminal Command Line** - Full support
- ✅ **CMake** - Native build system
- ✅ **Xcode IDE** - Proper schemes and working directories
- ✅ **Xcode Terminal** - Command-line builds within Xcode
- ✅ **VS Code** - Works with CMake Tools extension
- ✅ **CLion** - Compatible with CMake integration

## 🛠 Advanced Usage

### Watch Mode (Development)
Monitor files for changes and automatically sync build systems:
```bash
./sync_build_systems.sh --watch
```
This is perfect during development as it keeps both build systems in sync automatically.

### Daemon Mode (Background Service)
Run as a background service that continuously monitors and syncs:
```bash
./sync_build_systems.sh --daemon
```
The daemon logs all activity to `.sync_log` and runs silently in the background.

### Force Sync
If builds are out of sync or you want a clean regeneration:
```bash
./sync_build_systems.sh --force
```

## 🎯 How It Works

### Bidirectional Sync
The script monitors both CMake and Xcode files for changes:

```
CMakeLists.txt ←→ Xcode Project
     ↕                ↕
Source Files  ←→  Build Settings
     ↕                ↕
Build Output  ←→  Executables
```

### Smart Detection
- **File Hashing**: Detects actual content changes, not just timestamps
- **Dependency Tracking**: Understands relationships between files
- **Lock Management**: Prevents conflicts from concurrent runs
- **Error Recovery**: Gracefully handles build failures

### Working Directory Management
The script automatically configures Xcode schemes with proper working directories, ensuring:
- Resource files are found correctly
- Executables run from the right location
- Debug sessions work properly
- All environments behave consistently

## 🏗 Project Structure

After synchronization, your project will have:

```
SpaceDefender/
├── .git/                      # Version control
├── .gitignore                 # Git ignore rules
├── .sync_lock                 # Sync process lock file
├── .sync_log                  # Synchronization logs
├── CMakeLists.txt            # CMake configuration
├── SpaceDefender.xcodeproj/  # Xcode project (auto-generated)
├── build/                    # CMake build output
├── build_xcode/             # Xcode build intermediate files
├── src/                     # Source code
├── include/                 # Header files
├── resources/               # Game assets
├── sync_build_systems.sh    # Main synchronization script
├── build_unified.sh         # Universal build script (auto-generated)
├── build.sh                 # Original build script
└── README_BUILD_SYNC.md     # This file
```

## 🐛 Troubleshooting

### Common Issues

**Build fails in Xcode but works in terminal:**
```bash
# Force regenerate with proper scheme configuration
./sync_build_systems.sh --force
```

**Changes not being detected:**
```bash
# Check if the daemon/watcher is running
ps aux | grep sync_build_systems

# Restart the watcher
./sync_build_systems.sh --watch
```

**Xcode can't find resources:**
- The script automatically configures working directories
- If issues persist, check Xcode scheme settings (Edit Scheme → Run → Options)

**Lock file errors:**
```bash
# Remove stale lock file
rm .sync_lock
```

### Debug Information

Check the sync log for detailed information:
```bash
tail -f .sync_log
```

View current lock status:
```bash
cat .sync_lock  # Shows PID of running sync process
```

## 🔧 Customization

### Modifying Build Settings

Edit the script to customize CMake arguments:
```bash
# In sync_build_systems.sh, modify these arrays:
cmake_args=(
    -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    -DCMAKE_INSTALL_PREFIX="${PROJECT_DIR}/install"
    # Add your custom flags here
)
```

### Adding Custom Targets

The script automatically detects and configures targets defined in CMakeLists.txt.

### Changing Project Structure

If you modify the project structure, update these variables in the script:
- `PROJECT_NAME` - Your project name
- `CMAKE_BUILD_DIR` - CMake build directory
- `XCODE_BUILD_DIR` - Xcode build directory

## 📝 Integration with Development Tools

### VS Code
Add to `.vscode/settings.json`:
```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": false
}
```

### CLion
The project works automatically with CLion's CMake integration.

### Git Integration
Add to `.gitignore`:
```
.sync_lock
.sync_log
build_xcode/build/
```

## ⚡ Performance Tips

1. **Use Watch Mode**: Instead of manual syncing, use `--watch` during development
2. **Background Daemon**: For long development sessions, use `--daemon` mode
3. **Selective Sync**: The script only regenerates what's necessary
4. **Parallel Builds**: Automatically uses all CPU cores for faster compilation

## 🎮 Game-Specific Notes

For the SpaceDefender game:
- Resources are automatically located relative to the project root
- The game window positioning is optimized for development
- All build systems produce the same executable behavior
- Console output is consistent across all environments

---

**Happy coding!** 🚀 The build system sync ensures you can focus on development rather than build configuration issues.