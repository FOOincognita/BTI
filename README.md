# Download Code
  1. Click the down-arrow on the green "code" button
  2. Download zip
  3. Place .zip on Desktop
  4. Extract .zip to folder on Desktop

# Compilation
  1. Open PowerShell in Administrator mode
  2. Type: ```wsl``` 
      - Hit enter
  3. Type: ```sudo apt update``` 
      - Hit enter
  4. Type: ```sudo apt install g++```
      - Hit enter
  5. Navigate to the extracted folder within wsl
  6. Type: ```g++ -c main.cpp -o main.o```
      - Hit enter
  7. Type: ```g++ -o main main.o -lbtiCard -L.```
      - Hit enter
  8. Type: ```LD_LIBRARY_PATH="$(pwd)" ./main```
      - Hit enter
  
# Displayed Text
**Once step 8 is complete, you should see colored text begin to appear in terminal.** 
  - If the terminal window hangs, that means it's attempting to connect to the WebFB. 
  - Magenta text is a status message (where the program is at)
  - Green text is a success message
  - Red text is an error message
  - White text is either errors bult in to the BTI-Driver, or WebFB data
  - Use ```CTRL+C``` to stop the program at any point

  
