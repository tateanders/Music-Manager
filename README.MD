# Music Manager

Music Manager is a lightweight, interactive MP3 file manager with a GUI frontend built using **Raylib** and **Clay UI**.  
It is designed to help DJs and music enthusiasts organize their music collection efficiently and prepare their files for **Serato** smart crates.

**This has only been tested on an ARM mac but should in theory work on Linux as well.**

---

## Features

- **Scan and display MP3 files**  
  Opens a directory called `mp3` (can be changed in the code) on the user's desktop, scans for all MP3 files contained within, and displays them in an interactive frontend.

- **Add Tags**  
  A button labeled **"Add Tags"** automatically adds a comment to each MP3 file containing the file path relative to the root `mp3` directory.  
  This is compatible with Serato smart crates for automatic sorting.

- **Planned Features**  
  - Edit metadata such as **title** and **artist**.
  - Find and remove duplicate files.
  - Remove metadata added by Serato to prepare files for re-analysis.

---

## Installation

1. Clone the repository:

```bash
cd Desktop
git clone <your-repo-url>
cd <your-repo-name>
mkdir build
cd build
cmake ..
make
./lib
cmake clean-all
```

---
acknowledgements:
  - name: Clay UI
    url: https://github.com/hughsk/clay
    reason: "Lightweight and flexible UI components"
  - name: Raylib
    url: https://www.raylib.com/
    reason: "Providing a simple and powerful graphics library to build the GUI frontend"
---