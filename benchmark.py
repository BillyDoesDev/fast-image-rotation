import argparse
import re
import csv
import json
import pickle
import subprocess
from datetime import datetime
from os import path, listdir, mkdir
from time import perf_counter_ns
from PIL import Image
import matplotlib.pyplot as plt
import shutil

# Hideous monkeypatch. ymmv
# fixes issue where JPEGs are identified as MPOs (they're the same)
from PIL import JpegImagePlugin
JpegImagePlugin._getmp = lambda x: None


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

angle_range = range(*args.angle_range)

def get_image_info(image_path):
    try:
        with Image.open(image_path) as img:
            img_name = path.basename(image_path)
            img_size = img.size  # (width, height)
            img_mode = img.mode  # "RGB", "L" (grayscale), etc.
            img_format = img.format  # "JPEG", "PNG", etc.
            img_res = img.info.get("dpi", (72, 72))  # defaulting to (72, 72) if no DPI info
            return {
                "name": img_name,
                "size": img_size,
                "mode": img_mode,
                "channels": len(img.getbands()),
                "format": img_format,
                "resolution": img_res if img_res != (72, 72) else ("-", "-")
            }
    except Exception as e:
        print(f"Failed to get image info for {image_path}: {e}")
        return None

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

data = {target: {img: [] for img in input_imgs} for target in targets}
summary = {target: {img: 0 for img in input_imgs} for target in targets}

with open("logs/benchmark_summary.txt", encoding="utf-8", mode="w") as logfile:

    for input_img in input_imgs:
        print(f"\n\n[operating on {input_img}]...")
        logfile.write(f"\n\n[operating on {input_img}]...\n")

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
                    logfile.write(f"{target} took {time_elapsed} ns to process {input_img}\n")
                    timestamps.append(time_elapsed)
                else:
                    print("\n" + "ERROR".center(90, "="))
                    print(r.stderr)
                    print("=" * 90 + "\n")
                    logfile.write(f"\n\n[ERROR]\n{r.stderr}\n\n")

            try:
                avg_time = round((sum(timestamps) / len(timestamps)) / 1e9, 3)
                print(f"[{target} took about {avg_time} s on average]\n")
                logfile.write(f"[{target} took about {avg_time} s on average]\n\n")
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
    # Save the plot
    pickle_dump_path = f'logs/{datetime.now().strftime("%b%d_%H_%M")}.fig.pickle'
    pickle.dump(fig, open(pickle_dump_path, 'wb'))
    print(f"Plot log saved at {pickle_dump_path}")
    plt.show()

    # Save image metadata to CSV
    csv_file_path = f'logs/image_metadata_{datetime.now().strftime("%b%d_%H_%M")}.csv'
    with open(csv_file_path, mode="w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["Image Name", "Image Size (WxH)", "Channels", "Type", "Resolution (DPI)", "Format"])

        for img_path in input_imgs:
            img_info = get_image_info(img_path)
            if img_info:
                writer.writerow([
                    img_info["name"],
                    f"{img_info['size'][0]}x{img_info['size'][1]}",
                    img_info["channels"],
                    img_info["mode"],
                    f"{img_info['resolution'][0]}x{img_info['resolution'][1]}",
                    img_info["format"]
                ])

    print(f"Image metadata saved at {csv_file_path}")
else:
    print("[DID NOT SAVE PLOT LOG]\n[NOT DISPLAYING PLOT]")

with open("logs/stuff_tested.json", mode="w", encoding="utf-8") as f:
    json.dump({"input_images": test_input_images, "binaries": targets, "output_images": outputs}, f)

print(f"\nangle range tested: {angle_range}")
print(f"images tested: {input_imgs}"[:50] + "...")
print(f"#images tested: {len(input_imgs)}")
print(f"#targets tested: {len(targets)}")
print("json output stored at logs/stuff_tested.json")

print("\nTest summary:")
with open("logs/execution_time_summary.csv", encoding="utf-8", mode="w") as f:
    writer = csv.writer(f)
    header = ["algorithm"]
    for target in summary:
        for img in summary[target]:
            header.append(img.split("/")[-1])
        break
    writer.writerow(header)
    print(*header)

    for target in summary:
        row_ = []
        row_.append(target.split('/')[-1][:-3])
        for img in summary[target]:
            row_.append(summary[target][img])
        print(*row_)
        writer.writerow(row_)

print("Test summary saved at logs/execution_time_summary.csv")

print("="*80)
