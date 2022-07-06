# Install gcc
- Type: sudo apt update
- Type: sudo apt install gcc

# Download
  - Click the down-arrow on the green "code" button
  - Download zip
  - Place .zip on Desktop
  - Extract .zip to folder on Desktop

# Compilation
  - Open PowerShell in Administrator mode
  - Type: '''sudo apt update'''
  - Hit enter
  - Type: sudo apt install gcc
  - Hit enter
  - Type: wsl 
  - Hit enter
  - Navigate to the extracted folder within wsl
  - Type: gcc -c main.c -o main.o
  - Hit enter
  - Type: gcc -o main main.o -lbtiCard -L.
  - Hit enter
  - Type: LD_LIBRARY_PATH="$(pwd)" ./main
  - Hit enter
  
**You should see magenta & red/green messages begin to appear. 
If the terminal window hangs, that means it's attempting to connect to the WebFB. 
Once data starts spamming on the terminal, check for relevance by pressing any key. 
If that doesn't stop the proccess, press CTRL+C. 
Now you can scroll through the displayed data to check for relevance.**
  
