#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numba
import datetime

dir = "../iofiles/"

file = dir + "render.txt"
out  = dir + "anim.mp4"


def gen_func(ims, timestamp):
    def func(n):
        i = n
        if n >= len(ims):
            i = len(ims) - 1
        s = '{:8.6f} [sec]'.format(timestamp[i])
        print(s)
        plt.xlabel(s)
        return ims[i]
    return func

@numba.jit
def main():
    scale = 0.5
    element_num = 13
    data = np.loadtxt(file)
    time = data.shape[0]
    node_num = int((data.shape[1] - 1) / element_num)

    print(time, node_num)
    timestamp = np.zeros(time)
    parsed = np.zeros((time, node_num, element_num))
    for i in range(len(data)):
        timestamp[i] = data[i][0]
        parsed[i] = data[i][1:].reshape(-1, element_num)

    x_max = np.max(parsed[0, :, 3]) * scale
    x_min = np.min(parsed[0, :, 3]) * scale
    y_max = np.max(parsed[0, :, 4]) * scale
    y_min = np.min(parsed[0, :, 4]) * scale

    fig = plt.figure(figsize=(x_max - x_min, y_max - y_min), dpi=100)
    plt.subplots_adjust(left=0, right=1, bottom=0, top=1)
    ims = []
    plt.xlim(x_min - 2, x_max + 2)
    plt.ylim(y_min - 2, y_max + 2)
    i = 0
    for frame in parsed:
        current_frame = []
        x = []
        y = []
        size = []
        color = []
        arrow = []
        completed_node_num = 0
        for node in frame:
            if node[10] != 0:
                x.append(node[3] * scale)
                y.append(node[4] * scale)
                size.append(450)
                if node[10] == 1:
                    color.append([0.8, 0.8, 0.9])
                if node[10] == 2:
                    color.append([0.3, 0.3, 1.0])
                    # color.append([0.8, 0.8, 0.9])
                if node[10] == 3:
                    color.append([1.0, 0.3, 0.3])
                    # color.append([0.8, 0.8, 0.9])
                    completed_node_num += 1
                if node[10] == 4:
                    color.append([0.3, 0.3, 1.0])

            x.append(node[3] * scale)
            y.append(node[4] * scale)
            color.append([node[7] / 255, node[8] / 255, node[9] / 255])
            if node[2] == 1:
                size.append(300*scale)   # CH
            elif node[2] == 2:
                size.append(50*scale)   # CM
            else:
                size.append(20*scale)   # STANDALONE

            if node[11] != 0.0 or node[12] != 0.0:
                arrow.append(plt.arrow(x=node[3]*scale, y=node[4]*scale, dx=node[11]*scale, dy=node[12]*scale, width=0.01, head_width=0.05,
                                  head_length=0.2*scale,
                                  length_includes_head=True, color='red') )

        im1 = plt.scatter(x, y, s=size, c=color, linestyle='-', linewidth=10)
        ttl = plt.text((x_max - x_min) / 2, -2.0, '{:8.6f} [sec]'.format(timestamp[i]),
        horizontalalignment='center', verticalalignment='bottom')

        current_frame += [im1, ttl] + arrow

        ims.append(current_frame)
        i += 1
        print("plotting frame ", i)
        print("completed ", completed_node_num, " / total ", len(frame), " = ", completed_node_num/len(frame))

    for node in parsed[0]:
        plt.annotate(int(node[0]), xy=(node[3]*scale, node[4]*scale), size=10)

    # アニメーション作成
    print("rendering...")
    ani = animation.ArtistAnimation(fig, ims, interval=10)
    func = gen_func(ims, timestamp)
    ani.save(out, writer='ffmpeg', fps=5)
    print("finish")

    print(x_max, x_min, y_max, y_min)


main()

print("input: ", file)
print("outout: ", out)
print(datetime.datetime.today())
