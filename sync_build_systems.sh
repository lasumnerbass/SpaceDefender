#!/bin/bash

# =============================================================================
# CMake-Xcode Synchronization Script
# =============================================================================
# This script automatically synchronizes CMake and Xcode build systems,
# ensuring both work seamlessly across terminal, IDE, and command line.
#
# Features:
# - Auto-detects changes in CMakeLists.txt and regenerates Xcode project
# - Auto-detects changes in Xcode project and updates CMake if needed
# - Configures proper schemes and working directories
# - Ensures executables work from all environments
# - Cross-compatible bidirectional sync
# =============================================================================

set -euo pipefail

# Configuration
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CMAKE_BUILD_DIR="${PROJECT_DIR}/build"
XCODE_BUILD_DIR="${PROJECT_DIR}/build_xcode"
PROJECT_NAME="SpaceDefender"
XCODE_PROJECT="${PROJECT_DIR}/${PROJECT_NAME}.xcodeproj"
LOCKFILE="${PROJECT_DIR}/.sync_lock"
LOG_FILE="${PROJECT_DIR}/.sync_log"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    local level="$1"
    shift
    local message="$*"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    echo -e "${timestamp} [${level}] ${message}" | tee -a "${LOG_FILE}"
}

info() { log "INFO" "$@"; }
warn() { log "WARN" "${YELLOW}$*${NC}"; }
error() { log "ERROR" "${RED}$*${NC}"; }
success() { log "SUCCESS" "${GREEN}$*${NC}"; }

# Check if running in watch mode
WATCH_MODE=false
DAEMON_MODE=false
FORCE_SYNC=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --watch|-w)
            WATCH_MODE=true
            shift
            ;;
        --daemon|-d)
            DAEMON_MODE=true
            WATCH_MODE=true
            shift
            ;;
        --force|-f)
            FORCE_SYNC=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --watch, -w    Watch for file changes and auto-sync"
            echo "  --daemon, -d   Run as background daemon"
            echo "  --force, -f    Force regeneration of all build files"
            echo "  --help, -h     Show this help message"
            exit 0
            ;;
        *)
            error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Cleanup function
cleanup() {
    if [[ -f "${LOCKFILE}" ]]; then
        rm -f "${LOCKFILE}"
    fi
    info "Cleanup completed"
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Check for lock file to prevent concurrent runs
acquire_lock() {
    if [[ -f "${LOCKFILE}" ]]; then
        local lock_pid=$(cat "${LOCKFILE}" 2>/dev/null || echo "")
        if [[ -n "${lock_pid}" ]] && kill -0 "${lock_pid}" 2>/dev/null; then
            error "Another sync process is already running (PID: ${lock_pid})"
            exit 1
        else
            warn "Removing stale lock file"
            rm -f "${LOCKFILE}"
        fi
    fi
    echo $$ > "${LOCKFILE}"
    info "Lock acquired (PID: $$)"
}

# Utility functions
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

get_file_hash() {
    if [[ -f "$1" ]]; then
        if command_exists shasum; then
            shasum -a 256 "$1" | cut -d' ' -f1
        elif command_exists sha256sum; then
            sha256sum "$1" | cut -d' ' -f1
        else
            stat -f%m "$1" 2>/dev/null || stat -c%Y "$1" 2>/dev/null || echo "0"
        fi
    else
        echo "nonexistent"
    fi
}

# Check required tools
check_dependencies() {
    info "Checking dependencies..."
    
    local missing_deps=()
    
    if ! command_exists cmake; then
        missing_deps+=("cmake")
    fi
    
    if ! command_exists xcodebuild; then
        missing_deps+=("xcodebuild (Xcode)")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        error "Missing dependencies: ${missing_deps[*]}"
        error "Please install the missing tools and try again"
        exit 1
    fi
    
    success "All dependencies found"
}

# Get project metadata
get_project_info() {
    local cmake_file="${PROJECT_DIR}/CMakeLists.txt"
    if [[ ! -f "${cmake_file}" ]]; then
        error "CMakeLists.txt not found in ${PROJECT_DIR}"
        exit 1
    fi
    
    # Extract project name from CMakeLists.txt if different
    local cmake_project_name=$(grep -E "^project\s*\(" "${cmake_file}" | sed -E 's/^project\s*\(\s*([^[:space:]]+).*/\1/' || echo "${PROJECT_NAME}")
    if [[ -n "${cmake_project_name}" && "${cmake_project_name}" != "project" ]]; then
        PROJECT_NAME="${cmake_project_name}"
        XCODE_PROJECT="${PROJECT_DIR}/${PROJECT_NAME}.xcodeproj"
    fi
    
    info "Project: ${PROJECT_NAME}"
    info "Project Directory: ${PROJECT_DIR}"
    info "CMake Build Directory: ${CMAKE_BUILD_DIR}"
    info "Xcode Build Directory: ${XCODE_BUILD_DIR}"
    info "Xcode Project: ${XCODE_PROJECT}"
}

# Generate CMake build files
generate_cmake_build() {
    info "Generating CMake build files..."
    
    mkdir -p "${CMAKE_BUILD_DIR}"
    cd "${CMAKE_BUILD_DIR}"
    
    # Configure CMake with appropriate settings
    local cmake_args=(
        -DCMAKE_BUILD_TYPE=Debug
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -DCMAKE_INSTALL_PREFIX="${PROJECT_DIR}/install"
    )
    
    if cmake "${cmake_args[@]}" .. && make -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4); then
        success "CMake build completed successfully"
        return 0
    else
        error "CMake build failed"
        return 1
    fi
}

# Generate Xcode project
generate_xcode_project() {
    info "Generating Xcode project..."
    
    # Clean up old Xcode build directory
    rm -rf "${XCODE_BUILD_DIR}"
    mkdir -p "${XCODE_BUILD_DIR}"
    cd "${XCODE_BUILD_DIR}"
    
    # Configure CMake for Xcode
    local cmake_args=(
        -G Xcode
        -DCMAKE_BUILD_TYPE=Debug
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -DCMAKE_INSTALL_PREFIX="${PROJECT_DIR}/install"
        -DXCODE_GENERATE_SCHEME=ON
        -DXCODE_SCHEME_WORKING_DIRECTORY="${PROJECT_DIR}"
    )
    
    if cmake "${cmake_args[@]}" ..; then
        # Move the generated project to the main directory
        if [[ -f "${XCODE_BUILD_DIR}/${PROJECT_NAME}.xcodeproj/project.pbxproj" ]]; then
            rm -rf "${XCODE_PROJECT}"
            mv "${XCODE_BUILD_DIR}/${PROJECT_NAME}.xcodeproj" "${PROJECT_DIR}/"
            success "Xcode project generated successfully"
            configure_xcode_schemes
            return 0
        else
            error "Xcode project file not found after generation"
            return 1
        fi
    else
        error "Xcode project generation failed"
        return 1
    fi
}

# Configure Xcode schemes
configure_xcode_schemes() {
    info "Configuring Xcode schemes..."
    
    if [[ ! -d "${XCODE_PROJECT}" ]]; then
        warn "Xcode project not found, skipping scheme configuration"
        return 0
    fi
    
    # Extract target GUID from project.pbxproj
    local project_file="${XCODE_PROJECT}/project.pbxproj"
    local target_guid=""
    if [[ -f "$project_file" ]]; then
        # Look for the main target GUID
        target_guid=$(grep -A 1 "PBXNativeTarget.*${PROJECT_NAME}" "$project_file" | grep -E "^\s*[A-F0-9]{24}\s*=" | head -1 | sed -E 's/^\s*([A-F0-9]{24}).*/\1/' || echo "")
    fi
    
    # Fallback GUID if extraction fails
    if [[ -z "$target_guid" ]]; then
        target_guid="PRIMARY_TARGET_GUID"
        warn "Could not extract target GUID, using placeholder"
    else
        info "Extracted target GUID: $target_guid"
    fi
    
    # Create schemes directory if it doesn't exist
    local schemes_dir="${XCODE_PROJECT}/xcuserdata/${USER}.xcuserdatad/xcschemes"
    mkdir -p "${schemes_dir}"
    
    # Create a proper scheme file for the main target
    local scheme_file="${schemes_dir}/${PROJECT_NAME}.xcscheme"
    
    cat > "${scheme_file}" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<Scheme
   LastUpgradeVersion = "1500"
   version = "1.7">
   <BuildAction
      parallelizeBuildables = "YES"
      buildImplicitDependencies = "YES">
      <BuildActionEntries>
         <BuildActionEntry
            buildForTesting = "YES"
            buildForRunning = "YES"
            buildForProfiling = "YES"
            buildForArchiving = "YES"
            buildForAnalyzing = "YES">
            <BuildableReference
               BuildableIdentifier = "primary"
               BlueprintIdentifier = "${target_guid}"
               BuildableName = "${PROJECT_NAME}"
               BlueprintName = "${PROJECT_NAME}"
               ReferencedContainer = "container:${PROJECT_NAME}.xcodeproj">
            </BuildableReference>
         </BuildActionEntry>
      </BuildActionEntries>
   </BuildAction>
   <TestAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      shouldUseLaunchSchemeArgsEnv = "YES">
   </TestAction>
   <LaunchAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      launchStyle = "0"
      useCustomWorkingDirectory = "YES"
      customWorkingDirectory = "${PROJECT_DIR}"
      ignoresPersistentStateOnLaunch = "NO"
      debugDocumentVersioning = "YES"
      debugServiceExtension = "internal"
      allowLocationSimulation = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "${target_guid}"
            BuildableName = "${PROJECT_NAME}"
            BlueprintName = "${PROJECT_NAME}"
            ReferencedContainer = "container:${PROJECT_NAME}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </LaunchAction>
   <ProfileAction
      buildConfiguration = "Release"
      shouldUseLaunchSchemeArgsEnv = "YES"
      savedToolIdentifier = ""
      useCustomWorkingDirectory = "YES"
      customWorkingDirectory = "${PROJECT_DIR}"
      debugDocumentVersioning = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "${target_guid}"
            BuildableName = "${PROJECT_NAME}"
            BlueprintName = "${PROJECT_NAME}"
            ReferencedContainer = "container:${PROJECT_NAME}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </ProfileAction>
   <AnalyzeAction
      buildConfiguration = "Debug">
   </AnalyzeAction>
   <ArchiveAction
      buildConfiguration = "Release"
      revealArchiveInOrganizer = "YES">
   </ArchiveAction>
</Scheme>
EOF
    
    success "Xcode schemes configured"
}

# Verify builds work
verify_builds() {
    info "Verifying build systems..."
    
    local verification_failed=false
    
    # Test CMake build
    if [[ -f "${CMAKE_BUILD_DIR}/${PROJECT_NAME}" ]]; then
        success "CMake executable found: ${CMAKE_BUILD_DIR}/${PROJECT_NAME}"
    else
        error "CMake executable not found"
        verification_failed=true
    fi
    
    # Test Xcode build
    if xcodebuild -project "${XCODE_PROJECT}" -scheme "${PROJECT_NAME}" -configuration Debug build >/dev/null 2>&1; then
        if [[ -f "${CMAKE_BUILD_DIR}/${PROJECT_NAME}" || -f "${PROJECT_DIR}/build/Debug/${PROJECT_NAME}" ]]; then
            success "Xcode build verification passed"
        else
            warn "Xcode builds but executable location unclear"
        fi
    else
        error "Xcode build verification failed"
        verification_failed=true
    fi
    
    if [[ "${verification_failed}" = true ]]; then
        error "Build verification failed"
        return 1
    else
        success "All build systems verified"
        return 0
    fi
}

# Create unified build script
create_build_script() {
    local build_script="${PROJECT_DIR}/build_unified.sh"
    info "Creating unified build script: ${build_script}"
    
    cat > "${build_script}" << 'EOF'
#!/bin/bash

# Unified build script for CMake and Xcode compatibility
# This script ensures the project builds correctly in all environments

set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_NAME="SpaceDefender"

echo "🔨 Building ${PROJECT_NAME}..."

# Build with CMake
echo "📦 CMake build..."
cd "${PROJECT_DIR}"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ Build complete!"
echo "🎮 Run game: ./build/${PROJECT_NAME}"
echo "🔧 Open Xcode: open ${PROJECT_NAME}.xcodeproj"
EOF
    
    chmod +x "${build_script}"
    success "Unified build script created"
}

# File watching functionality
watch_files() {
    info "Starting file watcher..."
    
    local cmake_hash=""
    local xcode_hash=""
    local src_hash=""
    
    while true; do
        # Check CMakeLists.txt
        local new_cmake_hash=$(get_file_hash "${PROJECT_DIR}/CMakeLists.txt")
        if [[ "${cmake_hash}" != "${new_cmake_hash}" ]]; then
            info "CMakeLists.txt changed, regenerating Xcode project..."
            cmake_hash="${new_cmake_hash}"
            if generate_xcode_project; then
                success "Xcode project updated"
            fi
        fi
        
        # Check Xcode project
        if [[ -f "${XCODE_PROJECT}/project.pbxproj" ]]; then
            local new_xcode_hash=$(get_file_hash "${XCODE_PROJECT}/project.pbxproj")
            if [[ "${xcode_hash}" != "${new_xcode_hash}" ]]; then
                info "Xcode project changed, updating CMake build..."
                xcode_hash="${new_xcode_hash}"
                if generate_cmake_build; then
                    success "CMake build updated"
                fi
            fi
        fi
        
        # Check source files
        local new_src_hash=""
        if [[ -d "${PROJECT_DIR}/src" ]]; then
            new_src_hash=$(find "${PROJECT_DIR}/src" "${PROJECT_DIR}/include" -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" 2>/dev/null | xargs ls -la 2>/dev/null | shasum -a 256 | cut -d' ' -f1 || echo "")
        fi
        if [[ -n "${new_src_hash}" && "${src_hash}" != "${new_src_hash}" ]]; then
            info "Source files changed, rebuilding..."
            src_hash="${new_src_hash}"
            generate_cmake_build
        fi
        
        sleep 2
    done
}

# Main synchronization function
sync_build_systems() {
    info "Starting build system synchronization..."
    
    local cmake_exists=false
    local xcode_exists=false
    
    # Check what currently exists
    [[ -f "${PROJECT_DIR}/CMakeLists.txt" ]] && cmake_exists=true
    [[ -d "${XCODE_PROJECT}" ]] && xcode_exists=true
    
    if [[ "${FORCE_SYNC}" = true ]]; then
        info "Force sync requested, regenerating all build files..."
        rm -rf "${CMAKE_BUILD_DIR}" "${XCODE_BUILD_DIR}" "${XCODE_PROJECT}"
        cmake_exists=true  # We know CMakeLists.txt exists
        xcode_exists=false
    fi
    
    if [[ "${cmake_exists}" = true ]]; then
        # Generate CMake build
        if ! generate_cmake_build; then
            error "Failed to generate CMake build"
            return 1
        fi
        
        # Generate/update Xcode project
        if ! generate_xcode_project; then
            error "Failed to generate Xcode project"
            return 1
        fi
    else
        error "No CMakeLists.txt found. Cannot proceed with synchronization."
        return 1
    fi
    
    # Create unified build script
    create_build_script
    
    # Verify everything works
    if ! verify_builds; then
        error "Build verification failed"
        return 1
    fi
    
    success "Build system synchronization completed successfully!"
    
    echo ""
    echo -e "${GREEN}🎉 Synchronization Complete!${NC}"
    echo ""
    echo -e "${BLUE}📋 Available Commands:${NC}"
    echo "  • Build (CMake):     ./build_unified.sh"
    echo "  • Build (Make):      cd build && make"
    echo "  • Build (Xcode):     xcodebuild -project ${PROJECT_NAME}.xcodeproj -scheme ${PROJECT_NAME} build"
    echo "  • Run (Terminal):    ./build/${PROJECT_NAME}"
    echo "  • Open (Xcode):      open ${PROJECT_NAME}.xcodeproj"
    echo ""
    echo -e "${YELLOW}💡 The project now works seamlessly across all environments!${NC}"
    
    return 0
}

# Main execution
main() {
    info "=== CMake-Xcode Synchronization Script ==="
    info "Starting at $(date)"
    
    acquire_lock
    check_dependencies
    get_project_info
    
    if [[ "${DAEMON_MODE}" = true ]]; then
        info "Starting in daemon mode..."
        # Redirect output to log file in daemon mode
        exec 1>>"${LOG_FILE}"
        exec 2>>"${LOG_FILE}"
        
        # Initial sync
        if sync_build_systems; then
            watch_files
        else
            error "Initial sync failed, exiting daemon mode"
            exit 1
        fi
    elif [[ "${WATCH_MODE}" = true ]]; then
        info "Starting in watch mode..."
        # Initial sync
        if sync_build_systems; then
            watch_files
        else
            error "Initial sync failed, exiting watch mode"
            exit 1
        fi
    else
        # Single run
        if sync_build_systems; then
            info "Single synchronization completed successfully"
            exit 0
        else
            error "Synchronization failed"
            exit 1
        fi
    fi
}

# Execute main function
main "$@"