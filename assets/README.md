to generate fishes, do:
```sh
for i in {360..2880..360}; do inkscape -h $i fish.svg --export-filename="fishes/fish_$i.png"; done
```

