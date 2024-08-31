to generate fishes, do:
```sh
for i in {360..2880..360}; do inkscape -h $i fish.svg --export-filename="fishes/fish_$i.png"; done
```

to bulk rotate images for testing, we're using imagemagick, which uses Paeth rotation:;
```sh
for i in *; do magick $i -background black -rotate 26 $i; done
```