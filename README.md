# ppcd
*This fork of PPCD specializes on extended features that improve the reverse-engineering-experience overall.
The main focus are game consoles' CPUs such as the Gekko (Nintendo GameCube), Broadway (Wii) and Espresso (Wii U), but general improvements
and bug-fixes are looked forward to as well.*

# Implemented features:
 * Counting the number of different branches in the code.
 * Storing the offsets/addresses that are being called.
 * Automatically writing a label before a called (sub-)function.

# Planned features:
 * Detecting common file formats for better/easier usage.
 * Writing code into a file instead of using "printf()"

# TODO:
 * **Clean-up**
 * Improve performance of branch-detecting
 * Find out what is executed when the Wii U executable branches out of the file. (implement solution after implementing easy .ELF detection)
 
# Compiling:
 Open the Solution(.sln)-File in Visual Studio 2015 and hit *compile*!
 (until now it best performs when compiled as a **x64 executable**, that might change in the future)
