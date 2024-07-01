import subprocess

img = "assets/fish_2880.png"
for i in range(361):
    r = subprocess.run(
        ["./build/darotator", img, "lol.png", str(i)],
        capture_output=True
    )
    if r.returncode != 0:
        print("error reported at angle", i, "for", img)
