#!/bin/bash
# Script to help add screenshots to the SpaceDefender repository

echo "🚀 Space Defender - Screenshot Upload Helper"
echo "==========================================="
echo ""

# Check if screenshots directory exists
if [ ! -d "screenshots" ]; then
    echo "Creating screenshots directory..."
    mkdir -p screenshots
fi

echo "Please save your screenshots with these exact names in the screenshots/ folder:"
echo ""
echo "📱 Required screenshots:"
echo "  • screenshots/main_menu.png  - Main menu screenshot"
echo "  • screenshots/gameplay.png   - Gameplay screenshot" 
echo "  • screenshots/game_over.png  - Game over screenshot"
echo ""

# Check which screenshots exist
echo "🔍 Checking for screenshots..."
missing_files=()

if [ ! -f "screenshots/main_menu.png" ]; then
    echo "❌ Missing: screenshots/main_menu.png"
    missing_files+="main_menu.png "
else
    echo "✅ Found: screenshots/main_menu.png"
fi

if [ ! -f "screenshots/gameplay.png" ]; then
    echo "❌ Missing: screenshots/gameplay.png"
    missing_files+="gameplay.png "
else
    echo "✅ Found: screenshots/gameplay.png"
fi

if [ ! -f "screenshots/game_over.png" ]; then
    echo "❌ Missing: screenshots/game_over.png"
    missing_files+="game_over.png "
else
    echo "✅ Found: screenshots/game_over.png"
fi

echo ""

if [ ${#missing_files[@]} -eq 0 ]; then
    echo "🎉 All screenshots found! Adding to repository..."
    
    # Add screenshots to git
    git add screenshots/
    git add README.md
    
    # Commit the changes
    git commit -m "Add game screenshots to README

- Added main menu screenshot showing space-themed interface
- Added gameplay screenshot with particle effects and combat
- Added game over screen with score display
- Updated README with proper screenshots section"
    
    # Push to repository
    echo "📤 Pushing to GitHub..."
    git push origin main
    
    echo ""
    echo "✅ Screenshots successfully uploaded to repository!"
    echo "🌐 Your README now includes beautiful game screenshots!"
    
else
    echo "⚠️  Please add the missing screenshot files first, then run this script again."
    echo ""
    echo "💡 Tip: You can drag and drop the screenshot files into the screenshots/ folder"
    echo "    or use the following commands after saving your screenshots:"
    echo ""
    echo "    cp /path/to/your/main_menu_screenshot.png screenshots/main_menu.png"
    echo "    cp /path/to/your/gameplay_screenshot.png screenshots/gameplay.png" 
    echo "    cp /path/to/your/game_over_screenshot.png screenshots/game_over.png"
    echo ""
    echo "    Then run: ./add_screenshots.sh"
fi

echo ""
echo "📚 The README has been updated to reference the screenshots."
echo "   Once you add the image files, they will display automatically!"