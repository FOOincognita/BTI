# Download Code
  1. Click the down-arrow on the green "code" button
  2. Download zip
  3. Place .zip on Desktop
  4. Extract .zip to folder on Desktop

# Compilation
  1. Open PowerShell in Administrator mode
  2. Type: ```sudo apt update```
  3. Hit enter
  4. Type: ```sudo apt install gcc```
  5. Hit enter
  6. Type: ```wsl```
  7. Hit enter
  8. Navigate to the extracted folder within wsl
  9. Type: ```gcc -c main.c -o main.o```
  10. Hit enter
  11. Type: ```gcc -o main main.o -lbtiCard -L.```
  12. Hit enter
  13. Type: ```LD_LIBRARY_PATH="$(pwd)" ./main```
  14. Hit enter
  
**You should see magenta & red/green messages begin to appear. 
If the terminal window hangs, that means it's attempting to connect to the WebFB. 
Once data starts spamming on the terminal, check for relevance by pressing any key. 
If that doesn't stop the proccess, press CTRL+C. 
Now you can scroll through the displayed data to check for relevance.**
  
