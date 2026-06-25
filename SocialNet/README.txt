SOCIALNET Project
-----------------

Author: Pushkar Manoj Gattani
Entry Number: 2024EE10035
Course: COL106
Date: 05-11-2025

Description:
This is a simple social network simulation that supports:
- Adding users (ADD_USER)
- Adding friends (ADD_FRIEND)
- Listing friends (LIST_FRIENDS)
- Adding posts (ADD_POST)
- Viewing latest posts (OUTPUT_POSTS)
- Calculating degrees of separation (DEGREES_OF_SEPARATION)
- Suggesting friends (SUGGEST_FRIENDS)
- Exiting the program (EXIT)

---

Compilation Instructions:

Linux / macOS / WSL:
1. Open terminal in the SOCIALNET folder.
2. Run:
   ./compile.sh
   This will create an executable named 'SocialNet'.
3. Run:
   ./SocialNet

Windows (Command Prompt / PowerShell):
1. Open Command Prompt in the SOCIALNET folder.
2. Compile:
   g++ -std=c++17 -O2 -o SocialNet.exe src\main.cpp
3. Run interactively:
   SocialNet.exe
4. Run with a testcases file (sample_input.txt):
   SocialNet.exe < sample_input.txt
   Note: In PowerShell, you can use:
   Get-Content sample_input.txt | .\SocialNet.exe
5. Optional: You can also double-click compile.bat to compile and automatically run sample_input.txt.
6. SocialNet.exe is included for convenience on Windows. If using Linux/macOS/WSL, compile using ./compile.sh.

---

Running the Program:
- To run interactively, type commands one by one after launching the program.
- To run with a provided input file, use the redirection commands shown above.
- Quotes are NOT required around post content for ADD_POST commands.
- Commands must follow the exact syntax.
- OUTPUT_POSTS N parameter: -1 or any number greater than total posts will print all posts.

---

Assumptions:
- Each post has a unique timestamp, so even if two posts have the same content, they are considered different posts.
- Posts can have identical content; the program does not treat them as duplicates.
- Empty posts are not allowed; every ADD_POST command is assumed to include some content.
- Username comparisons are case-insensitive.
- All commands are case-sensitive and must match exactly as specified.
- Posts are stored and displayed in reverse chronological order (newest first).

---

Example Commands (from sample_input.txt):
ADD_USER Alice
ADD_USER Bob
ADD_FRIEND Alice Bob
ADD_POST Alice Hello world
OUTPUT_POSTS Alice 5
LIST_FRIENDS Alice
DEGREES_OF_SEPARATION Alice Bob
SUGGEST_FRIENDS Alice 3
EXIT
