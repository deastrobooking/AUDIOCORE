# AUDIOCORE Repository Setup Script
param(
    [string]$RepoUrl = "https://github.com/deastrobooking/AUDIOCORE.git",
    [string]$DefaultBranch = "main"
)

Write-Host "Setting up AUDIOCORE repository..." -ForegroundColor Green

# Check if we're already in a git repository
if (-not (Test-Path ".git")) {
    Write-Host "Initializing Git repository..." -ForegroundColor Yellow
    git init
    git checkout -b $DefaultBranch
} else {
    Write-Host "Git repository already exists" -ForegroundColor Yellow
    # Ensure we're on the main branch
    git checkout $DefaultBranch 2>$null
    if ($LASTEXITCODE -ne 0) {
        git checkout -b $DefaultBranch
    }
}

# Ensure we have the branch set correctly
git branch -M $DefaultBranch

Write-Host "Project structure already created with component organization:" -ForegroundColor Cyan
Write-Host "  ? LIVE-SYSTEMS/components/ (Diodes, Resistors, Filters, Distortions, Amplifiers, Oscillators)" -ForegroundColor Green
Write-Host "  ? LIVE-SYSTEMS/audio_effects/ (Delays, Reverbs, Chorus, Compressor)" -ForegroundColor Green  
Write-Host "  ? LIVE-SYSTEMS/synths/ (Analog, Digital, Wavetable)" -ForegroundColor Green
Write-Host "  ? GitHub Actions workflows (.github/workflows/)" -ForegroundColor Green
Write-Host "  ? Visual Studio .gitignore" -ForegroundColor Green

# Add all files to git
Write-Host "Adding files to git..." -ForegroundColor Yellow
git add .

# Check if there are any changes to commit
$gitStatus = git status --porcelain
if ($gitStatus) {
    # Create initial commit
    git commit -m "feat: complete AUDIOCORE repository setup with organized component structure

- Created comprehensive directory structure for components, audio effects, and synths
- Added GitHub Actions CI/CD workflows for Windows, macOS, and Linux
- Implemented modular CMake architecture for easy component development
- Added Visual Studio .gitignore for C++/JUCE projects
- Created DistortionBase component as example implementation
- Set up branch strategy for core library and product development
- Configured for CMake 3.31.6 with Ninja generator and C++20"

    Write-Host "Initial commit created" -ForegroundColor Green
} else {
    Write-Host "No changes to commit" -ForegroundColor Yellow
}

# Remove existing origin if it exists
$remoteExists = git remote | Where-Object { $_ -eq "origin" }
if ($remoteExists) {
    Write-Host "Removing existing origin remote..." -ForegroundColor Yellow
    git remote remove origin
}

# Add remote repository
Write-Host "Adding remote origin: $RepoUrl" -ForegroundColor Green
git remote add origin $RepoUrl

# Push to remote
Write-Host "Pushing to remote repository..." -ForegroundColor Yellow
try {
    git push -u origin $DefaultBranch
    Write-Host "Successfully pushed to remote repository!" -ForegroundColor Green
} catch {
    Write-Host "Push failed. You may need to authenticate with GitHub first." -ForegroundColor Red
    Write-Host "Try running: gh auth login" -ForegroundColor Yellow
    Write-Host "Or check your GitHub credentials and repository permissions." -ForegroundColor Yellow
}

# Create and push development branches for organized workflow
Write-Host "Creating development branches..." -ForegroundColor Yellow
$branches = @(
    @{name="develop"; description="Integration branch for new features"},
    @{name="core"; description="Core framework development"},
    @{name="product-distortion"; description="Distortion plugin product line"},
    @{name="product-synth"; description="Synthesizer plugin product line"},
    @{name="feature/component-development"; description="Component library development"}
)

foreach ($branch in $branches) {
    Write-Host "Creating branch: $($branch.name)" -ForegroundColor Cyan
    git checkout -b $branch.name 2>$null
    if ($LASTEXITCODE -eq 0) {
        try {
            git push -u origin $branch.name
            Write-Host "  ? Created and pushed: $($branch.name) - $($branch.description)" -ForegroundColor Green
        } catch {
            Write-Host "  ? Failed to push: $($branch.name)" -ForegroundColor Red
        }
    } else {
        Write-Host "  ? Branch already exists: $($branch.name)" -ForegroundColor Yellow
    }
}

# Return to main branch
git checkout $DefaultBranch

Write-Host "`n?? Repository setup complete!" -ForegroundColor Green
Write-Host "`n?? Project Structure:" -ForegroundColor Yellow
Write-Host "   • Components: Circuit modeling (Diodes, Resistors, Filters, Distortions, Amplifiers, Oscillators)" -ForegroundColor White
Write-Host "   • Audio Effects: Professional effects (Delays, Reverbs, Chorus, Compressor)" -ForegroundColor White
Write-Host "   • Synthesizers: Synthesis engines (Analog, Digital, Wavetable)" -ForegroundColor White
Write-Host "   • GitHub Actions: CI/CD workflows for all platforms" -ForegroundColor White

Write-Host "`n?? Branches created:" -ForegroundColor Yellow
Write-Host "   • main: Stable releases" -ForegroundColor White
Write-Host "   • develop: Integration branch" -ForegroundColor White
Write-Host "   • core: Core framework development" -ForegroundColor White
Write-Host "   • product-distortion: Distortion product line" -ForegroundColor White
Write-Host "   • product-synth: Synthesizer product line" -ForegroundColor White
Write-Host "   • feature/component-development: Component development" -ForegroundColor White

Write-Host "`n?? Next Steps:" -ForegroundColor Yellow
Write-Host "   1. Build the project: cmake -B build -G Ninja && cmake --build build" -ForegroundColor White
Write-Host "   2. Create your first plugin in the plugins/ directory" -ForegroundColor White
Write-Host "   3. Develop components in LIVE-SYSTEMS/components/" -ForegroundColor White
Write-Host "   4. Use branches for product-specific development" -ForegroundColor White

Write-Host "`n?? Remote: $RepoUrl" -ForegroundColor Cyan