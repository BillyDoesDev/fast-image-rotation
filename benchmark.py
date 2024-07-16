import re
import csv
import pickle
import subprocess
from datetime import datetime
from os import path, listdir
from time import perf_counter_ns
import matplotlib.pyplot as plt

# Gather input images
input_imgs = [
    path.join("./assets/real_life/", _)
    for _ in listdir("./assets/real_life")
    # if re.findall(r"_\d+\.png", _)
]
# input_imgs.sort(key=lambda x: int(re.findall(r"\d+", x)[0]))
# input_imgs = input_imgs[-3:]

# Gather targets
targets = [
    x
    for _ in listdir("./build/")
    if "__b" in _ # so benchmark only those files with a __b in them
    and not re.findall(r"\.", _)
    and path.isfile(x := path.join("./build/", _))
]
# print(f"{targets = }");exit()

# angle = 30
angles = 361
del_angle = 10

# Initialize data collection
data = {target: {img: [] for img in input_imgs} for target in targets}

for input_img in input_imgs:
    print(f"\n\n[operating on {input_img}]...")

    with open(f"logs/resolution_{input_img.split('/')[-1]}.csv", mode="w", newline="", encoding="utf-8", ) as f:
        writer = csv.writer(f)
        writer.writerow(["target", "execution time in ns"])

        for target in targets:
            timestamps = []

            for angle in range(0, angles, del_angle):
                start = perf_counter_ns()
                r = subprocess.run(
                    [
                        "qemu-aarch64-static",
                        target,
                        input_img,
                        path.join(
                            "./outputs/",
                            f"{target.split('/')[-1]}_{input_img.split('/')[-1]}",
                        ),
                        str(angle), # insert angle here
                    ],
                    capture_output=True,
                )
                time_elapsed = perf_counter_ns() - start
                if r.returncode == 0:
                    print(f"{target} took {time_elapsed} ns to process {input_img}")
                    writer.writerow([target, time_elapsed])
                    timestamps.append(time_elapsed)
                else:
                    print("\n" + "ERROR".center(90, "="))
                    print(r.stderr)
                    print("="*90 + "\n")

            try:
                avg_time = round((sum(timestamps) / len(timestamps)) / 1e+9, 3)
                print(f"[{target} took about {avg_time} s on average]\n")
                writer.writerow([f"{target} took about {avg_time} s on average", ""])
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
            x = range(0, angles, del_angle)
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
pickle.dump(fig, open(f'logs/{datetime.now().strftime('%b%d_%H_%M')}.fig.pickle', 'wb'))
plt.show()
