# Image-manipulation
**CSE-1101L final project**
**This is an image manipulation software**

#Features :
- Open & Save BMP: Supports loading and saving 24-bit uncompressed .bmp files via IUP file dialogs.
- Grayscale Conversion: Converts color images to grayscale using weighted RGB intensity (0.299R + 0.587G + 0.114B).
- Brightness Adjustment: Adjusts image brightness with user inputs bounded between -255 and 255.
- Invert Color: Inverts RGB components (255 - RGB) to produce a negative effect.
- Flip Operations: Supports Horizontal (left-right) and Vertical (top-bottom) mirroring.
- Rotation: Rotates the image 90 degrees clockwise with dynamic dimension recalculation.
- Crop Image: Extracts user-defined rectangular regions with boundary validation.
- Blur: Applies a 3x3 neighborhood spatial averaging filter.
- 1-Level Undo: Preserves previous image state for restoration.
- Image sharpening: Applies  3 × 3 neighborhood method, multiply each neighboring pixel value by the corresponding kernel value, and                       add the results to obtain  new pixel value. The resulting RGB values range between 0 to 255.

#Required File Structure :

Ensure the folder layout strictly matches the following structure so Makefile can locate the library dependencies:
‘''
Project_Folder/
├── main.c
├── gui.c / gui.h
├── image.c / image.h
├── process.c / process.h
├── Makefile
└── iup/
    ├── include/
    │   └── (IUP header files)
    └── lib/
        ├── Mac/
        │   └── libiup.a
        └── Linux/
            └── libiup.a
‘''
 #Supported Image Format:   
  This program only supports 24-bit uncompressed standard RGB file. PNG,JPEG,GIF are not supported here.

 #Prerequisites & Dependencies :

 1. macOS

 - Install Homebrew (if it’s not installed):
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
 - Install Required Build Tools & Packages:
  brew install gtk+3 pkg-config gcc make


 2. Linux

 - Ubuntu / Debian / Linux Mint

  sudo apt update
  sudo apt install -y build-essential libgtk-3-dev pkg-config
 - Fedora / RHEL:

  sudo dnf install -y gcc make gtk3-devel pkgconfig
 - Arch Linux:

  sudo pacman -S --noconfirm base-devel gtk3 pkgconf

 #IUP Library Setup (iup.zip) :

  Unzip iup.zip inside your main project folder.
  Nested Folder Fix: Unzipping sometimes creates a nested folder structure like iup/iup/. If this happens:
  Open the inner iup folder, select all files and folders inside it, and Cut (Ctrl+X / Cmd+X).
  Go back one level to the outer iup folder and Paste (Ctrl+V / Cmd+V).
  Delete the remaining empty inner folder and iup.zip.
  Ensure the final relative path is iup/include and iup/lib.

 #Manual Execution Instructions :

  Download and extract the project .zip file manually.
  Complete the IUP Library Setup step above.
  Open your terminal and navigate into the extracted project folder:
  cd path/to/project_folder
  Compile the project using make:
  make
 #Run the executable :
 
  ./image_editor   
  Clean up temporary build files (Optional):
  make clean

 #Notes :

 
  Here the given iup.zip file is developed only for macOS. For other operating system, you have to download the specialized iup file     for your specific operating system.
