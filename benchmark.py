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

# Gather input images
img_dir = "./assets/standard_test_images/"
input_imgs = [
    path.join(img_dir, _)
    for _ in listdir(img_dir)
    # if re.findall(r"_\d+\.png", _)
    if path.isfile(path.join(img_dir, _))
]
# input_imgs.sort(key=lambda x: int(re.findall(r"\d+", x)[0]))
input_imgs = input_imgs[:]

# test_input_images = [_.replace("standard_test_images", "tests") for _ in input_imgs]

# angle_range = range(0, 361, 10)
angle_range = range(26, 27, 10)
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
                [
                    "magick",
                    img,
                    "-background",
                    "black",
                    "-rotate",
                    str(angle_range[0]), # insert angle here
                    out_test_img_path,
                ],
                capture_output=True,
            )
            test_input_images.append(out_test_img_path)

            if r.returncode != 0:
                print("\n" + "ERROR".center(90, "="))
                print(r.stderr)
                print("="*90 + "\n")

# Gather targets
targets = [
    x
    for _ in listdir("./build/")
    if "__b" in _ # so benchmark only those files with a __b in them
    and not re.findall(r"\.", _)
    and path.isfile(x := path.join("./build/", _))
]
# print(f"{targets = }");exit()
outputs = []

# angle_range = range(0, 361, 10)
# angle_range = range(26, 27, 10)

# Initialize data collection
data = {target: {img: [] for img in input_imgs} for target in targets}

summary = { target:{img:0 for img in input_imgs} for target in targets}
for input_img in input_imgs:
    print(f"\n\n[operating on {input_img}]...")

    # with open(f"logs/log_{input_img.split('/')[-1]}.csv", mode="w", newline="", encoding="utf-8", ) as f:
        # writer = csv.writer(f)
        # writer.writerow(["target", "execution time in ns"])

    for target in targets:
        timestamps = []

        for angle in angle_range:
            start = perf_counter_ns()
            output_path = path.join("./outputs/", f"{target.split('/')[-1]}_{input_img.split('/')[-1]}")
            outputs.append(output_path)
            r = subprocess.run(
                [
                    "qemu-aarch64-static",
                    target,
                    input_img,
                    output_path,
                    str(angle), # insert angle here
                ],
                capture_output=True,
            )
            time_elapsed = perf_counter_ns() - start
            if r.returncode == 0:
                print(f"{target} took {time_elapsed} ns to process {input_img}")
                # writer.writerow([target, time_elapsed])
                timestamps.append(time_elapsed)
            else:
                print("\n" + "ERROR".center(90, "="))
                print(r.stderr)
                print("="*90 + "\n")

        try:
            avg_time = round((sum(timestamps) / len(timestamps)) / 1e+9, 3)
            print(f"[{target} took about {avg_time} s on average]\n")
            # writer.writerow([f"{target} took about {avg_time} s on average", ""])
            summary[target][input_img] = avg_time
            ## Store data for plotting
            data[target][input_img] = timestamps
        except ZeroDivisionError:
            pass

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
                f"{target.split('/')[-1][:-3]} on {path.basename(input_img)}, took {round((sum(times) / len(times)) / 1e+9, 3)}s on avg",
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
# save the plot for later :D
print("="*40)
if (len(sys.argv) > 1 and sys.argv[1] == "0"):
    print("[DID NOT SAVE PLOT LOG]\n[NOT DISPLAYING PLOT]")

else:
    pickle_dump_path = f'logs/{datetime.now().strftime('%b%d_%H_%M')}.fig.pickle'
    pickle.dump(fig, open(pickle_dump_path, 'wb'))
    print(f"plot log saved at {pickle_dump_path}")
    plt.show()

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
print("="*40)
