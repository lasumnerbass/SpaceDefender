#!/bin/bash
# Git CLI commands to upload screenshots to SpaceDefender repo

echo "🚀 Space Defender - Git Screenshot Upload"
echo "========================================="
echo ""

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "❌ Error: Not in a git repository"
    exit 1
fi

# Check if screenshots exist
echo "🔍 Checking for screenshot files..."
missing_files=()

if [ ! -f "screenshots/main_menu.png" ]; then
    echo "❌ Missing: screenshots/main_menu.png"
    missing_files+=("main_menu.png")
else
    echo "✅ Found: screenshots/main_menu.png"
fi

if [ ! -f "screenshots/gameplay.png" ]; then
    echo "❌ Missing: screenshots/gameplay.png"
    missing_files+=("gameplay.png")
else
    echo "✅ Found: screenshots/gameplay.png"
fi

if [ ! -f "screenshots/game_over.png" ]; then
    echo "❌ Missing: screenshots/game_over.png"
    missing_files+=("game_over.png")
else
    echo "✅ Found: screenshots/game_over.png"
fi

echo ""

if [ ${#missing_files[@]} -ne 0 ]; then
    echo "⚠️  Please save the missing screenshot files first:"
    echo ""
    echo "📱 Required files in screenshots/ folder:"
    echo "  • screenshots/main_menu.png  - Main menu screenshot"
    echo "  • screenshots/gameplay.png   - Gameplay screenshot"
    echo "  • screenshots/game_over.png  - Game over screenshot"
    echo ""
    echo "💡 Save the screenshots from your conversation, then run this script again."
    exit 1
fi

echo "🎉 All screenshots found! Proceeding with git upload..."
echo ""

# Show current git status
echo "📋 Current git status:"
git status --porcelain

echo ""

# Add screenshots to staging area
echo "📁 Adding screenshots to git staging area..."
git add screenshots/

# Also add README if it was updated
git add README.md

echo ""

# Show what will be committed
echo "📝 Files to be committed:"
git status --cached

echo ""

# Commit the screenshots
echo "💾 Committing screenshots..."
git commit -m "Add game screenshots to README

🎮 Added three game screenshots:
- Main menu: Beautiful space-themed interface with game controls
- Gameplay: Active space combat with particle effects and UI
- Game over: Score display with play again options

📸 Screenshots enhance the README with visual game preview
🔗 Images are properly linked in the README screenshots section"

echo ""

# Push to remote repository
echo "📤 Pushing to GitHub repository..."
git push origin main

echo ""
echo "✅ Screenshots successfully uploaded to SpaceDefender repository!"
echo "🌐 Your README now displays the game screenshots on GitHub!"
echo ""
echo "🔗 View your updated repository at: https://github.com/lasumnerbass/SpaceDefender"