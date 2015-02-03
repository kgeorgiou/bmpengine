#bmpengine

###A library for editing 24-bit bitmap images.

Useful information on bitmap file format:
http://en.wikipedia.org/wiki/BMP_file_format

##Getting Started

Once you have the source code,
while in the bmpengine directory

###__Compilation__ (Makefile's parameters)

```
$make
```

###__Run__

```
$./bmpengine
```

###__Commands__

**Display a menu with the available commands**
```
$./bmpengine --help
```

**List the metadata of the chosen bitmap image(s)**
```
$./bmpengine -list img1.bmp img2.bmp ... imgn.bmp
```

**Horizontally flip the chosen bitmap image(s)**
```
$./bmpengine -hflip img1.bmp img2.bmp ... imgn.bmp
```

**Vertically flip the chosen bitmap image(s)**
```
$./bmpengine -vflip img1.bmp img2.bmp ... imgn.bmp
```

**Rotate by 90 degrees to the left the chosen bitmap image(s)**
```
$./bmpengine -left90 img1.bmp img2.bmp ... imgn.bmp
```

**Rotate by 90 degrees to the right the chosen bitmap image(s)**
```
$./bmpengine -right90 img1.bmp img2.bmp ... imgn.bmp
```

**Transform into grayscale the chosen bitmap image(s)**
```
$./bmpengine -gray img1.bmp img2.bmp ... imgn.bmp
```

###__Remove object files and executables__
```
$make clean
```


##Testing

Each .c file (except *main.c* and *test.c*) comes with it's driver that carries out some checks on the file's functions by covering most of the cases the function will have to handle.

**You can test each module in the following manner:**

(Let's crush test gray.c for example)
```
$gcc -DDEBUG test.c gray.c -o graytest
$./graytest
```


##Documentation

You can build the project's documentation using Doxygen (http://www.stack.nl/~dimitri/doxygen/)

**Using the Makefile and the .doxyfile**
```
$make doxy
```
