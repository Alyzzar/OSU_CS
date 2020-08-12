#### INFO:
Assignment 4: Multi-threaded Programming  
Written by Ben Chan for OSU CS 344, Summer 2020  
  
##### [C] Multi-threaded Line-Processor:    
Due to a race condition, this code has a 75%-80% success rate. If it fails to execute correctly on the first try,  
re-run a couple of times. Interestingly, the code has a 98% success rate when printing with debugging information.  
This can be done by running the code with the parameter '1'.  
  
To run normally:  
> $ line_processor < input.txt > output.txt  
  
To run with debugging info:  
> $ line_processor 1 < input.txt > output.txt  
  
##### [Rust] Multi-threaded Map Reduce:  
This code worked correctly during testing. No Debug statements were included.
  
#### FILES:
>README.md  
>parser.c
>main.rs
>makefile

#### COMPILER:
>'make' - Compile line_processor.  
>'make clean' - Clean up directory.  
