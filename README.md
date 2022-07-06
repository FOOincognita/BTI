# Download Code
  1. Click the down-arrow on the green "code" button
  2. Download zip
  3. Place .zip on Desktop
  4. Extract .zip to folder on Desktop

# Compilation
  1. Open PowerShell in Administrator mode
  2. Type: ```sudo apt update``` 
      - Hit enter
  3. Type: ```sudo apt install gcc```
      - Hit enter
  4. Type: ```wsl``` then hit enter
    - Hit enter
  5. Navigate to the extracted folder within wsl
  6. Type: ```gcc -c main.c -o main.o```
    - Hit enter
  7. Type: ```gcc -o main main.o -lbtiCard -L.```
    - Hit enter
  8. Type: ```LD_LIBRARY_PATH="$(pwd)" ./main```
    - Hit enter
  
**You should see magenta & red/green messages begin to appear. 
If the terminal window hangs, that means it's attempting to connect to the WebFB. 
Once data starts spamming on the terminal, check for relevance by pressing any key. 
If that doesn't stop the proccess, press CTRL+C. 
Now you can scroll through the displayed data to check for relevance.**
  
