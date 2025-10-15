# Music Manager

Music Manager is a lightweight, interactive MP3 file manager with a GUI frontend built using **Raylib** and **Clay UI**.  

It is designed to help DJs organize their music collection efficiently and prepare their files for **Serato** smart crates.  

This has only been tested on an ARM mac but should in theory work on Linux as well (let me know)

#### **Note: this program edits the metadata of ID3v2.3 and ID3v2.4 mp3 files and will overwrite any current comments in your mp3 files.**
---

## Features

- **Scan and display MP3 files**  
  Opens a directory called `mp3` (can be changed in the code) on the user's desktop, scans for all MP3 files contained within, and displays them in an interactive frontend.

- **Add Tags**  
  A button labeled **"Add Tags"** automatically adds a comment to each MP3 file containing the file path relative to the root `mp3` directory.  
  This is compatible with Serato smart crates for automatic sorting.

- **Planned Features**  
  - Edit metadata such as **title** and **artist**.
  - Remove metadata added by Serato to prepare files for re-analysis.

- **Issues**  
  - Does not handle foreign languages well.
  - Could be more preformant.
  - Wierd bugs where strings act very wrong randomly.

---

## Installation

1. Clone the repository:

```bash
cd Desktop
git clone https://github.com/tateanders/Music-Manager.git
cd Music-Manager
```

2. Build the executable:

```bash
mkdir build
cd build
cmake ..
make
```

3. Run:

```bash
./lib
```

4. Decompile:

```bash
cmake clean-all
```

---
acknowledgements:
  - name: Clay UI  
    url: https://github.com/nicbarker/clay.git 
    reason: "Lightweight and flexible UI components"
  - name: Raylib  
    url: https://github.com/raysan5/raylib.git
    reason: "Providing a simple and powerful graphics library to build the GUI frontend"
---