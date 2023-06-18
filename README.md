# My Benchmarking IO 

This program is used to perform simple benchmarking using io_uring as the main engine. This program is still running in Read Only (O_RDONLY) & Write Only (O_WRONLY) mode

## How to run 
You need to build and compile the main program first
examples :
```
gcc -o your_output_file_name main.c -L/path/to/liburing/lib -Wl,-rpath=/path/to/liburing/lib -luring
```

Then run your output file
```
./yout_output_file_name
```

if you dont have library iouring library, install it first by type this command
Linux :
```
sudo apt-get install liburing-dev
```

## File Path 
If your confusing with file path just put your benchmarking test file to examples folder and copy the path 
examples :
```
/examples/examples.txt
```
Or you can copy benchmarking test file path outside of examples folder
examples :
```
/home/your_user/examples.txt
```