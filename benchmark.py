import argparse
import re
import csv
import json
import pickle
import subprocess
from datetime import datetime
from os import path, listdir, mkdir
from time import perf_counter_ns
import matplotlib.pyplot as plt
import sys
import shutil

# Argument parser setup
parser = argparse.ArgumentParser(description="Process images with specified targets and angles.")
parser.add_argument("--angle-range", nargs=3, type=int, metavar=('START', 'STOP', 'STEP'),
                    default=(0, 361, 10), help="Set custom angle range (start, stop, step).")
parser.add_argument("--img-dir", type=str, default="./assets/standard_test_images/",
                    help="Set the image input directory.")
parser.add_argument("--num-images", type=int, default=None,
                    help="Specify number of images to use from the input directory.")
parser.add_argument("--save-log", action="store_true", help="Save output logs if this flag is set.")

args = parser.parse_args()

# Gather input images
img_dir = args.img_dir
input_imgs = [path.join(img_dir, _) for _ in listdir(img_dir) if path.isfile(path.join(img_dir, _))]

# Limit number of images if specified
if args.num_images is not None:
    input_imgs = input_imgs[:args.num_images]

# Set angle range
angle_range = range(*args.angle_range)

# Rest of the script remains mostly the same
test_input_images = []
if angle_range[0] == angle_range[-1]:
    img_dir_ = img_dir.strip("/").split("/")[-1]
    test_img_dir = img_dir.replace(img_dir_, "tests")
    try:
        mkdir(test_img_dir)
    except FileExistsError:
        shutil.rmtree(test_img_dir)
        mkdir(test_img_dir)

    for img in input_imgs:
        out_test_img_path = path.join(test_img_dir, img.split("/")[-1])
        r = subprocess.run(
            ["magick", img, "-background", "black", "-rotate", str(angle_range[0]), out_test_img_path],
            capture_output=True,
        )
        test_input_images.append(out_test_img_path)

        if r.returncode != 0:
            print("\n" + "ERROR".center(90, "="))
            print(r.stderr)
            print("=" * 90 + "\n")

# Gather targets
targets = [
    x
    for _ in listdir("./build/")
    if "__b" in _ and not re.findall(r"\.", _) and path.isfile(x := path.join("./build/", _))
]
outputs = []

# Initialize data collection
data = {target: {img: [] for img in input_imgs} for target in targets}
summary = {target: {img: 0 for img in input_imgs} for target in targets}

for input_img in input_imgs:
    print(f"\n\n[operating on {input_img}]...")

    for target in targets:
        timestamps = []

        for angle in angle_range:
            start = perf_counter_ns()
            output_path = path.join("./outputs/", f"{target.split('/')[-1]}_{input_img.split('/')[-1]}")
            outputs.append(output_path)
            r = subprocess.run(
                ["qemu-aarch64-static", target, input_img, output_path, str(angle)], capture_output=True,
            )
            time_elapsed = perf_counter_ns() - start
            if r.returncode == 0:
                print(f"{target} took {time_elapsed} ns to process {input_img}")
                timestamps.append(time_elapsed)
            else:
                print("\n" + "ERROR".center(90, "="))
                print(r.stderr)
                print("=" * 90 + "\n")

        try:
            avg_time = round((sum(timestamps) / len(timestamps)) / 1e9, 3)
            print(f"[{target} took about {avg_time} s on average]\n")
            summary[target][input_img] = avg_time
            data[target][input_img] = timestamps
        except ZeroDivisionError:
            pass

# Plotting
fig, ax = plt.subplots(figsize=(12, 8))
fig.patch.set_facecolor("#000")
ax.set_facecolor("#000")

for target in targets:
    for input_img in input_imgs:
        times = data[target][input_img]
        if times:
            x = angle_range
            y = [t / 1e9 for t in times]
            ax.plot(x, y)
            ax.text(
                x[-1],
                y[-1],
                f"{target.split('/')[-1][:-3]} on {path.basename(input_img)}, took {round((sum(times) / len(times)) / 1e9, 3)}s on avg",
                fontsize="small",
                color="white",
                ha="left",
                va="center",
            )

ax.set_title("Processing Time for Targets over Multiple Trials", color="white")
ax.set_xlabel("Angle (check only multiples of 10)", color="white")
ax.set_ylabel("Time (s)", color="white")
ax.grid(True, which="both", linestyle="--", linewidth=0.5, color="gray")
ax.tick_params(axis="both", colors="white")

plt.tight_layout()

print("="*80)
if args.save_log:
    pickle_dump_path = f'logs/{datetime.now().strftime("%b%d_%H_%M")}.fig.pickle'
    pickle.dump(fig, open(pickle_dump_path, 'wb'))
    print(f"Plot log saved at {pickle_dump_path}")
    plt.show()
else:
    print("[DID NOT SAVE PLOT LOG]\n[NOT DISPLAYING PLOT]")

with open("logs/stuff_tested.json", mode="w", encoding="utf-8") as f:
    json.dump({"input_images": test_input_images, "binaries": targets, "output_images": outputs}, f)

print(f"\nAngle range tested: {angle_range}")
print(f"Images tested: {input_imgs}")
print(f"#Images tested: {len(input_imgs)}")
print(f"#Targets tested: {len(targets)}")
print("JSON output stored at logs/stuff_tested.json")
print("="*80)
