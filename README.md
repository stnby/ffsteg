# ffsteg

[Farbfeld image format](https://tools.suckless.org/farbfeld/) steganography tool with **zero** dependencies!!!

Using it is straight forward too...
```
$ cat in.ff | ./ffsteg enc `echo sitenbay | ./stdin2bin.sh` > out.ff
encoded: 72 bits

$ cat out.ff | ./ffsteg dec 72
011100110110100101110100011001010110111001100010011000010111100100001010
```

You can also use some handy farbfeld tools to convert it back and forth to **png**
```
$ cat in.png | png2ff | ./ffsteg enc `echo ':)' | ./stdin2bin.sh` | ff2png > out.png
encoded: 24 bits

$ cat out.png | png2ff | ./ffsteg dec 24
001110100010100100001010
```
